#include "stdafx.h"
#include "SpriteView.h"
#include "GameLibrary.h"

SpriteView::SpriteView(int toX, int toY, int w, int h) : Widget(toX, toY) {
	width = w;
	height = h;

	sprPoint.x = width / 2;
	sprPoint.y = height * 0.75;

	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
}


SpriteView::~SpriteView() {
	for (auto spr : sprites) {
		delete spr.second;
	}
}


void SpriteView::render() {
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 255);
	SDL_RenderClear(renderer);

	if (sprite) {
		if (!sprite->started) sprite->start();
		sprite->setLocation(sprPoint.x, sprPoint.y);
		sprite->render();
	}

	SDL_SetRenderTarget(renderer, NULL);

	SDL_Rect renderRect = { x,y,width,height };
	SDL_RenderCopy(renderer, renderTexture, NULL, &renderRect);
}


void SpriteView::setLook(int lk) {
	look = lk;

	role = getRoleFromLook(look);
	animation = Walk;
	direction = 0;

	for (auto spr : sprites) {
		delete spr.second;
	}
	sprites.clear();
	sprite = nullptr;

	loadSprite();
}


void SpriteView::loadSprite() {
	int sprDir = direction + 1;
	if (sprDir > 7) sprDir -= 8;
	animString = animationTypeToString(animation) + std::to_string(sprDir);

	if (sprites.find(animString) == sprites.end()) {
		sprites[animString] = new Sprite(renderer, getSpriteFramesFromAni(role, animation, sprDir), stCharacter);
	}

	sprite = sprites[animString];
	sprite->speed = 10;
}