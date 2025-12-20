// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoreClaveUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class CORECLAVE_API UCoreClaveUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 카드를 뽑는 효과 함수
	UFUNCTION(BlueprintCallable, Category = "Card Motion")
	void PlayerDrawAnimation();

protected:
	// 위젯 블루프린트에 있는 애니메이션을 가져오기 위한 함수
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* DrawAnim;
};
