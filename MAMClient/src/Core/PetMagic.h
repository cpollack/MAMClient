#pragma once

#include "Define.h"

enum {
	PETMAGIC_NONE,
	PETMAGIC_INITSOURCE,
	PETMAGIC_SOURCE,
	PETMAGIC_INITDEST,
	PETMAGIC_DEST,
	PETMAGIC_IDLE,
	PETMAGIC_END,
};

class Sprite;

struct PetMagic_Entity {
	~PetMagic_Entity() {
		if (sprite) delete sprite;
		clearEffects();
	}

	int look;
	Sprite *sprite = nullptr;
	ColorShifts colorShifts;
	SDL_Point coordinate;
	SDL_Point position;

	std::vector<Effect> effects;
	void clearEffects() {
		for (auto effect : effects) if (effect.sprite) delete effect.sprite;
		effects.clear();
	}
};

class CPetMagic {
public:
	CPetMagic(SDL_Renderer *r, int mode);
	~CPetMagic();

	void render();
	void renderEntity(PetMagic_Entity &entity);
	void step();

	void setCoordinate(SDL_Point coord) { coordinate = coord; }
	void addSource(int look, ColorShift shifts[3]);
	void addDestination(int look, ColorShift shifts[3]);
	void start();
	bool completed() { return (state == PETMAGIC_END); }

private:
	void createSprite(PetMagic_Entity &entity);
	void addEffect(PetMagic_Entity &entity, int effect, int delay=0);

private:
	SDL_Renderer *renderer;
	SDL_Point coordinate;

	int mode;
	int state = PETMAGIC_NONE;
	int sourceCount = 0, destCount = 0;
	PetMagic_Entity source[2], dest[2];
	DWORD timer = 0;
};