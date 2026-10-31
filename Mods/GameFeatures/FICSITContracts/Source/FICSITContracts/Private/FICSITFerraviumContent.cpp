#include "FICSITFerraviumContent.h"

#include "FICSITContractsSubsystem.h"
#include "FICSITDispatchTerminalContent.h"
#include "Buildables/FGBuildableHologram.h"
#include "Equipment/FGBuildGun.h"
#include "FGFactoryConnectionComponent.h"
#include "FGBuildCategory.h"
#include "FGCategory.h"
#include "FGSchematicCategory.h"
#include "FGClearanceData.h"
#include "IncludeInBuild.h"
#include "ItemAmount.h"
#include "Resources/FGItemDescriptor.h"
#include "Subsystem/SubsystemActorManager.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "UObject/EnumProperty.h"
#include "UObject/UnrealType.h"

namespace
{
    constexpr const TCHAR* OreMeshPath = TEXT("/FICSITContracts/Resources/Ferravium/Mesh/SM_FerraviumOre.SM_FerraviumOre");
    constexpr const TCHAR* ConcentrateMeshPath = TEXT("/FICSITContracts/Resources/Ferravium/Mesh/SM_FerraviumConcentrate.SM_FerraviumConcentrate");
    constexpr const TCHAR* IngotMeshPath = TEXT("/FICSITContracts/Resources/Ferravium/Mesh/SM_FerraviumIngot.SM_FerraviumIngot");
    constexpr const TCHAR* SmelterMeshPath = TEXT("/FICSITContracts/Buildables/FerraviumSmelter/SM_FerraviumSmelter.SM_FerraviumSmelter");

    TSubclassOf<UFGItemDescriptor> LoadItem(const TCHAR* Path)
    {
        return LoadClass<UFGItemDescriptor>(nullptr, Path);
    }

    void AddItem(TArray<FItemAmount>& Items, const TCHAR* Path, int32 Amount)
    {
        const TSubclassOf<UFGItemDescriptor> Item = LoadItem(Path);
        if (Item) Items.Emplace(Item, Amount);
        else UE_LOG(LogTemp, Warning, TEXT("FICSIT Ferravium: cannot resolve item class %s"), Path);
    }

    void AddProducer(TArray<TSoftClassPtr<UObject>>& Producers, const TCHAR* Path)
    {
        if (UClass* Producer = LoadClass<UObject>(nullptr, Path))
            Producers.Add(TSoftClassPtr<UObject>(Producer));
        else
            UE_LOG(LogTemp, Warning, TEXT("FICSIT Ferravium: cannot resolve producer %s"), Path);
    }

    UTexture2D* LoadFerraviumIcon(const TCHAR* Name)
    {
        const FString Path = FString::Printf(TEXT("/FICSITContracts/Resources/Ferravium/UI/%s.%s"), Name, Name);
        return LoadObject<UTexture2D>(nullptr, *Path);
    }

    void MarkPublic(UFGSchematic* Schematic)
    {
#if WITH_EDITORONLY_DATA
        if (!Schematic) return;
        if (FEnumProperty* Property = FindFProperty<FEnumProperty>(Schematic->GetClass(), TEXT("mIncludeInBuilds")))
        {
            if (FNumericProperty* Underlying = Property->GetUnderlyingProperty())
                Underlying->SetIntPropertyValue(Property->ContainerPtrToValuePtr<void>(Schematic),
                    static_cast<int64>(EIncludeInBuilds::IIB_PublicBuilds));
        }
#endif
    }
}

UFICSITFerraviumOreDescriptor::UFICSITFerraviumOreDescriptor()
{
    mUseDisplayNameAndDescription = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Ore"));
    mDescription = FText::FromString(TEXT("Dense blue-black mineral threaded with bright orange conductive veins."));
    mAbbreviatedDisplayName = FText::FromString(TEXT("F-ORE"));
    mForm = EResourceForm::RF_SOLID;
    mStackSize = EStackSize::SS_BIG;
    mCanBeDiscarded = true;
    mRememberPickUp = true;
    mRadioactiveDecay = 0.0f;
    mConveyorMesh = LoadObject<UStaticMesh>(nullptr, OreMeshPath);
    mDepositMesh = mConveyorMesh;
    mCompassTexture = LoadFerraviumIcon(TEXT("TX_FerraviumOre_256"));
    mPingColor = FLinearColor(1.0f, 0.32f, 0.055f, 1.0f);
    mCollectSpeedMultiplier = 1.0f;
    mDecalSize = 180.0f;
    mSmallIcon = mCompassTexture;
    mPersistentBigIcon = mCompassTexture;
    mCategory = LoadClass<UFGCategory>(nullptr, TEXT("/Game/FactoryGame/Resource/ItemCategories/Cat_RawMaterials.Cat_RawMaterials_C"));
}

UFICSITFerraviumConcentrateDescriptor::UFICSITFerraviumConcentrateDescriptor()
{
    mUseDisplayNameAndDescription = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Concentrate"));
    mDescription = FText::FromString(TEXT("Washed Ferravium feedstock, prepared for high-temperature smelting."));
    mAbbreviatedDisplayName = FText::FromString(TEXT("F-CONC"));
    mForm = EResourceForm::RF_SOLID;
    mStackSize = EStackSize::SS_BIG;
    mCanBeDiscarded = true;
    mRememberPickUp = true;
    mRadioactiveDecay = 0.0f;
    mConveyorMesh = LoadObject<UStaticMesh>(nullptr, ConcentrateMeshPath);
    mSmallIcon = LoadFerraviumIcon(TEXT("TX_FerraviumConcentrate_256"));
    mPersistentBigIcon = mSmallIcon;
    mCategory = LoadClass<UFGCategory>(nullptr, TEXT("/Game/FactoryGame/Resource/ItemCategories/Cat_Minerals.Cat_Minerals_C"));
}

UFICSITFerraviumIngotDescriptor::UFICSITFerraviumIngotDescriptor()
{
    mUseDisplayNameAndDescription = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Ingot"));
    mDescription = FText::FromString(TEXT("Refined high-strength Ferravium metal for advanced industrial structures and equipment."));
    mAbbreviatedDisplayName = FText::FromString(TEXT("F-ING"));
    mForm = EResourceForm::RF_SOLID;
    mStackSize = EStackSize::SS_BIG;
    mCanBeDiscarded = true;
    mRememberPickUp = true;
    mRadioactiveDecay = 0.0f;
    mConveyorMesh = LoadObject<UStaticMesh>(nullptr, IngotMeshPath);
    mSmallIcon = LoadFerraviumIcon(TEXT("TX_FerraviumIngot_256"));
    mPersistentBigIcon = mSmallIcon;
    mCategory = LoadClass<UFGCategory>(nullptr, TEXT("/Game/FactoryGame/Resource/ItemCategories/Cat_Ingots.Cat_Ingots_C"));
}

AFICSITFerraviumSmelter::AFICSITFerraviumSmelter()
    : Super()
{
    PrimaryActorTick.bCanEverTick = false;
    SetReplicates(true);
    SetCanBeDamaged(false);
    mDisplayName = FText::FromString(TEXT("Ferravium Smelter"));
    mDescription = FText::FromString(TEXT("A high-temperature industrial furnace for Ferravium concentrate."));
    mHologramClass = AFGBuildableHologram::StaticClass();
    mManufacturingSpeed = 1.0f;

    // The game declares this base property private, so set it through reflection.
    if (FFloatProperty* Power = FindFProperty<FFloatProperty>(AFGBuildableFactory::StaticClass(), TEXT("mPowerConsumption")))
        Power->SetPropertyValue_InContainer(this, 90.0f);
    else
        UE_LOG(LogTemp, Warning, TEXT("FICSIT Ferravium: mPowerConsumption property was not found."));

    FFGClearanceData Clearance;
    Clearance.Type = EClearanceType::CT_Default;
    Clearance.ClearanceBox = FBox(FVector(-165.0f, -125.0f, 0.0f), FVector(165.0f, 125.0f, 255.0f));
    Clearance.RelativeTransform = FTransform::Identity;
    Clearance.ExcludeForSnapping = false;
    mClearanceData.Add(Clearance);

    SolidCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FerraviumSmelterCollision"));
    SolidCollision->SetupAttachment(RootComponent);
    SolidCollision->SetBoxExtent(FVector(145.0f, 108.0f, 122.0f));
    SolidCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 122.0f));
    SolidCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SolidCollision->SetCollisionObjectType(ECC_WorldStatic);
    SolidCollision->SetCollisionResponseToAllChannels(ECR_Block);
    SolidCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    SolidCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    SolidCollision->SetGenerateOverlapEvents(false);

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FerraviumSmelterVisual"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetCastShadow(true);

    if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, SmelterMeshPath))
    {
        VisualMesh->SetStaticMesh(Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FICSIT Ferravium: smelter model is not imported; run BuildFerraviumAssets.py."));
        if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
        {
            VisualMesh->SetStaticMesh(Cube);
            VisualMesh->SetRelativeScale3D(FVector(2.8f, 2.1f, 2.2f));
        }
    }

    // Keep the canonical Input0/Output0 component names used by vanilla machines.\n    InputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"));
    InputConnection->SetupAttachment(RootComponent);
    InputConnection->SetRelativeLocation(FVector(-155.0f, 0.0f, 76.0f));
    InputConnection->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    InputConnection->SetDirection(EFactoryConnectionDirection::FCD_INPUT);
    InputConnection->SetConnectorClearance(85.0f);
    InputConnection->SetForwardPeekAndGrabToBuildable(true);

    OutputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Output0"));
    OutputConnection->SetupAttachment(RootComponent);
    OutputConnection->SetRelativeLocation(FVector(155.0f, 0.0f, 76.0f));
    OutputConnection->SetRelativeRotation(FRotator::ZeroRotator);
    OutputConnection->SetDirection(EFactoryConnectionDirection::FCD_OUTPUT);
    OutputConnection->SetConnectorClearance(85.0f);
    OutputConnection->SetForwardPeekAndGrabToBuildable(true);
}

UFICSITFerraviumSmelterDescriptor::UFICSITFerraviumSmelterDescriptor()
{
    mBuildableClass = AFICSITFerraviumSmelter::StaticClass();
    mUseDisplayNameAndDescription = false;
    mMenuPriority = 178.0f;
    mCategory = LoadClass<UFGBuildCategory>(nullptr,
        TEXT("/Game/FactoryGame/Interface/UI/InGame/BuildMenu/BuildCategories/BC_Production.BC_Production_C"));
    if (UTexture2D* Icon = LoadFerraviumIcon(TEXT("TX_FerraviumSmelter_256")))
    {
        mSmallIcon = Icon;
        mPersistentBigIcon = Icon;
    }
}

UFICSITFerraviumSmelterBuildRecipe::UFICSITFerraviumSmelterBuildRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Construct Ferravium Smelter"));
    mManufactoringDuration = 0.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITFerraviumSmelterDescriptor::StaticClass(), 1);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFGBuildGun::StaticClass()));

    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C"), 30);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C"), 80);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C"), 20);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"), 10);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C"), 100);
}

UFICSITFerraviumRecoveryRecipe::UFICSITFerraviumRecoveryRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Recovery"));
    mManufactoringDuration = 18.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITFerraviumOreDescriptor::StaticClass(), 60);

    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/RawResources/OreBauxite/Desc_OreBauxite.Desc_OreBauxite_C"), 120);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/RawResources/Coal/Desc_Coal.Desc_Coal_C"), 30);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/RawResources/Water/Desc_Water.Desc_Water_C"), 60);
    AddProducer(mProducedIn, TEXT("/Game/FactoryGame/Buildable/Factory/OilRefinery/Build_OilRefinery.Build_OilRefinery_C"));
}

UFICSITFerraviumConcentrateRecipe::UFICSITFerraviumConcentrateRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Washing"));
    mManufactoringDuration = 12.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITFerraviumConcentrateDescriptor::StaticClass(), 45);

    AddItem(mIngredients, TEXT("/Script/FICSITContracts.FICSITFerraviumOreDescriptor"), 60);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/RawResources/Water/Desc_Water.Desc_Water_C"), 30);
    AddProducer(mProducedIn, TEXT("/Game/FactoryGame/Buildable/Factory/OilRefinery/Build_OilRefinery.Build_OilRefinery_C"));
}

UFICSITFerraviumSmeltingRecipe::UFICSITFerraviumSmeltingRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Ferravium Ingot"));
    mManufactoringDuration = 10.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITFerraviumIngotDescriptor::StaticClass(), 30);

    AddItem(mIngredients, TEXT("/Script/FICSITContracts.FICSITFerraviumConcentrateDescriptor"), 45);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/RawResources/Coal/Desc_Coal.Desc_Coal_C"), 30);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFICSITFerraviumSmelter::StaticClass()));
}

UFICSITFerraviumRecipeUnlock::UFICSITFerraviumRecipeUnlock()
{
    mRecipes.Add(UFICSITFerraviumRecoveryRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumConcentrateRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumSmeltingRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumSmelterBuildRecipe::StaticClass());
}

UFICSITFrontierSurveyPrerequisite::UFICSITFrontierSurveyPrerequisite()
{
    mSchematics.Add(UFICSITDispatchTerminalSchematic::StaticClass());
    mRequireAllSchematicsToBePurchased = true;
}

bool UFICSITFrontierSurveyPrerequisite::AreDependenciesMet(UObject* WorldContext) const
{
    if (!Super::AreDependenciesMet(WorldContext) || !WorldContext) return false;

    UWorld* World = WorldContext->GetWorld();
    USubsystemActorManager* Manager = World ? World->GetSubsystem<USubsystemActorManager>() : nullptr;
    AFICSITContractsSubsystem* Contracts = Manager
        ? Manager->GetSubsystemActor<AFICSITContractsSubsystem>() : nullptr;

    // The real vanilla Phase V callback must set this flag before Terminal I becomes available.
    return Contracts && Contracts->GetAvailableTerminalResearchPhase() == 1;
}

UFICSITFrontierSurveySchematic::UFICSITFrontierSurveySchematic()
{
    mType = ESchematicType::EST_Milestone;
    mDisplayName = FText::FromString(TEXT("Frontier Survey // Ferravium Processing"));
    mDescription = FText::FromString(TEXT("Unlock Ferravium Recovery, washing and smelting, plus the Ferravium Smelter build recipe. The recovery recipe is a temporary production source until natural resource-node placement is integrated."));
    mSchematicCategory = LoadClass<UFGSchematicCategory>(
        nullptr, TEXT("/Game/FactoryGame/Schematics/SchematicCategories/SC_Logistics.SC_Logistics_C"));
    mMenuPriority = 178.0f;
    mTechTier = 9;
    mTimeToComplete = 0.0f;
    mIsPlayerSpecific = false;
    mDependenciesBlocksSchematicAccess = true;
    mHiddenUntilDependenciesMet = true;
    mSchematicDependencies.Add(
        CreateDefaultSubobject<UFICSITFrontierSurveyPrerequisite>(TEXT("RequiresTerminalAndPhaseV")));

    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/RawResources/OreBauxite/Desc_OreBauxite.Desc_OreBauxite_C"), 1000);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/RawResources/Coal/Desc_Coal.Desc_Coal_C"), 500);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"), 50);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C"), 25);

    if (UTexture2D* Icon = LoadFerraviumIcon(TEXT("TX_FerraviumOre_256")))
    {
        mSmallSchematicIcon = Icon;
        mSchematicIcon.SetResourceObject(Icon);
    }

    mUnlocks.Add(CreateDefaultSubobject<UFICSITFerraviumRecipeUnlock>(TEXT("UnlockFerraviumRecipes")));
    MarkPublic(this);
}
