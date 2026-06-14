#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "CardStatData.h"
#include "DeckBuilderCardWidget.generated.h"

class UDeckBuilderCardDragDropOperation;
class UWidget;

UCLASS()
class CORECLAVE_API UDeckBuilderCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetCardStatData(const FCardStatData& Data);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetCardData(FName InCardId, int32 InCardCost);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetCardCount(int32 InCardCount);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetDraggingState(bool bInIsDragging);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FName GetCardId() const { return CardId; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetCardCost() const { return CardCost; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetCardCount() const { return CardCount; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	const FCardStatData& GetCachedCardData() const { return CachedCardData; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	UDeckBuilderCardDragDropOperation* CreateCardDragDropOperation(UWidget* DefaultDragVisual, EDragPivot Pivot = EDragPivot::MouseDown, FVector2D Offset = FVector2D::ZeroVector) const;

	protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnCardDataChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnCardCountChanged(int32 InCardCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnDragStateChanged(bool bInIsDragging);

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	FCardStatData CachedCardData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	FName CardId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	int32 CardCost = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	int32 CardCount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	bool bIsDragging = false;
};
