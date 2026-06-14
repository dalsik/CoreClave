#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "DeckBuilderWidget.generated.h"

class ADeckBuilderPlayerController;
class UDeckBuilderSubsystem;

UCLASS()
class CORECLAVE_API UDeckBuilderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RefreshFromSubsystem();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestAddCard(FName CardId);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestRemoveCard(int32 CardIndex);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestSaveDeck();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestLoadDeck();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestStartBattle();

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	UDeckBuilderSubsystem* GetDeckBuilderSubsystem() const { return CachedDeckBuilderSubsystem; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	ADeckBuilderPlayerController* GetDeckBuilderPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FDeckData GetWorkingDeck() const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetWorkingDeckCardCount() const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetOwnedCardCount(FName CardId) const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	bool HasOwnedCard(FName CardId) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void GetOwnedCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void GetWorkingDeckUniqueCardCounts(TArray<FName>& OutCardIds, TArray<int32>& OutCounts) const;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void GetWorkingDeckManaCurve(TArray<int32>& OutManaCurve, int32 MaxManaCost = 10) const;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnWorkingDeckChanged(FDeckData WorkingDeck);

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnOwnedCardsChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnSubsystemBound();

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnSubsystemUnbound();

private:
	UPROPERTY(Transient)
	TObjectPtr<UDeckBuilderSubsystem> CachedDeckBuilderSubsystem;

	void BindSubsystem();
	void UnbindSubsystem();

	UFUNCTION()
	void HandleSubsystemWorkingDeckChanged(FDeckData WorkingDeck);

	UFUNCTION()
	void HandleSubsystemOwnedCardsChanged();
};
