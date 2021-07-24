#pragma once

class Entity;

class BattleAI {
public:
	BattleAI() {}
	~BattleAI() {}

	Entity* GetNextTarget(Entity* entity, std::vector<Entity*> Enemies);
};