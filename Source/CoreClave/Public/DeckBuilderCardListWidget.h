#pragma once

#include "CoreMinimal.h"
#include "DeckBuilderWidget.h"
#include "CardStatData.h"
#include "DeckBuilderCardListWidget.generated.h"

class UButton;
class UDataTable;
class UDeckBuilderCardWidget;
class UHorizontalBox;
class UTextBlock;
class UVerticalBox;

UCLASS()
class CORECLAVE_API UDeckBuilderCardListWidget : public UDeckBuilderWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void RefreshCardList();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void NextPage();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void PreviousPage();

	UFUNCTION(BlueprintCallable, Category = "Deck Builder")
	void SetGridDimensions(int32 NewRows, int32 NewColumns);

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetCurrentPageIndex() const { return CurrentPageIndex; }

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetTotalPageCount() const;

	UFUNCTION(BlueprintPure, Category = "Deck Builder")
	int32 GetCardsPerPage() const { return CardsPerPage; }

protected:
	UFUNCTION()
	void HandleNextPageClicked();

	UFUNCTION()
	void HandlePreviousPageClicked();

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UVerticalBox> CardListPanel;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UButton> NextButton;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UButton> PrevButton;

	UPROPERTY(meta = (BindWidgetOptional), BlueprintReadOnly, Category = "Deck Builder")
	TObjectPtr<UTextBlock> PageText;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	TSubclassOf<UDeckBuilderCardWidget> CardEntryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	TObjectPtr<UDataTable> CardStatDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	int32 Rows = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	int32 Columns = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	float CardSpacing = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Deck Builder")
	float RowSpacing = 8.0f;

	UPROPERTY(Transient)
	int32 CardsPerPage = 8;

	UPROPERTY(Transient)
	int32 CurrentPageIndex = 0;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UDeckBuilderCardWidget>> CardSlots;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UHorizontalBox>> RowBoxes;

	UPROPERTY(Transient)
	TArray<FName> TrackedCardIds;

	UPROPERTY(Transient)
	TWeakObjectPtr<class UDeckBuilderSubsystem> CachedListSubsystem;

	void BindListSubsystem();
	void UnbindListSubsystem();
	void BindNavigationButtons();
	void UnbindNavigationButtons();
	void RebuildCardSlots();
	void PopulateCurrentPage();
	void UpdatePageText();
	void SyncTrackedCardIds();
	void SortTrackedCardIds();
	void ClearCardSlot(UDeckBuilderCardWidget* CardWidget);
	UHorizontalBox* CreateRowBox();
	UDeckBuilderCardWidget* GetOrCreateCardSlot(int32 SlotIndex);
	void ApplyCardToSlot(UDeckBuilderCardWidget* CardWidget, FName CardId, int32 CardCount);
	bool ResolveCardData(FName CardId, FCardStatData& OutCardData) const;
	int32 GetOwnedCardCountInternal(FName CardId) const;
};
