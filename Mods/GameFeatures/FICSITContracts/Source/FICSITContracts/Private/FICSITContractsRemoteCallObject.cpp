#include "FICSITContractsRemoteCallObject.h"
#include "FICSITContractsSubsystem.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"

bool UFICSITContractsRemoteCallObject::ResolveAuthorizedRequest(
    AFGCharacterPlayer*& OutPlayer, AFGPlayerController*& OutController) const
{
    OutPlayer = GetOwnerPlayerCharacter();
    OutController = GetOwnerPlayerController();
    if (!IsValid(OutPlayer) || !IsValid(OutController) || !OutPlayer->HasAuthority() || !OutController->HasAuthority())
        return false;
    if (OutPlayer->GetController() != OutController)
        return false;
    return true;
}

AFICSITContractsSubsystem* UFICSITContractsRemoteCallObject::FindAuthoritativeBoard() const
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;
    for (TActorIterator<AFICSITContractsSubsystem> It(World); It; ++It)
        if (It->HasAuthority()) return *It;
    return nullptr;
}

void UFICSITContractsRemoteCallObject::Server_AcceptContract_Implementation(FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player, Controller) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->AcceptContract(ContractId, Message);
    Controller->ClientMessage(Message);
}

void UFICSITContractsRemoteCallObject::Server_DeliverContract_Implementation(FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player, Controller) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->DeliverFromInventory(Player, ContractId, Message);
    Controller->ClientMessage(Message);
}

void UFICSITContractsRemoteCallObject::Server_CancelContract_Implementation(FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player, Controller) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->AbandonContract(ContractId, Message);
    Controller->ClientMessage(Message);
}
