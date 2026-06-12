#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DeckBuilderPlayerController.generated.h"

class UUserWidget;
class UDeckBuilderSubsystem;
class UDeckBuilderWidget;

UCLASS()
class CORECLAVE_API ADeckBuilderPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADeckBuilderPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Deck Builder")
	TSubclassOf<UDeckBuilderWidget> DeckBuilderWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UDeckBuilderWidget> DeckBuilderWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deck Builder")
	FName BattleMapName = TEXT("BattleMap");

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void OpenDeckBuilderWidget();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void AddCardToWorkingDeck(FName CardId);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RemoveCardFromWorkingDeck(int32 CardIndex);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SaveCurrentDeckToSlot();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void LoadDeckFromSlot();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RequestStartBattle();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	UDeckBuilderSubsystem* GetDeckBuilderSubsystem() const;

protected:
	void HandleStartBattleInput();
	void HandleSaveInput();
	void HandleLoadInput();

	UPROPERTY(Transient)
	mutable TObjectPtr<UDeckBuilderSubsystem> CachedDeckBuilderSubsystem;
};
