// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyCoreClaveModeBase.generated.h"

// ���ӿ� ������ ���Ӹ�� ����

UCLASS()
class CORECLAVE_API AMyCoreClaveModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	// ���� ���� �� ��� �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	float WarmupTime = 5.0f;
	// ���� ���� �ð�
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	float RoundTime = 30.0f;

	FTimerHandle RoundTimerHandle;

public:
	// �÷��̾ �α����ϸ� ȣ��˴ϴ�. 2���� �� ���Դ��� üũ�մϴ�.
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// �÷��̾ �׾��� �� ȣ��� �ٽ� �Լ��Դϴ�
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnPlayerKilled(AController* VictimController, AController* KillerController);

	// ���� ������ �˸��� �Լ�
	void StartRound();
	// ���� ���Ḧ �˸��� �Լ�
	void EndRound();
};
