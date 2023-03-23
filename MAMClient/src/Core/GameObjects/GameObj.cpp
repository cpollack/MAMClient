#include "stdafx.h"
#include "GameObj.h"

#include "Define.h"
#include "GameMap.h"

GameObj::GameObj(SDL_Renderer* r) {
	renderer = r;
	Type = OBJ_NONE;
}

void GameObj::render() {
	RenderPos.x = Position.x + SpriteOffset.x + map->mapOffsetX - map->cameraX;
	RenderPos.y = Position.y + SpriteOffset.y + map->mapOffsetY - map->cameraY;
	if (!sprite) return;

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
	SDL_Rect renderRect = { 0, 0, 0, 0 };
	if (sprite) {
		//Always reload the sprite location in case its location became invalid
		sprite->setLocation(Position.x, Position.y);
		renderRect = sprite->getRenderRect();
		renderRect.x += SpriteOffset.x;
		renderRect.y += SpriteOffset.y;
	}
	return renderRect;
}

SDL_Rect GameObj::GetRenderRect(int frame) {
	SDL_Rect renderRect = { 0, 0, 0, 0 };
	if (sprite) {
		//Always reload the sprite location in case its location became invalid
		sprite->setLocation(Position.x + SpriteOffset.x, Position.y + SpriteOffset.x);
		renderRect = sprite->getRenderRect(frame);
	}
	return renderRect;
}