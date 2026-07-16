#include "CardUserWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/Texture2D.h"

namespace
{
	FText FormatCardStat(float Value)
	{
		FNumberFormattingOptions NumberOptions;
		NumberOptions.SetMinimumFractionalDigits(0);
		NumberOptions.SetMaximumFractionalDigits(1);

		return FText::AsNumber(Value, &NumberOptions);
	}
}

void UCardUserWidget::SetCardData(const FCardStatData& Data)
{
	// 값 데이터와 Soft Object 경로를 포함한 구조체 전체를 한 곳에 저장한다.
	CachedCardData = Data;

	// 수치와 텍스트 데이터는 로드 과정 없이 바로 UI에 적용한다.
	if (CARD_NAME_UI)
	{
		CARD_NAME_UI->SetText(CachedCardData.CardName);
	}

	if (HP_UI)
	{
		HP_UI->SetText(FormatCardStat(CachedCardData.HP));
	}

	if (SPD_UI)
	{
		SPD_UI->SetText(FormatCardStat(CachedCardData.SPD));
	}

	if (COST_UI)
	{
		COST_UI->SetText(FText::AsNumber(FMath::Max(CachedCardData.Cost, 0)));
	}

	// CardIcon만 Soft Object이므로 실제 텍스처가 필요할 때 비동기로 로드한다.
	RequestCardIconLoad();
}

void UCardUserWidget::RequestCardIconLoad()
{
	if (CardIconLoadHandle.IsValid())
	{
		CardIconLoadHandle->CancelHandle();
		CardIconLoadHandle.Reset();
	}

	if (!ICON_UI)
	{
		return;
	}

	// 위젯 재사용 시 이전 카드 아이콘이 남지 않도록 먼저 비운다.
	ICON_UI->SetBrushFromTexture(nullptr);

	if (CachedCardData.CardIcon.IsNull())
	{
		return;
	}

	// 이미 메모리에 올라온 텍스처라면 추가 요청 없이 즉시 사용한다.
	if (UTexture2D* LoadedTexture = CachedCardData.CardIcon.Get())
	{
		ICON_UI->SetBrushFromTexture(LoadedTexture);
		return;
	}

	const FSoftObjectPath RequestedIconPath = CachedCardData.CardIcon.ToSoftObjectPath();
	TWeakObjectPtr<UCardUserWidget> WeakThis(this);

	CardIconLoadHandle = UAssetManager::GetStreamableManager().RequestAsyncLoad(
		RequestedIconPath,
		FStreamableDelegate::CreateLambda([WeakThis, RequestedIconPath]()
		{
			if (UCardUserWidget* Widget = WeakThis.Get())
			{
				Widget->ApplyLoadedCardIcon(RequestedIconPath);
			}
		}));
}

void UCardUserWidget::ApplyLoadedCardIcon(const FSoftObjectPath& RequestedIconPath)
{
	// 로딩 도중 다른 카드 데이터가 들어온 경우, 이전 요청의 결과를 적용하지 않는다.
	if (CachedCardData.CardIcon.ToSoftObjectPath() != RequestedIconPath)
	{
		return;
	}

	if (ICON_UI)
	{
		if (UTexture2D* LoadedTexture = Cast<UTexture2D>(RequestedIconPath.ResolveObject()))
		{
			ICON_UI->SetBrushFromTexture(LoadedTexture);
		}
	}

	CardIconLoadHandle.Reset();
}

void UCardUserWidget::SetDraggingState(bool bDragging)
{
	bIsDragging = bDragging;
}

void UCardUserWidget::ResetCardVisual()
{
	bIsDragging = false;
	bIgnoreHoverOnce = true;

	SetRenderTransformAngle(0.0f);
	SetRenderTranslation(FVector2D::ZeroVector);
	SetRenderScale(FVector2D(1.0f, 1.0f));
}

void UCardUserWidget::NativeDestruct()
{
	if (CardIconLoadHandle.IsValid())
	{
		CardIconLoadHandle->CancelHandle();
		CardIconLoadHandle.Reset();
	}

	Super::NativeDestruct();
}

void UCardUserWidget::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);
	bIgnoreHoverOnce = false;
}
