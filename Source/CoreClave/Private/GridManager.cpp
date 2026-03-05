#include "GridManager.h"
#include "GridCell.h"
#include "Components/BoxComponent.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();
    // 시작 시 그리드 판 깔아주기
    InitializeGrid();
}

void AGridManager::InitializeGrid()
{
    // 기존 셀 정리
    for (AGridCell* Cell : GridCells)
    {
        if (Cell) Cell->Destroy();
    }
    GridCells.Empty();

    if (!GridCellClass)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("GridManager: GridCellClass"));
        return;
    }

    // 4x4 셀 생성 및 배치
    for (int32 r = 0; r < GridRows; r++)
    {
        for (int32 c = 0; c < GridCols; c++)
        {
            FVector SpawnPos = GetWorldPositionFromCell(r, c);
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            AGridCell* Cell = GetWorld()->SpawnActor<AGridCell>(
                GridCellClass, SpawnPos, FRotator::ZeroRotator, Params);

            if (Cell)
            {
                // 셀에 위치 정보 주입
                Cell->Row = r;
                Cell->Col = c;

                // BoxCollision 크기를 CellSize에 맞게 조정
                if (UBoxComponent* Box = Cell->FindComponentByClass<UBoxComponent>())
                {
                    Box->SetBoxExtent(
                        FVector(CellSize * 0.49f, CellSize * 0.49f, 10.f));
                }

                GridCells.Add(Cell);
            }
        }
    }

    //UE_LOG(LogTemp, Log,
        //TEXT("GridManager: %d개 셀 초기화 완료"), GridCells.Num());
}

AActor* AGridManager::SpawnUnitAtCell(int32 Row, int32 Col,
    TSubclassOf<AActor> UnitClass)
{
    // 유효성 검사
    if (!UnitClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtCell: UnitClass."));
        return nullptr;
    }
    if (!IsValidCell(Row, Col))
    {
        UE_LOG(LogTemp, Warning,TEXT("SpawnUnitAtCell: [%d, %d]"), Row, Col);
        return nullptr;
    }
    if (!IsCellEmpty(Row, Col))
    {
        UE_LOG(LogTemp, Warning,TEXT("SpawnUnitAtCell: [%d, %d]"), Row, Col);
        return nullptr;
    }

    // 해당 Row,Col에 해당하는 Cell의 인덱스를 찾아서 Cell에 할당한다
    AGridCell* Cell = GetCell(Row, Col);
    if (!Cell) return nullptr;

    // 유닛을 셀 위치에 스폰 (Z 오프셋으로 바닥 위에 올림)
    FVector SpawnPos = Cell->GetActorLocation() + FVector(0, 0, 10.f);
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Unit = GetWorld()->SpawnActor<AActor>(
        UnitClass, SpawnPos, FRotator::ZeroRotator, Params);

    if (Unit)
    {
        // 셀 상태 업데이트(해당 타일에게 점유되어 있다고 상태를 업데이트 하기)
        Cell->SetOccupied(Unit);
        Cell->OnUnitSpawned(Unit);

        // 델리게이트 브로드캐스트 (BP에서 바인딩 가능)
        // 유닛이 스폰이 되었을 경우에 델리게이트를 통해 소식 알리기
        OnCellStateChanged.Broadcast(Row, Col);
    }

    return Unit;
}

AGridCell* AGridManager::GetCellFromHitActor(AActor* HitActor)
{
    // 레이캐스트로 히트된 액터가 GridCell인지 확인
    return Cast<AGridCell>(HitActor);
}

AGridCell* AGridManager::GetCellFromWorldPosition(FVector WorldPosition)
{
    // 월드 좌표 → 셀 인덱스 계산
    FVector LocalPos = WorldPosition - GetActorLocation();
    int32 Col = FMath::RoundToInt(LocalPos.X / CellSize);
    int32 Row = FMath::RoundToInt(LocalPos.Y / CellSize);

    return GetCell(Row, Col);
}

AGridCell* AGridManager::GetCell(int32 Row, int32 Col)
{
    if (!IsValidCell(Row, Col)) return nullptr;
    return GridCells[GetIndex(Row, Col)];
}

bool AGridManager::IsCellEmpty(int32 Row, int32 Col) const
{
    if (!IsValidCell(Row, Col)) return false;
    AGridCell* Cell = GridCells[GetIndex(Row, Col)];
    return Cell && Cell->IsEmpty();
}

bool AGridManager::IsValidCell(int32 Row, int32 Col) const
{
    return Row >= 0 && Row < GridRows &&
        Col >= 0 && Col < GridCols;
}

FVector AGridManager::GetWorldPositionFromCell(int32 Row, int32 Col) const
{
    FVector Origin = GetActorLocation();
    return FVector(
        Origin.X + (Row * CellSize),
        Origin.Y + (Col * CellSize),
        Origin.Z
    );
}

bool AGridManager::RemoveUnitFromCell(int32 Row, int32 Col)
{
    AGridCell* Cell = GetCell(Row, Col);
    if (!Cell || Cell->IsEmpty()) return false;

    if (Cell->OccupyingUnit)
    {
        Cell->OccupyingUnit->Destroy();
    }
    Cell->ClearCell();
    OnCellStateChanged.Broadcast(Row, Col);
    return true;
}

bool AGridManager::MoveUnit(int32 FromRow, int32 FromCol,
    int32 ToRow, int32 ToCol)
{
    if (!IsValidCell(FromRow, FromCol)) return false;
    if (!IsCellEmpty(ToRow, ToCol)) return false;

    AGridCell* FromCell = GetCell(FromRow, FromCol);
    AGridCell* ToCell = GetCell(ToRow, ToCol);
    if (!FromCell || !ToCell) return false;

    AActor* Unit = FromCell->OccupyingUnit;
    if (!Unit) return false;

    // 유닛 이동
    Unit->SetActorLocation(
        ToCell->GetActorLocation() + FVector(0, 0, 50.f));

    // 셀 상태 업데이트
    ToCell->SetOccupied(Unit);
    ToCell->OnUnitSpawned(Unit);
    FromCell->ClearCell();

    OnCellStateChanged.Broadcast(FromRow, FromCol);
    OnCellStateChanged.Broadcast(ToRow, ToCol);
    return true;
}

// 유닛들을 앞에라인부터 움직이도록 하게 해주는 함수
void AGridManager::AdvanceAllUnits()
{
    if (!HasAuthority()) return;

    // +1 방향 유닛 → 높은 Row부터 처리 (앞에 있는 유닛 먼저 이동)
    for (int32 r = GridRows - 1; r >= 0; r--)
    {
        for (int32 c = 0; c < GridCols; c++)
        {
            AGridCell* Cell = GetCell(r, c);
            if (!Cell || Cell->IsEmpty()) continue;

            ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
            if (!Unit || Unit->MoveDirection != 1) continue;

            int32 NextRow = r + 1;
            if (!IsValidCell(NextRow, c)) continue;

            MoveUnit(r, c, NextRow, c);
        }
    }

    // -1 방향 유닛 → 낮은 Row부터 처리
    for (int32 r = 0; r < GridRows; r++)
    {
        for (int32 c = 0; c < GridCols; c++)
        {
            AGridCell* Cell = GetCell(r, c);
            if (!Cell || Cell->IsEmpty()) continue;

            ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
            if (!Unit || Unit->MoveDirection != -1) continue;

            int32 NextRow = r - 1;
            if (!IsValidCell(NextRow, c)) continue;

            MoveUnit(r, c, NextRow, c);
        }
    }
}


void AGridManager::UpdateDragHighlight(AGridCell* HoveredCell)
{
    // 이전 하이라이트 끄기
    if (CurrentHighlightedCell &&
        CurrentHighlightedCell != HoveredCell)
    {
        CurrentHighlightedCell->SetHighlight(false, false);
    }

    // 새 셀 하이라이트
    if (HoveredCell)
    {
        bool bCanPlace = HoveredCell->IsEmpty();
        HoveredCell->SetHighlight(true, bCanPlace);
        CurrentHighlightedCell = HoveredCell;
    }
}

void AGridManager::ClearAllHighlights()
{
    // 모든 하이라이트 된 셀 제거하기
    for (AGridCell* Cell : GridCells)
    {
        if (Cell) Cell->SetHighlight(false, false);
    }
    CurrentHighlightedCell = nullptr;
}

int32 AGridManager::GetIndex(int32 Row, int32 Col) const
{
    // 우리는 행/열로 생각하지만 인덱스를 반환해야 하므로 인덱스 계산 로직
    return Row * GridCols + Col;
}
