#pragma once

#include "CoreMinimal.h"
#include "FGSaveInterface.h"
#include "ContractEngine.h"
#include "Subsystem/ModSubsystem.h"
#include "FICSITContractsSubsystem.generated.h"

class AFGCharacterPlayer;
class UFGItemDescriptor;

UENUM(BlueprintType)
enum class EFICSITContractDifficulty : uint8
{
    Small = 0 UMETA(DisplayName = "Small"),
    Medium = 1 UMETA(DisplayName = "Medium"),
    Large = 2 UMETA(DisplayName = "Large"),
    Special = 3 UMETA(DisplayName = "Special")
};

USTRUCT(BlueprintType)
struct FICSITCONTRACTS_API FFICSITContractRequirement
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FString ItemClassPath;
    UPROPERTY(SaveGame, BlueprintReadOnly) FString DisplayName;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 RequiredAmount = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 DeliveredAmount = 0;
};

USTRUCT(BlueprintType)
struct FICSITCONTRACTS_API FFICSITContractStage
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FString StageTitle;
    UPROPERTY(SaveGame, BlueprintReadOnly) TArray<FFICSITContractRequirement> Requirements;
};

USTRUCT(BlueprintType)
struct FICSITCONTRACTS_API FFICSITContract
{
    GENERATED_BODY()
    UPROPERTY(SaveGame, BlueprintReadOnly) FName ContractId;
    UPROPERTY(SaveGame, BlueprintReadOnly) FString TemplateId;
    UPROPERTY(SaveGame, BlueprintReadOnly) FString Title;
    UPROPERTY(SaveGame, BlueprintReadOnly) EFICSITContractDifficulty Difficulty = EFICSITContractDifficulty::Small;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 RewardReputation = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bTimeLimited = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) int64 ExpiresAtUnix = 0;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bLongTerm = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bCompleted = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bExpired = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) bool bAbandoned = false;
    UPROPERTY(SaveGame, BlueprintReadOnly) int64 FinishedAtUnix = 0;
    // Requirements is kept for legacy single-stage contracts and older saves.
    UPROPERTY(SaveGame, BlueprintReadOnly) TArray<FFICSITContractRequirement> Requirements;
    UPROPERTY(SaveGame, BlueprintReadOnly) TArray<FFICSITContractStage> Stages;
    UPROPERTY(SaveGame, BlueprintReadOnly) int32 CurrentStageIndex = 0;
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") int32 GetProgressPercent() const;
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") int32 GetStageCount() const { return Stages.Num() > 0 ? Stages.Num() : 1; }
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") FString GetCurrentStageTitle() const
    {
        return Stages.IsValidIndex(CurrentStageIndex) ? Stages[CurrentStageIndex].StageTitle : FString();
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFICSITContractBoardChanged);

/** Server-authoritative contract board persisted through IFGSaveInterface. */
UCLASS()
class FICSITCONTRACTS_API AFICSITContractsSubsystem : public AModSubsystem, public IFGSaveInterface
{
    GENERATED_BODY()
public:
    AFICSITContractsSubsystem();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") int32 GetReputation() const { return Reputation; }
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") FString GetRankName() const;
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") TArray<FFICSITContract> GetAvailableContracts() const { return AvailableContracts; }
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") TArray<FFICSITContract> GetActiveContracts() const { return ActiveContracts; }
    UFUNCTION(BlueprintPure, Category = "FICSIT Contracts") TArray<FFICSITContract> GetContractHistory() const { return ContractHistory; }

    bool AcceptContract(FName ContractId, FString& OutMessage);
    bool AbandonContract(FName ContractId, FString& OutMessage);
    int32 DeliverFromInventory(AFGCharacterPlayer* Player, FName ContractId, FString& OutMessage);
    FString FormatContract(const FFICSITContract& Contract) const;
    UPROPERTY(BlueprintAssignable, Category = "FICSIT Contracts") FOnFICSITContractBoardChanged OnContractBoardChanged;

    virtual void PreSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override {}
    virtual void PostSaveGame_Implementation(int32 saveVersion, int32 gameVersion) override {}
    virtual void PreLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override {}
    virtual void PostLoadGame_Implementation(int32 saveVersion, int32 gameVersion) override {}
    virtual void GatherDependencies_Implementation(TArray<UObject*>& OutDependentObjects) override;
    virtual bool NeedTransform_Implementation() override { return false; }
    virtual bool ShouldSave_Implementation() const override { return true; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
    int32 Reputation = 0;
    UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard) int64 NextSerial = 1;
    UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard) TArray<FFICSITContract> AvailableContracts;
    UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard) TArray<FFICSITContract> ActiveContracts;
    UPROPERTY(SaveGame, ReplicatedUsing = OnRep_ContractBoard) TArray<FFICSITContract> ContractHistory;
    FTimerHandle ExpiryTimerHandle;

    UFUNCTION() void OnRep_ContractBoard();
    UFUNCTION() void RefreshExpiredContracts();
    void GenerateInitialBoard();
    FicsitContracts::Snapshot BuildSnapshot() const;
    void ApplySnapshot(const FicsitContracts::Snapshot& Snapshot);
};
