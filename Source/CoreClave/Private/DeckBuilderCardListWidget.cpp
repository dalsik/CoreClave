#include "DeckBuilderCardListWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DeckBuilderCardWidget.h"
#include "DeckBuilderSubsystem.h"
#include "CardStatData.h"

void UDeckBuilderCardListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 이벤트 디스패처 바인딩
	BindListSubsystem();
	BindNavigationButtons();
	RebuildCardSlots();
	RefreshCardList();

	UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] Constructed. Grid=%s, Slots=%d, Subsystem=%s"),
		CardListPanel ? TEXT("Valid") : TEXT("Null"),
		CardSlots.Num(),
		CachedListSubsystem.IsValid() ? TEXT("Valid") : TEXT("Null"));
}

void UDeckBuilderCardListWidget::NativeDestruct()
{
	UnbindNavigationButtons();
	UnbindListSubsystem();

	Super::NativeDestruct();
}

void UDeckBuilderCardListWidget::BindListSubsystem()
{
	UnbindListSubsystem();

	CachedListSubsystem = GetDeckBuilderSubsystem();
	if (CachedListSubsystem.IsValid())
	{
		CachedListSubsystem->OnOwnedCardsChanged.AddUniqueDynamic(this, &UDeckBuilderCardListWidget::RefreshCardList);
	}
}

void UDeckBuilderCardListWidget::UnbindListSubsystem()
{
	if (CachedListSubsystem.IsValid())
	{
		CachedListSubsystem->OnOwnedCardsChanged.RemoveDynamic(this, &UDeckBuilderCardListWidget::RefreshCardList);
		CachedListSubsystem.Reset();
	}
}

/// <summary>
/// 버튼 이벤트 디스패처 바인딩
/// </summary>
void UDeckBuilderCardListWidget::BindNavigationButtons()
{
	if (NextButton)
	{
		NextButton->OnClicked.RemoveDynamic(this, &UDeckBuilderCardListWidget::HandleNextPageClicked);
		NextButton->OnClicked.AddDynamic(this, &UDeckBuilderCardListWidget::HandleNextPageClicked);
	}

	if (PrevButton)
	{
		PrevButton->OnClicked.RemoveDynamic(this, &UDeckBuilderCardListWidget::HandlePreviousPageClicked);
		PrevButton->OnClicked.AddDynamic(this, &UDeckBuilderCardListWidget::HandlePreviousPageClicked);
	}
}

/// <summary>
///  이벤트 디스패처 언바인딩
/// </summary>
void UDeckBuilderCardListWidget::UnbindNavigationButtons()
{
	if (NextButton)
	{
		NextButton->OnClicked.RemoveDynamic(this, &UDeckBuilderCardListWidget::HandleNextPageClicked);
	}

	if (PrevButton)
	{
		PrevButton->OnClicked.RemoveDynamic(this, &UDeckBuilderCardListWidget::HandlePreviousPageClicked);
	}
}

/// <summary>
///  카드 슬롯 재구성
/// </summary>
void UDeckBuilderCardListWidget::RebuildCardSlots()
{
	CardsPerPage = FMath::Max(Rows, 1) * FMath::Max(Columns, 1);
	RowBoxes.Reset();

	if (!CardListPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] CardListPanel is null. Add a VerticalBox named 'CardListPanel' to the widget blueprint."));
	}

	if (!CardEntryWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] CardEntryWidgetClass is null. Assign WBP_DeckBuilderCard."));
	}
}

void UDeckBuilderCardListWidget::RefreshCardList()
{
	SyncTrackedCardIds();
	SortTrackedCardIds();
	PopulateCurrentPage();
	UpdatePageText();

	UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] Refresh. Tracked=%d, TotalPages=%d, CurrentPage=%d"),
		TrackedCardIds.Num(), GetTotalPageCount(), CurrentPageIndex);
}

/// <summary>
/// 소유하고 있는 카드 ID를 동기화하여 TrackedCardIds 배열을 업데이트.
/// </summary>
void UDeckBuilderCardListWidget::SyncTrackedCardIds()
{
	if (!GetDeckBuilderSubsystem())
	{
		UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] Subsystem is null while syncing cards."));
		return;
	}

	TArray<FName> OwnedCardIds;
	TArray<int32> OwnedCardCounts;
	GetOwnedCardCounts(OwnedCardIds, OwnedCardCounts);

	for (int32 Index = 0; Index < OwnedCardIds.Num(); ++Index)
	{
		const FName CardId = OwnedCardIds[Index];
		const int32 CardCount = OwnedCardCounts.IsValidIndex(Index) ? OwnedCardCounts[Index] : 0;
		if (CardId.IsNone() || CardCount <= 0)
		{
			continue;
		}

		if (!TrackedCardIds.Contains(CardId))
		{
			TrackedCardIds.Add(CardId);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[DeckBuilderCardList] Owned cards found=%d, Tracked now=%d"), OwnedCardIds.Num(), TrackedCardIds.Num());
}

void UDeckBuilderCardListWidget::SortTrackedCardIds()
{
	TrackedCardIds.Sort([](const FName& Left, const FName& Right)
	{
		return Left.ToString() < Right.ToString();
	});
}

void UDeckBuilderCardListWidget::PopulateCurrentPage()
{
	const int32 TotalPages = GetTotalPageCount();
	if (TotalPages <= 0)
	{
	CurrentPageIndex = 0;
		if (CardListPanel)
		{
			CardListPanel->ClearChildren();
		}
		return;
	}

	CurrentPageIndex = FMath::Clamp(CurrentPageIndex, 0, TotalPages - 1);
	const int32 StartIndex = CurrentPageIndex * CardsPerPage;
	const int32 EndIndex = FMath::Min(StartIndex + CardsPerPage, TrackedCardIds.Num());
	const int32 VisibleCardCount = FMath::Max(EndIndex - StartIndex, 0);

	if (CardListPanel)
	{
		CardListPanel->ClearChildren();
	}

	RowBoxes.Reset();
	const int32 ColumnCount = FMath::Max(Columns, 1);
	const int32 RowCount = FMath::DivideAndRoundUp(VisibleCardCount, ColumnCount);

	for (int32 RowIndex = 0; RowIndex < RowCount; ++RowIndex)
	{
		UHorizontalBox* RowBox = CreateRowBox();
		if (!RowBox || !CardListPanel)
		{
			continue;
		}

		RowBoxes.Add(RowBox);
		CardListPanel->AddChildToVerticalBox(RowBox);
		if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(RowBox->Slot))
		{
			VerticalSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, RowSpacing));
			VerticalSlot->SetHorizontalAlignment(HAlign_Left);
			VerticalSlot->SetVerticalAlignment(VAlign_Top);
		}
	}

	for (int32 SlotIndex = 0; SlotIndex < VisibleCardCount; ++SlotIndex)
	{
		UDeckBuilderCardWidget* CardWidget = GetOrCreateCardSlot(SlotIndex);

		if (!CardWidget)
		{
			continue;
		}

		const int32 CardIndex = StartIndex + SlotIndex;
		if (!TrackedCardIds.IsValidIndex(CardIndex))
		{
			continue;
		}

		const FName CardId = TrackedCardIds[CardIndex];
		const int32 CardCount = GetOwnedCardCountInternal(CardId);
		ApplyCardToSlot(CardWidget, CardId, CardCount);

		const int32 RowIndex = FMath::Max(SlotIndex / ColumnCount, 0);
		if (!RowBoxes.IsValidIndex(RowIndex) || !RowBoxes[RowIndex])
		{
			continue;
		}

		if (UHorizontalBox* RowBox = RowBoxes[RowIndex])
		{
			RowBox->AddChildToHorizontalBox(CardWidget);
			if (UHorizontalBoxSlot* HorizontalSlot = Cast<UHorizontalBoxSlot>(CardWidget->Slot))
			{
				HorizontalSlot->SetPadding(FMargin(0.0f, 0.0f, CardSpacing, 0.0f));
				HorizontalSlot->SetHorizontalAlignment(HAlign_Left);
				HorizontalSlot->SetVerticalAlignment(VAlign_Top);
			}
		}
	}

	for (int32 SlotIndex = VisibleCardCount; SlotIndex < CardSlots.Num(); ++SlotIndex)
	{
		ClearCardSlot(CardSlots[SlotIndex]);
	}
}

void UDeckBuilderCardListWidget::UpdatePageText()
{
	if (!PageText)
	{
		return;
	}

	const int32 TotalPages = GetTotalPageCount();
	const int32 DisplayPage = TotalPages > 0 ? CurrentPageIndex + 1 : 0;
	PageText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), DisplayPage, TotalPages)));
}

void UDeckBuilderCardListWidget::ClearCardSlot(UDeckBuilderCardWidget* CardWidget)
{
	if (!CardWidget)
	{
		return;
	}

	CardWidget->SetCardData(NAME_None, 0);
	CardWidget->SetCardCount(0);
	CardWidget->SetDraggingState(false);
	CardWidget->SetIsEnabled(false);
	CardWidget->SetVisibility(ESlateVisibility::Collapsed);
	CardWidget->SetRenderOpacity(1.0f);
}

UHorizontalBox* UDeckBuilderCardListWidget::CreateRowBox()
{
	if (!GetOwningPlayer())
	{
		return nullptr;
	}

	UHorizontalBox* NewRowBox = WidgetTree ? WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass()) : NewObject<UHorizontalBox>(this);
	if (!NewRowBox)
	{
		return nullptr;
	}

	return NewRowBox;
}

UDeckBuilderCardWidget* UDeckBuilderCardListWidget::GetOrCreateCardSlot(int32 SlotIndex)
{
	if (CardSlots.IsValidIndex(SlotIndex) && CardSlots[SlotIndex])
	{
		return CardSlots[SlotIndex];
	}

	if (!CardEntryWidgetClass || !GetOwningPlayer())
	{
		return nullptr;
	}

	UDeckBuilderCardWidget* CardWidget = CreateWidget<UDeckBuilderCardWidget>(GetOwningPlayer(), CardEntryWidgetClass);
	if (!CardWidget)
	{
		return nullptr;
	}

	while (CardSlots.Num() <= SlotIndex)
	{
		CardSlots.Add(nullptr);
	}

	CardSlots[SlotIndex] = CardWidget;
	return CardWidget;
}

void UDeckBuilderCardListWidget::ApplyCardToSlot(UDeckBuilderCardWidget* CardWidget, FName CardId, int32 CardCount)
{
	if (!CardWidget)
	{
		return;
	}

	CardWidget->SetVisibility(ESlateVisibility::Visible);
	CardWidget->SetIsEnabled(CardCount > 0);
	CardWidget->SetRenderOpacity(CardCount > 0 ? 1.0f : 0.45f);

	FCardStatData CardData;
	if (ResolveCardData(CardId, CardData))
	{
		CardWidget->SetCardStatData(CardData);
	}
	else
	{
		CardWidget->SetCardData(CardId, 0);
	}

	CardWidget->SetCardCount(CardCount);
}

bool UDeckBuilderCardListWidget::ResolveCardData(FName CardId, FCardStatData& OutCardData) const
{
	if (!CardStatDataTable || CardId.IsNone())
	{
		OutCardData = FCardStatData();
		OutCardData.CardId = CardId;
		return false;
	}

	static const FString ContextString(TEXT("DeckBuilderCardListWidget"));
	if (const FCardStatData* CardData = CardStatDataTable->FindRow<FCardStatData>(CardId, ContextString))
	{
		OutCardData = *CardData;
		OutCardData.CardId = CardId;
		return true;
	}

	OutCardData = FCardStatData();
	OutCardData.CardId = CardId;
	return false;
}

int32 UDeckBuilderCardListWidget::GetOwnedCardCountInternal(FName CardId) const
{
	return GetDeckBuilderSubsystem() ? GetOwnedCardCount(CardId) : 0;
}

void UDeckBuilderCardListWidget::NextPage()
{
	const int32 TotalPages = GetTotalPageCount();
	if (TotalPages <= 0)
	{
		CurrentPageIndex = 0;
		UpdatePageText();
		return;
	}

	CurrentPageIndex = FMath::Clamp(CurrentPageIndex + 1, 0, TotalPages - 1);
	PopulateCurrentPage();
	UpdatePageText();
}

void UDeckBuilderCardListWidget::PreviousPage()
{
	const int32 TotalPages = GetTotalPageCount();
	if (TotalPages <= 0)
	{
		CurrentPageIndex = 0;
		UpdatePageText();
		return;
	}

	CurrentPageIndex = FMath::Clamp(CurrentPageIndex - 1, 0, TotalPages - 1);
	PopulateCurrentPage();
	UpdatePageText();
}

void UDeckBuilderCardListWidget::SetGridDimensions(int32 NewRows, int32 NewColumns)
{
	Rows = FMath::Max(NewRows, 1);
	Columns = FMath::Max(NewColumns, 1);
	RebuildCardSlots();
	RefreshCardList();
}

int32 UDeckBuilderCardListWidget::GetTotalPageCount() const
{
	if (TrackedCardIds.Num() <= 0 || CardsPerPage <= 0)
	{
		return 0;
	}

	return FMath::DivideAndRoundUp(TrackedCardIds.Num(), CardsPerPage);
}

void UDeckBuilderCardListWidget::HandleNextPageClicked()
{
	NextPage();
}

void UDeckBuilderCardListWidget::HandlePreviousPageClicked()
{
	PreviousPage();
}
