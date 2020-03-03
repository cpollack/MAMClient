#include "stdafx.h"
#include "BattleAI.h"
#include "Battle.h"

#include "Entity.h"

Entity* BattleAI::GetNextTarget(Entity* entity, std::vector<Entity*> Enemies) {
	if (!entity) return nullptr;

	for (auto enemy : Enemies) {
		if (enemy && enemy->IsAlive()) return enemy;
	}

	return nullptr;
}