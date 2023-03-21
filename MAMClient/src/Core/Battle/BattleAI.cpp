#include "stdafx.h"
#include "BattleAI.h"
#include "Battle.h"

#include "Fighter.h"

Fighter* BattleAI::GetNextTarget(Fighter* fighter, std::vector<Fighter*> Enemies) {
	if (!fighter) return nullptr;

	for (auto enemy : Enemies) {
		if (enemy && fighter->IsAlive()) return enemy;
	}

	return nullptr;
}