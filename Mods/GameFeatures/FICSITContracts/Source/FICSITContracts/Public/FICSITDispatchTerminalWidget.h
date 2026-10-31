#pragma once

#include "CoreMinimal.h"
#include "UI/FGInteractWidget.h"
#include "FICSITDispatchTerminalWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UFICSITDispatchTerminalWidget;

UENUM()
enum class EFICSITDispatchAction : uint8
{
    None,
    Accept,
    Deliver,
    Cancel
};

/** UObject bridge so UButton's dynamic OnClicked delegate can carry an order ID. */
UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalActionHandler : public UObject
{
    GENERATED_BODY()
public:
    void Initialize(UFICSITDispatchTerminalWidget* InOwner, FName InContractId, EFICSITDispatchAction InAction);

    UFUNCTION()
    void HandleClicked();

private:
    TWeakObjectPtr<UFICSITDispatchTerminalWidget> OwnerWidget;
    FName ContractId;
    EFICSITDispatchAction Action = EFICSITDispatchAction::None;
};

UCLASS()
class FICSITCONTRACTS_API UFICSITDispatchTerminalWidget : public UFGInteractWidget
{
    GENERATED_BODY()
public:
    UFICSITDispatchTerminalWidget(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    friend class UFICSITDispatchTerminalActionHandler;

    UPROPERTY()
    TObjectPtr<UTextBlock> SummaryText;

    UPROPERTY()
    TObjectPtr<UVerticalBox> BoardContent;

    UPROPERTY()
    TArray<TObjectPtr<UFICSITDispatchTerminalActionHandler>> ActionHandlers;

    UFUNCTION()
    void RefreshBoard();

    void AddSection(const FText& Heading);
    void AddContractEntry(const FText& GroupLabel, const FText& ContractText,
        bool bEmphasize, FName ContractId = NAME_None,
        EFICSITDispatchAction Action = EFICSITDispatchAction::None, int32 ProgressPercent = -1);
    void SendContractAction(FName ContractId, EFICSITDispatchAction Action);
    class AFICSITContractsSubsystem* FindBoard() const;
};
