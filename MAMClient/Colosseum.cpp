#include "stdafx.h"
#include "Client.h"
#include "Colosseum.h"
#include "GameMap.h"


Colosseum::Colosseum(pColosseum* packet) {
	renderer = gClient.renderer;

	id = packet->colId;
	x = packet->posX;
	y = packet->posY;
	real_pos = map->CoordToPoint(x, y);

	//manager = new NPC("Manager", packet->managerLook, real_pos);

	std::string path = "data/effect/duel/coloseum.tga";
	stage = new Texture(renderer, path);
}


Colosseum::~Colosseum() {
	delete manager;
}


void Colosseum::render(int offsetX, int offsetY) {
	if (stage) {
		SDL_Rect stageRect = { real_pos.x - 115 + offsetX, real_pos.y - 50 + offsetY, stage->rect.w, stage->rect.h };
		SDL_RenderCopy(renderer, stage->texture, NULL, &stageRect);
	}

	if (manager) {
		//
	}
}