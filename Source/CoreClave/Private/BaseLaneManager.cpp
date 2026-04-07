#include "BaseLaneManager.h"

#include "Net/UnrealNetwork.h"

ABaseLaneManager::ABaseLaneManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

void ABaseLaneManager::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		ResetBaseHealth();
	}
}

// 이 변수는 서버와 클라이언트가 똑같이 나눠 가져야 한다는 것을 의미.
void ABaseLaneManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 이 명령어를 통해서 서버에서 관리하면 클라이언트로 동기화해줄 수 있다.
	// 반드시 GetLifetimeReplicatedProps 함수 안에 있어야 한다.
	DOREPLIFETIME(ABaseLaneManager, TopBaseHealth);
	DOREPLIFETIME(ABaseLaneManager, BottomBaseHealth);
}

// 기지 체력 초기화
void ABaseLaneManager::ResetBaseHealth()
{
	if (!HasAuthority())
	{
		return;
	}

	TopBaseHealth = StartingBaseHealth;
	BottomBaseHealth = StartingBaseHealth;
	OnRep_BaseHealth();
}

// direction기반으로 상대편의 기지에 체력 입히기
bool ABaseLaneManager::ApplyBaseDamageByMoveDirection(int32 MoveDirection, int32 DamageAmount, int32& OutRemainingHealth)
{
	OutRemainingHealth = 0;

	if (!HasAuthority() || DamageAmount <= 0)
	{
		return false;
	}

	const bool bHitTopBase = (MoveDirection == 1);
	const bool bHitBottomBase = (MoveDirection == -1);
	if (!bHitTopBase && !bHitBottomBase)
	{
		return false;
	}

	int32& TargetBaseHealth = bHitTopBase ? TopBaseHealth : BottomBaseHealth;
	// 기지 체력이 0보다 낮을 수도 있으므로 max값으로 설정
	TargetBaseHealth = FMath::Max(TargetBaseHealth - DamageAmount, 0);
	OutRemainingHealth = TargetBaseHealth;

	BroadcastBaseState(bHitTopBase, TargetBaseHealth, DamageAmount);
	if (TargetBaseHealth == 0)
	{
		// 기지체력이 0이 되면 델리게이트 발생
		OnBaseDestroyed.Broadcast(bHitTopBase);
	}

	return true;
}

// Movedirection값에 따른 기지 체력 반환
int32 ABaseLaneManager::GetBaseHealthByMoveDirection(int32 MoveDirection) const
{
	if (MoveDirection == 1)
	{
		return TopBaseHealth;
	}
	if (MoveDirection == -1)
	{
		return BottomBaseHealth;
	}

	return 0;
}

void ABaseLaneManager::OnRep_BaseHealth()
{
	BroadcastBaseState(true, TopBaseHealth, 0);
	BroadcastBaseState(false, BottomBaseHealth, 0);
}

// 이벤트를 알리도록 하는 브로드캐스드가 담겨있는 함수
void ABaseLaneManager::BroadcastBaseState(bool bIsTopBase, int32 NewHealth, int32 DamageAmount)
{
	OnBaseHealthChanged.Broadcast(bIsTopBase, NewHealth, DamageAmount);
}
