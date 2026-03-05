#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridType.h"
#include "GridCell.generated.h"

/**
 * 3D 공간에 배치되는 개별 셀 액터
 * BP_GridCell이 이 클래스를 부모로 사용
 * 시각적 요소(메시, 하이라이트)는 BP에서 처리
 * 데이터와 로직은 C++에서 처리
 */
UCLASS()
class CORECLAVE_API AGridCell : public AActor
{
    GENERATED_BODY()

public:
    AGridCell();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ─────────────────────────────────────────
    // 셀 기본 데이터 (GridManager가 초기화 시 세팅)
    // ─────────────────────────────────────────

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
    int32 Row = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
    int32 Col = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
    ECellState CellState = ECellState::Empty;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Grid")
    AActor* OccupyingUnit = nullptr;


    // ─────────────────────────────────────────
    // BP에서 호출 가능한 함수들
    // ─────────────────────────────────────────

    /** 셀이 비어있는지 확인 */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    bool IsEmpty() const { return CellState == ECellState::Empty; }

    /** 셀 점유 상태 세팅 (GridManager가 호출) */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SetOccupied(AActor* Unit);

    /** 셀 비우기 (유닛 제거 시 호출) */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearCell();

    // ─────────────────────────────────────────
    // BP에서 오버라이드할 이벤트 (시각 처리용)
    // ─────────────────────────────────────────

    /** 하이라이트 on/off - BP에서 머터리얼 변경 구현 */
    UFUNCTION(BlueprintNativeEvent, Category = "Grid")
    void SetHighlight(bool bActive, bool bCanPlace);
    virtual void SetHighlight_Implementation(bool bActive, bool bCanPlace);

    /** 유닛 스폰됐을 때 - BP에서 이펙트/사운드 구현 */
    UFUNCTION(BlueprintNativeEvent, Category = "Grid")
    void OnUnitSpawned(AActor* SpawnedUnit);
    virtual void OnUnitSpawned_Implementation(AActor* SpawnedUnit);

    /** 유닛 제거됐을 때 - BP에서 이펙트/사운드 구현 */
    UFUNCTION(BlueprintNativeEvent, Category = "Grid")
    void OnUnitRemoved();
    virtual void OnUnitRemoved_Implementation();

protected:
    virtual void BeginPlay() override;

    // 콜리전 컴포넌트 (레이캐스트 히트용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    class UBoxComponent* BoxCollision;
};