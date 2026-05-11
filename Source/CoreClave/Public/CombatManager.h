// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

class ACardUnitActor;

// 전투 결과값을 열거형으로
UENUM(BlueprintType)
enum class ECombatResult : uint8
{
	AttackerWins UMETA(DisplayName = "AttackerWins"),
	DefenderWins UMETA(DisplayName = "DefenderWins"),
	BothDie UMETA(DisplayName = "BothDie")
};

UCLASS()
class CORECLAVE_API ACombatManager : public AActor
{
	GENERATED_BODY()

public:
	ACombatManager();

	// Only resolves combat. The caller applies movement and removal.
	ECombatResult ResolveCombat(
		ACardUnitActor* Attacker,
		ACardUnitActor* Defender,
		float& OutWinningRemainingHP) const;
};
