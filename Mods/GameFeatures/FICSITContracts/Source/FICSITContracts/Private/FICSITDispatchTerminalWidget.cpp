#include "FICSITDispatchTerminalWidget.h"
#include "FICSITContractsSubsystem.h"
#include "FICSITContractsRemoteCallObject.h"
#include "FGPlayerController.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/World.h"
#include "EngineUtils.h"

UFICSITDispatchTerminalWidget::UFICSITDispatchTerminalWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
    mUseKeyboard = true;
    mUseMouse = true;
    mDisablePlayerActions = true;
    mDisableBuildGunActions = true;
    mSupportsCaching = false;
    SetDesiredHorizontalAlignment(HAlign_Center);
    SetDesiredVerticalAlignment(VAlign_Center);
}

void UFICSITDispatchTerminalWidget::NativeConstruct()
{
    Super::NativeConstruct();
    if (!WidgetTree) return;

    USizeBox* RootSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("DispatchRootSize"));
    RootSize->SetWidthOverride(1040.0f);
    RootSize->SetHeightOverride(760.0f);
    UBorder* Frame = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("DispatchFrame"));
    Frame->SetPadding(FMargin(24.0f));
    Frame->SetBrushColor(FLinearColor(0.018f,0.028f,0.04f,0.97f));
    RootSize->SetContent(Frame);

    UVerticalBox* MainColumn = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DispatchMainColumn"));
    Frame->SetContent(MainColumn);
    UTextBlock* Eyebrow = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Eyebrow->SetText(FText::FromString(TEXT("F I C S I T   //   LOGISTICS DIVISION")));
    Eyebrow->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f,0.43f,0.08f)));
    MainColumn->AddChildToVerticalBox(Eyebrow);

    UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Title->SetText(FText::FromString(TEXT("DISPATCH TERMINAL")));
    Title->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f,0.94f,1.0f)));
    FSlateFontInfo TitleFont = Title->GetFont();
    TitleFont.Size = 30;
    Title->SetFont(TitleFont);
    MainColumn->AddChildToVerticalBox(Title);

    SummaryText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DispatchSummary"));
    SummaryText->SetAutoWrapText(true);
    SummaryText->SetColorAndOpacity(FSlateColor(FLinearColor(0.38f,0.78f,0.96f)));
    MainColumn->AddChildToVerticalBox(SummaryText);

    UButton* RefreshButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("RefreshBoardButton"));
    UTextBlock* RefreshLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    RefreshLabel->SetText(FText::FromString(TEXT("REFRESH CONTRACT BOARD")));
    RefreshButton->SetContent(RefreshLabel);
    RefreshButton->OnClicked.AddDynamic(this,&UFICSITDispatchTerminalWidget::RefreshBoard);
    MainColumn->AddChildToVerticalBox(RefreshButton);

    UScrollBox* Scroll = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), TEXT("DispatchBoardScroll"));
    MainColumn->AddChildToVerticalBox(Scroll);
    BoardContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("DispatchBoardContent"));
    Scroll->AddChild(BoardContent);

    UTextBlock* Footer = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Footer->SetText(FText::FromString(TEXT("SERVER-VALIDATED ACTIONS // ACCEPT, DELIVER AND CANCEL BUTTONS USE YOUR PLAYER INVENTORY AND THE SHARED FICSIT BOARD.")));
    Footer->SetAutoWrapText(true);
    Footer->SetColorAndOpacity(FSlateColor(FLinearColor(0.72f,0.76f,0.8f)));
    MainColumn->AddChildToVerticalBox(Footer);

    WidgetTree->RootWidget = RootSize;
    if (AFICSITContractsSubsystem* Board = FindBoard())
        Board->OnContractBoardChanged.AddDynamic(this,&UFICSITDispatchTerminalWidget::RefreshBoard);
    RefreshBoard();
}

void UFICSITDispatchTerminalWidget::NativeDestruct()
{
    if (AFICSITContractsSubsystem* Board = FindBoard())
        Board->OnContractBoardChanged.RemoveDynamic(this,&UFICSITDispatchTerminalWidget::RefreshBoard);
    Super::NativeDestruct();
}

AFICSITContractsSubsystem* UFICSITDispatchTerminalWidget::FindBoard() const
{
    if (!GetWorld()) return nullptr;
    for (TActorIterator<AFICSITContractsSubsystem> It(GetWorld()); It; ++It) return *It;
    return nullptr;
}

void UFICSITDispatchTerminalWidget::AddSection(const FText& Heading)
{
    if (!BoardContent || !WidgetTree) return;
    UTextBlock* Section = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Section->SetText(Heading);
    Section->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f,0.47f,0.12f)));
    FSlateFontInfo Font = Section->GetFont();
    Font.Size = 19;
    Section->SetFont(Font);
    BoardContent->AddChildToVerticalBox(Section);
}

void UFICSITDispatchTerminalWidget::AddContractEntry(const FText& GroupLabel,const FText& ContractText,
    bool bEmphasize,FName ContractId,EDispatchAction Action)
{
    if (!BoardContent || !WidgetTree) return;
    UBorder* Card = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
    Card->SetPadding(FMargin(12.0f,9.0f));
    Card->SetBrushColor(bEmphasize ? FLinearColor(0.075f,0.11f,0.15f,0.98f) : FLinearColor(0.035f,0.055f,0.075f,0.96f));
    UVerticalBox* CardContent = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass());
    Card->SetContent(CardContent);

    UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Label->SetText(GroupLabel);
    Label->SetColorAndOpacity(FSlateColor(FLinearColor(0.35f,0.72f,0.9f)));
    CardContent->AddChildToVerticalBox(Label);

    UTextBlock* Details = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
    Details->SetText(ContractText);
    Details->SetAutoWrapText(true);
    Details->SetColorAndOpacity(FSlateColor(FLinearColor(0.88f,0.91f,0.94f)));
    CardContent->AddChildToVerticalBox(Details);

    if (Action != EDispatchAction::None && !ContractId.IsNone())
    {
        UHorizontalBox* Actions = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass());
        CardContent->AddChildToVerticalBox(Actions);
        const auto AddActionButton = [this, Actions, ContractId](const TCHAR* LabelText, EDispatchAction ButtonAction)
        {
            UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass());
            UTextBlock* ButtonLabel = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
            ButtonLabel->SetText(FText::FromString(LabelText));
            Button->SetContent(ButtonLabel);
            Button->OnClicked.AddLambda([this, ContractId, ButtonAction]()
            {
                SendContractAction(ContractId,ButtonAction);
            });
            Actions->AddChildToHorizontalBox(Button);
        };
        if (Action == EDispatchAction::Accept) AddActionButton(TEXT("ACCEPT ORDER"),EDispatchAction::Accept);
        if (Action == EDispatchAction::Deliver)
        {
            AddActionButton(TEXT("DELIVER FROM INVENTORY"),EDispatchAction::Deliver);
            AddActionButton(TEXT("CANCEL ORDER"),EDispatchAction::Cancel);
        }
    }
    BoardContent->AddChildToVerticalBox(Card);
}

void UFICSITDispatchTerminalWidget::SendContractAction(FName ContractId,EDispatchAction Action)
{
    AFGPlayerController* Controller = Cast<AFGPlayerController>(GetOwningPlayer());
    if (!Controller || ContractId.IsNone()) return;
    UFICSITContractsRemoteCallObject* RCO = Cast<UFICSITContractsRemoteCallObject>(
        Controller->GetRemoteCallObjectOfClass(UFICSITContractsRemoteCallObject::StaticClass()));
    if (!RCO)
        RCO = Cast<UFICSITContractsRemoteCallObject>(
            Controller->RegisterRemoteCallObjectClass(UFICSITContractsRemoteCallObject::StaticClass()));
    if (!RCO) return;

    switch (Action)
    {
        case EDispatchAction::Accept: RCO->Server_AcceptContract(ContractId); break;
        case EDispatchAction::Deliver: RCO->Server_DeliverContract(ContractId); break;
        case EDispatchAction::Cancel: RCO->Server_CancelContract(ContractId); break;
        default: break;
    }
}

void UFICSITDispatchTerminalWidget::RefreshBoard()
{
    if (!BoardContent || !WidgetTree || !GetWorld()) return;
    BoardContent->ClearChildren();
    AFICSITContractsSubsystem* Board = FindBoard();
    if (!Board)
    {
        SummaryText->SetText(FText::FromString(TEXT("NETWORK STATUS: CONTRACT SUBSYSTEM NOT AVAILABLE. Check that the FICSIT Contracts world module loaded.")));
        AddSection(FText::FromString(TEXT("BOARD OFFLINE")));
        AddContractEntry(FText::FromString(TEXT("SYSTEM MESSAGE")),FText::FromString(TEXT("No authoritative contract board was found in this world.")),true);
        return;
    }

    const TArray<FFICSITContract> Offers=Board->GetAvailableContracts();
    const TArray<FFICSITContract> Active=Board->GetActiveContracts();
    const TArray<FFICSITContract> History=Board->GetContractHistory();
    SummaryText->SetText(FText::FromString(FString::Printf(TEXT("NETWORK: ONLINE     FICSIT REP: %d     RANK: %s     AVAILABLE: %d     ACTIVE: %d"),
        Board->GetReputation(),*Board->GetRankName(),Offers.Num(),Active.Num())));

    AddSection(FText::FromString(TEXT("ACTIVE OPERATIONS")));
    if (Active.IsEmpty())
        AddContractEntry(FText::FromString(TEXT("NO ACTIVE ORDERS")),FText::FromString(TEXT("Accept an available contract to begin a new logistics operation.")),false);
    else
        for (const FFICSITContract& Contract:Active)
            AddContractEntry(FText::FromString(TEXT("IN PROGRESS")),FText::FromString(Board->FormatContract(Contract)),true,Contract.ContractId,EDispatchAction::Deliver);

    AddSection(FText::FromString(TEXT("AVAILABLE FICSIT CONTRACTS")));
    if (Offers.IsEmpty())
        AddContractEntry(FText::FromString(TEXT("NO AVAILABLE ORDERS")),FText::FromString(TEXT("The board is waiting for new eligible orders.")),false);
    else
        for (const FFICSITContract& Contract:Offers)
            AddContractEntry(FText::FromString(TEXT("AVAILABLE")),FText::FromString(Board->FormatContract(Contract)),false,Contract.ContractId,EDispatchAction::Accept);

    AddSection(FText::FromString(TEXT("RECENT DISPATCH HISTORY")));
    const int32 FirstHistoryIndex=FMath::Max(0,History.Num()-5);
    if (History.IsEmpty())
        AddContractEntry(FText::FromString(TEXT("NO HISTORY")),FText::FromString(TEXT("Completed, expired and cancelled orders will be recorded here.")),false);
    else
        for (int32 Index=FirstHistoryIndex;Index<History.Num();++Index)
            AddContractEntry(FText::FromString(TEXT("ARCHIVED ORDER")),FText::FromString(Board->FormatContract(History[Index])),false);
}
