#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DeckBuilderTypes.h"
#include "DeckBuilderSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWorkingDeckChanged, FDeckData, WorkingDeck);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOwnedCardsChanged);

UCLASS()
class CORECLAVE_API UDeckBuilderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintAssignable, Category = "Deck Builder")
	FOnWorkingDeckChanged OnWorkingDeckChanged;

	UPROPERTY(BlueprintAssignable, Category = "Deck Builder")
	FOnOwnedCardsChanged OnOwnedCardsChanged;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	FDeckData GetWorkingDeck() const { return WorkingDeck; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetWorkingDeck(const FDeckData& NewDeckData);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void ResetWorkingDeck();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool AddCard(FName CardId);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool RemoveCardAt(int32 CardIndex);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool ValidateWorkingDeck(FText& OutErrorText) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool LoadFromSlot();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool SaveToSlot();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetMaxDeckSize(int32 NewMaxDeckSize);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetMaxDeckSize() const { return MaxDeckSize; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetSaveSlotName(FName NewSaveSlotName);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FName GetSaveSlotName() const { return SaveSlotName; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetSaveUserIndex(int32 NewSaveUserIndex);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetSaveUserIndex() const { return SaveUserIndex; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetBattleMapName(FName NewBattleMapName);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FName GetBattleMapName() const { return BattleMapName; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetWorkingDeckCardCount() const { return WorkingDeck.CardIds.Num(); }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetOwnedCardCount(FName CardId) const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	bool HasOwnedCard(FName CardId) const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	void GetWorkingDeckUniqueCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	void GetWorkingDeckManaCurve(TArray<int32>& OutManaCurve, int32 MaxManaCost = 10) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void GetOwnedCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SeedSampleOwnedCards();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool AddOwnedCard(FName CardId, int32 Count = 1);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	bool ConsumeOwnedCard(FName CardId, int32 Count = 1);

	UFUNCTION(BlueprintNativeEvent, Category = "Deck Builder")
	int32 ResolveCardManaCost(FName CardId) const;
	virtual int32 ResolveCardManaCost_Implementation(FName CardId) const;

private:
	UPROPERTY()
	TMap<FName, int32> DefaultOwnedCardCounts;

	UPROPERTY()
	FDeckData WorkingDeck;

	UPROPERTY()
	int32 MaxDeckSize = 20;

	UPROPERTY()
	FName SaveSlotName = TEXT("DeckBuilderSlot");

	UPROPERTY()
	int32 SaveUserIndex = 0;

	UPROPERTY()
	FName BattleMapName = TEXT("BattleMap");

	UPROPERTY()
	TMap<FName, int32> OwnedCardCounts;

	void NormalizeWorkingDeck();
	void InitializeDefaultOwnedCards();
	void NormalizeOwnedCardCounts();
	void BroadcastWorkingDeckChanged();
	void BroadcastOwnedCardsChanged();
};
