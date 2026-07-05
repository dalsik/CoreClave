#include "DeckBuilderTopWidget.h"

#include "DeckBuilderSubsystem.h"
#include "Kismet/GameplayStatics.h"

UDeckBuilderSubsystem* UDeckBuilderTopWidget::GetDeckBuilderSubsystem() const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<UDeckBuilderSubsystem>();
	}

	return nullptr;
}

void UDeckBuilderTopWidget::RequestTopSaveDeck()
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		DeckBuilderSubsystem->SaveToSlot();
	}
}

void UDeckBuilderTopWidget::RequestTopLoadDeck()
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		DeckBuilderSubsystem->LoadFromSlot();
	}
}

void UDeckBuilderTopWidget::RequestTopSelectDeckSlot(int32 DeckSlotIndex)
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		DeckBuilderSubsystem->SelectDeckSlot(DeckSlotIndex);
	}
}

void UDeckBuilderTopWidget::RequestTopStartBattle()
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		FText ValidationError;
		if (!DeckBuilderSubsystem->ValidateWorkingDeck(ValidationError))
		{
			UE_LOG(LogTemp, Warning, TEXT("Deck validation failed: %s"), *ValidationError.ToString());
			return;
		}

		DeckBuilderSubsystem->SaveToSlot();
		UGameplayStatics::OpenLevel(this, DeckBuilderSubsystem->GetBattleMapName());
	}
}

int32 UDeckBuilderTopWidget::GetDeckSlotCount() const
{
	if (UDeckBuilderSubsystem* DeckBuilderSubsystem = GetDeckBuilderSubsystem())
	{
		return DeckBuilderSubsystem->GetDeckCollection().GetDeckCount();
	}

	return 0;
}

