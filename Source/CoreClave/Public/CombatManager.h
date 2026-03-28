// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatManager.generated.h"

class ACardUnitActor;
class AGridCell;
class AGridManager;

UENUM(BlueprintType)
enum class ECombatResult : uint8
{
	AttackerWins UMETA(DisplayName = "AttackerWins"),
	DefenderWins    UMETA(DisplayName = "DefenderWins"),
	BothDie         UMETA(DisplayName = "BothDie")
};

UCLASS()
class CORECLAVE_API ACombatManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatManager();

	// 전투처리 메인 함수
	// Attacker : 전진하는 유닛
	// Defender : 목표 셀에 이미 존재하는 유닛
	UFUNCTION(BlueprintCallable, Category = "Combat")
	ECombatResult ResolveCombat(
		ACardUnitActor* Attacker,
		ACardUnitActor* Defender,
		AGridManager* GridManager);
	
	UPROPERTY()
	ACombatManager* CombatManager = nullptr;
private:
	void RemoveUnit(ACardUnitActor* Unit);

};
