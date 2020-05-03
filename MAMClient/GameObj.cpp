#include "stdafx.h"
#include "GameObj.h"

#include "Define.h"
#include "GameMap.h"

GameObj::GameObj(SDL_Renderer* r) {
	renderer = r;
	Type = OBJ_NONE;
}

void GameObj::render() {
	RenderPos.x = Position.x + map->mapOffsetX - map->cameraX;
	RenderPos.y = Position.y + map->mapOffsetY - map->cameraY;
	if (!sprite) return;

	if (!sprite->started) sprite->start();

	sprite->setLocation(RenderPos);
	sprite->render();
}


void GameObj::SetCoord(SDL_Point coord) {
	Coord = coord;
	if (map) {
		Position = map->CenterOfCoord(Coord);
	}
}

SDL_Rect GameObj::GetRenderRect() {
	SDL_Rect renderRect;
	if (sprite) {
		//Always reload the sprite location in case its location became invalid
		sprite->setLocation(Position.x, Position.y);
		renderRect = sprite->getRenderRect();
	}
	return renderRect;
}

SDL_Rect GameObj::GetRenderRect(int frame) {
	SDL_Rect renderRect;
	if (sprite) {
		//Always reload the sprite location in case its location became invalid
		sprite->setLocation(Position.x, Position.y);
		renderRect = sprite->getRenderRect(frame);
	}
	return renderRect;
}