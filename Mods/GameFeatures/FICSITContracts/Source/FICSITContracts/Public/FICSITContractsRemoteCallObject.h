#pragma once

#include "CoreMinimal.h"
#include "FGRemoteCallObject.h"
#include "FICSITContractsRemoteCallObject.generated.h"

class AFICSITDispatchTerminal;

/**
 * Client-to-server bridge for terminal actions. The RCO is owned by the
 * requesting player's controller; every request is revalidated on the server.
 */
UCLASS()
class FICSITCONTRACTS_API UFICSITContractsRemoteCallObject : public UFGRemoteCallObject
{
    GENERATED_BODY()
public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(Server, Reliable)
    void Server_AcceptContract(AFICSITDispatchTerminal* Terminal, FName ContractId);

    UFUNCTION(Server, Reliable)
    void Server_DeliverContract(AFICSITDispatchTerminal* Terminal, FName ContractId);

    UFUNCTION(Server, Reliable)
    void Server_CancelContract(AFICSITDispatchTerminal* Terminal, FName ContractId);

private:
    UPROPERTY(Replicated, Meta = (NoAutoJson))
    bool mForceNetField_FICSITContractsRCO = false;

    bool ResolveAuthorizedRequest(class AFGCharacterPlayer*& OutPlayer,
        class AFGPlayerController*& OutController) const;
    bool IsValidTerminalRequest(const class AFGCharacterPlayer* Player,
        const AFICSITDispatchTerminal* Terminal) const;
    class AFICSITContractsSubsystem* FindAuthoritativeBoard() const;
};
