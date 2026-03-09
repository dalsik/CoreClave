// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CardStatData.h"
#include "CardUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CORECLAVE_API UCardUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* ICON_UI;
	
	// Padding용 카드UI에도 적용하기 위한 변수
	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FCardStatData CachedCardData;

	// 이후에 공격력도 바인딩하도록

	// 데이터를 받아서 UI를 갱신하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	void SetCardData(const FCardStatData& Data);

	// CardId를 반환하는 함수
	UFUNCTION(BlueprintCallable, Category = "Data")
	FName GetCardId() const {
		return CachedCardData.CardId;
	}
};
