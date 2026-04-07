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

    UPROPERTY(BlueprintReadOnly, Category = "Grid")
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

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateDragHighlight(AGridCell* HoveredCell);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearAllHighlights();

private:
    int32 GetIndex(int32 Row, int32 Col) const;
    bool TryAdvanceUnitFromCell(int32 Row, int32 Col, int32 NextRow);
    bool TryResolveBaseEntry(ACardUnitActor* Unit, int32 NextRow);
    void ResolveCombatBetweenUnits(ACardUnitActor* Attacker, ACardUnitActor* Defender);
    void RemoveUnitActor(ACardUnitActor* Unit);

    UPROPERTY()
    AGridCell* CurrentHighlightedCell = nullptr;
};
