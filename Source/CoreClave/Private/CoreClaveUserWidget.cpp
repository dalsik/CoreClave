// Fill out your copyright notice in the Description page of Project Settings.


#include "CoreClaveUserWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CardUserWidget.h"
#include "CardStatData.h"

void UCoreClaveUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 로컬 플레이어의 소유인지 확인
	// 서버에서는 UI 로직을 실행하지 않도록 하기 위함
	APlayerController* PC = GetOwningPlayer();
	if (!PC || !PC->IsLocalController())
	{
		return; // 서버이거나 다른 클라이언트 소유면 중단
	}

	// 현재는 테스트를 위해서 임의의 덱 리스트를 생성
	PlayerDeckStack.Add(FName("Card_002_Nor"));
	PlayerDeckStack.Add(FName("Card_001_Nor"));
	PlayerDeckStack.Add(FName("Card_002_Nor"));
	PlayerDeckStack.Add(FName("Card_001_Nor"));
	PlayerDeckStack.Add(FName("Card_002_Nor"));
	PlayerDeckStack.Add(FName("Card_001_Nor"));
	PlayerDeckStack.Add(FName("Card_002_Nor"));



	if (CardWidgetClass && CardHandPanel && UnitDataTable) 
	{
		for (int32 i = 0; i < START_CARD_NUMS; i++)
		{
			CreateCardAndAddToHand();
		}

		bNeedHandLayoutUpdate = true;

		// 초기 배치 즉시 적용
		//UpdateHandLayout(0.0f);
	}
}


void UCoreClaveUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	APlayerController* PC = GetOwningPlayer();
	if (!PC || !PC->IsLocalController()) return;
	
	//if (!bNeedHandLayoutUpdate) return;

	UpdateHandLayout(InDeltaTime);
}


void UCoreClaveUserWidget::AddCardTohand()
{
	CreateCardAndAddToHand();
}

// 실제 카드 생성 로직
void UCoreClaveUserWidget::CreateCardAndAddToHand()
{
	if (!CardWidgetClass || !CardHandPanel || !UnitDataTable) return;
	
	if (HandCards.Num() >= MAX_CARD_NUMS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hand is FULL"));
		return;
	}

	// 현재 덱에 남은 카드가 있는지 확인
	// **********************************************************
	// 나중에 이 부분에서 카드를 다 쓰면 피가 닳게하던, 아니면 다시 덱에 있는 카드에서 뽑게 하던 해야할 듯.
	if (PlayerDeckStack.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Deck Is Empty"));
		return;
	}
	FName NextCardName = PlayerDeckStack[0];
	
	PlayerDeckStack.RemoveAt(0);
	
	// 데이터 테이블에서 해당 이름의 정보 탐색
	static const FString ContextString(TEXT("CardData Context"));
	FCardStatData* CardData = UnitDataTable->FindRow<FCardStatData>(NextCardName, ContextString);
	
	if (!CardData)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find card data for table"));
		return;
	}

	CardData->CardId = NextCardName;

	UCardUserWidget* NewCard = CreateWidget<UCardUserWidget>(GetOwningPlayer(), CardWidgetClass);

	if (NewCard)
	{
		NewCard->SetCardData(*CardData); // 카드 정보 설정 및 할당하기(CardUserWidget.cpp)
		
		// 핸드 패널에 추가
		CardHandPanel->AddChildToCanvas(NewCard);
		HandCards.Add(NewCard);

		// 위치 초기화
		if (WBP_CardPack)
		{
			UCanvasPanelSlot* NewSlot = Cast<UCanvasPanelSlot>(NewCard->Slot);
			UCanvasPanelSlot* DeckSlot = Cast<UCanvasPanelSlot>(WBP_CardPack->Slot);
			
			if (NewSlot && DeckSlot)
			{
				NewSlot->SetPosition(DeckSlot->GetPosition());
			}
		}
	}
	//bNeedHandLayoutUpdate = true; // 핸드 레이아웃 업데이트 설정
}


// 카드를 제거해주는 함수
void UCoreClaveUserWidget::ReMoveCardFromHand(UUserWidget* CardToRemove)
{
	if (!CardToRemove) return;

	if (HandCards.Contains(CardToRemove))
	{
		HandCards.Remove(CardToRemove);
	}

	CardToRemove->RemoveFromParent();

	UpdateHandLayout(0.0f);
	//bNeedHandLayoutUpdate = true;
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

		// 캔버스 슬롯 가져오기 (위치 조정을 하기 위해서)
		UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Card->Slot);
		if (CanvasSlot)
		{
			// 앵커를 중앙 하단으로 설정
			CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f));
			CanvasSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // 피벗 중심
			CanvasSlot->SetAutoSize(true); // 카드 본래의 크기를 유지하도록 한다.


			UCardUserWidget* CardWidget = Cast<UCardUserWidget>(Card);
			if (!CardWidget) continue;

			const bool bHoveredNow = CardWidget->IsHovered();

			/*
			마우스가 카드 위에 있다
			그리고 드래그 중이 아니다
			그리고 드롭 직후 한 번 무시해야 하는 상태도 아니다
			*/
			const bool bIsHovered = bHoveredNow
				&& !CardWidget->bIsDragging;


			// 각도 계산
			float DefaultAngle = StartAngle + (i * AngleStep);

			// 원형좌표 계산(Sin, Cos 활용)
			// 반지름을 이용해 원 둘레상의 좌표를 구한다. (Sin,Cos는 각도가 아니라, Radian을 사용하기 때문에 변환 필요)
			float Pos_Radian = FMath::DegreesToRadians(DefaultAngle - 90.0f); // -90도는 12방향을 0도로 맞추기 위함.
			// 수학에서의 0도는 오른쪽 3시방향을 의미하기 때문에 -90도를  하는거임.


			float TargetX = ArchRadius * FMath::Cos(Pos_Radian);
			float TargetY = ArchRadius * FMath::Sin(Pos_Radian) + ArchRadius;

			const float TargetScaleValue = bIsHovered ? 1.7f : 1.0f;

			float TargetAngle = DefaultAngle;
			if (bIsHovered)
			{
				if (!CardWidget->bIgnoreHoverOnce)
				{
					TargetAngle = 0.0f;
					TargetY -= MouseEnter_TargetY;
				}
			}

			FVector2D TargetPos(TargetX, TargetY);

			FVector2D CurrentPos = CanvasSlot->GetPosition();

			//현재 위치에서 목 표 위치로 10의 속도로 부드럽게 이동(InterpTo)
			FVector2D NewPos = FMath::Vector2DInterpTo(CurrentPos, TargetPos, DeltaTime, Power);

			// 위치 적용
			CanvasSlot->SetPosition(NewPos);

			// 회전 적용
			float CurrentAngle = Card->GetRenderTransform().Angle;
			float NewAngle = FMath::FInterpTo(CurrentAngle, TargetAngle, DeltaTime, Power);
			Card->SetRenderTransformAngle(NewAngle);

			// 스케일 적용
			FVector2D CurrentScale = Card->GetRenderTransform().Scale;
			FVector2D TargetScale(TargetScaleValue, TargetScaleValue);
			FVector2D NewScale = FMath::Vector2DInterpTo(CurrentScale, TargetScale, DeltaTime, Power);
			Card->SetRenderScale(NewScale);

		}
	}
	//bNeedHandLayoutUpdate = false;
}

// 이거 나중에 최적화 할떄 bool형 기반으로 판단하도록 바꿔야할듯. 지금은 테스트로 그냥 매번 업데이트 하도록 해놨음.
void UCoreClaveUserWidget::RequestHandLayOutUpdate()
{
	bNeedHandLayoutUpdate = true;
}

