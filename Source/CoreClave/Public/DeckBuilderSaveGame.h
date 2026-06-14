#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DeckBuilderTypes.h"
#include "DeckBuilderSaveGame.generated.h"

UCLASS()
class CORECLAVE_API UDeckBuilderSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	FDeckCollection DeckCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	TMap<FName, int32> OwnedCardCounts;
};
