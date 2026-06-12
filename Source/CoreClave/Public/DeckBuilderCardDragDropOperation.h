#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "DeckBuilderCardDragDropOperation.generated.h"

class UDeckBuilderCardWidget;

UCLASS()
class CORECLAVE_API UDeckBuilderCardDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deck Builder")
	FName CardId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deck Builder")
	TObjectPtr<UDeckBuilderCardWidget> SourceCardWidget;
};
