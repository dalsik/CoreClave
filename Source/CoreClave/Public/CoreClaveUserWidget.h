// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CoreClaveUserWidget.generated.h"

/**
 * 
 */

class UCanvasPanel;

UCLASS()
class CORECLAVE_API UCoreClaveUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Card System")
	void AddCardTohand();

	// 카드를 부채꼴로 유지하는 함수
	void UpdateHandLayout(float InDeltaTime);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	
	// 카드가 배치될 캔버스 패널
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* CardHandPanel;

	// 생성할 카드 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Card System")
	TSubclassOf<UUserWidget> CardWidgetClass;

	// 현재 들고 있는 카드들의 목록 관리
	UPROPERTY()
	TArray<UUserWidget*> HandCards;

	UPROPERTY(EditAnywhere, Category = "Card System");
	UDataTable* UnitDataTable;

	UPROPERTY(EditAnywhere, Category = "Card System")
	int32 MAX_CARD_NUMS;
	
	UPROPERTY(EditAnywhere, Category = "Card System")
	int32 START_CARD_NUMS;

	UPROPERTY(EditAnywhere, Category = "Card System")
	int32 MouseEnter_TargetY;

	UPROPERTY(meta = (BindWidgetOptional)) // 카드팩 WBP 자동 할당.
	class UWidget* WBP_CardPack;

	UPROPERTY(EditAnywhere, CateGory = "Card System")
	float Power;

	// 부채꼴 모양 설정값
	const float ArchRadius = 800.0f; // 부채꼴 반지름 (클수록 완만함)
	const float MaxSpreadAngle = 40.0f; // 카드가 최대로 펼쳐질 각도 범위
};
