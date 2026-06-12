#pragma once

#include "CoreMinimal.h"
#include "DeckBuilderTypes.generated.h"

UENUM(BlueprintType)
enum class EDeckBuilderPhase : uint8
{
	Editing UMETA(DisplayName = "Editing"),
	ReadyToStart UMETA(DisplayName = "ReadyToStart"),
	Traveling UMETA(DisplayName = "Traveling")
};

USTRUCT(BlueprintType)
struct FDeckData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	FName DeckName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	TArray<FName> CardIds;

	bool IsEmpty() const
	{
		return CardIds.Num() == 0;
	}

	bool IsAtMaxSize(int32 MaxDeckSize) const
	{
		return CardIds.Num() >= MaxDeckSize;
	}

	bool IsBattleReady(int32 MaxDeckSize) const
	{
		return CardIds.Num() == MaxDeckSize;
	}

	void Reset()
	{
		DeckName = NAME_None;
		CardIds.Reset();
	}
};

USTRUCT(BlueprintType)
struct FDeckCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	TArray<FDeckData> Decks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	int32 SelectedDeckIndex = INDEX_NONE;

	bool IsValidSelection() const
	{
		return Decks.IsValidIndex(SelectedDeckIndex);
	}

	FDeckData* GetSelectedDeck()
	{
		return Decks.IsValidIndex(SelectedDeckIndex) ? &Decks[SelectedDeckIndex] : nullptr;
	}

	const FDeckData* GetSelectedDeck() const
	{
		return Decks.IsValidIndex(SelectedDeckIndex) ? &Decks[SelectedDeckIndex] : nullptr;
	}

	FDeckData& GetOrCreateSingleDeck()
	{
		if (Decks.Num() == 0)
		{
			Decks.AddDefaulted();
			Decks[0].DeckName = FName(TEXT("Deck_01"));
			SelectedDeckIndex = 0;
		}
		else if (!Decks.IsValidIndex(SelectedDeckIndex))
		{
			SelectedDeckIndex = 0;
		}

		return Decks[SelectedDeckIndex];
	}

	void EnsureOneDeckExists()
	{
		GetOrCreateSingleDeck();
	}
};
