#include "FICSITDispatchTerminal.h"
#include "FICSITDispatchTerminalWidget.h"
#include "FGCharacterPlayer.h"
#include "Hologram/FGBuildableHologram.h"
#include "FGColoredInstanceMeshProxy.h"
#include "FGClearanceData.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AFICSITDispatchTerminal::AFICSITDispatchTerminal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    SetReplicates(true);
    SetCanBeDamaged(false);
    mDisplayName = FText::FromString(TEXT("FICSIT Dispatch Terminal"));
    mDescription = FText::FromString(TEXT("A centralized FICSIT logistics console. Review production contracts, track deliveries, monitor reputation and coordinate your factory's supply chain."));
    mHologramClass = AFGBuildableHologram::StaticClass();
    mIsUseable = true;
    mInteractWidgetSoftClass = UFICSITDispatchTerminalWidget::StaticClass();

    FFGClearanceData TerminalClearance;
    TerminalClearance.Type = EClearanceType::CT_Default;
    TerminalClearance.ClearanceBox = FBox(FVector(-85.0f,-60.0f,0.0f),FVector(85.0f,60.0f,235.0f));
    TerminalClearance.RelativeTransform = FTransform::Identity;
    TerminalClearance.ExcludeForSnapping = false;
    mClearanceData.Add(TerminalClearance);

    SolidCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DispatchSolidCollision"));
    SolidCollision->SetupAttachment(RootComponent);
    SolidCollision->SetBoxExtent(FVector(76.0f, 52.0f, 112.0f));
    SolidCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 112.0f));
    SolidCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SolidCollision->SetCollisionObjectType(ECC_WorldStatic);
    SolidCollision->SetCollisionResponseToAllChannels(ECR_Block);
    SolidCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    SolidCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    SolidCollision->SetGenerateOverlapEvents(false);

    InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("DispatchInteractionVolume"));
    InteractionVolume->SetupAttachment(RootComponent);
    InteractionVolume->SetBoxExtent(FVector(105.0f, 90.0f, 125.0f));
    InteractionVolume->SetRelativeLocation(FVector(0.0f, 0.0f, 112.0f));
    InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionVolume->SetCollisionObjectType(ECC_WorldDynamic);
    InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionVolume->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    InteractionVolume->SetGenerateOverlapEvents(false);

    ImportedTerminalMesh = CreateDefaultSubobject<UFGColoredInstanceMeshProxy>(TEXT("ImportedTerminalMesh"));
    ImportedTerminalMesh->SetupAttachment(RootComponent);
    ImportedTerminalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ImportedTerminalMesh->SetCastShadow(true);
    FallbackBase = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackBase")); FallbackBase->SetupAttachment(RootComponent);
    FallbackPedestal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackPedestal")); FallbackPedestal->SetupAttachment(RootComponent);
    FallbackDesk = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackDesk")); FallbackDesk->SetupAttachment(RootComponent);
    FallbackMonitor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackMonitor")); FallbackMonitor->SetupAttachment(RootComponent);
    FallbackScreen = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackScreen")); FallbackScreen->SetupAttachment(RootComponent);
    FallbackStatusBar = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackStatusBar")); FallbackStatusBar->SetupAttachment(RootComponent);

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* TerminalMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/FICSITContracts/Buildables/DispatchTerminal/SM_DispatchTerminal.SM_DispatchTerminal"));
    if (TerminalMesh)
    {
        ImportedTerminalMesh->SetStaticMesh(TerminalMesh);
        FallbackBase->SetVisibility(false); FallbackPedestal->SetVisibility(false); FallbackDesk->SetVisibility(false);
        FallbackMonitor->SetVisibility(false); FallbackScreen->SetVisibility(false); FallbackStatusBar->SetVisibility(false);
    }
    else
    {
        ConfigureFallbackMesh(FallbackBase, Cube, FVector(0,0,12), FVector(1.55f,1.05f,0.18f));
        ConfigureFallbackMesh(FallbackPedestal, Cube, FVector(0,8,61), FVector(0.92f,0.72f,0.78f));
        ConfigureFallbackMesh(FallbackDesk, Cube, FVector(0,-2,105), FVector(1.55f,0.88f,0.16f));
        ConfigureFallbackMesh(FallbackMonitor, Cube, FVector(0,24,158), FVector(1.22f,0.16f,0.82f), FRotator(-12,0,0));
        ConfigureFallbackMesh(FallbackScreen, Cube, FVector(0,13,160), FVector(1.02f,0.035f,0.61f), FRotator(-12,0,0));
        ConfigureFallbackMesh(FallbackStatusBar, Cube, FVector(0,35,205), FVector(0.36f,0.12f,0.07f));
    }
    ScreenLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ScreenLight"));
    ScreenLight->SetupAttachment(RootComponent);
    ScreenLight->SetMobility(EComponentMobility::Movable);
    ScreenLight->SetRelativeLocation(FVector(0,-2,172));
    ScreenLight->SetLightColor(FLinearColor(0.08f,0.62f,1.0f));
    ScreenLight->SetIntensity(420.0f);
    ScreenLight->SetAttenuationRadius(260.0f);
    ScreenLight->SetCastShadows(false);
}

void AFICSITDispatchTerminal::ConfigureFallbackMesh(UStaticMeshComponent* Component, UStaticMesh* Cube,
    const FVector& Location, const FVector& Scale, const FRotator& Rotation)
{
    if (!Component || !Cube) return;
    Component->SetStaticMesh(Cube);
    Component->SetRelativeLocation(Location);
    Component->SetRelativeScale3D(Scale);
    Component->SetRelativeRotation(Rotation);
    Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Component->SetCastShadow(true);
}

bool AFICSITDispatchTerminal::IsUseable_Implementation() const { return !IsActorBeingDestroyed(); }

void AFICSITDispatchTerminal::OnUse_Implementation(AFGCharacterPlayer* ByCharacter, const FUseState& State)
{
    if (!IsValid(ByCharacter)) return;
    Super::OnUse_Implementation(ByCharacter, State);
}