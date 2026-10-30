#pragma once

#include "CoreMinimal.h"
#include "Command/ChatCommandInstance.h"
#include "FICSITContractsChatCommand.generated.h"

UCLASS()
class FICSITCONTRACTS_API AFICSITContractsChatCommand : public AChatCommandInstance
{
    GENERATED_BODY()
public:
    AFICSITContractsChatCommand();
    virtual EExecutionStatus ExecuteCommand_Implementation(
        class UCommandSender* Sender,
        const TArray<FString>& Arguments,
        const FString& Label) override;
};
