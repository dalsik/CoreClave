#include "GridManager.h"

#include "BaseLaneManager.h"
#include "CardUnitActor.h"
#include "CombatManager.h"
#include "GridCell.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AGridManager::AGridManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;
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

void AGridManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGridManager, GridCells);
    DOREPLIFETIME(AGridManager, CachedFrontlineRow_Dir1);
	DOREPLIFETIME(AGridManager, CachedFrontlineRow_DirMinus1);
}
void AGridManager::InitializeGrid()
{
    if (!HasAuthority()) return; // 서버에서만 스폰되도록 설정
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
    UpdateFrontlineCache(); // 최전선 라인 갱신
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

// 현재 있는 셀과 다음으로 가야할 셀을 전달받고 셀의 상태 변경 및 유닛 이동 로직 실행
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

    // 셀의 상태변경
    ToCell->SetOccupied(Unit);
    ToCell->OnUnitSpawned(Unit);
    FromCell->ClearCell();

	if (ACardUnitActor* CardUnit = Cast<ACardUnitActor>(Unit))
	{
		CardUnit->CurrentCell = ToCell;

        // 시각적 이동은 CardUnitActor에서 적용
        const FVector TargetPos =
			ToCell->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f);
        CardUnit->MoveToLocation(TargetPos, 1.0f);
	}

    OnCellStateChanged.Broadcast(FromRow, FromCol);
    OnCellStateChanged.Broadcast(ToRow, ToCol);
    return true;
}

void AGridManager::AdvanceAllUnits(int32 MoveDirection)
{
    // 서버에서만 실행되도록 설정
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
    UpdateFrontlineCache();
}

void AGridManager::UpdateDragHighlight(AGridCell* HoveredCell, int32 MoveDirection)
{
    if (CurrentHighlightedCell && CurrentHighlightedCell != HoveredCell)
    {
        CurrentHighlightedCell->SetHighlight(false, false);
    }

    if (HoveredCell)
    {
        // 해당 Row와 Col, 플레이어의 MoveDirection 변수를 확인하고 배치가능한 타일인지 확인한다.
        const bool bCanPlace = IsPlaceableCell(
            HoveredCell->Row,
            HoveredCell->Col,
            MoveDirection);

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

    // 만약 셀의 범위를 넘어간거면 상대방의 기지에 들어간거니까 피해를 입히도록 적용
    // 이거는 나중에 리팩토링으로 피해를 입히는 로직 자체는 BaseLaneManager에서 적용하는 것이 좋을 듯.
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
	const int DamageAmount = FMath::RoundToInt(Unit->CurrentHP); // 피해량은 유닛의 현재 HP로 결정
    const bool bAppliedDamage = BaseLaneManager->ApplyBaseDamageByMoveDirection(
        Unit->MoveDirection,
        DamageAmount,
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
    UpdateFrontlineCache();
}

// 시각적 피드백을 위한 프리뷰 생성 로직
void AGridManager::UpdateUnitPreview(AGridCell* HoveredCell, TSubclassOf<ACardUnitActor> UnitClass, FName CardID, int32 MoveDirection)
{
    if (!HoveredCell || !UnitClass)
    {
        /*
        * UE_LOG(LogTemp, Warning, TEXT("HoveredCell: %s, UnitClass: %s"),
            HoveredCell ? TEXT("Valid") : TEXT("NULL"),
            UnitClass ? TEXT("Valid") : TEXT("NULL"));
        */
        return;
    }

    const bool bCanPlace = IsPlaceableCell(
        HoveredCell->Row,
        HoveredCell->Col,
        MoveDirection);

    if (!bCanPlace)
    {
        if (PreviewUnitActor)
        {
			PreviewUnitActor->SetActorHiddenInGame(true);
        }
        CurrentPreviewCell = HoveredCell;
        return;
    }

    if (!PreviewUnitActor)
    {
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        PreviewUnitActor = GetWorld()->SpawnActor<ACardUnitActor>(
            UnitClass,
            HoveredCell->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f),
            FRotator(0.0f, MoveDirection * 90.0f, 0.0f),
            Params
        );

        if(!PreviewUnitActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn preview unit."));
			return;
		}   

        PreviewUnitActor->SetReplicates(false);
        PreviewUnitActor->SetPreviewMode(true);
    }
    
    if (CurrentPreviewCardID != CardID)
    {
        PreviewUnitActor->InitializeUnit(CardID);
		CurrentPreviewCardID = CardID;
    }

	PreviewUnitActor->SetActorHiddenInGame(false);
	PreviewUnitActor->SetActorLocation(HoveredCell->GetActorLocation() + FVector(0.0f, 0.0f, 10.0f));
    CurrentPreviewCell = HoveredCell;
}

// 프리뷰용 액터 제거
void AGridManager::ClearUnitPreview()
{
    if (PreviewUnitActor)
    {
        PreviewUnitActor->Destroy();
        PreviewUnitActor = nullptr;
    }
    CurrentPreviewCell = nullptr;
    CurrentPreviewCardID = NAME_None;
}

// 해당 셀이 가능한지 확인하는 로직
bool AGridManager::IsPlaceableCell(int32 Row, int32 Col, int32 MoveDirection) const
{
    // 유효한 셀인지 확인
    if (!IsValidCell(Row, Col)) return false;

    // 비어있는지 확인
    if (!IsCellEmpty(Row, Col)) return false;

    const int32 FrontlineRow = GetFrontlineRow(MoveDirection);

    UE_LOG(LogTemp, Warning,
        TEXT("IsPlaceableCell: Row=%d, Col=%d, MoveDir=%d, FrontlineRow=%d, GridRows=%d"),
        Row, Col, MoveDirection, FrontlineRow, GridRows);


    if (MoveDirection == 1)
    {
        // 위로 이동: FrontlineRow ~ GridRows-1 사이에 배치 가능
        return Row >= FrontlineRow && Row <= GridRows - 1;
    }
    else // MoveDirection == -1
    {
        // 아래로 이동: 0 ~ FrontlineRow 사이에 배치 가능
        return Row >= 0 && Row <= FrontlineRow;
    }
}

void AGridManager::UpdateFrontlineCache()
{
    // MoveDirection 1 최전선
    CachedFrontlineRow_Dir1 = GridRows - 1;
    for (int32 Row = 0; Row < GridRows; ++Row)
        for (int32 Col = 0; Col < GridCols; ++Col)
        {
            AGridCell* Cell = GridCells[GetIndex(Row, Col)];
            if (!Cell || Cell->IsEmpty()) continue;
            ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
            if (!Unit || Unit->MoveDirection != 1) continue;
            CachedFrontlineRow_Dir1 = FMath::Min(
                CachedFrontlineRow_Dir1, Row);
        }

    // MoveDirection -1 최전선
    CachedFrontlineRow_DirMinus1 = 0;
    for (int32 Row = GridRows - 1; Row >= 0; --Row)
        for (int32 Col = 0; Col < GridCols; ++Col)
        {
            AGridCell* Cell = GridCells[GetIndex(Row, Col)];
            if (!Cell || Cell->IsEmpty()) continue;
            ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
            if (!Unit || Unit->MoveDirection != -1) continue;
            CachedFrontlineRow_DirMinus1 = FMath::Max(
                CachedFrontlineRow_DirMinus1, Row);
        }

    bFrontlineDirty = false;
}

// 플레이어의 입장에서 가장 최전선에 있는 유닛의 Row를 반환
int32 AGridManager::GetFrontlineRow(int32 MoveDirection) const
{
    // 캐시 사용 (순회 없음)
    if (MoveDirection == 1)
        return CachedFrontlineRow_Dir1;
    else
        return CachedFrontlineRow_DirMinus1;
}

TArray<AGridCell*> AGridManager::GetReachableCells(AGridCell* Cell, int32 MaxMoveDistance)
{
	TArray<AGridCell*> ReachableCells;
    ReachableCells.Reserve(MaxMoveDistance * 4);
	if (!Cell || Cell->IsEmpty() || MaxMoveDistance <= 0)
	{
		return ReachableCells;
	}
    
	ACardUnitActor* Unit = Cast<ACardUnitActor>(Cell->OccupyingUnit);
    if (!Unit)
    {
        return ReachableCells;;
    }

    const int32 StartRow = Cell->Row;
    const int32 StartCol = Cell->Col;

    const TArray<FIntPoint> Directions = {
        FIntPoint(-1, 0), // 위
        FIntPoint(1, 0),  // 아래
        FIntPoint(0, -1), // 좌
        FIntPoint(0, 1)   // 우
    };

    // 타깃유닛에서의 상하좌우 셀을 확인해본다.
    for (const FIntPoint& Dir : Directions)
    {
        for (int32 Step = 1; Step <= MaxMoveDistance; ++Step)
        {
            const int32 TargetRow = StartRow + Dir.X * Step;
            const int32 TargetCol = StartCol + Dir.Y * Step;

            // 만약 범위 밖에 있는 셀이라면 체크 안함.
            if (!IsValidCell(TargetRow, TargetCol))
            {
                break;
            }

            AGridCell* TargetCell = GetCell(TargetRow, TargetCol);
            if (!TargetCell)
            {
                break;
            }
            
            // 빈 칸이면 이동 가능
            if (TargetCell->IsEmpty())
            {
                ReachableCells.Add(TargetCell);
                continue;
            }

			ACardUnitActor* OccupyingUnit = Cast<ACardUnitActor>(TargetCell->OccupyingUnit);
            
            if (!OccupyingUnit) break;
            // 해당 칸에 있는 유닛이 아군이면 못감
			if (OccupyingUnit->MoveDirection == Unit->MoveDirection)
			{
				break;
            }

            // 적군이면 그 칸까지는 가능하지만 통과는 못함
            ReachableCells.Add(TargetCell);
            break;
        }
    }

	return ReachableCells;
}

void AGridManager::HighlightReachableCells(const TArray<AGridCell*>& Cells)
{
    for (AGridCell* Cell : Cells)
    {
		if (Cell)
		{
			Cell->SetHighlight(true, true);
		}
    }
}

void AGridManager::ClearReachableHighlights()
{
	for (AGridCell* Cell : GridCells)
	{
		if (Cell)
		{
			Cell->SetHighlight(false, false);
		}
	}
}