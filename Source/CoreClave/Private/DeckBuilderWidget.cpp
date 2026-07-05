#include "DeckBuilderWidget.h"

#include "DeckBuilderSubsystem.h"

void UDeckBuilderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BindSubsystem();
}

void UDeckBuilderWidget::NativeDestruct()
{
	UnbindSubsystem();

	Super::NativeDestruct();
}

void UDeckBuilderWidget::BindSubsystem()
{
	UnbindSubsystem();

	if (UGameInstance* GameInstance = GetGameInstance())
	{
		CachedDeckBuilderSubsystem = GameInstance->GetSubsystem<UDeckBuilderSubsystem>();
	}

	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->OnWorkingDeckChanged.AddUniqueDynamic(this, &UDeckBuilderWidget::HandleSubsystemWorkingDeckChanged);
		CachedDeckBuilderSubsystem->OnOwnedCardsChanged.AddUniqueDynamic(this, &UDeckBuilderWidget::HandleSubsystemOwnedCardsChanged);
		CachedDeckBuilderSubsystem->OnDeckCollectionChanged.AddUniqueDynamic(this, &UDeckBuilderWidget::HandleSubsystemDeckCollectionChanged);
		BP_OnSubsystemBound();
		RefreshFromSubsystem();
	}
}

void UDeckBuilderWidget::UnbindSubsystem()
{
	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->OnWorkingDeckChanged.RemoveDynamic(this, &UDeckBuilderWidget::HandleSubsystemWorkingDeckChanged);
		CachedDeckBuilderSubsystem->OnOwnedCardsChanged.RemoveDynamic(this, &UDeckBuilderWidget::HandleSubsystemOwnedCardsChanged);
		CachedDeckBuilderSubsystem->OnDeckCollectionChanged.RemoveDynamic(this, &UDeckBuilderWidget::HandleSubsystemDeckCollectionChanged);
		CachedDeckBuilderSubsystem = nullptr;
		BP_OnSubsystemUnbound();
	}
}

void UDeckBuilderWidget::RefreshFromSubsystem()
{
	if (CachedDeckBuilderSubsystem)
	{
		BP_OnWorkingDeckChanged(CachedDeckBuilderSubsystem->GetWorkingDeck());
		BP_OnOwnedCardsChanged();
		BP_OnDeckCollectionChanged(CachedDeckBuilderSubsystem->GetDeckCollection());
	}
}

FDeckData UDeckBuilderWidget::GetWorkingDeck() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetWorkingDeck() : FDeckData();
}

int32 UDeckBuilderWidget::GetWorkingDeckCardCount() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetWorkingDeckCardCount() : 0;
}

int32 UDeckBuilderWidget::GetOwnedCardCount(FName CardId) const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetOwnedCardCount(CardId) : 0;
}

FDeckCollection UDeckBuilderWidget::GetDeckCollection() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetDeckCollection() : FDeckCollection();
}

int32 UDeckBuilderWidget::GetSelectedDeckIndex() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetSelectedDeckIndex() : INDEX_NONE;
}

int32 UDeckBuilderWidget::GetDeckCardCount(int32 DeckSlotIndex) const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetDeckCardCount(DeckSlotIndex) : 0;
}

bool UDeckBuilderWidget::HasOwnedCard(FName CardId) const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->HasOwnedCard(CardId) : false;
}

void UDeckBuilderWidget::GetOwnedCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const
{
	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->GetOwnedCardCounts(OutCardIds, OutCounts);
	}
	else
	{
		OutCardIds.Reset();
		OutCounts.Reset();
	}
}

void UDeckBuilderWidget::GetWorkingDeckUniqueCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const
{
	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->GetWorkingDeckUniqueCardCounts(OutCardIds, OutCounts);
	}
	else
	{
		OutCardIds.Reset();
		OutCounts.Reset();
	}
}

void UDeckBuilderWidget::GetWorkingDeckManaCurve(TArray<int32>& OutManaCurve, int32 MaxManaCost) const
{
	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->GetWorkingDeckManaCurve(OutManaCurve, MaxManaCost);
	}
	else
	{
		OutManaCurve.Reset();
	}
}

void UDeckBuilderWidget::HandleSubsystemWorkingDeckChanged(FDeckData WorkingDeck)
{
	BP_OnWorkingDeckChanged(WorkingDeck);
}

void UDeckBuilderWidget::HandleSubsystemOwnedCardsChanged()
{
	BP_OnOwnedCardsChanged();
}

void UDeckBuilderWidget::HandleSubsystemDeckCollectionChanged(FDeckCollection DeckCollection)
{
	BP_OnDeckCollectionChanged(DeckCollection);
}
