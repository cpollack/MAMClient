#pragma once

#include "Entity.h"

class Fighter : public Entity {
public:
	Fighter(SDL_Renderer* renderer, int id, int objectType, int battleType, std::string name, int look);
	~Fighter();

	virtual void render();
	virtual void render_floatingLabels();
	virtual void handleEvent(SDL_Event& e);

private:
	SDL_Point BasePos, TargetingPos;
	int BattleID;
	int BattleType = 0;
	int ArrayPos = -1;
	bool alive = true;
	bool defending = false;
	std::string BattleYell = "";
	std::vector<FloatingLabel> floatingLabels;

	bool Invisible = false; //Treat as if it is within renderable area, but do not render main Sprite. Useful for effects IE Sphere/Mirror

public:
	int GetBattleId() { return BattleID; }

	int GetBattleType() { return BattleType; }

	SDL_Point GetBattleBasePos() { return BasePos; }
	void SetBattleBasePos(SDL_Point pos) { BasePos = pos; }

	SDL_Point GetTargetingPos() { return TargetingPos; }
	void SetTargetingPos(SDL_Point pos) { TargetingPos = pos; }

	int GetArrayPos() { return ArrayPos; }
	void SetArrayPos(int pos) { ArrayPos = pos; }

	std::string GetBattleYell() { return BattleYell; }
	void SetBattleYell(std::string yell) { BattleYell = yell; }

	void SetAlive(bool bAlive) { alive = bAlive; }
	void SetDefending(bool bDefend) { defending = bDefend; }

	bool IsAlive() { return alive; }
	bool IsDefending() { return defending; }
	bool IsEnemy();

	void addFloatingLabel(std::string text);
};