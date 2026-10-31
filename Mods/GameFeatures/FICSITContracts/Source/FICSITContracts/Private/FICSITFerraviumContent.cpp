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
    constexpr const TCHAR* TitanAlloyIngotMeshPath = TEXT("/FICSITContracts/Resources/Ferravium/Mesh/SM_TitanAlloyIngot.SM_TitanAlloyIngot");
    constexpr const TCHAR* TitanAlloyBeamMeshPath = TEXT("/FICSITContracts/Resources/Ferravium/Mesh/SM_TitanAlloyBeam.SM_TitanAlloyBeam");
    constexpr const TCHAR* SmelterMeshPath = TEXT("/FICSITContracts/Buildables/FerraviumSmelter/SM_FerraviumSmelter.SM_FerraviumSmelter");
    constexpr const TCHAR* AdvancedAlloyFoundryMeshPath = TEXT("/FICSITContracts/Buildables/AdvancedAlloyFoundry/SM_AdvancedAlloyFoundry.SM_AdvancedAlloyFoundry");

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

UFICSITTitanAlloyIngotDescriptor::UFICSITTitanAlloyIngotDescriptor()
{
    mUseDisplayNameAndDescription = true;
    mDisplayName = FText::FromString(TEXT("Titan Alloy Ingot"));
    mDescription = FText::FromString(TEXT("A high-density structural alloy refined from Ferravium and steel in an advanced Foundry process."));
    mAbbreviatedDisplayName = FText::FromString(TEXT("T-ALY"));
    mForm = EResourceForm::RF_SOLID;
    mStackSize = EStackSize::SS_BIG;
    mCanBeDiscarded = true;
    mRememberPickUp = true;
    mRadioactiveDecay = 0.0f;
    mConveyorMesh = LoadObject<UStaticMesh>(nullptr, TitanAlloyIngotMeshPath);
    mSmallIcon = LoadFerraviumIcon(TEXT("TX_TitanAlloyIngot_256"));
    mPersistentBigIcon = mSmallIcon;
    mCategory = LoadClass<UFGCategory>(nullptr, TEXT("/Game/FactoryGame/Resource/ItemCategories/Cat_Ingots.Cat_Ingots_C"));
}

UFICSITTitanAlloyBeamDescriptor::UFICSITTitanAlloyBeamDescriptor()
{
    mUseDisplayNameAndDescription = true;
    mDisplayName = FText::FromString(TEXT("Titan Alloy Beam"));
    mDescription = FText::FromString(TEXT("A reinforced I-beam extruded from Titan Alloy for heavy industrial frames and structural assemblies."));
    mAbbreviatedDisplayName = FText::FromString(TEXT("T-BEAM"));
    mForm = EResourceForm::RF_SOLID;
    mStackSize = EStackSize::SS_BIG;
    mCanBeDiscarded = true;
    mRememberPickUp = true;
    mRadioactiveDecay = 0.0f;
    mConveyorMesh = LoadObject<UStaticMesh>(nullptr, TitanAlloyBeamMeshPath);
    mSmallIcon = LoadFerraviumIcon(TEXT("TX_TitanAlloyBeam_256"));
    mPersistentBigIcon = mSmallIcon;
    mCategory = LoadClass<UFGCategory>(nullptr,
        TEXT("/Game/FactoryGame/Resource/ItemCategories/Cat_SteelParts.Cat_SteelParts_C"));
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

    // Keep the canonical Input0/Output0 component names used by vanilla machines.
    InputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"));
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

AFICSITAdvancedAlloyFoundry::AFICSITAdvancedAlloyFoundry()
    : Super()
{
    PrimaryActorTick.bCanEverTick = false;
    SetReplicates(true);
    SetCanBeDamaged(false);
    mDisplayName = FText::FromString(TEXT("Advanced Alloy Foundry"));
    mDescription = FText::FromString(TEXT("A reinforced dual-feed foundry for high-density Titan Alloy and heavy structural components."));
    mHologramClass = AFGBuildableHologram::StaticClass();
    mManufacturingSpeed = 1.0f;

    if (FFloatProperty* Power = FindFProperty<FFloatProperty>(AFGBuildableFactory::StaticClass(), TEXT("mPowerConsumption")))
        Power->SetPropertyValue_InContainer(this, 125.0f);
    else
        UE_LOG(LogTemp, Warning, TEXT("FICSIT Advanced Alloy Foundry: mPowerConsumption property was not found."));

    FFGClearanceData Clearance;
    Clearance.Type = EClearanceType::CT_Default;
    Clearance.ClearanceBox = FBox(FVector(-225.0f, -165.0f, 0.0f), FVector(225.0f, 165.0f, 315.0f));
    Clearance.RelativeTransform = FTransform::Identity;
    Clearance.ExcludeForSnapping = false;
    mClearanceData.Add(Clearance);

    SolidCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("AdvancedAlloyFoundryCollision"));
    SolidCollision->SetupAttachment(RootComponent);
    SolidCollision->SetBoxExtent(FVector(205.0f, 145.0f, 140.0f));
    SolidCollision->SetRelativeLocation(FVector(0.0f, 0.0f, 140.0f));
    SolidCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    SolidCollision->SetCollisionObjectType(ECC_WorldStatic);
    SolidCollision->SetCollisionResponseToAllChannels(ECR_Block);
    SolidCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
    SolidCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
    SolidCollision->SetGenerateOverlapEvents(false);

    VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AdvancedAlloyFoundryVisual"));
    VisualMesh->SetupAttachment(RootComponent);
    VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    VisualMesh->SetCastShadow(true);
    if (UStaticMesh* Mesh = LoadObject<UStaticMesh>(nullptr, AdvancedAlloyFoundryMeshPath))
    {
        VisualMesh->SetStaticMesh(Mesh);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("FICSIT Advanced Alloy Foundry: model not imported; run BuildFerraviumAssets.py."));
        if (UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
        {
            VisualMesh->SetStaticMesh(Cube);
            VisualMesh->SetRelativeScale3D(FVector(4.0f, 2.8f, 2.8f));
        }
    }

    InputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Input0"));
    InputConnection->SetupAttachment(RootComponent);
    InputConnection->SetRelativeLocation(FVector(-215.0f, 0.0f, 105.0f));
    InputConnection->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
    InputConnection->SetDirection(EFactoryConnectionDirection::FCD_INPUT);
    InputConnection->SetConnectorClearance(105.0f);
    InputConnection->SetForwardPeekAndGrabToBuildable(true);

    OutputConnection = CreateDefaultSubobject<UFGFactoryConnectionComponent>(TEXT("Output0"));
    OutputConnection->SetupAttachment(RootComponent);
    OutputConnection->SetRelativeLocation(FVector(215.0f, 0.0f, 105.0f));
    OutputConnection->SetRelativeRotation(FRotator::ZeroRotator);
    OutputConnection->SetDirection(EFactoryConnectionDirection::FCD_OUTPUT);
    OutputConnection->SetConnectorClearance(105.0f);
    OutputConnection->SetForwardPeekAndGrabToBuildable(true);
}

UFICSITAdvancedAlloyFoundryDescriptor::UFICSITAdvancedAlloyFoundryDescriptor()
{
    mBuildableClass = AFICSITAdvancedAlloyFoundry::StaticClass();
    mUseDisplayNameAndDescription = false;
    mMenuPriority = 180.0f;
    mCategory = LoadClass<UFGBuildCategory>(nullptr,
        TEXT("/Game/FactoryGame/Interface/UI/InGame/BuildMenu/BuildCategories/BC_Production.BC_Production_C"));
    if (UTexture2D* Icon = LoadFerraviumIcon(TEXT("TX_AdvancedAlloyFoundry_256")))
    {
        mSmallIcon = Icon;
        mPersistentBigIcon = Icon;
    }
}

UFICSITAdvancedAlloyFoundryBuildRecipe::UFICSITAdvancedAlloyFoundryBuildRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Construct Advanced Alloy Foundry"));
    mManufactoringDuration = 0.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITAdvancedAlloyFoundryDescriptor::StaticClass(), 1);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFGBuildGun::StaticClass()));

    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/EncasedIndustrialBeam/Desc_EncasedIndustrialBeam.Desc_EncasedIndustrialBeam_C"), 50);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C"), 120);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C"), 25);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C"), 30);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"), 20);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C"), 200);
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

UFICSITTitanAlloyRecipe::UFICSITTitanAlloyRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Titan Alloy Ingot"));
    mManufactoringDuration = 8.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITTitanAlloyIngotDescriptor::StaticClass(), 60);

    AddItem(mIngredients, TEXT("/Script/FICSITContracts.FICSITFerraviumIngotDescriptor"), 60);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/SteelIngot/Desc_SteelIngot.Desc_SteelIngot_C"), 60);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFICSITAdvancedAlloyFoundry::StaticClass()));
}

UFICSITTitanAlloyBeamRecipe::UFICSITTitanAlloyBeamRecipe()
{
    mDisplayNameOverride = true;
    mDisplayName = FText::FromString(TEXT("Titan Alloy Beam"));
    mManufactoringDuration = 12.0f;
    mManualManufacturingMultiplier = 1.0f;
    mProduct.Emplace(UFICSITTitanAlloyBeamDescriptor::StaticClass(), 30);

    AddItem(mIngredients, TEXT("/Script/FICSITContracts.FICSITTitanAlloyIngotDescriptor"), 30);
    AddItem(mIngredients, TEXT("/Game/FactoryGame/Resource/Parts/SteelPipe/Desc_SteelPipe.Desc_SteelPipe_C"), 60);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFICSITAdvancedAlloyFoundry::StaticClass()));
}


UFICSITFerraviumRecipeUnlock::UFICSITFerraviumRecipeUnlock()
{
    mRecipes.Add(UFICSITFerraviumRecoveryRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumConcentrateRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumSmeltingRecipe::StaticClass());
    mRecipes.Add(UFICSITFerraviumSmelterBuildRecipe::StaticClass());
}

UFICSITTitanAlloyRecipeUnlock::UFICSITTitanAlloyRecipeUnlock()
{
    mRecipes.Add(UFICSITTitanAlloyRecipe::StaticClass());
    mRecipes.Add(UFICSITTitanAlloyBeamRecipe::StaticClass());
    mRecipes.Add(UFICSITAdvancedAlloyFoundryBuildRecipe::StaticClass());
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

UFICSITAdvancedMetallurgyPrerequisite::UFICSITAdvancedMetallurgyPrerequisite()
{
    mSchematics.Add(UFICSITFrontierSurveySchematic::StaticClass());
    mRequireAllSchematicsToBePurchased = true;
}

bool UFICSITAdvancedMetallurgyPrerequisite::AreDependenciesMet(UObject* WorldContext) const
{
    if (!Super::AreDependenciesMet(WorldContext) || !WorldContext) return false;

    UWorld* World = WorldContext->GetWorld();
    USubsystemActorManager* Manager = World ? World->GetSubsystem<USubsystemActorManager>() : nullptr;
    AFICSITContractsSubsystem* Contracts = Manager
        ? Manager->GetSubsystemActor<AFICSITContractsSubsystem>() : nullptr;

    return Contracts && Contracts->GetAvailableTerminalResearchPhase() == 2;
}

UFICSITAdvancedMetallurgySchematic::UFICSITAdvancedMetallurgySchematic()
{
    mType = ESchematicType::EST_Milestone;
    mDisplayName = FText::FromString(TEXT("Advanced Metallurgy // Titan Alloy"));
    mDescription = FText::FromString(TEXT("Unlocks the Advanced Alloy Foundry build recipe and two Titan Alloy production recipes. Combines Ferravium and Steel into high-strength ingots and structural beams."));
    mSchematicCategory = LoadClass<UFGSchematicCategory>(
        nullptr, TEXT("/Game/FactoryGame/Schematics/SchematicCategories/SC_Logistics.SC_Logistics_C"));
    mMenuPriority = 179.0f;
    mTechTier = 10;
    mTimeToComplete = 0.0f;
    mIsPlayerSpecific = false;
    mDependenciesBlocksSchematicAccess = true;
    mHiddenUntilDependenciesMet = true;
    mSchematicDependencies.Add(
        CreateDefaultSubobject<UFICSITAdvancedMetallurgyPrerequisite>(TEXT("RequiresFrontierSurvey")));

    AddItem(mCost, TEXT("/Script/FICSITContracts.FICSITFerraviumIngotDescriptor"), 200);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/SteelIngot/Desc_SteelIngot.Desc_SteelIngot_C"), 400);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C"), 40);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"), 25);
    AddItem(mCost, TEXT("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C"), 25);

    if (UTexture2D* Icon = LoadFerraviumIcon(TEXT("TX_TitanAlloyIngot_256")))
    {
        mSmallSchematicIcon = Icon;
        mSchematicIcon.SetResourceObject(Icon);
    }

    mUnlocks.Add(CreateDefaultSubobject<UFICSITTitanAlloyRecipeUnlock>(TEXT("UnlockTitanAlloyRecipe")));
    MarkPublic(this);
}
