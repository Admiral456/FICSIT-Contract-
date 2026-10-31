#include "FICSITDispatchTerminal.h"
#include "FICSITDispatchTerminalWidget.h"
#include "FGCharacterPlayer.h"
#include "FGHUD.h"
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

    FFGClearanceData TerminalClearance;
    TerminalClearance.Type = EClearanceType::CT_Default;
    TerminalClearance.ClearanceBox = FBox(FVector(-85.0f,-60.0f,0.0f),FVector(85.0f,60.0f,235.0f));
    TerminalClearance.RelativeTransform = FTransform::Identity;
    TerminalClearance.ExcludeForSnapping = false;
    mClearanceData.Add(TerminalClearance);

    SolidCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DispatchSolidCollision"));
    SolidCollision->SetupAttachment(RootComponent);
    SolidCollision->SetBoxExtent(FVector(80.0f, 57.0f, 112.0f));
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
    FallbackCanopy = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackCanopy")); FallbackCanopy->SetupAttachment(RootComponent);
    FallbackLeftModule = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackLeftModule")); FallbackLeftModule->SetupAttachment(RootComponent);
    FallbackRightModule = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackRightModule")); FallbackRightModule->SetupAttachment(RootComponent);
    FallbackFrontGrille = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackFrontGrille")); FallbackFrontGrille->SetupAttachment(RootComponent);
    FallbackBeacon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FallbackBeacon")); FallbackBeacon->SetupAttachment(RootComponent);

    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    UStaticMesh* Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    UStaticMesh* TerminalMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/FICSITContracts/Buildables/DispatchTerminal/SM_DispatchTerminal.SM_DispatchTerminal"));
    if (TerminalMesh)
    {
        ImportedTerminalMesh->SetStaticMesh(TerminalMesh);
        FallbackBase->SetVisibility(false); FallbackPedestal->SetVisibility(false); FallbackDesk->SetVisibility(false);
        FallbackMonitor->SetVisibility(false); FallbackScreen->SetVisibility(false); FallbackStatusBar->SetVisibility(false);
        FallbackCanopy->SetVisibility(false); FallbackLeftModule->SetVisibility(false);
        FallbackRightModule->SetVisibility(false); FallbackFrontGrille->SetVisibility(false);
        FallbackBeacon->SetVisibility(false);
    }
    else
    {
        ConfigureFallbackMesh(FallbackBase, Cube, FVector(0,0,10), FVector(1.58f,1.12f,0.12f));
        ConfigureFallbackMesh(FallbackPedestal, Cube, FVector(0,10,62), FVector(0.80f,0.76f,0.84f));
        ConfigureFallbackMesh(FallbackDesk, Cube, FVector(0,-5,116), FVector(1.58f,1.08f,0.15f));
        ConfigureFallbackMesh(FallbackMonitor, Cube, FVector(0,25,165), FVector(1.30f,0.18f,0.72f), FRotator(-10,0,0));
        ConfigureFallbackMesh(FallbackScreen, Cube, FVector(0,12,165), FVector(1.10f,0.035f,0.58f), FRotator(-10,0,0));
        ConfigureFallbackMesh(FallbackStatusBar, Cube, FVector(0,38,204), FVector(0.44f,0.12f,0.06f));
        ConfigureFallbackMesh(FallbackCanopy, Cube, FVector(0,12,218), FVector(1.64f,0.50f,0.055f));
        ConfigureFallbackMesh(FallbackLeftModule, Cube, FVector(-68,-15,144), FVector(0.16f,0.56f,0.92f));
        ConfigureFallbackMesh(FallbackRightModule, Cube, FVector(68,-15,144), FVector(0.16f,0.56f,0.92f));
        ConfigureFallbackMesh(FallbackFrontGrille, Cube, FVector(0,-55,83), FVector(0.90f,0.025f,0.13f));
        ConfigureFallbackMesh(FallbackBeacon, Sphere, FVector(0,12,228), FVector(0.13f,0.13f,0.13f));
    }
    ScreenLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("ScreenLight"));
    ScreenLight->SetupAttachment(RootComponent);
    ScreenLight->SetMobility(EComponentMobility::Movable);
    ScreenLight->SetRelativeLocation(FVector(0,-38,170));
    ScreenLight->SetLightColor(FLinearColor(0.08f,0.62f,1.0f));
    ScreenLight->SetIntensity(520.0f);
    ScreenLight->SetAttenuationRadius(320.0f);
    ScreenLight->SetCastShadows(false);

    BeaconLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("BeaconLight"));
    BeaconLight->SetupAttachment(RootComponent);
    BeaconLight->SetMobility(EComponentMobility::Movable);
    BeaconLight->SetRelativeLocation(FVector(0,12,226));
    BeaconLight->SetLightColor(FLinearColor(1.0f,0.24f,0.035f));
    BeaconLight->SetIntensity(260.0f);
    BeaconLight->SetAttenuationRadius(180.0f);
    BeaconLight->SetCastShadows(false);
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

bool AFICSITDispatchTerminal::IsUseable_Implementation() const
{
    return !IsActorBeingDestroyed();
}

void AFICSITDispatchTerminal::OnUse_Implementation(AFGCharacterPlayer* ByCharacter, const FUseState& State)
{
    if (!IsValid(ByCharacter) || IsActorBeingDestroyed()) return;

    // AFGBuildable keeps its default interaction-widget class private. Open the
    // same native Satisfactory interact UI explicitly for this custom buildable.
    APlayerController* Controller = Cast<APlayerController>(ByCharacter->GetController());
    if (!Controller || !Controller->IsLocalPlayerController()) return;

    if (AFGHUD* HUD = Cast<AFGHUD>(Controller->GetHUD()))
    {
        HUD->OpenInteractUI(UFICSITDispatchTerminalWidget::StaticClass(), this);
    }
}
