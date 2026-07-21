// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "BattleDeckSelectWidget.generated.h"


class UBattleDeckDetailWidget;
class UBattleDeckSlotWidget;
class UButton;
class UTextBlock;
class UDeckBuilderSubsystem;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class CORECLAVE_API UBattleDeckSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	// 최신 덱 목록을 가져와서 저장된 덱 슬롯들을 다시 생성
	UFUNCTION(BlueprintCallable, Category = "Battle Deck")
	void RefreshSavedDecks();

	// 현재 선택된 덱의 인덱스 반환
	UFUNCTION(BlueprintPure, Category = "Battle Deck")
	int32 GetSelectedDeckIndex() const
	{
		return SelectedDeckIndex;
	}

	// 선택된 데이터 복사해서 반환
	UFUNCTION(BlueprintPure, Category = "Battle Deck")
	FDeckData GetSelectedDeckData() const;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	// 개별 덱 슬롯들이 동적으로 들어갈 패널
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> DeckSlotPanel;

	// 오른쪽의 디테일 패널
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBattleDeckDetailWidget> DeckDetailWidget;

	// 저장된 덱이 하나도 없을 때 표시할 안내 문구
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyDeckMessageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EditDeckButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartMatchingButton;
	
	// 동적으로 생성할 개별 덱 슬롯 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Deck")
	TSubclassOf<UBattleDeckSlotWidget> DeckSlotWidgetClass;

	// 뒤로가기 버튼 클릭 BP 요청
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Deck|Navigation")
	void BP_OnBackRequested();
	
	// 선택된 덱을 편집해달라는 요청
	UFUNCTION(BlueprintImplementableEvent,Category = "Battle Deck|Navigation")
	void BP_OnEditDeckRequested(int32 DeckIndex);

	// 선택된 덱으로 전투 요청 
	// 이부분은 추후에 RPC로 검증해야 한다.ㅇ
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Deck")
	void BP_OnMatchRequested(int32 DeckIndex, const FDeckData& SelectedDeckData);

	// 매칭 시작 시 덱 검사 통과 못하면 호출되는 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle Deck")
	void BP_OnDeckValidationFailed(const FText& FailureReason);
	
private:
	
	// DeckBuilderSubSystem의 덱 목록 변경 이벤트 처리함수
	UFUNCTION()
	void HandleDeckCollectionChanged(FDeckCollection UpdatedCollection);

	/**
	 * 개별 덱 슬롯을 클릭했을 때 호출된다.
	 */
	UFUNCTION()
	void HandleDeckSlotClicked(int32 DeckIndex);

	UFUNCTION()
	void HandleBackButtonClicked();

	UFUNCTION()
	void HandleEditDeckButtonClicked();

	UFUNCTION()
	void HandleStartMatchingButtonClicked();

	/**
	 * CachedDeckCollection을 기반으로
	 * 슬롯 위젯들을 다시 생성한다.
	 */
	void RebuildDeckSlots();

	/**
	 * 지정된 실제 덱 인덱스를 현재 선택 상태로 만든다.
	 */
	void SelectDeckByIndex(int32 DeckIndex);

	/**
	 * 현재 선택 상태를 개별 슬롯들의 비주얼에 적용한다.
	 */
	void UpdateSlotSelectionVisuals();

	/**
	 * 현재 선택된 덱에 따라 버튼 활성화 상태를 변경한다.
	 */
	void UpdateButtonStates();

	/**
	 * 현재 선택을 초기화하고 상세 패널을 비운다.
	 */
	void ClearCurrentSelection();

	/**
	 * 현재 선택된 덱의 포인터를 반환한다.
	 *
	 * CachedDeckCollection 내부 데이터를 가리키므로
	 * 컬렉션이 갱신된 뒤에는 다시 호출해야 한다.
	 */
	const FDeckData* GetSelectedDeckPtr() const;

	/**
	 * GameInstance에서 DeckBuilderSubsystem을 가져온다.
	 */
	UDeckBuilderSubsystem* GetDeckBuilderSubsystem() const;

	/**
	 * Subsystem에서 가져온 현재 덱 목록 복사본.
	 */
	UPROPERTY(Transient)
	FDeckCollection CachedDeckCollection;

	/**
	 * 현재 화면에 생성된 개별 덱 슬롯 위젯들.
	 */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UBattleDeckSlotWidget>> CreatedDeckSlots;

	/**
	 * DeckCollection.Decks 배열에서의 실제 선택 인덱스.
	 *
	 * 화면에서 저장되지 않은 덱은 제외되므로
	 * 화면상의 순번과 실제 인덱스는 다를 수 있다.
	 */
	int32 SelectedDeckIndex = INDEX_NONE;
};
