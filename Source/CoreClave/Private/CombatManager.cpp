// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatManager.h"
#include "CardUnitActor.h"

ACombatManager::ACombatManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

ECombatResult ACombatManager::ResolveCombat(
	ACardUnitActor* Attacker,
	ACardUnitActor* Defender,
	float& OutWinningRemainingHP) const
{
	OutWinningRemainingHP = 0.0f;

	if (!Attacker || !Defender)
	{
		return ECombatResult::BothDie;
	}

	const float AttackerHP = Attacker->CurrentHP;
	const float DefenderHP = Defender->CurrentHP;
	const float RemainingHP = AttackerHP - DefenderHP;

	// Decide the result from the two HP values.
	if (RemainingHP > 0.0f)
	{
		OutWinningRemainingHP = RemainingHP;
		return ECombatResult::AttackerWins;
	}

	if (RemainingHP < 0.0f)
	{
		OutWinningRemainingHP = -RemainingHP;
		return ECombatResult::DefenderWins;
	}

	return ECombatResult::BothDie;
}
