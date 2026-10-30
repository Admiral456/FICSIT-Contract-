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
private:
    UPROPERTY() TObjectPtr<UTextBlock> SummaryText;
    UPROPERTY() TObjectPtr<UVerticalBox> BoardContent;
    UFUNCTION() void RefreshBoard();
    void AddSection(const FText& Heading);
    void AddContractEntry(const FText& GroupLabel, const FText& ContractText, bool bEmphasize);
};