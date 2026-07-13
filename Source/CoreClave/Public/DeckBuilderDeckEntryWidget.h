#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeckBuilderDeckEntryTypes.h"
#include "DeckBuilderDeckEntryWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 덱 편성 리스트의 한 줄 UI를 담당하는 위젯
 */
UCLASS()
class CORECLAVE_API UDeckBuilderDeckEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 외부에서 한 줄 데이터를 넣어줄 때 호출
	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetEntryData(const FDeckBuilderDeckEntryViewData& InEntryData);

	// 현재 캐시된 데이터를 읽고 싶을 때 사용
	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	FDeckBuilderDeckEntryViewData GetEntryData() const { return CachedEntryData; }

protected:
	// 카드 아이콘 이미지
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UImage> CardIconImage;

	// 카드 이름 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UTextBlock> CardNameText;

	// 카드 코스트 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UTextBlock> CardCostText;

	// 현재 덱에 들어간 장수 텍스트
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UTextBlock> DeckCountText;

	// 데이터 갱신 후 BP에서 추가 연출이 필요하면 사용
	UFUNCTION(BlueprintImplementableEvent, Category = "Deck Builder")
	void BP_OnEntryDataChanged();

private:
	// 현재 이 위젯이 표시 중인 데이터
	UPROPERTY(Transient)
	FDeckBuilderDeckEntryViewData CachedEntryData;

	// 캐시된 데이터를 실제 텍스트/이미지에 반영
	void UpdateVisuals();
};