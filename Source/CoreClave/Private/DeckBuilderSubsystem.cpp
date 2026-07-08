#include "DeckBuilderSubsystem.h"

#include "DeckBuilderSaveGame.h"
#include "Kismet/GameplayStatics.h"

int32 UDeckBuilderSubsystem::ResolveCardManaCost_Implementation(FName CardId) const
{
	return 0;
}

void UDeckBuilderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitializeDefaultOwnedCards();
	EnsureDeckSlotCount(5);
	LoadFromSlot();
}

void UDeckBuilderSubsystem::SetWorkingDeck(const FDeckData& NewDeckData)
{
	WorkingDeck = NewDeckData;
	NormalizeWorkingDeck();
	RestoreOwnedCardCountsForWorkingDeck();
	BroadcastWorkingDeckChanged();
	BroadcastDeckCollectionChanged();
	BroadcastOwnedCardsChanged();
}

void UDeckBuilderSubsystem::ResetWorkingDeck()
{
	WorkingDeck.Reset();
	WorkingDeck.DeckName = FName(TEXT("Deck_01"));
	NormalizeWorkingDeck();
	RestoreOwnedCardCountsForWorkingDeck();
	BroadcastWorkingDeckChanged();
	BroadcastDeckCollectionChanged();
	BroadcastOwnedCardsChanged();
}

/// <summary>
/// 이미 존재하는 덱 슬롯을 선택하거나 새로운 덱 슬롯을 선택하는 메서드
/// </summary>

bool UDeckBuilderSubsystem::SelectDeckSlot(int32 NewSelectedDeckIndex)
{
	EnsureDeckSlotCount(5);

	if (!DeckCollection.Decks.IsValidIndex(NewSelectedDeckIndex))
	{
		return false;
	}

	DeckCollection.SelectedDeckIndex = NewSelectedDeckIndex;
	WorkingDeck = DeckCollection.Decks[NewSelectedDeckIndex];
	NormalizeWorkingDeck();
	RestoreOwnedCardCountsForWorkingDeck();

	bHasUnsavedChanges = true; // 덱 편집 후 저장되지 않은 변경 사항이 있음을 표시

	// 이벤트 디스패처 호출
	BroadcastDeckCollectionChanged();
	BroadcastWorkingDeckChanged();
	BroadcastOwnedCardsChanged();
	return true;
}

// 현재 편성중인 카드를 덱에 추가해주는 메서드
bool UDeckBuilderSubsystem::AddCard(FName CardId)
{
	if (CardId.IsNone() || WorkingDeck.IsAtMaxSize(MaxDeckSize))
	{
		return false;
	}

	WorkingDeck.CardIds.Add(CardId);
	if (int32* StoredCount = OwnedCardCounts.Find(CardId))
	{
		*StoredCount = FMath::Max(*StoredCount - 1, 0);
	}
	
	bHasUnsavedChanges = true; // 덱 편집 후 저장되지 않은 변경 사항이 있음을 표시

	NormalizeWorkingDeck();
	BroadcastDeckCollectionChanged();
	BroadcastWorkingDeckChanged();
	BroadcastOwnedCardsChanged();
	return true;
}

bool UDeckBuilderSubsystem::RemoveCardAt(int32 CardIndex)
{
	if (!WorkingDeck.CardIds.IsValidIndex(CardIndex))
	{
		return false;
	}

	const FName RemovedCardId = WorkingDeck.CardIds[CardIndex];
	WorkingDeck.CardIds.RemoveAt(CardIndex);
	if (!RemovedCardId.IsNone())
	{
		int32& StoredCount = OwnedCardCounts.FindOrAdd(RemovedCardId);
		++StoredCount;
	}

	bHasUnsavedChanges = true; // 덱 편집 후 저장되지 않은 변경 사항이 있음을 표시

	NormalizeWorkingDeck();
	BroadcastDeckCollectionChanged();
	BroadcastWorkingDeckChanged();
	BroadcastOwnedCardsChanged();
	return true;
}

bool UDeckBuilderSubsystem::ValidateWorkingDeck(FText& OutErrorText) const
{
	if (WorkingDeck.CardIds.Num() != MaxDeckSize)
	{
		OutErrorText = FText::FromString(FString::Printf(TEXT("Deck must contain exactly %d cards."), MaxDeckSize));
		return false;
	}

	for (const FName& CardId : WorkingDeck.CardIds)
	{
		if (CardId.IsNone())
		{
			OutErrorText = FText::FromString(TEXT("Deck contains an invalid CardID."));
			return false;
		}
	}

	OutErrorText = FText::GetEmpty();
	return true;
}

bool UDeckBuilderSubsystem::LoadFromSlot()
{
	// 저장된 파일이 없을 경우
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName.ToString(), SaveUserIndex))
	{
		DeckCollection.Decks.Reset();
		DeckCollection.SelectedDeckIndex = 0;
		EnsureDeckSlotCount(5);
		WorkingDeck = DeckCollection.Decks.IsValidIndex(0) ? DeckCollection.Decks[0] : FDeckData();
		NormalizeWorkingDeck();
		OwnedCardCounts = DefaultOwnedCardCounts;
		NormalizeOwnedCardCounts();

		bHasUnsavedChanges = false; // 저장되지 않은 변경 사항이 없음을 표시
		// 이벤트 디스패처 호출
		BroadcastDeckCollectionChanged();
		BroadcastWorkingDeckChanged();
		BroadcastOwnedCardsChanged();
		return false;
	}

	// 저장된 파일을 정상적으로 읽었을 때
	if (UDeckBuilderSaveGame* SaveGame = Cast<UDeckBuilderSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName.ToString(), SaveUserIndex)))
	{
		DeckCollection = SaveGame->DeckCollection;
		EnsureDeckSlotCount(5);

		if (!DeckCollection.IsValidSelection())
		{
			DeckCollection.SelectedDeckIndex = 0;
		}

		WorkingDeck = DeckCollection.IsValidSelection() ? DeckCollection.Decks[DeckCollection.SelectedDeckIndex] : FDeckData();
		NormalizeWorkingDeck();

		OwnedCardCounts = SaveGame->OwnedCardCounts;
		if (OwnedCardCounts.Num() == 0)
		{
			OwnedCardCounts = DefaultOwnedCardCounts;
		}
		NormalizeOwnedCardCounts();

		bHasUnsavedChanges = false; // 저장된 파일을 읽은 후에는 변경 사항이 없음을 표시

		// 이벤트 디스패처 호출
		BroadcastDeckCollectionChanged();
		BroadcastWorkingDeckChanged();
		BroadcastOwnedCardsChanged();
		return true;
	}

	// 여기는 위에서 모두 캐스팅에 실패했을 때 도달하는 코드로, 저장된 파일이 손상되었거나 잘못된 형식일 경우를 처리

	DeckCollection.Decks.Reset();
	DeckCollection.SelectedDeckIndex = 0;
	EnsureDeckSlotCount(5);
	WorkingDeck = DeckCollection.Decks.IsValidIndex(0) ? DeckCollection.Decks[0] : FDeckData();
	NormalizeWorkingDeck();
	OwnedCardCounts = DefaultOwnedCardCounts;
	NormalizeOwnedCardCounts();

	bHasUnsavedChanges = false; // 저장되지 않은 변경 사항이 없음을 표시

	
	BroadcastDeckCollectionChanged();
	BroadcastWorkingDeckChanged();
	BroadcastOwnedCardsChanged();
	return false;
}

bool UDeckBuilderSubsystem::SaveToSlot()
{
	UDeckBuilderSaveGame* SaveGame = Cast<UDeckBuilderSaveGame>(UGameplayStatics::CreateSaveGameObject(UDeckBuilderSaveGame::StaticClass()));
	if (!SaveGame)
	{
		return false;
	}

	CommitWorkingDeckToSelectedSlot();
	EnsureDeckSlotCount(5);
	SaveGame->DeckCollection = DeckCollection;
	SaveGame->OwnedCardCounts = OwnedCardCounts;

	const bool bSaved = UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName.ToString(), SaveUserIndex);
	if (bSaved)
	{
		BroadcastWorkingDeckChanged();
		BroadcastDeckCollectionChanged();
		BroadcastOwnedCardsChanged();

		bHasUnsavedChanges = false; // 저장 후에는 변경 사항이 없음을 표시
	}

	return bSaved;
}

void UDeckBuilderSubsystem::SetMaxDeckSize(int32 NewMaxDeckSize)
{
	MaxDeckSize = FMath::Max(NewMaxDeckSize, 1);
	NormalizeWorkingDeck();
}

void UDeckBuilderSubsystem::SetSaveSlotName(FName NewSaveSlotName)
{
	SaveSlotName = NewSaveSlotName.IsNone() ? FName(TEXT("DeckBuilderSlot")) : NewSaveSlotName;
}

void UDeckBuilderSubsystem::SetSaveUserIndex(int32 NewSaveUserIndex)
{
	SaveUserIndex = FMath::Max(NewSaveUserIndex, 0);
}

void UDeckBuilderSubsystem::SetBattleMapName(FName NewBattleMapName)
{
	BattleMapName = NewBattleMapName.IsNone() ? FName(TEXT("BattleMap")) : NewBattleMapName;
}

int32 UDeckBuilderSubsystem::GetOwnedCardCount(FName CardId) const
{
	if (CardId.IsNone())
	{
		return 0;
	}

	if (const int32* FoundCount = OwnedCardCounts.Find(CardId))
	{
		return FMath::Max(*FoundCount, 0);
	}

	return 0;
}

int32 UDeckBuilderSubsystem::GetDeckCardCount(int32 DeckIndex) const
{
	return DeckCollection.GetDeckCardCount(DeckIndex);
}

bool UDeckBuilderSubsystem::HasOwnedCard(FName CardId) const
{
	return GetOwnedCardCount(CardId) > 0;
}

void UDeckBuilderSubsystem::GetOwnedCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const
{
	OutCardIds.Reset();
	OutCounts.Reset();

	for (const TPair<FName, int32>& Entry : OwnedCardCounts)
	{
		if (Entry.Key.IsNone() || Entry.Value <= 0)
		{
			continue;
		}

		OutCardIds.Add(Entry.Key);
		OutCounts.Add(Entry.Value);
	}
}

void UDeckBuilderSubsystem::SeedSampleOwnedCards()
{
	InitializeDefaultOwnedCards();
	OwnedCardCounts = DefaultOwnedCardCounts;
	NormalizeOwnedCardCounts();
	BroadcastOwnedCardsChanged();
}

void UDeckBuilderSubsystem::InitializeDefaultOwnedCards()
{
	DefaultOwnedCardCounts.Reset();
	for (int32 CardIndex = 1; CardIndex <= 10; ++CardIndex)
	{
		const FName CardId(*FString::Printf(TEXT("Card_%03d_Nor"), CardIndex));
		DefaultOwnedCardCounts.Add(CardId, 2);
	}
}

bool UDeckBuilderSubsystem::AddOwnedCard(FName CardId, int32 Count)
{
	if (CardId.IsNone() || Count <= 0)
	{
		return false;
	}

	int32& StoredCount = OwnedCardCounts.FindOrAdd(CardId);
	StoredCount += Count;
	NormalizeOwnedCardCounts();
	BroadcastOwnedCardsChanged();
	return true;
}

bool UDeckBuilderSubsystem::ConsumeOwnedCard(FName CardId, int32 Count)
{
	if (CardId.IsNone() || Count <= 0)
	{
		return false;
	}

	int32* StoredCount = OwnedCardCounts.Find(CardId);
	if (!StoredCount || *StoredCount < Count)
	{
		return false;
	}

	*StoredCount -= Count;
	BroadcastOwnedCardsChanged();
	return true;
}

/// <summary>
/// 현재 편성중인 덱을 정규화하는 메서드
/// </summary>
void UDeckBuilderSubsystem::NormalizeWorkingDeck()
{
	if (WorkingDeck.DeckName.IsNone())
	{
		WorkingDeck.DeckName = FName(TEXT("Deck_01"));
	}

	if (WorkingDeck.CardIds.Num() > MaxDeckSize)
	{
		WorkingDeck.CardIds.SetNum(MaxDeckSize);
	}
}

/// <summary>
/// 카드가	없는 경우나 소유 카드 수가 0 이하인 경우를 제거하는 메서드
/// </summary>
void UDeckBuilderSubsystem::NormalizeOwnedCardCounts()
{
	TArray<FName> KeysToRemove;
	for (const TPair<FName, int32>& Entry : OwnedCardCounts)
	{
		if (Entry.Key.IsNone() || Entry.Value <= 0)
		{
			KeysToRemove.Add(Entry.Key);
		}
	}

	for (const FName& CardId : KeysToRemove)
	{
		OwnedCardCounts.Remove(CardId);
	}
}

void UDeckBuilderSubsystem::EnsureDeckSlotCount(int32 DesiredDeckCount)
{
	DeckCollection.EnsureDeckCount(DesiredDeckCount);

	if (!DeckCollection.IsValidSelection())
	{
		DeckCollection.SelectedDeckIndex = 0;
	}
}

void UDeckBuilderSubsystem::CommitWorkingDeckToSelectedSlot()
{
	if (FDeckData* SelectedDeck = DeckCollection.GetSelectedDeck())
	{
		*SelectedDeck = WorkingDeck;
	}
}

void UDeckBuilderSubsystem::RestoreOwnedCardCountsForWorkingDeck()
{
	OwnedCardCounts = DefaultOwnedCardCounts;

	for (const FName& CardId : WorkingDeck.CardIds)
	{
		if (CardId.IsNone())
		{
			continue;
		}

		int32& StoredCount = OwnedCardCounts.FindOrAdd(CardId);
		StoredCount = FMath::Max(StoredCount - 1, 0);
	}

	NormalizeOwnedCardCounts();
}

void UDeckBuilderSubsystem::GetWorkingDeckUniqueCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const
{
	OutCardIds.Reset();
	OutCounts.Reset();

	TMap<FName, int32> CardCounts;
	for (const FName& CardId : WorkingDeck.CardIds)
	{
		if (CardId.IsNone())
		{
			continue;
		}

		int32& Count = CardCounts.FindOrAdd(CardId);
		++Count;
	}

	for (const FName& CardId : WorkingDeck.CardIds)
	{
		if (CardId.IsNone() || OutCardIds.Contains(CardId))
		{
			continue;
		}

		OutCardIds.Add(CardId);
		OutCounts.Add(CardCounts.FindChecked(CardId));
	}
}

void UDeckBuilderSubsystem::GetWorkingDeckManaCurve(TArray<int32>& OutManaCurve, int32 MaxManaCost) const
{
	MaxManaCost = FMath::Max(MaxManaCost, 0);
	OutManaCurve.Init(0, MaxManaCost + 1);

	for (const FName& CardId : WorkingDeck.CardIds)
	{
		if (CardId.IsNone())
		{
			continue;
		}

		const int32 CardCost = FMath::Clamp(ResolveCardManaCost(CardId), 0, MaxManaCost);
		++OutManaCurve[CardCost];
	}
}

bool UDeckBuilderSubsystem::GetHasUnsavedChanges() const
{
	return bHasUnsavedChanges;
}

void UDeckBuilderSubsystem::SetHasUnsavedChanges(bool bInHasUnsavedChanges)
{
	bHasUnsavedChanges = bInHasUnsavedChanges;
}


// 이벤트 디스패처 메서드
void UDeckBuilderSubsystem::BroadcastWorkingDeckChanged()
{
	OnWorkingDeckChanged.Broadcast(WorkingDeck);
}

void UDeckBuilderSubsystem::BroadcastOwnedCardsChanged()
{
	OnOwnedCardsChanged.Broadcast();
}

void UDeckBuilderSubsystem::BroadcastDeckCollectionChanged()
{
	OnDeckCollectionChanged.Broadcast(DeckCollection);
}
