#include "FICSITContractsChatCommand.h"

#include "FICSITContractsSubsystem.h"
#include "Command/CommandSender.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"
#include "Subsystem/SubsystemActorManager.h"
#include "Engine/World.h"

AFICSITContractsChatCommand::AFICSITContractsChatCommand()
{
    bOnlyUsableByPlayer = true;
    MinNumberOfArguments = 0;
    CommandName = TEXT("contracts");
    Aliases.Add(TEXT("fcontracts"));
    Usage = FText::FromString(TEXT("/contracts [accept <ID> | deliver <ID> | history]"));
}

EExecutionStatus AFICSITContractsChatCommand::ExecuteCommand_Implementation(UCommandSender* Sender, const TArray<FString>& Arguments, const FString& Label)
{
    if (!Sender) return EExecutionStatus::UNCOMPLETED;
    USubsystemActorManager* Manager = GetWorld() ? GetWorld()->GetSubsystem<USubsystemActorManager>() : nullptr;
    AFICSITContractsSubsystem* Contracts = Manager ? Manager->GetSubsystemActor<AFICSITContractsSubsystem>() : nullptr;
    if (!Contracts)
    {
        Sender->SendChatMessage(TEXT("FICSIT Contracts is not ready in this world."), FLinearColor::Red);
        return EExecutionStatus::UNCOMPLETED;
    }

    if (Arguments.Num() == 0)
    {
        Sender->SendChatMessage(FString::Printf(TEXT("FICSIT CONTRACTS | %d REP | %s | active %d/3"), Contracts->GetReputation(), *Contracts->GetRankName(), Contracts->GetActiveContracts().Num()), FLinearColor(0.95f, 0.72f, 0.24f));
        const TArray<FFICSITContract> Offers = Contracts->GetAvailableContracts();
        Sender->SendChatMessage(TEXT("AVAILABLE CONTRACTS"), FLinearColor::Green);
        if (Offers.Num() == 0) Sender->SendChatMessage(TEXT("No offers are available right now."));
        for (const FFICSITContract& Contract : Offers) Sender->SendChatMessage(Contracts->FormatContract(Contract));
        const TArray<FFICSITContract> Active = Contracts->GetActiveContracts();
        Sender->SendChatMessage(TEXT("ACTIVE CONTRACTS"), FLinearColor::Green);
        if (Active.Num() == 0) Sender->SendChatMessage(TEXT("No active contracts. Accept one with /contracts accept <ID>."));
        for (const FFICSITContract& Contract : Active) Sender->SendChatMessage(Contracts->FormatContract(Contract));
        Sender->SendChatMessage(TEXT("Commands: /contracts accept <ID> | /contracts deliver <ID> | /contracts history"));
        return EExecutionStatus::COMPLETED;
    }

    const FString Subcommand = Arguments[0].ToLower();
    if (Subcommand == TEXT("history"))
    {
        const TArray<FFICSITContract> History = Contracts->GetContractHistory();
        Sender->SendChatMessage(TEXT("RECENT CONTRACT HISTORY"), FLinearColor::Green);
        if (History.Num() == 0) Sender->SendChatMessage(TEXT("No completed or expired contracts yet."));
        const int32 FirstIndex = FMath::Max(0, History.Num() - 10);
        for (int32 Index = FirstIndex; Index < History.Num(); ++Index)
        {
            const FFICSITContract& Contract = History[Index];
            const FString Status = Contract.bCompleted ? TEXT("COMPLETED") : TEXT("EXPIRED");
            Sender->SendChatMessage(FString::Printf(TEXT("%s | %s | %s"), *Status, *Contract.ContractId.ToString(), *Contract.Title));
        }
        return EExecutionStatus::COMPLETED;
    }

    if (Arguments.Num() < 2)
    {
        Sender->SendChatMessage(TEXT("Usage: /contracts accept <ID> | /contracts deliver <ID>"), FLinearColor::Red);
        return EExecutionStatus::BAD_ARGUMENTS;
    }
    const FName ContractId(*Arguments[1]);
    if (Subcommand == TEXT("accept"))
    {
        FString Message;
        const bool bAccepted = Contracts->AcceptContract(ContractId, Message);
        Sender->SendChatMessage(Message, bAccepted ? FLinearColor::Green : FLinearColor::Red);
        return bAccepted ? EExecutionStatus::COMPLETED : EExecutionStatus::UNCOMPLETED;
    }
    if (Subcommand == TEXT("deliver"))
    {
        if (!Sender->IsPlayerSender())
        {
            Sender->SendChatMessage(TEXT("Only a player can deliver contract items."), FLinearColor::Red);
            return EExecutionStatus::INSUFFICIENT_PERMISSIONS;
        }
        AFGPlayerController* PlayerController = Sender->GetPlayer();
        AFGCharacterPlayer* PlayerCharacter = PlayerController ? Cast<AFGCharacterPlayer>(PlayerController->GetPawn()) : nullptr;
        FString Message;
        const int32 Delivered = Contracts->DeliverFromInventory(PlayerCharacter, ContractId, Message);
        Sender->SendChatMessage(Message, Delivered > 0 ? FLinearColor::Green : FLinearColor::Red);
        return Delivered > 0 ? EExecutionStatus::COMPLETED : EExecutionStatus::UNCOMPLETED;
    }
    Sender->SendChatMessage(TEXT("Unknown subcommand. Use /contracts for the contract board."), FLinearColor::Red);
    return EExecutionStatus::BAD_ARGUMENTS;
}
