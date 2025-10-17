// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyCoreClaveModeBase.generated.h"

// 게임에 적용할 게임모드 설정

UCLASS()
class CORECLAVE_API AMyCoreClaveModeBase : public AGameModeBase
{
	GENERATED_BODY()
protected:
	// 라운드 시작 전 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	float WarmupTime = 5.0f;
	// 실제 라운드 시간
	UPROPERTY(EditDefaultsOnly, Category = "Game Rules")
	float RoundTime = 30.0f;

	FTimerHandle RoundTimerHandle;

public:
	// 플레이어가 로그인하면 호출됩니다. 2명이 다 들어왔는지 체크합니다.
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 죽었을 때 호출될 핵심 함수입니다
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnPlayerKilled(AController* VictimController, AController* KillerController);

	// 라운드 시작을 알리는 함수
	void StartRound();
	// 라운드 종료를 알리는 함수
	void EndRound();
};
