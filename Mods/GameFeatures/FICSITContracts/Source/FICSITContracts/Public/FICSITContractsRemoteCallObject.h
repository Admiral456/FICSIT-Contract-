#pragma once
#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "FICSITContractsRemoteCallObject.generated.h"

/**
 * Client-to-server bridge for terminal actions. The RCO is owned by the
 * requesting player's controller; every request is revalidated on the server.
 */
UCLASS()
class FICSITCONTRACTS_API UFICSITContractsRemoteCallObject : public UFGRemoteCallObject
{
    GENERATED_BODY()
public:
    UFUNCTION(Server, Reliable)
    void Server_AcceptContract(FName ContractId);

    UFUNCTION(Server, Reliable)
    void Server_DeliverContract(FName ContractId);

    UFUNCTION(Server, Reliable)
    void Server_CancelContract(FName ContractId);

private:
    bool ResolveAuthorizedRequest(class AFGCharacterPlayer*& OutPlayer,
        class AFGPlayerController*& OutController) const;
    class AFICSITContractsSubsystem* FindAuthoritativeBoard() const;
};
