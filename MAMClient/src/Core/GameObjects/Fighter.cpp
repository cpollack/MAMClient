#include "stdafx.h"
#include "Fighter.h"

#include "Define.h"

#include "Sprite.h"
#include "FloatingLabel.h"

Fighter::Fighter(SDL_Renderer *renderer, int id, int objectType, int battleType, std::string name, int look) : Entity(renderer, id, name, look) {
	Type = objectType;
	BattleType = battleType;

	ID = BattleID;
	BattleID = id;
	
	if (objectType == OBJTYPE_MONSTER) ID |= _IDMSK_MONSTER;		
}

Fighter::~Fighter() {
	ArrayPos = -1;
	BasePos = { 0,0 };
	Position = { 0,0 };
	TargetingPos = { 0,0 };
	alive = true;
	defending = false;
	floatingLabels.clear();
	clearEffects();
}

void Fighter::render() {
	if (!sprite) return;
	if (!sprite->started) sprite->start();

	sprite->setLocation(Position);
	sprite->render();

	render_effects(Position);

	render_floatingLabels();
}

void Fighter::render_floatingLabels() {
	if (floatingLabels.size() == 0)  return;

	for (int i = 0; i < floatingLabels.size(); i++) {
		if (!floatingLabels[i].started) {
			floatingLabels[i].started = true;
			floatingLabels[i].startTime = SDL_GetTicks();
		}
		else {
			SDL_Point p;
			p = floatingLabels[i].text->getPosition();
			p.x -= 1; p.y -= 1;
			floatingLabels[i].text->setPosition(p);
		}
		floatingLabels[i].text->Render();
	}

	//Remove expired labels
	std::vector<FloatingLabel>::iterator itr;
	for (itr = floatingLabels.begin(); itr != floatingLabels.end();) {
		int timeElapsed = (SDL_GetTicks() - itr->startTime) / 1000;
		if (timeElapsed >= 2) itr = floatingLabels.erase(itr);
		else ++itr;
	}
}

void Fighter::handleEvent(SDL_Event& e) {
	if (!sprite) return;

	SDL_Rect sprRect = GetRenderRect();

	if (e.type == SDL_MOUSEMOTION) {
		if (doesPointIntersect(sprRect, e.motion.x, e.motion.y)) {
			//Only focus a npc when its 'solid' pixels are moused over
			SDL_Point getPixel = { e.motion.x - sprRect.x, e.motion.y - sprRect.y };

			Asset currentTexture = sprite->getCurrentTexture();
			Uint32 pixel = currentTexture->getPixel(getPixel);
			Uint8 alpha = currentTexture->getPixelAlpha(pixel);

			if (alpha >= 64) {
				MouseOver = true;
			}
			else MouseOver = false;
		}
		else MouseOver = false;
	}
}

bool Fighter::IsEnemy() {
	return (BattleType == OBJTYPE_MONSTER || BattleType == OBJTYPE_VSPLAYER || BattleType == OBJTYPE_VSPET) ? true : false;
}

void Fighter::addFloatingLabel(std::string text) {
	FloatingLabel newLabel;
	SDL_Point point = GetBattleBasePos();
	point.y -= 50;

	newLabel.text.reset(stringToTexture(renderer, text, gui->font, 0, { 255, 255, 255, 255 }, 0));
	newLabel.text->setPosition(point);
	floatingLabels.push_back(newLabel);

	if (floatingLabels.size() <= 1) return;

	int top = point.y;
	for (int i = floatingLabels.size() - 2; i >= 0; i--) {
		SDL_Point pos = floatingLabels[i].text->getPosition();

		//Check if labels overlap, and push previous labels up
		if (floatingLabels[i].text->getPosition().y + floatingLabels[i].text->height > top) {
			int newY = top - floatingLabels[i].text->height;
			int dif = pos.y - newY;
			pos.x -= dif;
			pos.y = newY;
			floatingLabels[i].text->setPosition(pos);
		}
		top = pos.y;
	}
}