#include "DeckBuilderWidget.h"

#include "DeckBuilderPlayerController.h"
#include "DeckBuilderSubsystem.h"
#include "Kismet/GameplayStatics.h"

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
		BP_OnSubsystemBound();
		RefreshFromSubsystem();
	}
}

void UDeckBuilderWidget::UnbindSubsystem()
{
	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->OnWorkingDeckChanged.RemoveDynamic(this, &UDeckBuilderWidget::HandleSubsystemWorkingDeckChanged);
		CachedDeckBuilderSubsystem = nullptr;
		BP_OnSubsystemUnbound();
	}
}

void UDeckBuilderWidget::RefreshFromSubsystem()
{
	if (CachedDeckBuilderSubsystem)
	{
		BP_OnWorkingDeckChanged(CachedDeckBuilderSubsystem->GetWorkingDeck());
	}
}

void UDeckBuilderWidget::RequestAddCard(FName CardId)
{
	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = GetDeckBuilderPlayerController())
	{
		DeckBuilderPlayerController->AddCardToWorkingDeck(CardId);
		return;
	}

	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->AddCard(CardId);
	}
}

void UDeckBuilderWidget::RequestRemoveCard(int32 CardIndex)
{
	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = GetDeckBuilderPlayerController())
	{
		DeckBuilderPlayerController->RemoveCardFromWorkingDeck(CardIndex);
		return;
	}

	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->RemoveCardAt(CardIndex);
	}
}

void UDeckBuilderWidget::RequestSaveDeck()
{
	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = GetDeckBuilderPlayerController())
	{
		DeckBuilderPlayerController->SaveCurrentDeckToSlot();
		return;
	}

	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->SaveToSlot();
	}
}

void UDeckBuilderWidget::RequestLoadDeck()
{
	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = GetDeckBuilderPlayerController())
	{
		DeckBuilderPlayerController->LoadDeckFromSlot();
		return;
	}

	if (CachedDeckBuilderSubsystem)
	{
		CachedDeckBuilderSubsystem->LoadFromSlot();
	}
}

void UDeckBuilderWidget::RequestStartBattle()
{
	if (ADeckBuilderPlayerController* DeckBuilderPlayerController = GetDeckBuilderPlayerController())
	{
		DeckBuilderPlayerController->RequestStartBattle();
		return;
	}

	if (!CachedDeckBuilderSubsystem)
	{
		return;
	}

	FText ValidationError;
	if (!CachedDeckBuilderSubsystem->ValidateWorkingDeck(ValidationError))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deck validation failed: %s"), *ValidationError.ToString());
		return;
	}

	CachedDeckBuilderSubsystem->SaveToSlot();
	UGameplayStatics::OpenLevel(this, CachedDeckBuilderSubsystem->GetBattleMapName());
}

ADeckBuilderPlayerController* UDeckBuilderWidget::GetDeckBuilderPlayerController() const
{
	return GetOwningPlayer() ? Cast<ADeckBuilderPlayerController>(GetOwningPlayer()) : nullptr;
}

FDeckData UDeckBuilderWidget::GetWorkingDeck() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetWorkingDeck() : FDeckData();
}

int32 UDeckBuilderWidget::GetWorkingDeckCardCount() const
{
	return CachedDeckBuilderSubsystem ? CachedDeckBuilderSubsystem->GetWorkingDeckCardCount() : 0;
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
