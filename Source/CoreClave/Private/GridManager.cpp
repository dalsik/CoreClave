#include "GridManager.h"

#include "BaseLaneManager.h"
#include "CardUnitActor.h"
#include "CombatManager.h"
#include "GridCell.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridManager::BeginPlay()
{
    Super::BeginPlay();

    // 그리드 생성
    InitializeGrid();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), ACombatManager::StaticClass(), FoundActors);

    // CombatManager 캐싱
    if (FoundActors.Num() > 0)
    {
        CombatManager = Cast<ACombatManager>(FoundActors[0]);
    }

    FoundActors.Reset();
    UGameplayStatics::GetAllActorsOfClass(
        GetWorld(), ABaseLaneManager::StaticClass(), FoundActors);

    // BaseLaneManager 캐싱
    if (FoundActors.Num() > 0)
    {
        BaseLaneManager = Cast<ABaseLaneManager>(FoundActors[0]);
    }
}

void AGridManager::InitializeGrid()
{
    for (AGridCell* Cell : GridCells)
    {
        if (Cell)
        {
            Cell->Destroy();
        }
    }
    GridCells.Empty();

    if (!GridCellClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("GridManager: GridCellClass"));
        return;
    }

    for (int32 Row = 0; Row < GridRows; ++Row)
    {
        for (int32 Col = 0; Col < GridCols; ++Col)
        {
            const FVector SpawnPos = GetWorldPositionFromCell(Row, Col);
            FActorSpawnParameters Params;
            Params.SpawnCollisionHandlingOverride =
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            AGridCell* Cell = GetWorld()->SpawnActor<AGridCell>(
                GridCellClass, SpawnPos, FRotator::ZeroRotator, Params);

            if (!Cell)
            {
                continue;
            }

            Cell->Row = Row;
            Cell->Col = Col;

            if (UBoxComponent* Box = Cell->FindComponentByClass<UBoxComponent>())
            {
                Box->SetBoxExtent(
                    FVector(CellSize * 0.49f, CellSize * 0.49f, 10.0f));
            }

            GridCells.Add(Cell);
        }
    }
}

AActor* AGridManager::SpawnUnitAtCell(int32 Row, int32 Col, TSubclassOf<AActor> UnitClass)
{
    if (!UnitClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtCell: UnitClass."));
        return nullptr;
    }
    if (!IsValidCell(Row, Col))
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtCell: [%d, %d]"), Row, Col);
        return nullptr;
    }
    if (!IsCellEmpty(Row, Col))
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnUnitAtCell: [%d, %d]"), Row, Col);
        return nullptr;
    }

    AGridCell* Cell = GetCell(Row, Col);
    if (!Cell)
    {
        return nullptr;
    }

    const FVector SpawnPos = Cell->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f);
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* Unit = GetWorld()->SpawnActor<AActor>(
        UnitClass, SpawnPos, FRotator::ZeroRotator, Params);

    if (!Unit)
    {
        return nullptr;
    }

    Cell->SetOccupied(Unit);
    Cell->OnUnitSpawned(Unit);

    if (ACardUnitActor* CardUnit = Cast<ACardUnitActor>(Unit))
    {
        CardUnit->CurrentCell = Cell;
    }

    OnCellStateChanged.Broadcast(Row, Col);
    return Unit;
}

AGridCell* AGridManager::GetCellFromHitActor(AActor* HitActor)
{
    return Cast<AGridCell>(HitActor);
}

AGridCell* AGridManager::GetCellFromWorldPosition(FVector WorldPosition)
{
    const FVector Origin = GetActorLocation();

    const float TotalWidth = GridRows * CellSize;
    const float TotalHeight = GridCols * CellSize;

    const float StartX = Origin.X - (TotalWidth * 0.5f);
    const float StartY = Origin.Y - (TotalHeight * 0.5f);

    const int32 Row = FMath::FloorToInt((WorldPosition.X - StartX) / CellSize);
    const int32 Col = FMath::FloorToInt((WorldPosition.Y - StartY) / CellSize);

    if (!IsValidCell(Row, Col))
    {
        return nullptr;
    }

    return GetCell(Row, Col);
}

AGridCell* AGridManager::GetCell(int32 Row, int32 Col)
{
    if (!IsValidCell(Row, Col))
    {
        return nullptr;
    }
    return GridCells[GetIndex(Row, Col)];
}

bool AGridManager::IsCellEmpty(int32 Row, int32 Col) const
{
    if (!IsValidCell(Row, Col))
    {
        return false;
    }

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
    const FVector Origin = GetActorLocation();

    const float TotalWidth = GridRows * CellSize;
    const float TotalHeight = GridCols * CellSize;

    const float StartX = Origin.X - (TotalWidth * 0.5f) + (CellSize * 0.5f);
    const float StartY = Origin.Y - (TotalHeight * 0.5f) + (CellSize * 0.5f);

    return FVector(
        StartX + (Row * CellSize),
        StartY + (Col * CellSize),
        Origin.Z);
}

bool AGridManager::RemoveUnitFromCell(int32 Row, int32 Col)
{
    AGridCell* Cell = GetCell(Row, Col);
    if (!Cell || Cell->IsEmpty())
    {
        return false;
    }

    if (ACardUnitActor* CardUnit = Cast<ACardUnitActor>(Cell->OccupyingUnit))
    {
        RemoveUnitActor(CardUnit);
        return true;
    }

    if (Cell->OccupyingUnit)
    {
        Cell->OccupyingUnit->Destroy();
        Cell->ClearCell();
    }

    OnCellStateChanged.Broadcast(Row, Col);
    return true;
}

bool AGridManager::MoveUnit(int32 FromRow, int32 FromCol, int32 ToRow, int32 ToCol)
{
    if (!IsValidCell(FromRow, FromCol) || !IsCellEmpty(ToRow, ToCol))
    {
        return false;
    }

    AGridCell* FromCell = GetCell(FromRow, FromCol);
    AGridCell* ToCell = GetCell(ToRow, ToCol);
    if (!FromCell || !ToCell)
    {
        return false;
    }

    AActor* Unit = FromCell->OccupyingUnit;
    if (!Unit)
    {
        return false;
    }

    Unit->SetActorLocation(ToCell->GetActorLocation() + FVector(0.0f, 0.0f, 50.0f));

    if (ACardUnitActor* CardUnit = Cast<ACardUnitActor>(Unit))
    {
        CardUnit->CurrentCell = ToCell;
    }

    ToCell->SetOccupied(Unit);
    ToCell->OnUnitSpawned(Unit);
    FromCell->ClearCell();

    OnCellStateChanged.Broadcast(FromRow, FromCol);
    OnCellStateChanged.Broadcast(ToRow, ToCol);
    return true;
}

void AGridManager::AdvanceAllUnits(int32 MoveDirection)
{
    if (!HasAuthority())
    {
        return;
    }

    if (MoveDirection == -1)
    {
        for (int32 Row = GridRows - 1; Row >= 0; --Row)
        {
            for (int32 Col = 0; Col < GridCols; ++Col)
            {
                AGridCell* Cell = GetCell(Row, Col);
                if (!Cell || Cell->IsEmpty())
                {
                    continue;
                }

                ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
                if (!Unit || Unit->MoveDirection != -1)
                {
                    continue;
                }

                TryAdvanceUnitFromCell(Row, Col, Row + 1);
            }
        }
    }

    if (MoveDirection == 1)
    {
        for (int32 Row = 0; Row < GridRows; ++Row)
        {
            for (int32 Col = 0; Col < GridCols; ++Col)
            {
                AGridCell* Cell = GetCell(Row, Col);
                if (!Cell || Cell->IsEmpty())
                {
                    continue;
                }

                ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
                if (!Unit || Unit->MoveDirection != 1)
                {
                    continue;
                }

                TryAdvanceUnitFromCell(Row, Col, Row - 1);
            }
        }
    }
}

void AGridManager::UpdateDragHighlight(AGridCell* HoveredCell)
{
    if (CurrentHighlightedCell && CurrentHighlightedCell != HoveredCell)
    {
        CurrentHighlightedCell->SetHighlight(false, false);
    }

    if (HoveredCell)
    {
        const bool bCanPlace = HoveredCell->IsEmpty();
        HoveredCell->SetHighlight(true, bCanPlace);
        CurrentHighlightedCell = HoveredCell;
    }
}

void AGridManager::ClearAllHighlights()
{
    for (AGridCell* Cell : GridCells)
    {
        if (Cell)
        {
            Cell->SetHighlight(false, false);
        }
    }
    CurrentHighlightedCell = nullptr;
}

int32 AGridManager::GetIndex(int32 Row, int32 Col) const
{
    return Row * GridCols + Col;
}

bool AGridManager::TryAdvanceUnitFromCell(int32 Row, int32 Col, int32 NextRow)
{
    AGridCell* Cell = GetCell(Row, Col);
    if (!Cell || Cell->IsEmpty())
    {
        return false;
    }

    ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
    if (!Unit)
    {
        return false;
    }

    if (!IsValidCell(NextRow, Col))
    {
        return TryResolveBaseEntry(Unit, NextRow);
    }

    AGridCell* NextCell = GetCell(NextRow, Col);
    if (!NextCell)
    {
        return false;
    }

    if (NextCell->IsEmpty())
    {
        return MoveUnit(Row, Col, NextRow, Col);
    }

    ACardUnitActor* Enemy = Cast<ACardUnitActor>(NextCell->OccupyingUnit);
    if (!Enemy || !CombatManager || Unit->MoveDirection == Enemy->MoveDirection)
    {
        return false;
    }

    ResolveCombatBetweenUnits(Unit, Enemy);
    return true;
}

// 유닛이 상대방의 영역에 들어가면 피해를 입히고 해당 유닛 삭제해버림
bool AGridManager::TryResolveBaseEntry(ACardUnitActor* Unit, int32 NextRow)
{
    if (!Unit || !BaseLaneManager)
    {
        return false;
    }

    const bool bReachedTopBase = (Unit->MoveDirection == 1 && NextRow < 0);
    const bool bReachedBottomBase = (Unit->MoveDirection == -1 && NextRow >= GridRows);
    if (!bReachedTopBase && !bReachedBottomBase)
    {
        return false;
    }

    int32 RemainingBaseHealth = 0;
    const bool bAppliedDamage = BaseLaneManager->ApplyBaseDamageByMoveDirection(
        Unit->MoveDirection,
        BaseDamageOnReach,
        RemainingBaseHealth);

    if (!bAppliedDamage)
    {
        return false;
    }

    RemoveUnitActor(Unit);
    return true;
}

void AGridManager::ResolveCombatBetweenUnits(ACardUnitActor* Attacker, ACardUnitActor* Defender)
{
    if (!Attacker || !Defender || !CombatManager)
    {
        return;
    }

    AGridCell* AttackerCell = Attacker->CurrentCell;
    AGridCell* DefenderCell = Defender->CurrentCell;
    if (!AttackerCell || !DefenderCell)
    {
        return;
    }

    const int32 AttackerRow = AttackerCell->Row;
    const int32 AttackerCol = AttackerCell->Col;
    const int32 DefenderRow = DefenderCell->Row;
    const int32 DefenderCol = DefenderCell->Col;

    float WinningRemainingHP = 0.0f;
    const ECombatResult Result =
        CombatManager->ResolveCombat(Attacker, Defender, WinningRemainingHP);

    if (Result == ECombatResult::AttackerWins)
    {
        Attacker->CurrentHP = WinningRemainingHP;
        RemoveUnitActor(Defender);
        MoveUnit(AttackerRow, AttackerCol, DefenderRow, DefenderCol);
    }
    else if (Result == ECombatResult::DefenderWins)
    {
        Defender->CurrentHP = WinningRemainingHP;
        RemoveUnitActor(Attacker);
    }
    else
    {
        RemoveUnitActor(Attacker);
        RemoveUnitActor(Defender);
    }

    OnCombatResolved.Broadcast(
        AttackerRow, AttackerCol,
        DefenderRow, DefenderCol,
        Result);
}

void AGridManager::RemoveUnitActor(ACardUnitActor* Unit)
{
    if (!Unit)
    {
        return;
    }

    AGridCell* OccupiedCell = Unit->CurrentCell;
    if (OccupiedCell)
    {
        const int32 Row = OccupiedCell->Row;
        const int32 Col = OccupiedCell->Col;

        OccupiedCell->ClearCell();
        Unit->CurrentCell = nullptr;
        OnCellStateChanged.Broadcast(Row, Col);
    }

    Unit->Destroy();
}

