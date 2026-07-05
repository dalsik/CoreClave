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

	int32 GetDeckCount() const
	{
		return Decks.Num();
	}

	FDeckData* GetSelectedDeck()
	{
		return Decks.IsValidIndex(SelectedDeckIndex) ? &Decks[SelectedDeckIndex] : nullptr;
	}

	const FDeckData* GetSelectedDeck() const
	{
		return Decks.IsValidIndex(SelectedDeckIndex) ? &Decks[SelectedDeckIndex] : nullptr;
	}

	FDeckData* GetDeckAt(int32 DeckIndex)
	{
		return Decks.IsValidIndex(DeckIndex) ? &Decks[DeckIndex] : nullptr;
	}

	const FDeckData* GetDeckAt(int32 DeckIndex) const
	{
		return Decks.IsValidIndex(DeckIndex) ? &Decks[DeckIndex] : nullptr;
	}

	int32 GetDeckCardCount(int32 DeckIndex) const
	{
		return Decks.IsValidIndex(DeckIndex) ? Decks[DeckIndex].CardIds.Num() : 0;
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

	FDeckData& GetOrCreateDeckAt(int32 DeckIndex)
	{
		DeckIndex = FMath::Max(DeckIndex, 0);

		while (Decks.Num() <= DeckIndex)
		{
			const int32 NewDeckIndex = Decks.AddDefaulted();
			if (Decks[NewDeckIndex].DeckName.IsNone())
			{
				Decks[NewDeckIndex].DeckName = FName(*FString::Printf(TEXT("Deck_%02d"), NewDeckIndex + 1));
			}
		}

		if (!Decks.IsValidIndex(SelectedDeckIndex))
		{
			SelectedDeckIndex = 0;
		}

		return Decks[DeckIndex];
	}

	void EnsureDeckCount(int32 DesiredDeckCount)
	{
		DesiredDeckCount = FMath::Max(DesiredDeckCount, 1);

		while (Decks.Num() < DesiredDeckCount)
		{
			const int32 NewDeckIndex = Decks.AddDefaulted();
			if (Decks[NewDeckIndex].DeckName.IsNone())
			{
				Decks[NewDeckIndex].DeckName = FName(*FString::Printf(TEXT("Deck_%02d"), NewDeckIndex + 1));
			}
		}

		for (int32 DeckIndex = 0; DeckIndex < Decks.Num(); ++DeckIndex)
		{
			if (Decks[DeckIndex].DeckName.IsNone())
			{
				Decks[DeckIndex].DeckName = FName(*FString::Printf(TEXT("Deck_%02d"), DeckIndex + 1));
			}
		}

		if (!Decks.IsValidIndex(SelectedDeckIndex))
		{
			SelectedDeckIndex = 0;
		}
	}

	void EnsureOneDeckExists()
	{
		GetOrCreateSingleDeck();
	}
};
