#include "BattleDeckDetailWidget.h"

#include "CardStatData.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/DataTable.h"
#include "Engine/Texture2D.h"

namespace
{
	/**
	 * 주요 카드를 정렬하기 위해 사용하는 cpp 전용 임시 구조체.
	 *
	 * 다른 클래스에서 사용할 필요가 없으므로 헤더에는 노출하지 않는다.
	 */
	struct FFeaturedCardCandidate
	{
		FName CardId = NAME_None;
		int32 IncludedCount = 0;
		int32 Cost = 0;
		TSoftObjectPtr<UTexture2D> CardIcon;
	};
}

void UBattleDeckDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 부모로부터 덱을 전달받기 전에는 빈 상세 화면을 표시한다.
	ClearDeckData();
}

void UBattleDeckDetailWidget::SetDeckData(const FDeckData& InDeckData, int32 InMaxDeckSize)
{
	const int32 SafeMaxDeckSize = FMath::Max(InMaxDeckSize, 1);

	if (DeckNameText)
	{
		const FText DisplayName = InDeckData.DeckName.IsNone() ? FText::FromString(TEXT("이름 없는 덱")) : FText::FromName(InDeckData.DeckName);
		DeckNameText->SetText(DisplayName);
	}

	if (CardCountText)
	{
		CardCountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), InDeckData.CardIds.Num(), SafeMaxDeckSize)));
	}

	// CardId별 포함 개수를 계산한다.
	TMap<FName, int32> IncludedCardCounts;

	// 평균 비용 계산에 사용한다.
	int32 TotalCost = 0;
	int32 ResolvedCardCount = 0;

	for (const FName& CardId : InDeckData.CardIds)
	{
		if (CardId.IsNone())
		{
			continue;
		}

		++IncludedCardCounts.FindOrAdd(CardId);

		const FCardStatData* CardData = FindCardData(CardId);

		if (!CardData)
		{
			continue;
		}

		TotalCost += FMath::Max(CardData->Cost, 0);
		++ResolvedCardCount;
	}

	if (AverageCostText)
	{
		if (ResolvedCardCount > 0)
		{
			const float AverageCost = static_cast<float>(TotalCost) / static_cast<float>(ResolvedCardCount);
			AverageCostText->SetText(FText::FromString(FString::Printf(TEXT("%.1f"), AverageCost)));
		}
		else
		{
			AverageCostText->SetText(FText::FromString(TEXT("-")));
		}
	}

	// 주요 카드 선정을 위한 후보 배열을 생성한다.
	TArray<FFeaturedCardCandidate> FeaturedCandidates;
	FeaturedCandidates.Reserve(IncludedCardCounts.Num());

	for (const TPair<FName, int32>& CardCountPair : IncludedCardCounts)
	{
		FFeaturedCardCandidate Candidate;
		Candidate.CardId = CardCountPair.Key;
		Candidate.IncludedCount = CardCountPair.Value;

		if (const FCardStatData* CardData = FindCardData(Candidate.CardId))
		{
			Candidate.Cost = FMath::Max(CardData->Cost, 0);
			Candidate.CardIcon = CardData->CardIcon;
		}

		FeaturedCandidates.Add(Candidate);
	}

	/**
	 * 주요 카드 선정 기준:
	 *
	 * 1. 덱에 더 많이 들어간 카드
	 * 2. 같은 수량이면 비용이 높은 카드
	 * 3. 수량과 비용이 같으면 CardId 순서
	 *
	 * 나중에 에이스 카드나 희귀도 시스템이 생기면 이 정렬 기준만 변경하면 된다.
	 */
	FeaturedCandidates.Sort([](const FFeaturedCardCandidate& A, const FFeaturedCardCandidate& B)
		{
			if (A.IncludedCount != B.IncludedCount)
			{
				return A.IncludedCount > B.IncludedCount;
			}

			if (A.Cost != B.Cost)
			{
				return A.Cost > B.Cost;
			}

			return A.CardId.LexicalLess(B.CardId);
		});

	TArray<UImage*> FeaturedImages = { FeaturedCardImage1, FeaturedCardImage2, FeaturedCardImage3 };

	for (int32 ImageIndex = 0; ImageIndex < FeaturedImages.Num(); ++ImageIndex)
	{
		UImage* TargetImage = FeaturedImages[ImageIndex];

		if (!TargetImage)
		{
			continue;
		}

		if (!FeaturedCandidates.IsValidIndex(ImageIndex))
		{
			ClearCardImage(TargetImage);
			continue;
		}

		ApplyCardIcon(TargetImage, FeaturedCandidates[ImageIndex].CardIcon);
	}
}

void UBattleDeckDetailWidget::ClearDeckData()
{
	if (DeckNameText)
	{
		DeckNameText->SetText(FText::FromString(TEXT("-")));
	}

	if (CardCountText)
	{
		CardCountText->SetText(FText::FromString(TEXT("0 / 0")));
	}

	if (AverageCostText)
	{
		AverageCostText->SetText(FText::FromString(TEXT("-")));
	}

	ClearCardImage(FeaturedCardImage1);
	ClearCardImage(FeaturedCardImage2);
	ClearCardImage(FeaturedCardImage3);
}

const FCardStatData* UBattleDeckDetailWidget::FindCardData(FName CardId) const
{
	if (!CardStatDataTable || CardId.IsNone())
	{
		return nullptr;
	}

	/**
	 * 가장 빠른 검색 방식.
	 *
	 * DataTable의 RowName과 CardId가 같다면 바로 데이터를 찾을 수 있다.
	 */
	if (const FCardStatData* CardData = CardStatDataTable->FindRow<FCardStatData>(CardId, TEXT("BattleDeckDetailWidget"), false))
	{
		return CardData;
	}

	/**
	 * RowName과 CardId가 다를 경우를 위한 보조 검색.
	 *
	 * 현재 카드 수가 많지 않은 프로토타입에서는 문제가 없지만,
	 * 카드가 많아지면 RowName을 CardId와 통일하는 것이 더 효율적이다.
	 */
	TArray<FCardStatData*> AllCardRows;
	CardStatDataTable->GetAllRows<FCardStatData>(TEXT("BattleDeckDetailWidgetFallback"), AllCardRows);

	for (const FCardStatData* CardData : AllCardRows)
	{
		if (CardData && CardData->CardId == CardId)
		{
			return CardData;
		}
	}

	return nullptr;
}

void UBattleDeckDetailWidget::ApplyCardIcon(UImage* TargetImage, const TSoftObjectPtr<UTexture2D>& SoftTexture) const
{
	if (!TargetImage)
	{
		return;
	}

	if (SoftTexture.IsNull())
	{
		ClearCardImage(TargetImage);
		return;
	}

	/**
	 * 프로토타입에서는 최대 3개 아이콘만 필요하므로 동기 로드한다.
	 *
	 * 카드가 많아지거나 화면 전환 시 끊김이 보이면 StreamableManager를 이용한 비동기 로드로 교체한다.
	 */
	UTexture2D* LoadedTexture = SoftTexture.LoadSynchronous();

	if (!LoadedTexture)
	{
		ClearCardImage(TargetImage);
		return;
	}

	// false를 전달해 이미지 위젯 크기가 원본 텍스처 크기로 변경되지 않게 한다.
	TargetImage->SetBrushFromTexture(LoadedTexture, false);

	// 부모 버튼의 클릭을 막지 않도록 HitTest를 비활성화한다.
	TargetImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

void UBattleDeckDetailWidget::ClearCardImage(UImage* TargetImage) const
{
	if (!TargetImage)
	{
		return;
	}

	TargetImage->SetBrushFromTexture(nullptr, false);
	TargetImage->SetVisibility(ESlateVisibility::Collapsed);
}