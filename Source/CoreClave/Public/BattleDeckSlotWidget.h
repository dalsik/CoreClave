// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "BattleDeckSlotWidget.generated.h"


class UButton;
class UTextBlock;

// 개별 덱 슬롯을 클릭했을 때 실제 DeckCollection 배열 인덱스를 부모 위젯에 전달.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleDeckSlotClicked, int32, DeckIndex);
/**
 * 
 */
UCLASS()
class CORECLAVE_API UBattleDeckSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// 부모 위젯이 이 슬롯에 표시할 데이터를 전달한다.
	UFUNCTION(BlueprintCallable, Category = "Battle Deck")
	void SetDeckSlotData(int32 InDeckIndex, const FDeckData& InDeckData, bool bInBattleReady);


	// 현재 선택된 덱인지 설정한다.
	UFUNCTION(BlueprintCallable, Category = "Battle Deck")
	void SetSelected(bool bInSelected);

	// DeckCollection.Decks에서 이 덱의 실제 인덱스
	UFUNCTION(BlueprintPure, Category = "Battle Deck")
	int32 GetDeckIndex() const { return DeckIndex; }

	// 현재 슬롯이 기본적인 조건을 만족하는지?
	UFUNCTION(BlueprintPure, Category = "Battle Deck")
	bool IsBattleReady() const
	{
		return bBattleReady;
	}

	// 부모 위젯이 구독할 클릭 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Battle Deck")
	FOnBattleDeckSlotClicked OnDeckSlotClicked;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 버튼 
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DeckButton;

	// 덱 이름 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeckNameText;
	
	// 카드 수 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardCountText;

	// 선택 테두리와 애니메이션은 WBP에서 구현한다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Deck")
	void BP_OnSelectionChanged(bool bSelectedBP);

	// 완성 덱과 미완성 덱의 밝기·잠금 표시를 WBP에서 구현한다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Deck")
	void BP_OnBattleReadyChanged(bool bInBattleReady);
	
private:
	// 내부 버튼 클릭 처리
	UFUNCTION()
	void HandleDeckButtonClicked();

	// 현재 슬롯에 표시 중인 덱 데이터 복사본
	UPROPERTY(Transient)
	FDeckData CachedDeckData;
	
	// DeckCollection 배열 인덱스
	int32 DeckIndex = INDEX_NONE;

	// 현재 선택 상태
	bool bSelected = false;
	
	// 카드 수가 최대 덱 크기와 일치하는지
	bool bBattleReady = false;
};
