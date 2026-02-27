#include "GridCell.h"
#include "Components/BoxComponent.h"

AGridCell::AGridCell()
{
    PrimaryActorTick.bCanEverTick = false;

    // 루트 씬 컴포넌트
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(
        TEXT("Root"));
    SetRootComponent(Root);

    // 레이캐스트 감지용 Box Collision
    // 실제 크기는 GridManager에서 CellSize에 맞게 조정
    BoxCollision = CreateDefaultSubobject<UBoxComponent>(
        TEXT("BoxCollision"));
    BoxCollision->SetupAttachment(Root);
    BoxCollision->SetBoxExtent(FVector(50.f, 50.f, 10.f));

    // Visibility 채널로 레이캐스트 감지
    BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
    BoxCollision->SetCollisionResponseToChannel(
        ECC_Visibility, ECR_Block);
}

void AGridCell::BeginPlay()
{
    Super::BeginPlay();
}

void AGridCell::SetOccupied(AActor* Unit)
{
    OccupyingUnit = Unit;
    CellState = ECellState::Occupied;
}

void AGridCell::ClearCell()
{
    OccupyingUnit = nullptr;
    CellState = ECellState::Empty;

    // BP 이벤트 발생
    OnUnitRemoved();
}

// BlueprintNativeEvent 기본 구현 (BP에서 오버라이드 가능)
void AGridCell::SetHighlight_Implementation(bool bActive, bool bCanPlace)
{
    // 기본 구현은 비워둠
    // BP_GridCell에서 머터리얼 변경 로직 구현
}

void AGridCell::OnUnitSpawned_Implementation(AActor* SpawnedUnit)
{
    // 기본 구현은 비워둠
    // BP_GridCell에서 이펙트/사운드 구현
}

void AGridCell::OnUnitRemoved_Implementation()
{
    // 기본 구현은 비워둠
}