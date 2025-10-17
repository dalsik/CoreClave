// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyCoreClaveGameStateBase.generated.h"

/**
 * 
 */

// 게임의 현재 상태를 나타낼 열거형
UENUM(BlueprintType)
enum class EGameState : uint8
{
	WaitingToStart,
	InProgress,
	RoundOver
};

UCLASS()
class CORECLAVE_API AMyCoreClaveGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
protected:
	// Replicated = 이 변수가 변경되면 모든 클라이언트에게 자동으로 복제된다.
	// OnRep_OnRoundTimeChanged = 변수가 복제된 후 클라이언트에서 특정 함수를 호출하고 싶을 때 사용
	UPROPERTY(Replicated)
	int32 Player1_Score;
	
	UPROPERTY(Replicated)
	int32 Player2_Score;

	// 현재 게임 상태(모두가 알아야 함)
	UPROPERTY(Replicated)
	EGameState CurrentGameState;

public:
	// 변수들이 복제될 수 있도록 설정하는 필수 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 클라이언트에서 HUD 업데이트를 위해 호출될 함수
	UFUNCTION()
	void OnRep_OnRoundTimeChanged();
};
