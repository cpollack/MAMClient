#ifndef __COLOSSEUM_H
#define __COLOSSEUM_H

#include "pColosseum.h"
#include "NPC.h"

class Colosseum {
public:
	Colosseum(pColosseum* packet);
	~Colosseum();

	void Colosseum::render(int offsetX, int offsetY);
	void Colosseum::handleEvent(SDL_Event* e);
	bool Colosseum::handleWidgetEvent();

private:
	SDL_Renderer* renderer;
	int id, x, y;
	SDL_Point real_pos;
	int minLevel, maxLevel;

	NPC* manager = nullptr;
	Texture* stage = nullptr;
};

#endif