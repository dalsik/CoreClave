// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreClaveUserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UCoreClaveUserWidget::NativeConstruct()
{
	Super::NativeConstruct();


	if (CardWidgetClass && CardHandPanel) {
		for (int32 i = 0; i < START_CARD_NUMS; i++) {
			//게임 시작 시 카드 가지고 시작(변수는 에디터에 쉽게 적용할 수 있도록)
			UUserWidget* NewCard = CreateWidget<UUserWidget>(GetOwningPlayer(), CardWidgetClass);
			if (NewCard) {
				CardHandPanel->AddChildToCanvas(NewCard);

				// 관리할 수 있도록 카드 배열에 추가하고
				HandCards.Add(NewCard);

				// 덱 위치로 초기화 (중여기서 덱 위치로 안 보내면 화면 중앙에서 뿅 하고 나타남)
				if (WBP_CardPack)
				{
					UCanvasPanelSlot* NewSlot = Cast<UCanvasPanelSlot>(NewCard->Slot);
					UCanvasPanelSlot* DeckSlot = Cast<UCanvasPanelSlot>(WBP_CardPack->Slot);
					if (NewSlot && DeckSlot)
					{
						NewSlot->SetPosition(DeckSlot->GetPosition());
					}
				}

				//0.0f 전달 (처음엔 보간 없이 즉시 위치 잡고 싶으면 로직 분리 필요하지만, 일단 0 전달)
				UpdateHandLayout(0.0f);
			}
		}
	}
}

void UCoreClaveUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	UpdateHandLayout(InDeltaTime);
}


void UCoreClaveUserWidget::AddCardTohand()
{
	// 예외 처리 -> 카드 클래스가 없거나 패널 할당되지 않으면 에러
	if (!CardWidgetClass || !CardHandPanel) return;
	
	// 최대 카드 확인
	if (HandCards.Num() >= MAX_CARD_NUMS)
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

		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(NewCard->Slot);

		if (CanvasSlot && WBP_CardPack)
		{
			UCanvasPanelSlot* DeckSlot = Cast<UCanvasPanelSlot>(WBP_CardPack->Slot);
			if (DeckSlot)
			{
				CanvasSlot->SetPosition(DeckSlot->GetPosition());
			}
		}
	}
}

// 카드를 부채꼴로 만들어주는 함수

void UCoreClaveUserWidget::UpdateHandLayout(float DeltaTime)
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


			bool bIsHovered = Card->IsHovered();

			// 각도 계산
			float TargetAngle = bIsHovered ? 0.0f : StartAngle + (i * AngleStep);
			
			// 원형좌표 계산(Sin, Cos 활용)
			// 반지름을 이용해 원 둘레상의 좌표를 구한다. (Sin,Cos는 각도가 아니라, Radian을 사용하기 때문에 변환 필요)
			float Radian = FMath::DegreesToRadians(TargetAngle - 90.0f); // -90도는 12방향을 0도로 맞추기 위함.
			// 수학에서의 0도는 오른쪽 3시방향을 의미하기 때문에 -90도를  하는거임.


			float TargetX = ArchRadius * FMath::Cos(Radian);
			float TargetY = ArchRadius * FMath::Sin(Radian) + ArchRadius;

			if (bIsHovered) {
				TargetY -= MouseEnter_TargetY;
			}

			FVector2D TargetPos(TargetX, TargetY);

			FVector2D CurrentPos = CanvasSlot->GetPosition();
			float InterpSpeed = bIsHovered ? 15.0f : Power;

			//현재 위치에서 목표 위치로 10의 속도로 부드럽게 이동(InterpTo)
			FVector2D NewPos = FMath::Vector2DInterpTo(CurrentPos, TargetPos, DeltaTime, Power);

			// 위치 적용
			CanvasSlot->SetPosition(NewPos);

			
			// 회전 적용
			float CurrentAngle = Card->GetRenderTransform().Angle;
			float NewAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, Power);
			Card->SetRenderTransformAngle(NewAngle);
		}
	}
}