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
	/**
	 * SaveGame 데이터 형식 버전.
	 *
	 * 기존 SaveGame에는 이 필드가 없으므로 로드 시 기본값 0을 유지한다.
	 * 새로운 형식으로 저장할 때는 CurrentSaveDataVersion을 명시적으로 기록한다.
	 */
	static constexpr int32 CurrentSaveDataVersion = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Save Version")
	int32 SaveDataVersion = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	FDeckCollection DeckCollection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Deck")
	TMap<FName, int32> OwnedCardCounts;
};
