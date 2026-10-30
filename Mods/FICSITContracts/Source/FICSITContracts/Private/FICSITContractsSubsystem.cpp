#include "FICSITContractsSubsystem.h"

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
        Result.finishedAtUnix = Source.FinishedAtUnix;
        for (const FFICSITContractRequirement& Requirement : Source.Requirements)
            Result.requirements.push_back({ToUtf8(Requirement.ItemClassPath), ToUtf8(Requirement.DisplayName), Requirement.RequiredAmount, Requirement.DeliveredAmount});
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
        Result.FinishedAtUnix = Source.finishedAtUnix;
        for (const FicsitContracts::Requirement& Requirement : Source.requirements)
        {
            FFICSITContractRequirement UnrealRequirement;
            UnrealRequirement.ItemClassPath = FromUtf8(Requirement.itemClassPath);
            UnrealRequirement.DisplayName = FromUtf8(Requirement.displayName);
            UnrealRequirement.RequiredAmount = Requirement.requiredAmount;
            UnrealRequirement.DeliveredAmount = Requirement.deliveredAmount;
            Result.Requirements.Add(MoveTemp(UnrealRequirement));
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
    int64 Required = 0;
    int64 Delivered = 0;
    for (const FFICSITContractRequirement& Requirement : Requirements)
    {
        Required += FMath::Max(0, Requirement.RequiredAmount);
        Delivered += FMath::Min(FMath::Max(0, Requirement.DeliveredAmount), FMath::Max(0, Requirement.RequiredAmount));
    }
    return Required > 0 ? static_cast<int32>((Delivered * 100) / Required) : 0;
}

AFICSITContractsSubsystem::AFICSITContractsSubsystem()
{
    ReplicationPolicy = ESubsystemReplicationPolicy::SpawnOnServer_Replicate;
}

void AFICSITContractsSubsystem::BeginPlay()
{
    Super::BeginPlay();
    if (HasAuthority())
    {
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

FString AFICSITContractsSubsystem::GetRankName() const
{
    return UTF8_TO_TCHAR(FicsitContracts::ContractEngine::rankName(Reputation));
}

void AFICSITContractsSubsystem::GenerateInitialBoard()
{
    if (!HasAuthority()) return;
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    const int64 Now = GetUtcUnixTime();
    Engine.ensureOffers(3, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
}

void AFICSITContractsSubsystem::RefreshExpiredContracts()
{
    if (!HasAuthority()) return;
    const int64 Now = GetUtcUnixTime();
    const auto HasExpired = [Now](const FFICSITContract& Contract) { return Contract.ExpiresAtUnix > 0 && Now >= Contract.ExpiresAtUnix; };
    const bool bNeedsRefresh = AvailableContracts.Num() < 3 || AvailableContracts.ContainsByPredicate(HasExpired) || ActiveContracts.ContainsByPredicate(HasExpired);
    if (!bNeedsRefresh) return;
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    Engine.expire(Now);
    Engine.ensureOffers(3, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
}

bool AFICSITContractsSubsystem::AcceptContract(FName ContractId, FString& OutMessage)
{
    if (!HasAuthority()) { OutMessage = TEXT("Contract changes must be processed by the session host."); return false; }
    FicsitContracts::ContractEngine Engine(BuildSnapshot());
    const int64 Now = GetUtcUnixTime();
    const bool bAccepted = Engine.accept(ToUtf8(ContractId.ToString()), Now);
    Engine.ensureOffers(3, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
    if (!bAccepted)
    {
        OutMessage = TEXT("That contract is unavailable, expired, or you already have three active contracts.");
        return false;
    }
    OutMessage = FString::Printf(TEXT("Contract %s accepted. Deliver the requested items with /contracts deliver %s."), *ContractId.ToString(), *ContractId.ToString());
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
    int32 TotalDelivered = 0;
    int32 UnresolvedItemClasses = 0;
    for (const FicsitContracts::Requirement& Requirement : ContractCopy.requirements)
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
        const int32 Available = Inventory->GetNumItems(ItemClass);
        const int32 ToDeliver = FMath::Min(Remaining, FMath::Max(0, Available));
        if (ToDeliver <= 0) continue;
        Inventory->Remove(ItemClass, ToDeliver);
        const FicsitContracts::DeliveryResult Delivery = Engine.deliver(ContractIdUtf8, Requirement.itemClassPath, ToDeliver, Now);
        TotalDelivered += Delivery.acceptedAmount;
    }

    Engine.ensureOffers(3, Now, static_cast<uint32>(NextSerial + Now));
    ApplySnapshot(Engine.state());
    if (TotalDelivered <= 0)
    {
        OutMessage = UnresolvedItemClasses > 0
            ? TEXT("No items were taken. One or more vanilla item classes could not be resolved; see the mod log.")
            : TEXT("You do not have any of the outstanding required items in your main inventory.");
        return 0;
    }

    const FFICSITContract* CompletedContract = ContractHistory.FindByPredicate([ContractId](const FFICSITContract& Contract)
    { return Contract.ContractId == ContractId && Contract.bCompleted; });
    if (CompletedContract)
        OutMessage = FString::Printf(TEXT("Contract completed. FICSIT reputation +%d. Current reputation: %d (%s)."), CompletedContract->RewardReputation, Reputation, *GetRankName());
    else
        OutMessage = FString::Printf(TEXT("Delivered %d item(s). Contract progress: check /contracts."), TotalDelivered);
    return TotalDelivered;
}

FString AFICSITContractsSubsystem::FormatContract(const FFICSITContract& Contract) const
{
    FString Result = FString::Printf(TEXT("%s | %s | %s | +%d REP | %d%%"),
        *Contract.ContractId.ToString(), *Contract.Title, DifficultyLabel(Contract.Difficulty),
        Contract.RewardReputation, Contract.GetProgressPercent());
    if (Contract.bLongTerm) Result += TEXT(" | LONG-TERM");
    if (Contract.bTimeLimited && Contract.ExpiresAtUnix > 0)
    {
        const int64 SecondsRemaining = FMath::Max<int64>(0, Contract.ExpiresAtUnix - GetUtcUnixTime());
        Result += FString::Printf(TEXT(" | %lld min left"), SecondsRemaining / 60);
    }
    for (const FFICSITContractRequirement& Requirement : Contract.Requirements)
        Result += FString::Printf(TEXT("\n  - %s: %d / %d"), *Requirement.DisplayName, Requirement.DeliveredAmount, Requirement.RequiredAmount);
    return Result;
}
