#include "stdafx.h"
#include "PetMagic.h"

#include "INI.h"
#include "Entity.h"
#include "Sprite.h"
#include "GameMap.h"

CPetMagic::CPetMagic(SDL_Renderer *r, int mode) {
	renderer = r;
	this->mode = mode;
}

CPetMagic::~CPetMagic() {
	//entity sprite will self free
}

void CPetMagic::render() {
	//check if within renderable frame?
	if (state == PETMAGIC_SOURCE) {
		for (int i = 0; i < sourceCount; i++) renderEntity(source[i]);
	}

	if (state == PETMAGIC_DEST || state == PETMAGIC_IDLE) {
		for (int i = 0; i < destCount; i++) renderEntity(dest[i]);
	}
}

void CPetMagic::renderEntity(PetMagic_Entity &entity) {
	if (!entity.sprite) return;

	SDL_Point RenderPos;
	RenderPos.x = entity.position.x + map->mapOffsetX - map->cameraX;
	RenderPos.y = entity.position.y + map->mapOffsetY - map->cameraY;

	if (!entity.sprite->started) entity.sprite->start();

	entity.sprite->setLocation(RenderPos);
	entity.sprite->render();

	//render effects ontop of pet
	for (auto effect : entity.effects) effect.sprite->render(RenderPos.x, RenderPos.y);
}

void CPetMagic::step() {
	if (state == PETMAGIC_INITSOURCE) {
		source[0].coordinate = coordinate;
		source[0].position = map->CenterOfCoord(coordinate);
		//check for mode evolve versus compose?
		addEffect(source[0], EFFECT_SOULFLY);
		addEffect(source[0], EFFECT_SOULFLY, 600);
		addEffect(source[0], EFFECT_SOULSHINE);
		if (sourceCount > 1) {
			source[0].position.x -= 60;
			source[1].coordinate = coordinate;
			source[1].position = map->CenterOfCoord(coordinate);
			source[1].position.x += 60;
			addEffect(source[1], EFFECT_SOULFLY);
			addEffect(source[1], EFFECT_SOULFLY, 600);
			addEffect(source[1], EFFECT_SOULSHINE);
		}

		state = PETMAGIC_SOURCE;
		timer = SDL_GetTicks();
		return;
	}

	if (state == PETMAGIC_SOURCE) {
		DWORD current = SDL_GetTicks();
		DWORD elapsed = current - timer;

		if (elapsed > 3000) {
			state = PETMAGIC_INITDEST;
		}
	}

	if (state == PETMAGIC_INITDEST) {
		dest[0].coordinate = coordinate;
		dest[0].position = map->CenterOfCoord(coordinate);
		addEffect(dest[0], EFFECT_SOULRETURN);
		addEffect(dest[0], EFFECT_SOULRETURN, 500);
		addEffect(dest[0], EFFECT_SOULSHINE);
		if (destCount > 1) {
			dest[0].position.x -= 60;
			dest[1].coordinate = coordinate;
			dest[1].position = map->CenterOfCoord(coordinate);
			dest[1].position.x += 60;
			addEffect(dest[1], EFFECT_SOULRETURN);
			addEffect(dest[1], EFFECT_SOULRETURN, 500);
			addEffect(dest[1], EFFECT_SOULSHINE);
		}

		state = PETMAGIC_DEST;
		timer = SDL_GetTicks();
		return;
	}

	if (state == PETMAGIC_DEST) {
		DWORD current = SDL_GetTicks();
		DWORD elapsed = current - timer;

		if (elapsed > 3000) {
			state = PETMAGIC_IDLE;

			// Let effects end on their own
			for (int i = 0; i < destCount; i++) {
				//dest[i].clearEffects();
				for (auto effect : dest[i].effects) {
					effect.sprite->StopOnNextLoop();
					//effect.sprite->repeatMode = 1;
				}
			}
			timer = SDL_GetTicks();
			return;
		}
	}

	if (state == PETMAGIC_IDLE) {
		DWORD current = SDL_GetTicks();
		DWORD elapsed = current - timer;

		if (elapsed > 3000) {
			state = PETMAGIC_END;
			return;
		}
	}
}

void CPetMagic::addSource(int look, ColorShift shifts[3]) {
	source[sourceCount].look = look;
	for (int i = 0; i < 3; i++) source[sourceCount].colorShifts.push_back(shifts[i]);
	createSprite(source[sourceCount]);
	sourceCount++;
}

void CPetMagic::addDestination(int look, ColorShift shifts[3]) {
	dest[destCount].look = look;
	for (int i = 0; i < 3; i++) dest[destCount].colorShifts.push_back(shifts[i]);
	createSprite(dest[destCount]);
	destCount++;
}

void CPetMagic::createSprite(PetMagic_Entity &entity) {
	std::string role = Entity::getRole(entity.look);
	entity.sprite = new Sprite(renderer, getSpriteFramesFromAni(role, StandBy, 0), stCharacter, entity.colorShifts);
}

void CPetMagic::addEffect(PetMagic_Entity &entity, int effect, int delay) {
	std::string effectName = EffectToString(effect);
	if (effectName.size() == 0) {
		gClient.addToDebugLog("Failed to get effect name [" + std::to_string(effect) + "] in PetMagic");
		return;
	}

	int speed = 50;
	SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
	if (effect == EFFECT_SOULSHINE) {
		speed = 80;
	}

	INI commonIni("INI\\Common.ini", effectName);

	std::vector<Asset> assets;
	std::string frameAmount = commonIni.getEntry("FrameAmount");
	if (frameAmount.size() == 0) {
		gClient.addToDebugLog("Failed to get frame amount [" + effectName + "] for in PetMagic");
		return;
	}

	for (int i = 0; i < stoi(frameAmount); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);
		std::string path = commonIni.getEntry(nextFrame);
		if (path[0] == '.') path = "data\\" + path.substr(2);
		else path = "data\\" + path;
		Asset asset(new Texture(renderer, path, true));
		asset->setBlendMode(blendMode);
		assets.push_back(asset);
	}
	Sprite *sprEffect = new Sprite(renderer, assets, stEffect);
	sprEffect->setFrameInterval(speed);
	if (effect == EFFECT_SOULSHINE) {
		sprEffect->SetLoopTimer(800);
		sprEffect->repeatMode = 1;
	}
	else sprEffect->repeatMode = 0;
	if (delay) sprEffect->SetStartDelay(delay);
	sprEffect->start();

	Effect ef;
	ef.type = effect;
	ef.sprite = sprEffect;
	entity.effects.push_back(ef);
}

void CPetMagic::start() {
	state = PETMAGIC_INITSOURCE;
}