#pragma once
#include "CoreMinimal.h"
#include "Resources/FGBuildingDescriptor.h"
#include "FGRecipe.h"
#include "FGSchematic.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "FICSITDispatchTerminalContent.generated.h"
UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalDescriptor : public UFGBuildingDescriptor
{ GENERATED_BODY() public: UFICSITDispatchTerminalDescriptor(); };
UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalRecipe : public UFGRecipe
{ GENERATED_BODY() public: UFICSITDispatchTerminalRecipe(); };
UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalRecipeUnlock : public UFGUnlockRecipe
{ GENERATED_BODY() public: UFICSITDispatchTerminalRecipeUnlock(); };
UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalSchematic : public UFGSchematic
{ GENERATED_BODY() public: UFICSITDispatchTerminalSchematic(); };