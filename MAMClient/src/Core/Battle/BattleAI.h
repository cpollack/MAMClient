#pragma once

class Fighter;

class BattleAI {
public:
	BattleAI() {}
	~BattleAI() {}

	Fighter* GetNextTarget(Fighter* fighter, std::vector<Fighter*> Enemies);
};