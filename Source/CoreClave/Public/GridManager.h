#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridType.h"
#include "GridManager.generated.h"

class AGridCell;
class AUnitBase;
class ACardUnitActor;
class ACombatManager;
class ABaseLaneManager;

/**
 * 그리드 생성과 상호작용 기능 클래스
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnCellStateChanged, int32, Row, int32, Col);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(
    FOnCombatResolved, int32, AttackerRow, int32, AttackerCol, int32, DefenderRow, int32, DefenderCol, ECombatResult, Result);

UCLASS()
class CORECLAVE_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    int32 GridRows = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    int32 GridCols = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    float CellSize = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    TSubclassOf<AGridCell> GridCellClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Rules")
    int32 BaseDamageOnReach = 1;

	// 클라이언트1도 GridCells 정보를 알 수 있도록 Replicated 설정
    UPROPERTY(BlueprintReadOnly, Category = "Grid", Replicated)
    TArray<AGridCell*> GridCells;

    UPROPERTY()
    ACombatManager* CombatManager = nullptr;

    UPROPERTY()
    ABaseLaneManager* BaseLaneManager = nullptr;

    UPROPERTY(BlueprintAssignable, Category = "Grid|Events")
    FOnCellStateChanged OnCellStateChanged;

    UPROPERTY(BlueprintAssignable, Category = "Grid|Events")
    FOnCombatResolved OnCombatResolved;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void InitializeGrid();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    AActor* SpawnUnitAtCell(int32 Row, int32 Col, TSubclassOf<AActor> UnitClass);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    AGridCell* GetCellFromHitActor(AActor* HitActor);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    AGridCell* GetCellFromWorldPosition(FVector WorldPosition);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    AGridCell* GetCell(int32 Row, int32 Col);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsCellEmpty(int32 Row, int32 Col) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsValidCell(int32 Row, int32 Col) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    FVector GetWorldPositionFromCell(int32 Row, int32 Col) const;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    bool RemoveUnitFromCell(int32 Row, int32 Col);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    bool MoveUnit(int32 FromRow, int32 FromCol, int32 ToRow, int32 ToCol);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void AdvanceAllUnits(int32 MoveDirection);

    /// <summary>
    ///  하이라이트 및 프리뷰 유닛
    /// </summary>
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateDragHighlight(AGridCell* HoveredCell, int32 MoveDirection);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearAllHighlights();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateUnitPreview(AGridCell* HoveredCell, TSubclassOf<ACardUnitActor> PreviewUnit, FName CardID, int32 MoveDirection);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearUnitPreview();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    int32 GetFrontlineRow(int32 MoveDirection) const;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsPlaceableCell(int32 Row, int32 Col, int32 MoveDirection) const;

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateFrontlineCache();

	UFUNCTION(BlueprintCallable, Category = "Grid")
    TArray<AGridCell*> GetReachableCells(AGridCell* Cell, int32 MaxMoveDistance);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void HighlightReachableCells(const TArray<AGridCell*>& Cells);
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearReachableHighlights();

private:
    int32 GetIndex(int32 Row, int32 Col) const;
    bool TryAdvanceUnitFromCell(int32 Row, int32 Col, int32 NextRow);
    bool TryResolveBaseEntry(ACardUnitActor* Unit, int32 NextRow);
    void ResolveCombatBetweenUnits(ACardUnitActor* Attacker, ACardUnitActor* Defender);
    void RemoveUnitActor(ACardUnitActor* Unit);

    /// <summary>
    /// 최전선을 파악하기 위한 캐시 변수들
    /// <summary>
    UPROPERTY(Replicated, EditAnywhere)
	int32 CachedFrontlineRow_Dir1; // MoveDirection+1 플레이어
    UPROPERTY(Replicated, EditAnywhere)
	int32 CachedFrontlineRow_DirMinus1; // MoveDirection-1 플레이어
    bool bFrontlineDirty = true; // 갱신 필요 여부

    /// <summary>
    /// 프리뷰 유닛을 위한 변수들
    /// </summary>
    UPROPERTY()
    ACardUnitActor* PreviewUnitActor = nullptr;

    UPROPERTY()
    AGridCell* CurrentPreviewCell = nullptr;
   
    UPROPERTY()
    AGridCell* CurrentHighlightedCell = nullptr;

    UPROPERTY()
    FName CurrentPreviewCardID;

    UFUNCTION()
    bool IsBeyondFrontline(int32 MoveDirection, int32 Row) const;
};
