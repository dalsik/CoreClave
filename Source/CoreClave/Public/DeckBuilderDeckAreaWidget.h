#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderDeckAreaWidget.generated.h"

class UDragDropOperation;
class USoundBase;

UCLASS()
class CORECLAVE_API UDeckBuilderDeckAreaWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TObjectPtr<USoundBase> CardDropSound;

protected:
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_SetHoverState(bool bIsHovered);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void SetCardDropSound(USoundBase* InSound)
	{
		CardDropSound = InSound;
	}

private:
	bool TryResolveCardIdFromOperation(UDragDropOperation* InOperation, FName& OutCardId) const;
	bool CanAcceptCardDrop(UDragDropOperation* InOperation, FName& OutCardId) const;
};
