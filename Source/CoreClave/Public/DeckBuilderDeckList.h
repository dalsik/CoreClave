// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardStatData.h"
#include "DeckBuilderTypes.h"
#include "DeckBuilderDeckEntryTypes.h"
#include "DeckBuilderDeckList.generated.h"


class UVerticalBox;
class UTextBlock;
class UDataTable;
class UDeckBuilderSubsystem;
class UDeckBuilderDeckEntryWidget;
class UTexture2D;

/**
 * 
 */


UCLASS()
class CORECLAVE_API UDeckBuilderDeckList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RefreshDeckList(); 

protected:
	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UVerticalBox> DeckListPanel;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UTextBlock> DeckCountText;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Deck Builder")
	TSubclassOf<UDeckBuilderDeckEntryWidget> DeckEntryWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UDataTable> CardStatDataTable;

private:
	// 개별 위젯 풀링을 위한 배열
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDeckBuilderDeckEntryWidget>> EntryWidgetPool; 

	UPROPERTY(Transient)
	TWeakObjectPtr<UDeckBuilderSubsystem> CachedDeckSubsystem;

	UFUNCTION()
	void HandleWorkingDeckChanged(FDeckData WorkingDeck);

	void BindDeckSubsystem();
	void UnbindDeckSubsystem();

	void RebuildVisibleEntries(const TArray<FDeckBuilderDeckEntryViewData>& InEntries);
	void HideUnusedEntryWidgets(int32 UsedCount);
	void UpdateDeckCountText();

	UDeckBuilderDeckEntryWidget* GetOrCreateEntryWidget(int32 Index);
	bool BuildEntryViewData(FName CardId, int32 DeckCount, FDeckBuilderDeckEntryViewData& OutViewData) const;
	bool ResolveCardStatData(FName CardId, FCardStatData& OutCardData) const;

	UDeckBuilderSubsystem* GetDeckBuilderSubsystem() const;
};
