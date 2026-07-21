// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleDeckSelectWidget.h"

#include "BattleDeckDetailWidget.h"
#include "BattleDeckSlotWidget.h"
#include "DeckBuilderSubsystem.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/TextBlock.h"

#include "Engine/GameInstance.h"


void UBattleDeckSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 중복연결 방지
	
	if (BackButton)
	{
		BackButton->OnClicked.RemoveDynamic(this, &UBattleDeckSelectWidget::HandleBackButtonClicked);

		BackButton->OnClicked.AddDynamic(this, &UBattleDeckSelectWidget::HandleBackButtonClicked);
	}

	if (EditDeckButton)
	{
		EditDeckButton->OnClicked.RemoveDynamic(this, &UBattleDeckSelectWidget::HandleEditDeckButtonClicked);

		EditDeckButton->OnClicked.AddDynamic(this, &UBattleDeckSelectWidget::HandleEditDeckButtonClicked);
	}

	if (StartMatchingButton)
	{
		StartMatchingButton->OnClicked.RemoveDynamic(this, &UBattleDeckSelectWidget::HandleStartMatchingButtonClicked);

		StartMatchingButton->OnClicked.AddDynamic(this, &UBattleDeckSelectWidget::HandleStartMatchingButtonClicked);
	}
	
	// 덱 편성 데이터가 변경되었을 때 선택 화면 갱신하기 위한 이벤트 구독
	if (UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem())
	{
		DeckSubsystem->OnDeckCollectionChanged.RemoveDynamic(this, &UBattleDeckSelectWidget::HandleDeckCollectionChanged);

		DeckSubsystem->OnDeckCollectionChanged.AddDynamic(this,&UBattleDeckSelectWidget::HandleDeckCollectionChanged);
	}

	// 화면 최초 진입 시 저장된 덱 슬롯 생성
	RefreshSavedDecks();
}

void UBattleDeckSelectWidget::NativeDestruct()
{
	if(BackButton)
	{
		BackButton->OnClicked.RemoveDynamic(
			this,
			&UBattleDeckSelectWidget::HandleBackButtonClicked
		);
	}

	if (EditDeckButton)
	{
		EditDeckButton->OnClicked.RemoveDynamic(
			this,
			&UBattleDeckSelectWidget::HandleEditDeckButtonClicked
		);
	}

	if (StartMatchingButton)
	{
		StartMatchingButton->OnClicked.RemoveDynamic(
			this,
			&UBattleDeckSelectWidget::
			HandleStartMatchingButtonClicked
		);
	}

	if (UDeckBuilderSubsystem* DeckSubsystem =
		GetDeckBuilderSubsystem())
	{
		DeckSubsystem->OnDeckCollectionChanged.RemoveDynamic(
			this,
			&UBattleDeckSelectWidget::
			HandleDeckCollectionChanged
		);
	}

	/*
	 * 생성한 개별 슬롯이 부모 위젯을 가리키는
	 * 델리게이트 연결을 정리한다.
	 */
	for (UBattleDeckSlotWidget* SlotWidget : CreatedDeckSlots)
	{
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->OnDeckSlotClicked.RemoveDynamic(
			this,
			&UBattleDeckSelectWidget::HandleDeckSlotClicked
		);
	}

	CreatedDeckSlots.Reset();

	Super::NativeDestruct();
}

void UBattleDeckSelectWidget::RefreshSavedDecks()
{
	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();


	if (!DeckSubsystem)
	{
		CachedDeckCollection = FDeckCollection();
		
		ClearCurrentSelection();
		return;
	}

	// Subsystem에서 이미 SaveGame을 불러오고 있으므로 다시 호출 x
	CachedDeckCollection = DeckSubsystem->GetDeckCollection();
	
	RebuildDeckSlots();
}

void UBattleDeckSelectWidget::HandleDeckCollectionChanged(FDeckCollection UpdatedCollection)
{
	// 이벤트를 통해 최신 컬렉션 전달받아서 다시 조회 X
	CachedDeckCollection = UpdatedCollection;
	
	RebuildDeckSlots();
}

// 저장된 슬롯에 대해서 생성하는 메서드
void UBattleDeckSelectWidget::RebuildDeckSlots()
{
	if (!DeckSlotPanel)
	{
		ClearCurrentSelection();
		return;
	}

	// 기존 슬롯들의 클릭 이벤트 해제
	for (UBattleDeckSlotWidget* SlotWidget : CreatedDeckSlots)
	{
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->OnDeckSlotClicked.RemoveDynamic(this,&UBattleDeckSelectWidget::HandleDeckSlotClicked);
	}

	DeckSlotPanel->ClearChildren();
	CreatedDeckSlots.Reset();


	// 새로 생성 후에도 선택 상태 유지위해 기존 인덱스 보관
	const int32 PreviousSelectedIndex = SelectedDeckIndex;

	SelectedDeckIndex = INDEX_NONE;
	
	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();

	if (!DeckSubsystem || !DeckSlotWidgetClass)
	{
		ClearCurrentSelection();
		return;
	}
	
	const int32 MaxDecksize = DeckSubsystem->GetMaxDeckSize();
	
	int32 FirstSavedDeckIndex = INDEX_NONE;

	for (int32 DeckIndex = 0; DeckIndex < CachedDeckCollection.Decks.Num(); ++DeckIndex)
	{
		const FDeckData& DeckData = CachedDeckCollection.Decks[DeckIndex];


		// 기본으로 생성된 빈 슬롯은 표시 X, 사용자가 저장 버튼 누른 덱만 표시
		if (!DeckData.bHasBeenSaved)
		{
			continue;
		}

		FText FailureReason;

		// 규칙은 직접 판단안하고 Subsystem이 담다아
		const bool bBattleReady = DeckSubsystem->ValidateDeckData(DeckData, FailureReason);

		UBattleDeckSlotWidget* NewSlotWidget = CreateWidget<UBattleDeckSlotWidget>(this, DeckSlotWidgetClass);

		if (!NewSlotWidget)
		{
			continue;
		}

		// 슬롯은 데이터와 검사 결과를 화면에 표시
		NewSlotWidget->SetDeckSlotData(
			DeckIndex,
			DeckData,
			bBattleReady
		);

		NewSlotWidget->OnDeckSlotClicked.AddDynamic(this, &UBattleDeckSelectWidget::HandleDeckSlotClicked);

		// 가로박스에 슬롯 자식으로 추가
		UHorizontalBoxSlot* HorizontalSlot = DeckSlotPanel->AddChildToHorizontalBox(NewSlotWidget);

		if (HorizontalSlot)
		{
			HorizontalSlot->SetPadding(FMargin(8.0f, 0.0f));
		}

		CreatedDeckSlots.Add(NewSlotWidget);

		if (FirstSavedDeckIndex == INDEX_NONE)
		{
			FirstSavedDeckIndex = DeckIndex;
		}

	}
	const bool bHasSavedDeck = FirstSavedDeckIndex != INDEX_NONE;

	// 만약 저장이 되었다면 메시지창 안보이게, 안되었다면 메시지창 보이게
	if (EmptyDeckMessageText)
	{
		EmptyDeckMessageText->SetVisibility(bHasSavedDeck ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}

	if (!bHasSavedDeck)
	{
		ClearCurrentSelection();
		return;
	}

	/// 슬롯 재생성 전에 사용자가 선택했던 덱 유지
	if (CachedDeckCollection.Decks.IsValidIndex(PreviousSelectedIndex))
	{
		const FDeckData& PreviousDeck = CachedDeckCollection.Decks[PreviousSelectedIndex];

		if (PreviousDeck.bHasBeenSaved)
		{
			SelectDeckByIndex(PreviousSelectedIndex);
			return;
		}
	}
		
	// Subsystem에 저장된 선택 인덱스 활용
	const int32 SubsystemSelectedIndex = CachedDeckCollection.SelectedDeckIndex;

	if (CachedDeckCollection.Decks.IsValidIndex(SubsystemSelectedIndex))
	{
		const FDeckData& SubsystemSelectedDeck = CachedDeckCollection.Decks[SubsystemSelectedIndex];

		if (SubsystemSelectedDeck.bHasBeenSaved)
		{
			SelectDeckByIndex(SubsystemSelectedIndex);
			return;
		}
	}
	
	// 선택 상태 없다면 첫번째 저장덱 선택
	SelectDeckByIndex(FirstSavedDeckIndex);
}

void UBattleDeckSelectWidget::HandleDeckSlotClicked(int32 DeckIndex)
{
	SelectDeckByIndex(DeckIndex);
}

void UBattleDeckSelectWidget::SelectDeckByIndex(int32 DeckIndex)
{
	// 만약 데이터가 없다면 그냥종료
	if (!CachedDeckCollection.Decks.IsValidIndex(DeckIndex))
	{
		return;
	}
	
	const FDeckData& SelectedDeck = CachedDeckCollection.Decks[DeckIndex];

	// 저장되지 않은 덱은 선택 화면의 대상이 아니다.
	if (!SelectedDeck.bHasBeenSaved)
	{
		return;
	}
	SelectedDeckIndex = DeckIndex;

	// 왼쪽 슬롯의 선택 테두리 등을 갱신한다.
	UpdateSlotSelectionVisuals();


	/*
	 * 오른쪽 상세 패널에 선택된 덱을 전달한다.
	 *
	 * 평균 비용과 주요 카드 계산은
	 * BattleDeckDetailWidget이 담당한다.
	 */
	if (DeckDetailWidget)
	{
		UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();
		const int32 MaxDeckSize = DeckSubsystem ? DeckSubsystem->GetMaxDeckSize() : 20;
		DeckDetailWidget->SetDeckData(SelectedDeck, MaxDeckSize);
	}

	UpdateButtonStates();
}

// 선택된 슬롯의 비주얼 업데이트
void UBattleDeckSelectWidget::UpdateSlotSelectionVisuals()
{
	for (UBattleDeckSlotWidget* SlotWidget : CreatedDeckSlots)
	{
		if (!SlotWidget)
		{
			continue;
		}

		const bool bShouldBeSelected = SlotWidget->GetDeckIndex() == SelectedDeckIndex;

		SlotWidget->SetSelected(bShouldBeSelected);
	}
}


void UBattleDeckSelectWidget::UpdateButtonStates()
{
	const FDeckData* SelectedDeck = GetSelectedDeckPtr();

	const bool bHasSelection = SelectedDeck != nullptr;

	/*
	 * 저장된 덱이 선택되어 있다면 카드 수가 부족하더라도
	 * 편성 화면으로 들어갈 수 있어야 한다.
	 */
	if (EditDeckButton)
	{
		EditDeckButton->SetIsEnabled(bHasSelection);
	}

	bool bCanStartMatching = false;

	if (bHasSelection)
	{
		if (UDeckBuilderSubsystem* DeckSubsystem =
			GetDeckBuilderSubsystem())
		{
			FText FailureReason;

			bCanStartMatching = DeckSubsystem->ValidateDeckData(*SelectedDeck, FailureReason);
		}
	}

	if (StartMatchingButton)
	{
		StartMatchingButton->SetIsEnabled(bCanStartMatching);
	}
}

/// <summary>
/// 현재 슬롯들을 안보이게 설정
/// </summary>
void UBattleDeckSelectWidget::ClearCurrentSelection()
{
	SelectedDeckIndex = INDEX_NONE;

	// 모든 슬롯의 선택 연출을 해제한다.
	UpdateSlotSelectionVisuals();

	// 상세 정보는 전용 자식 위젯에서 초기화한다.
	if (DeckDetailWidget)
	{
		DeckDetailWidget->ClearDeckData();
	}

	if (EditDeckButton)
	{
		EditDeckButton->SetIsEnabled(false);
	}

	if (StartMatchingButton)
	{
		StartMatchingButton->SetIsEnabled(false);
	}
}

// 선택된 데이터의 포인터 주소 반환
const FDeckData* UBattleDeckSelectWidget::GetSelectedDeckPtr() const
{
	if (!CachedDeckCollection.Decks.IsValidIndex(SelectedDeckIndex))
	{
		return nullptr;
	}

	const FDeckData& SelectedDeck = CachedDeckCollection.Decks[SelectedDeckIndex];

	if (!SelectedDeck.bHasBeenSaved)
	{
		return nullptr;
	}

	return &SelectedDeck;
}

FDeckData UBattleDeckSelectWidget::GetSelectedDeckData() const
{

	// 선택한 데이터의 포인터가 존재하면 반환, 없으면 일반 구조체 반환
	const FDeckData* SelectedDeck = GetSelectedDeckPtr();

	return SelectedDeck ? *SelectedDeck : FDeckData();
}

void UBattleDeckSelectWidget::HandleBackButtonClicked()
{
	/*
	 * 위젯은 직접 OpenLevel을 호출하지 않는다.
	 * 실제 화면 이동은 Blueprint 또는 PlayerController가 처리한다.
	 */
	BP_OnBackRequested();
}

void UBattleDeckSelectWidget::HandleEditDeckButtonClicked()
{
	const FDeckData* SelectedDeck = GetSelectedDeckPtr();

	if (!SelectedDeck)
	{
		return;
	}

	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();

	if (!DeckSubsystem)
	{
		return;
	}

	/*
	 * 편성 화면으로 이동하기 전에
	 * 해당 덱을 WorkingDeck으로 설정한다.
	 */
	if (!DeckSubsystem->SelectDeckSlot(SelectedDeckIndex))
	{
		return;
	}

	BP_OnEditDeckRequested(SelectedDeckIndex);
}

/// <summary>
///  매칭 시작 시 호출하는 핸들러 이벤트 
/// 현재는 프로토타입 기능 구현이므로 RPC 검증은 제외. 
/// </summary>
void UBattleDeckSelectWidget::HandleStartMatchingButtonClicked()
{
	const FDeckData* SelectedDeck = GetSelectedDeckPtr();

	if (!SelectedDeck)
	{
		BP_OnDeckValidationFailed( FText::FromString(TEXT("Can't Find Selected Deck")));
		return;
	}

	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();

	if (!DeckSubsystem)
	{
		BP_OnDeckValidationFailed( FText::FromString(TEXT("Can't Find DeckSubsystem")));

		return;
	}

	FText FailureReason;

	/*
	 * 버튼 활성화 상태와 별개로
	 * 실제 시작 요청 시점에 한 번 더 검사한다.
	 */

	if (!DeckSubsystem->ValidateDeckData(*SelectedDeck,	FailureReason))
	{
		BP_OnDeckValidationFailed(FailureReason);
		return;
	}

	/*
	 * 프로토타입에서는 선택된 덱을 WorkingDeck으로 설정한다.
	 *
	 * DeckBuilderSubsystem은 GameInstanceSubsystem이므로
	 * 전투 레벨로 이동한 뒤에도 이 데이터가 유지된다.
	 */
	// ***********************추후에 인스턴스가 아니라 RPC 환경이나 서버 백그라운드에서 검증하고 레벨 넘어가야 한다.
	if (!DeckSubsystem->SelectDeckSlot(SelectedDeckIndex))
	{
		BP_OnDeckValidationFailed(FText::FromString(TEXT("선택한 덱을 적용하지 못했습니다.")));

		return;
	}

	/*
	 * SelectDeckSlot()을 호출하면 Subsystem 이벤트가 발생하면서
	 * CachedDeckCollection이 갱신될 수 있다.
	 *
	 * 따라서 갱신된 컬렉션에서 데이터를 다시 가져온다.
	 */

	const FDeckData* ConfirmedDeck = GetSelectedDeckPtr();

	if (!ConfirmedDeck)
	{
		BP_OnDeckValidationFailed(FText::FromString(TEXT("선택된 덱 정보를 확인하지 못했습니다.")));

		return;
	}

	BP_OnMatchRequested(SelectedDeckIndex,*ConfirmedDeck);
}

UDeckBuilderSubsystem* UBattleDeckSelectWidget::GetDeckBuilderSubsystem() const
{
	UGameInstance* GameInstance = GetGameInstance();

	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UDeckBuilderSubsystem>();
}