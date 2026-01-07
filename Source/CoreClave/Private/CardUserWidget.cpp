// Fill out your copyright notice in the Description page of Project Settings.


#include "CardUserWidget.h"

void UCardUserWidget::SetCardData(const FCardStatData& Data)
{
	// Padding용 변수에도 할당 진행
	CachedCardData = Data;

	// 아이콘 설정 로직
	if (!Data.CardIcon.IsNull())
	{
		// UI 아이콘은 즉시 필요하므로 동기 로드를 사용하도록 한다
		UTexture2D* LoadedTexture = Data.CardIcon.LoadSynchronous();
		
		if (ICON_UI && LoadedTexture)
		{
			ICON_UI->SetBrushFromTexture(LoadedTexture);
		}
	}
}