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

void UCardUserWidget::SetDraggingState(bool bDragging)
{
	bIsDragging = bDragging;
}

void UCardUserWidget::ResetCardVisual()
{
	// 위치 초기화
	bIsDragging = false;
	bIgnoreHoverOnce = true; // 해당 변수로 비주얼을 한번 리셋을 해주어야 CoreClaveUserWidget에서 호버 효과가 다시 작동할 수 있게 한다.
	SetRenderTransformAngle(0.f);
	SetRenderTranslation(FVector2D::ZeroVector);
	SetRenderScale(FVector2D(1.f, 1.f));
}

void UCardUserWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	bIgnoreHoverOnce = false; // 마우스가 카드를 벗어날 때 리셋
}
