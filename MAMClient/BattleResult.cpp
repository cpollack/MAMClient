#include "stdafx.h"
#include "BattleResult.h"
#include "Text.h"

BattleResult::BattleResult(SDL_Renderer* r, bool victory, int money, int exp, int petExp, int centerX, int centerY) {
	renderer = r;
	int width = 240;
	int height = 85;

	std::string victorious;
	if (victory) victorious = "You won the battle!";
	else victorious = "You lost the battle.";

	std::string gainMoney = "And gained $" + std::to_string(money) + ".";
	std::string gainExp = "You gained " + std::to_string(exp) + " experience points.";
	std::string gainPetExp = "Your pet gained " + std::to_string(petExp) + " experience points.";

	mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(mainTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_RenderClear(renderer);
	
	hlineRGBA(renderer, 0, width, 0, 0, 0, 0, 0xFF);
	hlineRGBA(renderer, 0, width, height - 1, 0, 0, 0, 0xFF);
	vlineRGBA(renderer, 0, 0, height - 1, 0, 0, 0, 0xFF);
	vlineRGBA(renderer, width - 1, 0, height - 1, 0, 0, 0, 0xFF);

	Text drawText(renderer, victorious, 7, 14);
	drawText.render();
	
	drawText.setText(gainMoney);
	drawText.setPosition(14, 28);
	drawText.render();

	drawText.setText(gainExp);
	drawText.setY(42);
	drawText.render();

	drawText.setText(gainPetExp);
	drawText.setY(56);
	drawText.render();

	SDL_SetRenderTarget(renderer, priorTarget);
	renderRect = { centerX - (width / 2), centerY - (height / 2), width, height };
}


BattleResult::~BattleResult() {
	SDL_DestroyTexture(mainTexture);
}


void BattleResult::render() {
	if (startTime == 0) startTime = SDL_GetTicks();
	SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
	currentTime = SDL_GetTicks() - startTime;
}


float BattleResult::getElapsedTime() {
	float elapsedTime = currentTime / 1000.0;
	return elapsedTime;
}