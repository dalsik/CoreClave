// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyCoreClaveGameStateBase.generated.h"

/**
 * 
 */

// ������ ���� ���¸� ��Ÿ�� ������
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
	// Replicated = �� ������ ����Ǹ� ��� Ŭ���̾�Ʈ���� �ڵ����� �����ȴ�.
	// OnRep_OnRoundTimeChanged = ������ ������ �� Ŭ���̾�Ʈ���� Ư�� �Լ��� ȣ���ϰ� ���� �� ���
	UPROPERTY(Replicated)
	int32 Player1_Score;
	
	UPROPERTY(Replicated)
	int32 Player2_Score;

	// ���� ���� ����(��ΰ� �˾ƾ� ��)
	UPROPERTY(Replicated)
	EGameState CurrentGameState;

public:
	// �������� ������ �� �ֵ��� �����ϴ� �ʼ� �Լ�
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Ŭ���̾�Ʈ���� HUD ������Ʈ�� ���� ȣ��� �Լ�
	UFUNCTION()
	void OnRep_OnRoundTimeChanged();
};
