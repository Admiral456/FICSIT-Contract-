#include "FICSITContractsRemoteCallObject.h"
#include "FICSITContractsSubsystem.h"
#include "FICSITDispatchTerminal.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"

void UFICSITContractsRemoteCallObject::GetLifetimeReplicatedProps(
    TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UFICSITContractsRemoteCallObject,mForceNetField_FICSITContractsRCO);
}

bool UFICSITContractsRemoteCallObject::ResolveAuthorizedRequest(
    AFGCharacterPlayer*& OutPlayer, AFGPlayerController*& OutController) const
{
    OutPlayer = GetOwnerPlayerCharacter();
    OutController = GetOwnerPlayerController();
    if (!IsValid(OutPlayer) || !IsValid(OutController) ||
        !OutPlayer->HasAuthority() || !OutController->HasAuthority())
    {
        return false;
    }
    return OutPlayer->GetController() == OutController;
}

bool UFICSITContractsRemoteCallObject::IsValidTerminalRequest(
    const AFGCharacterPlayer* Player, const AFICSITDispatchTerminal* Terminal) const
{
    // Never trust a client-provided buildable pointer without checking both
    // its lifetime and the requester's physical proximity on the server.
    constexpr float MaximumUseDistance = 450.0f;
    return IsValid(Player) && IsValid(Terminal) &&
        !Terminal->IsActorBeingDestroyed() &&
        Player->GetDistanceTo(Terminal) <= MaximumUseDistance;
}

AFICSITContractsSubsystem* UFICSITContractsRemoteCallObject::FindAuthoritativeBoard() const
{
    UWorld* World = GetWorld();
    if (!World) return nullptr;
    for (TActorIterator<AFICSITContractsSubsystem> It(World); It; ++It)
        if (It->HasAuthority()) return *It;
    return nullptr;
}

void UFICSITContractsRemoteCallObject::Server_AcceptContract_Implementation(
    AFICSITDispatchTerminal* Terminal, FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player,Controller) ||
        !IsValidTerminalRequest(Player,Terminal) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->AcceptContract(ContractId,Message);
    Controller->ClientMessage(Message);
}

void UFICSITContractsRemoteCallObject::Server_DeliverContract_Implementation(
    AFICSITDispatchTerminal* Terminal, FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player,Controller) ||
        !IsValidTerminalRequest(Player,Terminal) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->DeliverFromInventory(Player,ContractId,Message);
    Controller->ClientMessage(Message);
}

void UFICSITContractsRemoteCallObject::Server_CancelContract_Implementation(
    AFICSITDispatchTerminal* Terminal, FName ContractId)
{
    AFGCharacterPlayer* Player = nullptr;
    AFGPlayerController* Controller = nullptr;
    if (!ResolveAuthorizedRequest(Player,Controller) ||
        !IsValidTerminalRequest(Player,Terminal) || ContractId.IsNone()) return;
    AFICSITContractsSubsystem* Board = FindAuthoritativeBoard();
    if (!Board) return;
    FString Message;
    Board->AbandonContract(ContractId,Message);
    Controller->ClientMessage(Message);
}
