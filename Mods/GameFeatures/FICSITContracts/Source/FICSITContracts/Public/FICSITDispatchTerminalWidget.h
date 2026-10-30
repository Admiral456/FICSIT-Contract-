#pragma once
#include "CoreMinimal.h"
#include "UI/FGInteractWidget.h"
#include "FICSITDispatchTerminalWidget.generated.h"
class UTextBlock;
class UVerticalBox;

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
    enum class EDispatchAction : uint8 { None, Accept, Deliver, Cancel };
    UPROPERTY() TObjectPtr<UTextBlock> SummaryText;
    UPROPERTY() TObjectPtr<UVerticalBox> BoardContent;
    UFUNCTION() void RefreshBoard();
    void AddSection(const FText& Heading);
    void AddContractEntry(const FText& GroupLabel, const FText& ContractText,
        bool bEmphasize, FName ContractId = NAME_None, EDispatchAction Action = EDispatchAction::None);
    void SendContractAction(FName ContractId, EDispatchAction Action);
    class AFICSITContractsSubsystem* FindBoard() const;
};
