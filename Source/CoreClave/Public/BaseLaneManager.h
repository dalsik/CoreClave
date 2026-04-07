#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseLaneManager.generated.h"

// 이 이벤트가 직렬화가 되어 블루프린트에서 사용이 가능하도록 한다.(ThreeParams는 이벤트가 발생할 때 함께 전달할 데이터의 개수)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnBaseHealthChanged, bool, bIsTopBase, int32, NewHealth, int32, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnBaseDestroyed, bool, bIsTopBase);

UCLASS()
class CORECLAVE_API ABaseLaneManager : public AActor
{
	GENERATED_BODY()

public:
	ABaseLaneManager();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base|Rules")
	int32 StartingBaseHealth = 10;

	UPROPERTY(ReplicatedUsing = OnRep_BaseHealth, BlueprintReadOnly, Category = "Base|State")
	int32 TopBaseHealth = 10;

	UPROPERTY(ReplicatedUsing = OnRep_BaseHealth, BlueprintReadOnly, Category = "Base|State")
	int32 BottomBaseHealth = 10;

	UPROPERTY(BlueprintAssignable, Category = "Base|Events")
	FOnBaseHealthChanged OnBaseHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Base|Events")
	FOnBaseDestroyed OnBaseDestroyed;

	// 기지 체력 초기화
	UFUNCTION(BlueprintCallable, Category = "Base")
	void ResetBaseHealth();

	// 유닛의 direction을 기반으로 기지에 데미지를 입히도록 일단 적용
	UFUNCTION(BlueprintCallable, Category = "Base")
	bool ApplyBaseDamageByMoveDirection(int32 MoveDirection, int32 DamageAmount, int32& OutRemainingHealth);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Base")
	int32 GetBaseHealthByMoveDirection(int32 MoveDirection) const;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_BaseHealth();

private:
	void BroadcastBaseState(bool bIsTopBase, int32 NewHealth, int32 DamageAmount);
};
