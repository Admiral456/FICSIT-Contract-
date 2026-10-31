#include "FICSITContractsSubsystem.h"
#include "EndgameProgression.h"

#include "FGCharacterPlayer.h"
#include "FGInventoryComponent.h"
#include "Resources/FGItemDescriptor.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include <algorithm>

namespace
{
    FString FromUtf8(const std::string& Value)
    {
        return UTF8_TO_TCHAR(Value.c_str());
    }

    std::string ToUtf8(const FString& Value)
    {
        FTCHARToUTF8 Converted(*Value);
        return std::string(Converted.Get(), Converted.Length());
    }

    int64 GetUtcUnixTime()
    {
        return FDateTime::UtcNow().ToUnixTimestamp();
    }

    FicsitContracts::Contract ToCoreContract(const FFICSITContract& Source)
    {
        FicsitContracts::Contract Result;
        Result.id = ToUtf8(Source.ContractId.ToString());
        Result.templateId = ToUtf8(Source.TemplateId);
        Result.title = ToUtf8(Source.Title);
        Result.difficulty = static_cast<FicsitContracts::Difficulty>(Source.Difficulty);
        Result.rewardReputation = Source.RewardReputation;
        Result.timeLimited = Source.bTimeLimited;
        Result.expiresAtUnix = Source.ExpiresAtUnix;
        Result.longTerm = Source.bLongTerm;
        Result.completed = Source.bCompleted;
        Result.expired = Source.bExpired;
        Result.abandoned = Source.bAbandoned;
        Result.finishedAtUnix = Source.FinishedAtUnix;
        Result.currentStageIndex = static_cast<std::size_t>(FMath::Max(0, Source.CurrentStageIndex));
        for (const FFICSITContractRequirement& Requirement : Source.Requirements)
            Result.requirements.push_back({ToUtf8(Requirement.ItemClassPath), ToUtf8(Requirement.DisplayName), Requirement.RequiredAmount, Requirement.DeliveredAmount});
        for (const FFICSITContractStage& SourceStage : Source.Stages)
        {
            FicsitContracts::ContractStage Stage;
            Stage.title = ToUtf8(SourceStage.StageTitle);
            for (const FFICSITContractRequirement& Requirement : SourceStage.Requirements)
                Stage.requirements.push_back({ToUtf8(Requirement.ItemClassPath), ToUtf8(Requirement.DisplayName), Requirement.RequiredAmount, Requirement.DeliveredAmount});
            Result.stages.push_back(MoveTemp(Stage));
        }
        return Result;
    }

    FFICSITContract ToUnrealContract(const FicsitContracts::Contract& Source)
    {
        FFICSITContract Result;
        Result.ContractId = FName(*FromUtf8(Source.id));
        Result.TemplateId = FromUtf8(Source.templateId);
        Result.Title = FromUtf8(Source.title);
        Result.Difficulty = static_cast<EFICSITContractDifficulty>(Source.difficulty);
        Result.RewardReputation = Source.rewardReputation;
        Result.bTimeLimited = Source.timeLimited;
        Result.ExpiresAtUnix = Source.expiresAtUnix;
        Result.bLongTerm = Source.longTerm;
        Result.bCompleted = Source.completed;
        Result.bExpired = Source.expired;
        Result.bAbandoned = Source.abandoned;
        Result.FinishedAtUnix = Source.finishedAtUnix;
        Result.CurrentStageIndex = static_cast<int32>(Source.currentStageIndex);
        for (const FicsitContracts::Requirement& Requirement : Source.requirements)
        {
            FFICSITContractRequirement UnrealRequirement;
            UnrealRequirement.ItemClassPath = FromUtf8(Requirement.itemClassPath);
            UnrealRequirement.DisplayName = FromUtf8(Requirement.displayName);
            UnrealRequirement.RequiredAmount = Requirement.requiredAmount;
            UnrealRequirement.DeliveredAmount = Requirement.deliveredAmount;
            Result.Requirements.Add(MoveTemp(UnrealRequirement));
        }
        for (const FicsitContracts::ContractStage& SourceStage : Source.stages)
        {
            FFICSITContractStage UnrealStage;
            UnrealStage.StageTitle = FromUtf8(SourceStage.title);
            for (const FicsitContracts::Requirement& Requirement : SourceStage.requirements)
            {
                FFICSITContractRequirement UnrealRequirement;
                UnrealRequirement.ItemClassPath = FromUtf8(Requirement.itemClassPath);
                UnrealRequirement.DisplayName = FromUtf8(Requirement.displayName);
                UnrealRequirement.RequiredAmount = Requirement.requiredAmount;
                UnrealRequirement.DeliveredAmount = Requirement.deliveredAmount;
                UnrealStage.Requirements.Add(MoveTemp(UnrealRequirement));
            }
            Result.Stages.Add(MoveTemp(UnrealStage));
        }
        return Result;
    }

    const TCHAR* DifficultyLabel(EFICSITContractDifficulty Difficulty)
    {
        switch (Difficulty)
        {
        case EFICSITContractDifficulty::Small: return TEXT("Small");
        case EFICSITContractDifficulty::Medium: return TEXT("Medium");
        case EFICSITContractDifficulty::Large: return TEXT("Large");
        case EFICSITContractDifficulty::Special: return TEXT("Special");
        default: return TEXT("Unknown");
        }
    }
}

int32 FFICSITContract::GetProgressPercent() const
{
    const auto GetRequirementsPercent = [](const TArray<FFICSITContractRequirement>& Items)
    {
        int64 Required = 0;
        int64 Delivered = 0;
        for (const FFICSITContractRequirement& Requirement : Items)
        {
            Required += FMath::Max(0, Requirement.RequiredAmount);
            Delivered += FMath::Min(FMath::Max(0, Requirement.DeliveredAmount), FMath::Max(0, Requirement.RequiredAmount));
        }
        return Required > 0 ? static_cast<int32>((Delivered * 100) / Required) : 0;
    };

    if (Stages.Num() == 0) return GetRequirementsPercent(Requirements);
    int64 StagePercentTotal = 0;
    for (const FFICSITContractStage& Stage : Stages)
        StagePercentTotal += GetRequirementsPercent(Stage.Requirements);
    return static_cast<int32>(StagePercentTotal / Stages.Num());
}

AFICSITContractsSubsystem::AFICSITContractsSubsystem()
{
    ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
    TerminalResearchPhasesComplete.Init(0, 6);
    ElevatorExtensionsComplete.Init(0, 3);
}

void AFICSITContractsSubsystem::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
        // Normalize older or malformed save data before exposing progression to clients.
        ApplyEndgameProgression(BuildEndgameProgression());
        GenerateInitialBoard();
        GetWorldTimerManager().SetTimer(ExpiryTimerHandle, this,
            &AFICSITContractsSubsystem::RefreshExpiredContracts, 15.0f, true);
    }
}

void AFICSITContractsSubsystem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AFICSITContractsSubsystem, Reputation);
    DOREPLIFETIME(AFICSITContractsSubsystem, NextSerial);
    DOREPLIFETIME(AFICSITContractsSubsystem, AvailableContracts);
    DOREPLIFETIME(AFICSITContractsSubsystem, ActiveContracts);
    DOREPLIFETIME(AFICSITContractsSubsystem, ContractHistory);
    DOREPLIFETIME(AFICSITContractsSubsystem, bVanillaSpaceElevatorPhase5Complete);
    DOREPLIFETIME(AFICSITContractsSubsystem, TerminalResearchPhasesComplete);
    DOREPLIFETIME(AFICSITContractsSubsystem, ElevatorExtensionsComplete);
}

void AFICSITContractsSubsystem::OnRep_ContractBoard()
{
    OnContractBoardChanged.Broadcast();
}

void AFICSITContractsSubsystem::GatherDependencies_Implementation(TArray<UObject*>& OutDependentObjects)
{
    // Persisted contract data is value data; no UObject dependencies need ordering.
}

FicsitContracts::Snapshot AFICSITContractsSubsystem::BuildSnapshot() const
{
    FicsitContracts::Snapshot Result;
    Result.reputation = Reputation;
    Result.nextSerial = NextSerial > 0 ? static_cast<std::uint64_t>(NextSerial) : 1;
    for (const FFICSITContract& Contract : AvailableContracts) Result.offers.push_back(ToCoreContract(Contract));
    for (const FFICSITContract& Contract : ActiveContracts) Result.active.push_back(ToCoreContract(Contract));
    for (const FFICSITContract& Contract : ContractHistory) Result.history.push_back(ToCoreContract(Contract));
    return Result;
}

void AFICSITContractsSubsystem::ApplySnapshot(const FicsitContracts::Snapshot& Snapshot)
{
    Reputation = FMath::Max(0, Snapshot.reputation);
    NextSerial = Snapshot.nextSerial > 0 ? static_cast<int64>(Snapshot.nextSerial) : 1;
    AvailableContracts.Reset(); ActiveContracts.Reset(); ContractHistory.Reset();
    for (const FicsitContracts::Contract& Contract : Snapshot.offers) AvailableContracts.Add(ToUnrealContract(Contract));
    for (const FicsitContracts::Contract& Contract : Snapshot.active) ActiveContracts.Add(ToUnrealContract(Contract));
    for (const FicsitContracts::Contract& Contract : Snapshot.history) ContractHistory.Add(ToUnrealContract(Contract));
    ForceNetUpdate();
    OnContractBoardChanged.Broadcast();
}

FicsitContracts::EndgameProgression AFICSITContractsSubsystem::BuildEndgameProgression() const
{
    FicsitContracts::EndgameProgressionSnapshot Snapshot;
    Snapshot.vanillaSpaceElevatorPhase5Complete = bVanillaSpaceElevatorPhase5Complete;
    for (int32 Index = 0; Index < 6; ++Index)
        Snapshot.terminalPhasesComplete[static_cast<std::size_t>(Index)] =
            TerminalResearchPhasesComplete.IsValidIndex(Index) && TerminalResearchPhasesComplete[Index] != 0;
    for (int32 Index = 0; Index < 3; ++Index)
        Snapshot.elevatorExtensionsComplete[static_cast<std::size_t>(Index)] =
            ElevatorExtensionsComplete.IsValidIndex(Index) && ElevatorExtensionsComplete[Index] != 0;
    return FicsitContracts::EndgameProgression(Snapshot);
}

void AFICSITContractsSubsystem::ApplyEndgameProgression(const FicsitContracts::EndgameProgression& Progression)
{
    const FicsitContracts::EndgameProgressionSnapshot& Snapshot = Progression.state();
    bVanillaSpaceElevatorPhase5Complete = Snapshot.vanillaSpaceElevatorPhase5Complete;
    TerminalResearchPhasesComplete.Init(0, 6);
    ElevatorExtensionsComplete.Init(0, 3);
    for (int32 Index = 0; Index < 6; ++Index)
        TerminalResearchPhasesComplete[Index] = Snapshot.terminalPhasesComplete[static_cast<std::size_t>(Index)] ? 1 : 0;
    for (int32 Index = 0; Index < 3; ++Index)
        ElevatorExtensionsComplete[Index] = Snapshot.elevatorExtensionsComplete[static_cast<std::size_t>(Index)] ? 1 : 0;
    ForceNetUpdate();
    OnContractBoardChanged.Broadcast();
}

int32 AFICSITContractsSubsystem::GetAvailableTerminalResearchPhase() const
{
    return BuildEndgameProgression().availableTerminalPhase();
}

int32 AFICSITContractsSubsystem::GetAvailableElevatorExtensionPhase() const
{
    return BuildEndgameProgression().availableElevatorPhase();
}

bool AFICSITContractsSubsystem::IsTerminalResearchPhaseComplete(int32 Phase) const
{
    return BuildEndgameProgression().isTerminalPhaseComplete(Phase);
}

bool AFICSITContractsSubsystem::IsElevatorExtensionComplete(int32 Phase) const
{
    return BuildEndgameProgression().isElevatorPhaseComplete(Phase);
}

FString AFICSITContractsSubsystem::GetEndgameProgressionSummary() const
{
    const FicsitContracts::EndgameProgression Progression = BuildEndgameProgression();
    const auto GetStatus = [](bool bComplete, bool bAvailable) -> const TCHAR*
    {
        return bComplete ? TEXT("COMPLETE") : (bAvailable ? TEXT("AVAILABLE") : TEXT("LOCKED"));
    };

    static const TCHAR* TerminalPhaseNames[] = {
        TEXT("Frontier Survey"),
        TEXT("Advanced Metallurgy"),
        TEXT("Cryogenic Systems"),
        TEXT("Photonic Manufacturing"),
        TEXT("Gravitic Engineering"),
        TEXT("Interplanetary Logistics")
    };
    static const TCHAR* ElevatorPhaseNames[] = {
        TEXT("Frontier Expansion"),
        TEXT("Planetary Network"),
        TEXT("Orbital Assembly")
    };

    FString Result = TEXT("FICSIT ENDGAME RESEARCH // SERVER-SAVED PROGRESSION");
    Result += FString::Printf(TEXT("\nVANILLA SPACE ELEVATOR V // FINAL PROJECT: %s"),
        Progression.state().vanillaSpaceElevatorPhase5Complete ? TEXT("CONFIRMED") : TEXT("NOT CONFIRMED"));

    for (int32 Phase = 1; Phase <= 6; ++Phase)
    {
        const bool bComplete = Progression.isTerminalPhaseComplete(Phase);
        const bool bAvailable = Progression.canResearchTerminalPhase(Phase);
        Result += FString::Printf(TEXT("\nTERMINAL %02d // %s // %s"),
            Phase, TerminalPhaseNames[Phase - 1], GetStatus(bComplete, bAvailable));
    }
    for (int32 Phase = 6; Phase <= 8; ++Phase)
    {
        const bool bComplete = Progression.isElevatorPhaseComplete(Phase);
        const bool bAvailable = Progression.canCompleteElevatorPhase(Phase);
        Result += FString::Printf(TEXT("\nSPACE ELEVATOR %02d // %s // %s"),
            Phase, ElevatorPhaseNames[Phase - 6], GetStatus(bComplete, bAvailable));
    }

    const int32 AvailableTerminal = Progression.availableTerminalPhase();
    const int32 AvailableElevator = Progression.availableElevatorPhase();
    Result += AvailableTerminal > 0
        ? FString::Printf(TEXT("\nNEXT RESEARCH: TERMINAL PHASE %d"), AvailableTerminal)
        : TEXT("\nNEXT RESEARCH: NONE");
    Result += AvailableElevator > 0
        ? FString::Printf(TEXT("\nNEXT ELEVATOR PROJECT: PHASE %d"), AvailableElevator)
        : TEXT("\nNEXT ELEVATOR PROJECT: NONE");
    return Result;
}

bool AFICSITContractsSubsystem::RecordVanillaSpaceElevatorPhase5Complete()
{
    if (!HasAuthority()) return false;
    FicsitContracts::EndgameProgression Progression = BuildEndgameProgression();
    if (Progression.state().vanillaSpaceElevatorPhase5Complete) return false;
    Progression.recordVanillaSpaceElevatorPhase5();
    ApplyEndgameProgression(Progression);
    return true;
}

bool AFICSITContractsSubsystem::CompleteTerminalResearchPhase(int32 Phase)
{
    if (!HasAuthority()) return false;
    FicsitContracts::EndgameProgression Progression = BuildEndgameProgression();
    if (!Progression.completeTerminalPhase(Phase)) return false;
    ApplyEndgameProgression(Progression);
    return true;
}

bool AFICSITContractsSubsystem::CompleteElevatorExtensionPhase(int32 Phase)
{
    if (!HasAuthority()) return false;
    FicsitContracts::EndgameProgression Progression = BuildEndgameProgression();
    if (!Progression.completeElevatorPhase(Phase)) return false;
    ApplyEndgameProgression(Progression);
    return true;
}

FString AFICSITContractsSubsystem::GetRankName() const
{
    return UTF8_TO_TCHAR(FicsitContracts::ContractEngine::rankName(Reputation));
}

void AFICSITContractsSubsystem::GenerateInitialBoard()
{
    if (!HasAuthority()) return;
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    const int64 Now = GetUtcUnixTime();
    Engine.ensureOffers(5, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
}

void AFICSITContractsSubsystem::RefreshExpiredContracts()
{
    if (!HasAuthority()) return;
    const int64 Now = GetUtcUnixTime();
    const auto HasExpired = [Now](const FFICSITContract& Contract) { return Contract.ExpiresAtUnix > 0 && Now >= Contract.ExpiresAtUnix; };
    // A board can legitimately have fewer than five free offers when all unlocked templates
    // are already offered or active. Do not replicate an unchanged board every 15 seconds.
    const bool bHasExpiredContract = AvailableContracts.ContainsByPredicate(HasExpired) || ActiveContracts.ContainsByPredicate(HasExpired);
    if (!bHasExpiredContract) return;
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    Engine.expire(Now);
    Engine.ensureOffers(5, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
}

bool AFICSITContractsSubsystem::AcceptContract(FName ContractId, FString& OutMessage)
{
    if (!HasAuthority()) { OutMessage = TEXT("Contract changes must be processed by the session host."); return false; }
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    const int64 Now = GetUtcUnixTime();
    const bool bAccepted = Engine.accept(ToUtf8(ContractId.ToString()), Now);
    Engine.ensureOffers(5, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
    if (!bAccepted)
    {
        OutMessage = TEXT("That contract is unavailable, expired, or you already have four active contracts.");
        return false;
    }
    OutMessage = FString::Printf(TEXT("Contract %s accepted. Deliver the requested items with /contracts deliver %s."), *ContractId.ToString(), *ContractId.ToString());
    return true;
}

bool AFICSITContractsSubsystem::AbandonContract(FName ContractId, FString& OutMessage)
{
    if (!HasAuthority())
    {
        OutMessage = TEXT("Contract changes must be processed by the session host.");
        return false;
    }

    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    const FicsitContracts::AbandonResult Result = Engine.abandon(ToUtf8(ContractId.ToString()), GetUtcUnixTime());
    ApplySnapshot(Engine.state());
    if (!Result.contractFound)
    {
        OutMessage = TEXT("No active contract with that ID was found. It may already be completed or expired.");
        return false;
    }

    OutMessage = FString::Printf(
        TEXT("Contract %s cancelled. Reputation penalty: -%d. Current reputation: %d (%s)."),
        *ContractId.ToString(), Result.reputationPenalty, Reputation, *GetRankName());
    return true;
}

int32 AFICSITContractsSubsystem::DeliverFromInventory(AFGCharacterPlayer* Player, FName ContractId, FString& OutMessage)
{
    if (!HasAuthority() || !IsValid(Player) || !Player->HasAuthority())
    { OutMessage = TEXT("Delivery must be processed by the host while the player is in the session."); return 0; }

    UFGInventoryComponent* Inventory = Player->GetInventory();
    if (!IsValid(Inventory) || !Inventory->HasAuthority())
    { OutMessage = TEXT("The player's authoritative inventory is not available."); return 0; }

    const int64 Now = GetUtcUnixTime();
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    Engine.expire(Now);
    const std::string ContractIdUtf8 = ToUtf8(ContractId.ToString());
    const auto ContractIterator = std::find_if(Engine.state().active.begin(), Engine.state().active.end(),
        [&ContractIdUtf8](const FicsitContracts::Contract& Contract) { return Contract.id == ContractIdUtf8; });
    if (ContractIterator == Engine.state().active.end())
    {
        ApplySnapshot(Engine.state());
        OutMessage = TEXT("No active contract with that ID was found. Check /contracts.");
        return 0;
    }

    const FicsitContracts::Contract ContractCopy = *ContractIterator;
    const int32 PreviousStageIndex = static_cast<int32>(ContractCopy.currentStageIndex);
    const FString PreviousStageTitle = FromUtf8(FicsitContracts::ContractEngine::currentStageTitle(ContractCopy));
    int32 TotalDelivered = 0;
    bool bStageAdvanced = false;
    FString NextStageTitle;
    int32 UnresolvedItemClasses = 0;
    int32 InventoryRemovalMismatches = 0;
    for (const FicsitContracts::Requirement& Requirement : FicsitContracts::ContractEngine::currentRequirements(ContractCopy))
    {
        const int32 Remaining = FMath::Max(0, Requirement.requiredAmount - Requirement.deliveredAmount);
        if (Remaining <= 0) continue;
        const FString ItemClassPath = FromUtf8(Requirement.itemClassPath);
        UClass* LoadedItemClass = StaticLoadClass(UFGItemDescriptor::StaticClass(), nullptr, *ItemClassPath);
        if (!LoadedItemClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("FICSIT Contracts could not resolve vanilla item class: %s"), *ItemClassPath);
            ++UnresolvedItemClasses;
            continue;
        }
        const TSubclassOf<UFGItemDescriptor> ItemClass(LoadedItemClass);
        const int32 AvailableBefore = Inventory->GetNumItems(ItemClass);
        const int32 RequestedRemoval = FMath::Min(Remaining, FMath::Max(0, AvailableBefore));
        if (RequestedRemoval <= 0) continue;

        // Remove() is void in the game API. Credit the order only for the quantity
        // that is verifiably absent from inventory after the operation.
        Inventory->Remove(ItemClass, RequestedRemoval);
        const int32 AvailableAfter = Inventory->GetNumItems(ItemClass);
        const int32 ActuallyRemoved = FMath::Clamp(AvailableBefore - AvailableAfter, 0, RequestedRemoval);
        if (ActuallyRemoved != RequestedRemoval)
        {
            ++InventoryRemovalMismatches;
            UE_LOG(LogTemp, Warning,
                TEXT("FICSIT Contracts inventory removal mismatch for %s: requested %d, observed %d"),
                *ItemClassPath, RequestedRemoval, ActuallyRemoved);
        }
        if (ActuallyRemoved <= 0) continue;

        const FicsitContracts::DeliveryResult Delivery = Engine.deliver(ContractIdUtf8, Requirement.itemClassPath, ActuallyRemoved, Now);
        TotalDelivered += Delivery.acceptedAmount;
        if (Delivery.stageCompleted)
        {
            bStageAdvanced = !Delivery.completed;
            if (bStageAdvanced)
            {
                const auto ActiveAfterDelivery = std::find_if(Engine.state().active.begin(), Engine.state().active.end(),
                    [&ContractIdUtf8](const FicsitContracts::Contract& Contract) { return Contract.id == ContractIdUtf8; });
                if (ActiveAfterDelivery != Engine.state().active.end())
                    NextStageTitle = FromUtf8(FicsitContracts::ContractEngine::currentStageTitle(*ActiveAfterDelivery));
            }
            break; // One dispatch action completes at most one stage.
        }
    }

    Engine.ensureOffers(5, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
    if (TotalDelivered <= 0)
    {
        if (InventoryRemovalMismatches > 0)
            OutMessage = TEXT("The game did not remove the requested inventory quantity. No progress was credited; check the mod log.");
        else if (UnresolvedItemClasses > 0)
            OutMessage = TEXT("No items were taken. One or more vanilla item classes could not be resolved; see the mod log.");
        else
            OutMessage = TEXT("You do not have any of the outstanding required items in your main inventory.");
        return 0;
    }

    const FFICSITContract* CompletedContract = ContractHistory.FindByPredicate([ContractId](const FFICSITContract& Contract)
    { return Contract.ContractId == ContractId && Contract.bCompleted; });
    if (CompletedContract)
        OutMessage = FString::Printf(TEXT("Contract completed. FICSIT reputation +%d. Current reputation: %d (%s)."), CompletedContract->RewardReputation, Reputation, *GetRankName());
    else if (bStageAdvanced)
        OutMessage = FString::Printf(TEXT("Stage %d/%d complete: %s. Next: %s. Total chain progress: check /contracts."),
            PreviousStageIndex + 1, static_cast<int32>(FicsitContracts::ContractEngine::stageCount(ContractCopy)),
            *PreviousStageTitle, *NextStageTitle);
    else
        OutMessage = FString::Printf(TEXT("Delivered %d item(s). Contract progress: check /contracts."), TotalDelivered);
    return TotalDelivered;
}

FString AFICSITContractsSubsystem::FormatContract(const FFICSITContract& Contract) const
{
    FString Result = FString::Printf(TEXT("%s | %s | %s | +%d REP | %d%%"),
        *Contract.ContractId.ToString(), *Contract.Title, DifficultyLabel(Contract.Difficulty),
        Contract.RewardReputation, Contract.GetProgressPercent());
    if (Contract.bLongTerm) Result += TEXT(" | STRATEGIC PROGRAM");
    if (Contract.bTimeLimited && Contract.ExpiresAtUnix > 0)
    {
        const int64 SecondsRemaining = FMath::Max<int64>(0, Contract.ExpiresAtUnix - GetUtcUnixTime());
        Result += FString::Printf(TEXT(" | DEADLINE %lld min"), SecondsRemaining / 60);
    }

    if (Contract.Stages.Num() > 0)
    {
        Result += FString::Printf(TEXT("\nPROGRAM ROUTE // %d PHASES // CURRENT %d/%d"),
            Contract.Stages.Num(), FMath::Clamp(Contract.CurrentStageIndex + 1, 1, Contract.Stages.Num()),
            Contract.Stages.Num());

        for (int32 StageIndex = 0; StageIndex < Contract.Stages.Num(); ++StageIndex)
        {
            const FFICSITContractStage& Stage = Contract.Stages[StageIndex];
            const TCHAR* StageState = Contract.bCompleted || StageIndex < Contract.CurrentStageIndex
                ? TEXT("COMPLETE")
                : StageIndex == Contract.CurrentStageIndex ? TEXT("CURRENT") : TEXT("LOCKED");
            Result += FString::Printf(TEXT("\n  [%s] %02d. %s"), StageState, StageIndex + 1, *Stage.StageTitle);
            for (const FFICSITContractRequirement& Requirement : Stage.Requirements)
            {
                Result += FString::Printf(TEXT("\n      - %s: %d / %d"),
                    *Requirement.DisplayName, Requirement.DeliveredAmount, Requirement.RequiredAmount);
            }
        }
    }
    else
    {
        Result += TEXT("\nDELIVERY MANIFEST");
        for (const FFICSITContractRequirement& Requirement : Contract.Requirements)
        {
            Result += FString::Printf(TEXT("\n  - %s: %d / %d"),
                *Requirement.DisplayName, Requirement.DeliveredAmount, Requirement.RequiredAmount);
        }
    }
    return Result;
}
