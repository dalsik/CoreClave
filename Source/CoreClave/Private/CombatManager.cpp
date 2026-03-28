// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatManager.h"
#include "CardUnitActor.h"
#include "GridManager.h"
#include "GridCell.h"

// Sets default values
ACombatManager::ACombatManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}


ECombatResult ACombatManager::ResolveCombat(
	ACardUnitActor* Attacker,
	ACardUnitActor* Defender,
	AGridManager* GridManager)
{
	if (!Attacker || !Defender || !GridManager)
	{
		return ECombatResult::BothDie;
	}

	float AttackerHP = Attacker->CurrentHP;
	float DefenderHP = Defender->CurrentHP;


	// 서로의 HP를 비교해서 승패 결정
	float RemainingHP = AttackerHP - DefenderHP;

	// 만약 공격자의 HP가 더 많은 경우
	if (RemainingHP > 0)
	{
		Attacker->CurrentHP = RemainingHP;


		// Attacker를 Defender의 셀로 이동
		AGridCell* AttackerCell = Attacker->CurrentCell;
		AGridCell* DefenderCell = Defender->CurrentCell;

		// 상대방 제거
		RemoveUnit(Defender);


		if (DefenderCell && AttackerCell)
		{
			GridManager->MoveUnit(
				AttackerCell->Row, AttackerCell->Col,
				DefenderCell->Row, DefenderCell->Col);
		}

		return ECombatResult::AttackerWins;
	}
	else if (RemainingHP < 0)
	{
		Defender->CurrentHP = RemainingHP;

		// 상대방 제거
		RemoveUnit(Attacker);

		// 원래 그 자리에 있던 상대방 유닛이 이겨도 제자리에 있음
		return ECombatResult::DefenderWins;
	}
	else
	{
		// 서로의 HP가 같다면 두 유닛 모두 지워버림.
		RemoveUnit(Attacker);
		RemoveUnit(Defender);

		return ECombatResult::BothDie;
	}
}



void ACombatManager::RemoveUnit(ACardUnitActor* Unit)
{
	if (!Unit) return;

	if (Unit->CurrentCell)
	{
		Unit->CurrentCell->ClearCell();
		Unit->CurrentCell = nullptr;
	}
	Unit->Destroy();
}
