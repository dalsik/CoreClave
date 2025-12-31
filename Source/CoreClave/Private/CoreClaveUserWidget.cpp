// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreClaveUserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UCoreClaveUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 테스트 : 게임 시작 시 카드 1장 가지고 시작
	UUserWidget* NewCard = CreateWidget<UUserWidget>(GetOwningPlayer(), CardWidgetClass);
	HandCards.Add(NewCard);
}



void UCoreClaveUserWidget::AddCardTohand()
{
	// 예외 처리 -> 카드 클래스가 없거나 패널 할당되지 않으면 에러
	if (!CardWidgetClass || !CardHandPanel) return;
	
	// 최대 카드 확인
	if (HandCards.Num() >= 6)
	{
		UE_LOG(LogTemp, Warning, TEXT("Card is FULL"));
		return;
	}

	// 카드 위젯 생성
	UUserWidget* NewCard = CreateWidget<UUserWidget>(GetOwningPlayer(), CardWidgetClass);

	if (NewCard)
	{
		// 패널에 자식으로 추가
		CardHandPanel->AddChildToCanvas(NewCard);

		// 관리 배열에 추가
		HandCards.Add(NewCard);

		// 함수 호출
		UpdateHandLayout();
	}

}

// 카드를 부채꼴로 만들어주는 함수

void UCoreClaveUserWidget::UpdateHandLayout()
{
	int32 CardCount = HandCards.Num();
	if (CardCount == 0) return;

	// 카드가 1장이면 중앙에 카만히 배치한다
	// 1장 이후부터는 각도를 계산하도록 한다.
	float CurrentSpread = (CardCount > 1) ? MaxSpreadAngle : 0.0f;

	// 카드가 적을 때는 넓게 퍼지지 않도록 보정한다.
	if (CardCount < 6)
	{
		CurrentSpread *= (float)(CardCount - 1) / 5.0f;
	}

	float AngleStep = (CardCount > 1) ? (CurrentSpread / (CardCount - 1)) : 0.0f;
	float StartAngle = -CurrentSpread / 2.0f; //왼쪽 끝 각도
	
	// 반복문으로 각 카드의 위치와 회전 설정
	for (int32 i = 0; i < CardCount; i++)
	{
		UUserWidget* Card = HandCards[i];
		if (!Card) return;

		// 캔버스 슬롯 가져오기 ( 위치 조정을 하기 위해서)
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Card->Slot);
		if (CanvasSlot)
		{
			// 앵커를 중앙 하단으로 설정
			CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f));
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // 피벗 중심
			CanvasSlot->SetAutoSize(true); // 카드 본래의 크기를 유지하도록 한다.


			// 각도 계산
			float TargetAngle = StartAngle + (i * AngleStep);
			
			// 원형좌표 계산(Sin, Cos 활용)
			// 반지름을 이용해 원 둘레상의 좌표를 구한다.
			float Radian = FMath::DegreesToRadians(TargetAngle - 90.0f); // -90도는 12방향을 0도로 맞추기 위함.

			float X = ArchRadius * FMath::Cos(Radian);
			float Y = ArchRadius * FMath::Sin(Radian);

			// 원의 중심이 화면 아래쪽에 있다고 가정하고 보정
			Y += ArchRadius;

			// 위치 적용
			CanvasSlot->SetPosition(FVector2D(X, Y));

			// 회전 적용
			Card->SetRenderTransformAngle(TargetAngle);

		}

	}
}





