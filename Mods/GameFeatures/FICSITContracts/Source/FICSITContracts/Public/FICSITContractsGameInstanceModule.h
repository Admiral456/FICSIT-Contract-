#pragma once
#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "FICSITContractsGameInstanceModule.generated.h"

/** Registers the owner-bound RCO used by terminal buttons. */
UCLASS()
class FICSITCONTRACTS_API UFICSITContractsGameInstanceModule : public UGameInstanceModule
{
    GENERATED_BODY()
public:
    UFICSITContractsGameInstanceModule();
};
