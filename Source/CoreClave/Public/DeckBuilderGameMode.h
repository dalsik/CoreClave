#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DeckBuilderGameMode.generated.h"

class ADeckBuilderPlayerController;

UCLASS()
class CORECLAVE_API ADeckBuilderGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADeckBuilderGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Deck Builder")
	FName BattleMapName = TEXT("BattleMap");
};
