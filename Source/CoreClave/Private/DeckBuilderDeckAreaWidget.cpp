#include "DeckBuilderDeckAreaWidget.h"

#include "DeckBuilderCardDragDropOperation.h"
#include "DeckBuilderPlayerController.h"
#include "DeckBuilderSubsystem.h"
#include "Blueprint/DragDropOperation.h"

bool UDeckBuilderDeckAreaWidget::TryResolveCardIdFromOperation(UDragDropOperation* InOperation, FName& OutCardId) const
{
	if (const UDeckBuilderCardDragDropOperation* CardDropOperation = Cast<UDeckBuilderCardDragDropOperation>(InOperation))
	{
		OutCardId = CardDropOperation->CardId;
		return !OutCardId.IsNone();
	}

	OutCardId = NAME_None;
	return false;
}

bool UDeckBuilderDeckAreaWidget::CanAcceptCardDrop(UDragDropOperation* InOperation, FName& OutCardId) const
{
	if (!TryResolveCardIdFromOperation(InOperation, OutCardId))
	{
		return false;
	}

	const ADeckBuilderPlayerController* DeckBuilderPlayerController = Cast<ADeckBuilderPlayerController>(GetOwningPlayer());
	if (!DeckBuilderPlayerController)
	{
		return false;
	}

	const UDeckBuilderSubsystem* DeckBuilderSubsystem = DeckBuilderPlayerController->GetDeckBuilderSubsystem();
	if (!DeckBuilderSubsystem)
	{
		return false;
	}

	return DeckBuilderSubsystem->HasOwnedCard(OutCardId) && DeckBuilderSubsystem->GetWorkingDeckCardCount() < DeckBuilderSubsystem->GetMaxDeckSize();
}

bool UDeckBuilderDeckAreaWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	FName CardId = NAME_None;
	const bool bCanAcceptDrop = CanAcceptCardDrop(InOperation, CardId);
	BP_SetHoverState(bCanAcceptDrop);
	return bCanAcceptDrop;
}

bool UDeckBuilderDeckAreaWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	FName CardId = NAME_None;
	if (!CanAcceptCardDrop(InOperation, CardId))
	{
		BP_SetHoverState(false);
		return false;
	}

	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = Cast<ADeckBuilderPlayerController>(GetOwningPlayer()))
	{
		DeckBuilderPlayerController->AddCardToWorkingDeck(CardId);
		if (UDeckBuilderSubsystem* DeckBuilderSubsystem = DeckBuilderPlayerController->GetDeckBuilderSubsystem())
		{
			DeckBuilderSubsystem->ConsumeOwnedCard(CardId);
		}
		BP_SetHoverState(false);
		return true;
	}

	BP_SetHoverState(false);
	return false;
}

void UDeckBuilderDeckAreaWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	FName CardId = NAME_None;
	BP_SetHoverState(CanAcceptCardDrop(InOperation, CardId));
}

void UDeckBuilderDeckAreaWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	BP_SetHoverState(false);
}
