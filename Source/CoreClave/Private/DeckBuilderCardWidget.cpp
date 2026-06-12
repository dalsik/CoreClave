#include "DeckBuilderCardWidget.h"

#include "DeckBuilderCardDragDropOperation.h"

void UDeckBuilderCardWidget::SetCardData(FName InCardId, int32 InCardCost)
{
	CardId = InCardId;
	CardCost = FMath::Max(InCardCost, 0);
	BP_OnCardDataChanged();
}

void UDeckBuilderCardWidget::SetDraggingState(bool bInIsDragging)
{
	bIsDragging = bInIsDragging;
	BP_OnDragStateChanged(bInIsDragging);
}

// 드래그 오퍼레이션을 생성해서 드래그한 카드의 정보를 담도록 설정
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
