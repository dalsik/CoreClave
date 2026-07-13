// Fill out your copyright notice in the Description page of Project Settings.


#include "DeckBuilderDeckList.h"


#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "DeckBuilderDeckEntryWidget.h"
#include "DeckBuilderDeckEntryTypes.h"
#include "DeckBuilderSubsystem.h"

void UDeckBuilderDeckList::NativeConstruct()
{
	Super::NativeConstruct();
	
	BindDeckSubsystem();
	RefreshDeckList();
}

void UDeckBuilderDeckList::NativeDestruct()
{
	UnbindDeckSubsystem();
	
	Super::NativeDestruct();
}

UDeckBuilderSubsystem* UDeckBuilderDeckList::GetDeckBuilderSubsystem() const
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		return GameInstance->GetSubsystem<UDeckBuilderSubsystem>();
	}
	return nullptr;
}

/// <summary>
///  편집 중인 덱의 변경 이벤트를 바인딩
/// </summary>
void UDeckBuilderDeckList::BindDeckSubsystem()
{
	UnbindDeckSubsystem();
	
	CachedDeckSubsystem = GetDeckBuilderSubsystem();

	if (CachedDeckSubsystem.IsValid())
	{
		CachedDeckSubsystem->OnWorkingDeckChanged.AddUniqueDynamic(this, &UDeckBuilderDeckList::HandleWorkingDeckChanged);
	}
}

/// <summary>
///  편집 중인 덱의 변경 이벤트를 언바인딩
/// </summary>
void UDeckBuilderDeckList::UnbindDeckSubsystem()
{
	if (CachedDeckSubsystem.IsValid())
	{
		CachedDeckSubsystem->OnWorkingDeckChanged.RemoveDynamic(this, &UDeckBuilderDeckList::HandleWorkingDeckChanged);
		CachedDeckSubsystem.Reset();
	}
}

/// <summary>
// Subsystem측의 이벤트 디스패처가 매개변수를 1개만 전달하므로 , 덱 리스트 갱신을 위해서는 매개변수 1개짜리 이벤트 핸들러를 사용해야 한다.
/// </summary>
void UDeckBuilderDeckList::HandleWorkingDeckChanged(FDeckData WorkingDeck)
{
	RefreshDeckList();
}

/// <summary>
/// 카드가 덱에 추가될 때마다 리스트를 갱신하는 메서드
/// </summary>
void UDeckBuilderDeckList::RefreshDeckList()
{
	// 만약 에디터에 패널이 설정되어 있지 않다면 반환
	if (!DeckListPanel)
	{
		return;
	}
	
	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();
	if (!DeckSubsystem)
	{
		return;
	}

	TArray<FName> CardIds;
	TArray<int32> Counts;
	DeckSubsystem->GetWorkingDeckUniqueCardCounts(CardIds, Counts); // 참조로 데이터가져오기

	TArray<FDeckBuilderDeckEntryViewData> ViewEntries; // 구조체 배열
	ViewEntries.Reserve(CardIds.Num()); // Reserve로 메모리 미리 할당
	
	for (int32 Index = 0; Index < CardIds.Num(); ++Index)
	{
		// 만약 인덱스가 유효하지 않다면 건너뛰기
		if (!CardIds.IsValidIndex(Index) || !Counts.IsValidIndex(Index))
		{
			continue;
		}

		FDeckBuilderDeckEntryViewData ViewData;
		// ViewData 구조체에 정상적으로 설정이 되었다면 ViewEntries 배열에 추가
		if (BuildEntryViewData(CardIds[Index], Counts[Index], ViewData))
		{
			ViewEntries.Add(ViewData);
		}
	}

	// ViewEntries에 카드를 다 넣었다면 Cost순으로 정렬
	ViewEntries.Sort([](const FDeckBuilderDeckEntryViewData& Left, const FDeckBuilderDeckEntryViewData& Right)
	{
		// 코스트가 같다면 이름순으로 정렬
		if (Left.CardCost == Right.CardCost)
		{
			return Left.CardName.ToString() < Right.CardName.ToString();
		}

		return Left.CardCost < Right.CardCost;
	});

	RebuildVisibleEntries(ViewEntries);
	UpdateDeckCountText();
}


/// <summary>
/// 개별 위젯을 재구성하여 덱 리스트를 갱신
/// </summary>
void UDeckBuilderDeckList::RebuildVisibleEntries(const TArray<FDeckBuilderDeckEntryViewData>& InEntries)
{
	if (!DeckListPanel)
	{
		return;
	}
	
	// 데이터가 존재하는 카드 수만큼 반복하여 개별 위젯을 생성하거나 가져와서 데이터를 설정
	for (int32 Index = 0; Index < InEntries.Num(); ++Index)
	{
		// 이때 이미 만들어둔 위젯이 존재하는 경우 그대로 사용하고, 없다면 새로 만들어서 적용하도록 GetOrCreateEntryWidget 메서드를 호출
		UDeckBuilderDeckEntryWidget* EntryWidget = GetOrCreateEntryWidget(Index);
		if (!EntryWidget)
		{
			return;
		}
		
		EntryWidget->SetVisibility(ESlateVisibility::Visible);
		EntryWidget->SetEntryData(InEntries[Index]);
	}

	HideUnusedEntryWidgets(InEntries.Num());
}

/// <summary>
/// 풀은 유지하되 이번에 안쓰는 위젯은 숨기기만 하도록 하는 메서드
/// </summary>
void UDeckBuilderDeckList::HideUnusedEntryWidgets(int32 UsedCount)
{
	for (int32 Index = UsedCount; Index < EntryWidgetPool.Num(); ++Index)
	{
		// 만약	인덱스가 유효하고, 해당 위젯이 존재한다면 숨기기
		if (EntryWidgetPool[Index])
		{
			EntryWidgetPool[Index]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}


/// <summary>
/// 
/// </summary>

UDeckBuilderDeckEntryWidget* UDeckBuilderDeckList::GetOrCreateEntryWidget(int32 Index)
{
	// 만약 인덱스가 유효하고, 해당 위젯이 존재한다면 반환
	if (EntryWidgetPool.IsValidIndex(Index) && EntryWidgetPool[Index])
	{
		return EntryWidgetPool[Index];
	}

	// 만약 DeckEntryWidgetClass가 유효하지 않거나, 소유 플레이어가 없거나, DeckListPanel이 없다면 반환
	if (!DeckEntryWidgetClass || !GetOwningPlayer() || !DeckListPanel)
	{
		return nullptr;
	}

	// 위젯 풀에 없는 경우	, 새로 위젯을 생성하고 패널에 추가

	UDeckBuilderDeckEntryWidget* NewEntryWidget = CreateWidget<UDeckBuilderDeckEntryWidget>(GetOwningPlayer(), DeckEntryWidgetClass);

	// 이거는 위젯풀 배열의 크기를 미리 늘리는거임. 그래야 우리가 새로운 위젯을 추가할 수 있기 때문에
	while (EntryWidgetPool.Num() <= Index)
	{
		EntryWidgetPool.Add(nullptr);
	}

	EntryWidgetPool[Index] = NewEntryWidget;
	DeckListPanel->AddChildToVerticalBox(NewEntryWidget); // 패널에 추가

	// 만약 버티컬박스 슬롯으로 캐스팅이 가능하다면, 슬롯의 정렬 및 패딩 설정
	if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(NewEntryWidget->Slot))
	{
		VerticalSlot->SetHorizontalAlignment(HAlign_Fill); // 수평 정렬을 채우기로 설정
		VerticalSlot->SetVerticalAlignment(VAlign_Top); // 수직 정렬을 위쪽으로 설정
		VerticalSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 4.0f));	// 패딩 설정 (아래쪽에 4.0f)
	}
	
	return NewEntryWidget;
}

/// <summary>
///  텍스트 설정
/// </summary>
void UDeckBuilderDeckList::UpdateDeckCountText()
{
	if (!DeckCountText)
	{
		return;
	}

	UDeckBuilderSubsystem* DeckSubsystem = GetDeckBuilderSubsystem();
	if (!DeckSubsystem)
	{
		return;
	}

	const int32 CurrentCount = DeckSubsystem->GetWorkingDeckCardCount();
	const int32 MaxCount = DeckSubsystem->GetMaxDeckSize();

	DeckCountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentCount, MaxCount)));
}


/// <summary>
/// 주어진 CardId를 기반으로 데이터가 설정되었는지에 대한 유무를 반환
/// </summary>

bool UDeckBuilderDeckList::BuildEntryViewData(FName CardId, int32 DeckCount, FDeckBuilderDeckEntryViewData& OutViewData) const
{
	if (CardId.IsNone() || DeckCount <= 0)
	{
		return false;
	}

	FCardStatData CardData;
	// 만약 ResolveCardStatData가 실패하면 false 반환
	// CardId 기반으로 가져온 데이터가 있어야 한다.
	if (!ResolveCardStatData(CardId, CardData)) 
	{
		return false;
	}

	// 적용하고자 하는 데이터 설정
	OutViewData.CardId = CardId;
	OutViewData.CardName = CardData.CardName;
	OutViewData.CardCost = CardData.Cost;
	OutViewData.DeckCount = DeckCount;
	OutViewData.CardIcon = CardData.CardIcon;

	return true;
}

/// <summary>
/// 주어진 카드	ID를 기반으로 카드 데이터를 찾아 반환
/// </summary>

bool UDeckBuilderDeckList::ResolveCardStatData(FName CardId, FCardStatData& OutCardData) const
{
	if (!CardStatDataTable || CardId.IsNone())
	{
		return false;
	}

	if (const FCardStatData* FoundData = CardStatDataTable->FindRow<FCardStatData>(CardId, TEXT("DeckBuilderDeckList")))
	{
		OutCardData = *FoundData;
		return true;
	}
	
	return false;
}
