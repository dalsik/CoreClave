#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTopWidget.generated.h"

class UDeckBuilderSubsystem;

UCLASS()
class CORECLAVE_API UDeckBuilderTopWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Deck Builder|Top")
	void RequestTopSaveDeck();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder|Top")
	void RequestTopLoadDeck();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder|Top")
	void RequestTopSelectDeckSlot(int32 DeckSlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder|Top")
	void RequestTopStartBattle();

	UFUNCTION(BlueprintPure, Category = "Deck Builder|Top")
	int32 GetDeckSlotCount() const;

private:
	UDeckBuilderSubsystem* GetDeckBuilderSubsystem() const;
};
