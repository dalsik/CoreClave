#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "DeckBuilderCardWidget.generated.h"

class UDeckBuilderCardDragDropOperation;
class UWidget;

UCLASS()
class CORECLAVE_API UDeckBuilderCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetCardData(FName InCardId, int32 InCardCost);

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetDraggingState(bool bInIsDragging);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FName GetCardId() const { return CardId; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetCardCost() const { return CardCost; }

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	UDeckBuilderCardDragDropOperation* CreateCardDragDropOperation(UWidget* DefaultDragVisual, EDragPivot Pivot = EDragPivot::MouseDown, FVector2D Offset = FVector2D::ZeroVector) const;

	protected:
		UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
		void BP_OnCardDataChanged();

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnDragStateChanged(bool bInIsDragging);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	FName CardId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	int32 CardCost = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	bool bIsDragging = false;
};
