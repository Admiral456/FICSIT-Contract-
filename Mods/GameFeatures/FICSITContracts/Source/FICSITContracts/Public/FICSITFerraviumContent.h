#pragma once

#include "CoreMinimal.h"
#include "Resources/FGResourceDescriptor.h"
#include "Resources/FGItemDescriptor.h"
#include "Resources/FGBuildingDescriptor.h"
#include "FGRecipe.h"
#include "FGSchematic.h"
#include "Unlocks/FGUnlockRecipe.h"
#include "AvailabilityDependencies/FGSchematicPurchasedDependency.h"
#include "Buildables/FGBuildableManufacturer.h"
#include "FICSITFerraviumContent.generated.h"

class UFGFactoryConnectionComponent;
class UStaticMeshComponent;
class UBoxComponent;

/** Raw Ferravium resource item. Node placement is handled by the separate world-resource integration. */
UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumOreDescriptor : public UFGResourceDescriptor
{
    GENERATED_BODY()
public:
    UFICSITFerraviumOreDescriptor();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumConcentrateDescriptor : public UFGItemDescriptor
{
    GENERATED_BODY()
public:
    UFICSITFerraviumConcentrateDescriptor();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumIngotDescriptor : public UFGItemDescriptor
{
    GENERATED_BODY()
public:
    UFICSITFerraviumIngotDescriptor();
};

/** Native manufacturer for the first Ferravium processing line. */
UCLASS()
class FICSITCONTRACTS_API AFICSITFerraviumSmelter : public AFGBuildableManufacturer
{
    GENERATED_BODY()
public:
    AFICSITFerraviumSmelter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
    UPROPERTY()
    TObjectPtr<UStaticMeshComponent> VisualMesh;

    UPROPERTY()
    TObjectPtr<UBoxComponent> SolidCollision;

    UPROPERTY()
    TObjectPtr<UFGFactoryConnectionComponent> InputConnection;

    UPROPERTY()
    TObjectPtr<UFGFactoryConnectionComponent> OutputConnection;
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumSmelterDescriptor : public UFGBuildingDescriptor
{
    GENERATED_BODY()
public:
    UFICSITFerraviumSmelterDescriptor();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumSmelterBuildRecipe : public UFGRecipe
{
    GENERATED_BODY()
public:
    UFICSITFerraviumSmelterBuildRecipe();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumRecoveryRecipe : public UFGRecipe
{
    GENERATED_BODY()
public:
    UFICSITFerraviumRecoveryRecipe();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumConcentrateRecipe : public UFGRecipe
{
    GENERATED_BODY()
public:
    UFICSITFerraviumConcentrateRecipe();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumSmeltingRecipe : public UFGRecipe
{
    GENERATED_BODY()
public:
    UFICSITFerraviumSmeltingRecipe();
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFerraviumRecipeUnlock : public UFGUnlockRecipe
{
    GENERATED_BODY()
public:
    UFICSITFerraviumRecipeUnlock();
};

/** Keeps Frontier Survey hidden until the actual Phase V callback has opened Terminal I. */
UCLASS()
class FICSITCONTRACTS_API UFICSITFrontierSurveyPrerequisite : public UFGSchematicPurchasedDependency
{
    GENERATED_BODY()
public:
    UFICSITFrontierSurveyPrerequisite();
    virtual bool AreDependenciesMet(UObject* WorldContext) const override;
};

UCLASS()
class FICSITCONTRACTS_API UFICSITFrontierSurveySchematic : public UFGSchematic
{
    GENERATED_BODY()
public:
    UFICSITFrontierSurveySchematic();
};
