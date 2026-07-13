// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckBuilderDeckEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UDeckBuilderDeckEntryWidget::SetEntryData(const FDeckBuilderDeckEntryViewData& InEntryData)
{
	CachedEntryData = InEntryData;
	
	// 저장 데이터를 실제 UI에 반영
	UpdateVisuals();

	// 블루프린트 딴에서 추가 연출 처리
	BP_OnEntryDataChanged();
}

void UDeckBuilderDeckEntryWidget::UpdateVisuals()
{
	// 카드 이름 텍스트 반영
	if (CardNameText)
	{
		CardNameText->SetText(CachedEntryData.CardName);
	}

	// 카드 코스트 텍스트 반영
	if (CardCostText)
	{
		CardCostText->SetText(FText::AsNumber(CachedEntryData.CardCost));
	}

	// 덱에 들어간 장수 텍스트 반영
	if (DeckCountText)
	{
		DeckCountText->SetText(FText::AsNumber(CachedEntryData.DeckCount));
	}

	// 카드 아이콘 이미지 반영
	if (CardIconImage)
	{
		if (!CachedEntryData.CardIcon.IsNull())
		{
			// 현재는 동기로드 방식으로 사용
			UTexture2D* LoadedTexture = CachedEntryData.CardIcon.LoadSynchronous();
			if (LoadedTexture)
			{
				CardIconImage->SetBrushFromTexture(LoadedTexture);
			}
		}
		else
		{
			// 아이콘이 없으면 비워줌
			CardIconImage->SetBrushFromTexture(nullptr);
		}
	}
}

