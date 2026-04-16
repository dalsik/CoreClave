#include "ActionPointManager.h"

AActionPointManager::AActionPointManager()
{
	// 규칙 계산만 담당하므로 Tick은 필요 없다.
	PrimaryActorTick.bCanEverTick = false;
}

void AActionPointManager::RollActionPoints(
	int32 InStoredActionPoints, 
	int32& OutCurrentActionPoints,
	int32& OutStoredActionPoints,
	int32& OutLastRollResult) const
{
	// 저장 AP는 규칙 최대치까지만 반영한다.
	// LastRollResult는 이번 턴에 굴린 AP 주사위 결과를 나타내며, 다음 턴에 저장되지 않는다.
	// 최대 2AP까지 획득 가능
	const int32 ClampedStoredActionPoints =
		FMath::Clamp(InStoredActionPoints, 0, MaxStoredActionPoints);

	OutLastRollResult = FMath::RandRange(ActionPointRollMin, ActionPointRollMax); // 1~3 사이 무작위 값 
	OutCurrentActionPoints = BaseActionPoints + ClampedStoredActionPoints + OutLastRollResult;
	OutStoredActionPoints = 0;
}

// 특정 코스트만큼 ActionPoint를 사용
bool AActionPointManager::SpendActionPoints(
	int32 InCurrentActionPoints,
	int32 Cost,
	int32& OutRemainingActionPoints) const
{
	OutRemainingActionPoints = InCurrentActionPoints;

	if (Cost <= 0 || InCurrentActionPoints < Cost)
	{
		return false;
	}

	OutRemainingActionPoints = InCurrentActionPoints - Cost;
	return true;
}

// 사용하지 않은 AP는 저장해둔다.
void AActionPointManager::BankRemainingActionPoints(
	int32 InCurrentActionPoints,
	int32& OutStoredActionPoints,
	int32& OutRemainingActionPoints) const
{
	// 남은 AP는 최대 저장치까지만 다음 턴으로 넘긴다.
	OutStoredActionPoints = FMath::Clamp(InCurrentActionPoints, 0, MaxStoredActionPoints);
	OutRemainingActionPoints = 0;
}

// 카드 버렸을 시 AP획득 호출 로직
int32 AActionPointManager::AddActionPoints(int32 InCurrentActionPoints, int32 Amount) const
{
	if (Amount <= 0)
	{
		return InCurrentActionPoints;
	}

	return InCurrentActionPoints + Amount;
}


int32 AActionPointManager::GetDiscardCardActionPointGain() const
{
	return DiscardCardGain;
}

void AActionPointManager::ResetActionPoints(
	int32& OutCurrentActionPoints,
	int32& OutStoredActionPoints,
	int32& OutLastRollResult) const
{
	OutCurrentActionPoints = 0;
	OutStoredActionPoints = 0;
	OutLastRollResult = 0;
}

int32 AActionPointManager::RollUnitMoveRange() const
{
	// 이동 주사위는 랜덤이지만 유닛 최대 이동치는 넘지 않게 제한한다.
	const int32 RolledRange = FMath::RandRange(MoveRangeRollMin, MoveRangeRollMax);
	return FMath::Min(RolledRange, MaxUnitMoveRange);
}
