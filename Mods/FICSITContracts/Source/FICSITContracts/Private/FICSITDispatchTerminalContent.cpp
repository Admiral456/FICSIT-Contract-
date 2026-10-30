#include "FICSITDispatchTerminalContent.h"
#include "FICSITDispatchTerminal.h"
#include "Equipment/FGBuildGun.h"
#include "FGBuildCategory.h"
#include "ItemAmount.h"
#include "Resources/FGItemDescriptor.h"
#include "FGSchematicCategory.h"
#include "Engine/Texture2D.h"
namespace
{
    TSubclassOf<UFGItemDescriptor> LoadDispatchItem(const TCHAR* Path) { return LoadClass<UFGItemDescriptor>(nullptr,Path); }
    void AddCost(TArray<FItemAmount>& Costs,const TCHAR* Path,int32 Amount)
    {
        const TSubclassOf<UFGItemDescriptor> Item=LoadDispatchItem(Path);
        if(Item) Costs.Emplace(Item,Amount);
        else UE_LOG(LogTemp,Warning,TEXT("FICSIT Contracts: could not resolve terminal cost item %s"),Path);
    }
    UTexture2D* LoadTerminalIcon()
    {
        return LoadObject<UTexture2D>(nullptr,TEXT("/FICSITContracts/UI/TX_DispatchTerminal_256.TX_DispatchTerminal_256"));
    }
}
UFICSITDispatchTerminalDescriptor::UFICSITDispatchTerminalDescriptor()
{
    mBuildableClass=AFICSITDispatchTerminal::StaticClass();
    mUseDisplayNameAndDescription=false;
    mMenuPriority=145.0f;
    mCategory=LoadClass<UFGBuildCategory>(nullptr,TEXT("/Game/FactoryGame/Interface/UI/InGame/BuildMenu/BuildCategories/BC_TradingPost.BC_TradingPost_C"));
    mSubCategories.Add(LoadClass<UFGBuildCategory>(nullptr,TEXT("/Game/FactoryGame/Interface/UI/InGame/BuildMenu/BuildCategories/Sub_Special/SC_Progression.SC_Progression_C")));
    if(UTexture2D* Icon=LoadTerminalIcon()){mSmallIcon=Icon;mPersistentBigIcon=Icon;}
}
UFICSITDispatchTerminalRecipe::UFICSITDispatchTerminalRecipe()
{
    mDisplayNameOverride=true;
    mDisplayName=FText::FromString(TEXT("Construct FICSIT Dispatch Terminal"));
    mManufactoringDuration=0.0f;
    mManualManufacturingMultiplier=1.0f;
    mProduct.Emplace(UFICSITDispatchTerminalDescriptor::StaticClass(),1);
    mProducedIn.Add(TSoftClassPtr<UObject>(AFGBuildGun::StaticClass()));
    AddCost(mIngredients,TEXT("/Game/FactoryGame/Resource/Parts/ModularFrameHeavy/Desc_ModularFrameHeavy.Desc_ModularFrameHeavy_C"),20);
    AddCost(mIngredients,TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"),20);
    AddCost(mIngredients,TEXT("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C"),50);
    AddCost(mIngredients,TEXT("/Game/FactoryGame/Resource/Parts/IronPlateReinforced/Desc_IronPlateReinforced.Desc_IronPlateReinforced_C"),40);
    AddCost(mIngredients,TEXT("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C"),10);
}
UFICSITDispatchTerminalRecipeUnlock::UFICSITDispatchTerminalRecipeUnlock()
{ mRecipes.Add(UFICSITDispatchTerminalRecipe::StaticClass()); }
UFICSITDispatchTerminalSchematic::UFICSITDispatchTerminalSchematic()
{
    mType=ESchematicType::EST_Milestone;
    mDisplayName=FText::FromString(TEXT("FICSIT Dispatch Network"));
    mDescription=FText::FromString(TEXT("Establish a dedicated logistics dispatch point. Unlocks the FICSIT Dispatch Terminal, a centralized interface for production contracts, delivery tracking and factory reputation."));
    mSchematicCategory=LoadClass<UFGSchematicCategory>(nullptr,TEXT("/Game/FactoryGame/Schematics/SchematicCategories/SC_Logistics.SC_Logistics_C"));
    mMenuPriority=145.0f;mTechTier=5;mTimeToComplete=0.0f;mIsPlayerSpecific=false;
    AddCost(mCost,TEXT("/Game/FactoryGame/Resource/Parts/ModularFrame/Desc_ModularFrame.Desc_ModularFrame_C"),25);
    AddCost(mCost,TEXT("/Game/FactoryGame/Resource/Parts/Computer/Desc_Computer.Desc_Computer_C"),10);
    AddCost(mCost,TEXT("/Game/FactoryGame/Resource/Parts/Cable/Desc_Cable.Desc_Cable_C"),100);
    AddCost(mCost,TEXT("/Game/FactoryGame/Resource/Parts/Motor/Desc_Motor.Desc_Motor_C"),10);
    if(UTexture2D* Icon=LoadTerminalIcon()){mSmallSchematicIcon=Icon;mSchematicIcon.SetResourceObject(Icon);}
    mUnlocks.Add(CreateDefaultSubobject<UFICSITDispatchTerminalRecipeUnlock>(TEXT("UnlockDispatchTerminalRecipe")));
}