#ifndef __BATTLERESULT_H
#define __BATTLERESULT_H

#include "Client.h"

#include "Label.h"
#include "Texture.h"

class BattleResult {
public:
	BattleResult(bool victory, int money, int exp, int petExp, int centerX, int centerY);
	~BattleResult();

	void BattleResult::render();
	float BattleResult::getElapsedTime();

private:
	SDL_Renderer* renderer;
	SDL_Texture* mainTexture = NULL;
	SDL_Rect renderRect;
	int startTime = 0, currentTime;
};

#endif