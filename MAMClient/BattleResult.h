#pragma once

#include "Client.h"

#include "Label.h"
#include "Texture.h"

class BattleResult {
public:
	BattleResult(SDL_Renderer* r, bool victory, int money, int exp, int petExp, int centerX, int centerY);
	~BattleResult();

	void BattleResult::render();
	float BattleResult::getElapsedTime();

private:
	SDL_Renderer* renderer;
	SDL_Texture* mainTexture = NULL;
	SDL_Rect renderRect;
	int startTime = 0, currentTime;
};

extern BattleResult* battleResult;
extern bool PlayerLeveled; 
extern bool PetLeveled;