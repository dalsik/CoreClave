#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionPointManager.generated.h"

UCLASS()
class CORECLAVE_API AActionPointManager : public AActor
{
	GENERATED_BODY()

public:
	AActionPointManager();

	// 모든 플레이어가 공통으로 사용하는 기본 AP 규칙
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AP|Rules")
	int32 BaseActionPoints = 1;

	// 주사위처럼 굴려서 얻는 AP 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AP|Rules")
	int32 ActionPointRollMin = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AP|Rules")
	int32 ActionPointRollMax = 3;

	// 다음 턴으로 저장 가능한 AP 최대치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AP|Rules")
	int32 MaxStoredActionPoints = 2;

	// 카드 1장을 버렸을 때 얻는 AP
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AP|Rules")
	int32 DiscardCardGain = 1;

	// 이동 거리 랜덤 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Rules")
	int32 MoveRangeRollMin = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Rules")
	int32 MoveRangeRollMax = 4;

	// 한 유닛이 한 번에 이동 가능한 최대 칸 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move|Rules")
	int32 MaxUnitMoveRange = 3;

	// 저장 AP를 받아 이번 턴의 사용 가능한 AP를 계산
	UFUNCTION(BlueprintCallable, Category = "AP")
	void RollActionPoints(
		int32 InStoredActionPoints,
		int32& OutCurrentActionPoints,
		int32& OutStoredActionPoints,
		int32& OutLastRollResult) const;

	// 현재 AP에서 비용만큼 소모 가능한지 확인하고 남은 AP를 계산
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AP")
	bool SpendActionPoints(
		int32 InCurrentActionPoints,
		int32 Cost,
		int32& OutRemainingActionPoints) const;

	// 턴 종료 시 남은 AP를 저장 규칙에 맞게 변환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AP")
	void BankRemainingActionPoints(
		int32 InCurrentActionPoints,
		int32& OutStoredActionPoints,
		int32& OutRemainingActionPoints) const;

	// 카드 폐기 보상처럼 AP를 더하는 계산
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AP")
	int32 AddActionPoints(int32 InCurrentActionPoints, int32 Amount) const;

	// 카드 폐기로 얻는 AP를 계산
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AP")
	int32 GetDiscardCardActionPointGain() const;

	// AP 상태를 0으로 초기화할 때 사용하는 기본값 반환
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AP")
	void ResetActionPoints(
		int32& OutCurrentActionPoints,
		int32& OutStoredActionPoints,
		int32& OutLastRollResult) const;

	// 유닛 이동 가능 칸 수를 랜덤으로 계산
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Move")
	int32 RollUnitMoveRange() const;
};
