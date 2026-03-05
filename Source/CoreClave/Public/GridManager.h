#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridType.h"
#include "CardUnitActor.h"
#include "GridManager.generated.h"

class AGridCell;
class AUnitBase;

/**
 * 그리드 전체를 관리하는 매니저
 * 레벨에 1개 배치
 * BP에서 참조해서 함수 호출
 */

// 셀 선택 이벤트 델리게이트 (BP 바인딩용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnCellStateChanged, int32, Row, int32, Col);

UCLASS()
class CORECLAVE_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    AGridManager();

protected:
    virtual void BeginPlay() override;

public:
    // ─────────────────────────────────────────
    // 에디터에서 설정할 값들
    // ─────────────────────────────────────────

    /** 행 수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    int32 GridRows = 4;

    /** 열 수 */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    int32 GridCols = 4;

    /** 셀 간격 (언리얼 유닛) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    float CellSize = 100.0f;

    /** 스폰할 GridCell BP 클래스 (에디터에서 BP_GridCell 지정) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid|Settings")
    TSubclassOf<AGridCell> GridCellClass;

    /** 셀 배열 */
    UPROPERTY(BlueprintReadOnly, Category = "Grid")
    TArray<AGridCell*> GridCells;

    // ─────────────────────────────────────────
    // 델리게이트 (BP에서 바인딩)
    // ─────────────────────────────────────────

    /** 셀 상태 변경 시 브로드캐스트 */
    UPROPERTY(BlueprintAssignable, Category = "Grid|Events")
    FOnCellStateChanged OnCellStateChanged;

    // ─────────────────────────────────────────
    // 핵심 함수 (BP에서 호출)
    // ─────────────────────────────────────────

    /** 그리드 초기화 - BeginPlay에서 자동 호출 */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void InitializeGrid();

    /**
     * 특정 셀에 유닛 스폰
     * BP 드롭 이벤트에서 호출할 메인 함수
     * @param Row 행 인덱스
     * @param Col 열 인덱스
     * @param UnitClass 스폰할 유닛 클래스
     * @return 스폰된 유닛 (실패 시 nullptr)
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    AActor* SpawnUnitAtCell(int32 Row, int32 Col,
        TSubclassOf<AActor> UnitClass);

    /**
     * 레이캐스트 히트 결과로 셀 찾기
     * BP의 LineTrace 결과를 그대로 넘기면 됨
     * @param HitActor 레이캐스트에 히트된 액터
     * @param OutCell 찾은 셀 (없으면 nullptr)
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    AGridCell* GetCellFromHitActor(AActor* HitActor);

    /**
     * 월드 좌표로 셀 찾기
     * Deprojection 결과를 넘길 때 사용
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    AGridCell* GetCellFromWorldPosition(FVector WorldPosition);

    /** 셀 인덱스로 셀 가져오기 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    AGridCell* GetCell(int32 Row, int32 Col);

    /** 특정 셀이 비어있는지 확인 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsCellEmpty(int32 Row, int32 Col) const;

    /** 유효한 셀 인덱스인지 확인 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsValidCell(int32 Row, int32 Col) const;

    /** 셀 인덱스 → 월드 좌표 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    FVector GetWorldPositionFromCell(int32 Row, int32 Col) const;

    /** 유닛 제거 */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    bool RemoveUnitFromCell(int32 Row, int32 Col);

    /** 유닛 이동 */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    bool MoveUnit(int32 FromRow, int32 FromCol,
        int32 ToRow, int32 ToCol);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void AdvanceAllUnits();
    
    /**
     * 드래그 중 하이라이트 처리
     * BP PlayerController Tick에서 호출
     * @param HoveredCell 현재 커서가 올라간 셀
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void UpdateDragHighlight(AGridCell* HoveredCell);

    /** 모든 하이라이트 끄기 */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearAllHighlights();

private:
    int32 GetIndex(int32 Row, int32 Col) const;

    // 현재 하이라이트된 셀 추적
    UPROPERTY()
    AGridCell* CurrentHighlightedCell = nullptr;
};