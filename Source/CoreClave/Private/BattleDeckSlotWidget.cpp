// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleDeckSlotWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBattleDeckSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DeckButton)
	{
		// NativeConstruct가 재호출될 경우를 대비해 기존 연결을 제거
		DeckButton->OnClicked.RemoveDynamic(this, &UBattleDeckSlotWidget::HandleDeckButtonClicked);
		
		DeckButton->OnClicked.AddDynamic(this, &UBattleDeckSlotWidget::HandleDeckButtonClicked);
	}
}

void UBattleDeckSlotWidget::NativeDestruct()
{
	// 위젯이 제거될 때 이벤트 연결을 정리.
	if (DeckButton)
	{
		DeckButton->OnClicked.RemoveDynamic(
			this,
			&UBattleDeckSlotWidget::HandleDeckButtonClicked
		);
	}

	Super::NativeDestruct();
}

void UBattleDeckSlotWidget::SetDeckSlotData(int32 InDeckIndex, const FDeckData& InDeckData, bool bInBattleReady)
{
	// 부모가 전달한 덱의 인덱스와 덱 데이터를 저장
	DeckIndex = InDeckIndex;
	CachedDeckData = InDeckData;

	/// 이 부분은 추후에 편성창에도 넣을려면 넣어야되긴 함!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// 덱 이름 표시
	if (DeckNameText)
	{
		DeckNameText->SetText(FText::FromName(CachedDeckData.DeckName));
	}

	// 카드 수 20/20 형태로 표시
	if (CardCountText)
	{
		CardCountText->SetText(FText::AsNumber(CachedDeckData.CardIds.Num()));
	}

	// 부모 위젯에서 검사한 덱 사용 가능 여부를 저장한다
	bBattleReady = bInBattleReady;

	// 완성 여부에 따른 색상이나 표현은 BP에서 설정
	BP_OnBattleReadyChanged(bBattleReady);
}

void UBattleDeckSlotWidget::SetSelected(bool bInSelected)
{
	// 상태가 달라지지 않았다면 불필요한 갱신 X
	if (bSelected == bInSelected)
	{
		return;
	}

	bSelected = bInSelected;

	// 추가적인 연출은 BP에서
	BP_OnSelectionChanged(bSelected);
}

void UBattleDeckSlotWidget::HandleDeckButtonClicked()
{
	// 유효한 슬롯만 부모 위젯에 클릭 이벤트를 전달
	if (DeckIndex != INDEX_NONE)
	{
		OnDeckSlotClicked.Broadcast(DeckIndex);
	}
}

