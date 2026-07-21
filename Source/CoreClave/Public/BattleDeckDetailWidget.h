// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderTypes.h"
#include "BattleDeckDetailWidget.generated.h"

class UDataTable;
class UImage;
class UTextBlock;
class UTexture2D;
struct FCardStatData;

/**
 *  * 담당:
 * - 덱 이름 표시
 * - 카드 수 표시
 * - 평균 카드 비용 계산 및 표시
 * - 주요 카드 3개 선정
 * - 주요 카드 아이콘 표시
 *
 * 담당하지 않음:
 * - 덱 선택 상태 관리
 * - 덱 사용 가능 여부 검사
 * - DeckBuilderSubsystem 접근
 * - 버튼 입력과 레벨 이동
 */


UCLASS()
class CORECLAVE_API UBattleDeckDetailWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**
	 * 부모 위젯으로부터 선택된 덱을 전달받아 상세 정보를 갱신한다.
	 *
	 * InMaxDeckSize는 카드 수를 "20 / 20" 형식으로 표시하기 위해 사용한다.
	 */

	UFUNCTION(BlueprintCallable, Category = "Battle Deck|Detail")
	void SetDeckData(const FDeckData& InDeckData, int32 InMaxDeckSize);

	/**
 * 선택된 덱이 없을 때 상세 패널을 초기화한다.
 */
	UFUNCTION(BlueprintCallable, Category = "Battle Deck|Detail")
	void ClearDeckData();

protected:
	virtual void NativeConstruct() override;

	// 선택된 덱 이름
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DeckNameText;

	// 현재 카드 수 / 최대 카드 수
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CardCountText;

	// 평균 카드 비용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AverageCostText;

	// 주요 카드 아이콘 1
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FeaturedCardImage1;

	// 주요 카드 아이콘 2
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FeaturedCardImage2;

	// 주요 카드 아이콘 3
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> FeaturedCardImage3;

	/**
	 * 카드 비용과 아이콘 정보를 조회할 데이터 테이블.
	 *
	 * WBP_BattleDeckDetail의 Class Defaults에서 카드 데이터 테이블을 지정한다.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle Deck|Data")
	TObjectPtr<UDataTable> CardStatDataTable;

private:
	/**
	 * CardId에 해당하는 카드 데이터를 찾는다.
	 *
	 * 먼저 DataTable RowName으로 검색하고, 찾지 못하면 FCardStatData::CardId를 비교한다.
	 */
	const FCardStatData* FindCardData(FName CardId) const;

	/**
	 * 소프트 텍스처를 동기 로드하여 Image에 적용한다.
	 *
	 * 현재는 최대 3개만 로드하므로 프로토타입에서는 동기 로드를 사용한다.
	 */
	void ApplyCardIcon(UImage* TargetImage, const TSoftObjectPtr<UTexture2D>& SoftTexture) const;

	// 이미지의 브러시를 제거하고 화면에서 숨긴다.
	void ClearCardImage(UImage* TargetImage) const;
};
