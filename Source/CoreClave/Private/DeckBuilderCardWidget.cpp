#include "DeckBuilderCardWidget.h"

#include "DeckBuilderCardDragDropOperation.h"

void UDeckBuilderCardWidget::SetCardStatData(const FCardStatData& Data)
{
	CachedCardData = Data;
	CardId = Data.CardId;
	CardCost = FMath::Max(Data.Cost, 0);
	BP_OnCardDataChanged();
}

void UDeckBuilderCardWidget::SetCardData(FName InCardId, int32 InCardCost)
{
	CachedCardData.CardId = InCardId;
	CachedCardData.Cost = FMath::Max(InCardCost, 0);
	CardId = InCardId;
	CardCost = FMath::Max(InCardCost, 0);
	BP_OnCardDataChanged();
}

void UDeckBuilderCardWidget::SetCardCount(int32 InCardCount)
{
	CardCount = FMath::Max(InCardCount, 0);
	BP_OnCardCountChanged(CardCount);
}

void UDeckBuilderCardWidget::SetDraggingState(bool bInIsDragging)
{
	bIsDragging = bInIsDragging;
	BP_OnDragStateChanged(bInIsDragging);
}

UDeckBuilderCardDragDropOperation* UDeckBuilderCardWidget::CreateCardDragDropOperation(UWidget* DefaultDragVisual, EDragPivot Pivot, FVector2D Offset) const
{
	if (CardId.IsNone())
	{
		return nullptr;
	}

	UDeckBuilderCardDragDropOperation* DragDropOperation = NewObject<UDeckBuilderCardDragDropOperation>(const_cast<UDeckBuilderCardWidget*>(this));
	if (DragDropOperation)
	{
		DragDropOperation->CardId = CardId;
		DragDropOperation->SourceCardWidget = const_cast<UDeckBuilderCardWidget*>(this);
		DragDropOperation->DefaultDragVisual = DefaultDragVisual;
		DragDropOperation->Pivot = Pivot;
		DragDropOperation->Offset = Offset;
	}

	return DragDropOperation;
}
