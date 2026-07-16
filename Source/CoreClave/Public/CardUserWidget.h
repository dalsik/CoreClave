#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardStatData.h"
#include "CardUserWidget.generated.h"

class UImage;
class UTextBlock;
struct FStreamableHandle;

UCLASS()
class CORECLAVE_API UCardUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 카드 데이터를 저장하고 현재 카드 UI를 갱신한다.
	UFUNCTION(BlueprintCallable, Category = "Card Data")
	void SetCardData(const FCardStatData& Data);

	UFUNCTION(BlueprintPure, Category = "Card Data")
	FName GetCardId() const
	{
		return CachedCardData.CardId;
	}

	UFUNCTION(BlueprintPure, Category = "Card Data")
	int32 GetCardCost() const
	{
		return CachedCardData.Cost;
	}

	UFUNCTION(BlueprintPure, Category = "Card Data")
	const FCardStatData& GetCachedCardData() const
	{
		return CachedCardData;
	}

	UFUNCTION(BlueprintCallable, Category = "Card System")
	void SetDraggingState(bool bDragging);

	UFUNCTION(BlueprintCallable, Category = "Card State")
	void ResetCardVisual();

	// 기존 블루프린트에서 직접 사용하고 있으므로 공개 상태를 유지한다.
	UPROPERTY(BlueprintReadOnly, Category = "Card System")
	bool bIsDragging = false;

	UPROPERTY(BlueprintReadWrite, Category = "Card System")
	bool bIgnoreHoverOnce = false;

protected:
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;

	// WBP_Card에 이미 존재하는 이미지 위젯.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ICON_UI;

	// 아래 TextBlock들은 WBP_Card에서 같은 이름으로 만들면 자동 바인딩된다.
	// 아직 위젯이 없어도 C++/블루프린트 컴파일이 가능하도록 Optional로 둔다.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CARD_NAME_UI;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HP_UI;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SPD_UI;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> COST_UI;

private:
	void RequestCardIconLoad();
	void ApplyLoadedCardIcon(const FSoftObjectPath& RequestedIconPath);

	// 카드 데이터의 단일 원본. CardId, HP, SPD, Cost 등을 별도 멤버로 중복 저장하지 않는다.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Card Data",
		meta = (AllowPrivateAccess = "true"))
	FCardStatData CachedCardData;

	// 진행 중인 비동기 아이콘 로드 요청을 관리한다.
	TSharedPtr<FStreamableHandle> CardIconLoadHandle;
};
