#pragma once

#include "CoreMinimal.h"
#include "DeckBuilderDeckEntryTypes.generated.h"


class UTexture2D;

USTRUCT(BlueprintType)
struct FDeckBuilderDeckEntryViewData
{
	GENERATED_BODY()

	// 카드 ID
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	FName CardId = NAME_None;

	// 카드 이름
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	FText CardName = FText::GetEmpty();

	// 카드 코스트
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	int32 CardCost = 0;

	// 현재 덱에 들어간 장수
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	int32 DeckCount = 0;

	// 카드 HP
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	int32 CardHP = 0;

	// 카드 SPD
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	int32 CardSPD = 0;

	// 카드 아이콘
	UPROPERTY(BlueprintReadOnly, Category = "Deck Builder")
	TSoftObjectPtr<UTexture2D> CardIcon;
};