#include "stdafx.h"
#include "Entity.h"

#include "Define.h"
#include "INI.h";
#include "Sprite.h"
#include "GameMap.h"
#include "Battle.h"
#include "Options.h"

Entity::Entity(SDL_Renderer* r, int id, std::string name, int look) : GameObj(r) {
	ID = id;
	Name = name;
	Look = look;
	setRole(Look);

	Direction = -1;
	Animation = -1;
}

Entity::~Entity() {
	if (sprite) delete sprite; sprite = nullptr;
	for (auto effect : effects) if (effect.sprite) delete effect.sprite;
}

void Entity::render() {
	GameObj::render();

	render_effects(RenderPos);
	
	//render_aura()?
}

void Entity::render_effects(SDL_Point point) {
	std::vector<Effect>::iterator itr = effects.begin();
	while (itr != effects.end()) {
		Sprite* effect = itr->sprite;
		effect->render(point.x, point.y);
		if (effect->completed) {
			itr = removeEffect(effect);
			if (itr != effects.end()) itr++;
		}
		else itr++;
	}
}

void Entity::render_nameplate() {
	SDL_Color nameColor = { 255, 255, 0, 255 };
	SDL_Color guildColor = { 255, 255, 0, 255 };
	SDL_Color shadow = { 32, 32, 32, 192 };

	SDL_Surface *nameSurface = TTF_RenderText_Blended(gui->chatFont, Name.c_str(), nameColor);
	SDL_Surface *shadowSurface = TTF_RenderText_Blended(gui->chatShadowFont, Name.c_str(), shadow);
	SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
	SDL_Texture *shadowTexture = SDL_CreateTextureFromSurface(renderer, shadowSurface);

	int w = nameSurface->w;
	int h = nameSurface->h;
	SDL_FreeSurface(nameSurface);
	SDL_FreeSurface(shadowSurface);

	if (nameTexture) {
		SDL_Rect srcRect = { 0, 0, w, h };
		SDL_Rect destRect = { RenderPos.x - (w / 2), RenderPos.y - 87 - h, w, h };

		if (NameplateBackground) {
			boxRGBA(renderer, destRect.x, destRect.y, destRect.x + destRect.w, destRect.y + destRect.h, 0, 0, 0, 96);
		}

		SDL_SetTextureBlendMode(nameTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		SDL_SetTextureBlendMode(shadowTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);

		SDL_RenderCopy(renderer, shadowTexture, &srcRect, &destRect);
		destRect.x -= 1;
		destRect.y -= 1;
		SDL_RenderCopy(renderer, nameTexture, &srcRect, &destRect);
		SDL_RenderCopy(renderer, nameTexture, &srcRect, &destRect);

		SDL_DestroyTexture(nameTexture);
		SDL_DestroyTexture(shadowTexture);
	}
}

void Entity::step() {
	//
}

void Entity::handleEvent(SDL_Event& e) {
	if (!sprite || !sprite->visible) return;

	SDL_Rect sprRect = GetRenderRect();

	if (e.type == SDL_MOUSEMOTION) {
		MouseOver = false;
		if (doesPointIntersect(sprRect, e.motion.x, e.motion.y)) {
			//Only focus a sprite when its 'solid' pixels are moused over
			SDL_Point getPixel = { e.motion.x - sprRect.x, e.motion.y - sprRect.y };

			Asset currentTexture = sprite->getCurrentTexture();
			if (currentTexture && currentTexture->loaded) {
				Uint32 pixel = currentTexture->getPixel(getPixel);
				Uint8 alpha = currentTexture->getPixelAlpha(pixel);

				if (alpha >= 64) {
					MouseOver = true;
				}
			}
		}
	}
}

std::string Entity::getRole(int look) {
	INI roleINI("INI/Roles.ini", "RolesInfo");
	std::string entry = "Role" + std::to_string(look);
	std::string role = roleINI.getEntry(entry);
	return role;
}

void Entity::setRole(int look) {
	Role = getRole(look);
}

void Entity::SetLook(int iLook) {
	Look = iLook;
	//effects?
}

void Entity::setDirection(int direction) {
	Direction = direction;

	sprDirection = direction + 1;
	if (sprDirection > 7) sprDirection -= 8;
}

void Entity::setDirectionToCoord(SDL_Point coordinate) {
	int newDirection = getDirectionToCoord(coordinate);
	setDirection(newDirection);
}

int Entity::getDirection() {
	return Direction;
}

int Entity::getDirectionToCoord(SDL_Point coordinate) {
	return map->getDirectionToCoord(Coord, coordinate);
}

void Entity::SetEmotion(int emotion) {
	int anim = Animation;
	switch (emotion) {
	case EMOTE_STANDBY:
		anim = StandBy;
		break;
	case EMOTE_WAVE:
		anim = SayHello;
		break;
	case EMOTE_FAINT:
		anim = Faint;
		break;
	case EMOTE_SIT:
		anim = SitDown;
		break;
	case EMOTE_KNEEL:
		anim = Genuflect;
		break;
	case EMOTE_LAUGH:
		anim = Laugh;
		break;
	case EMOTE_ANGRY:
		anim = Fury;
		break;
	case EMOTE_SAD:
		anim = Sad;
		break;
	case EMOTE_CAST:
		anim = Cast;
		break;
	case EMOTE_BOW:
		anim = Politeness;
		break;
	default:
		anim = StandBy;
	}

	setAnimation(anim);
}

void Entity::setAnimation(int animation) {
	Animation = animation;
}

void Entity::setColorShifts(ColorShifts shifts) {
	//Set last anim to -1 to force a reload{
	colorShifts.clear();
	colorShifts = shifts;
	lastSpriteAnimation = -1;
}

void Entity::loadSprite() {
	if (lastSpriteAnimation == Animation && lastSpriteDirection == sprDirection) return;
	lastSpriteAnimation = Animation;
	lastSpriteDirection = sprDirection;

	if (!sprite)
	{
		if (options.IsColorDisabled(Look)) sprite = new Sprite(renderer, stCharacter);
		else sprite = new Sprite(renderer, stCharacter, colorShifts);
	}
	if (Animation < 0 || sprDirection < 0) return;

	if (Role.length() == 0) setRole(Look);

	std::vector<std::string> frames = getSpriteFramesFromAni(Role, Animation, sprDirection);
	if (!frames.size()) return;

	std::vector<Asset> textures;
	for (auto frame : frames) {
		Asset texture(new Texture(renderer, frame));
		textures.push_back(texture);
	}

	sprite->setFrames(textures);
	
	switch (Animation) {
	case Attack01:
	case Attack02:
	case Attack03:
		sprite->setFrameInterval(50);
		sprite->repeatMode = 1;
		break;
	case Faint:
		sprite->setFrameInterval(40);
		sprite->repeatMode = 1;
		break;
	case Walk:
		sprite->speed = 750;
		sprite->setFrameInterval(50);
		sprite->repeatMode = 0;
		break;
	default:
		sprite->setFrameInterval(350);
		sprite->repeatMode = 0;
	}

	sprite->start();
}

bool Entity::Visible()
{ 
	return sprite ? sprite->visible : false; 
}

void Entity::SetVisible(bool bVisible)
{ 
	if (sprite) sprite->visible = bVisible;
}

Sprite* Entity::CreateEffectSprite(int effect) {
	Sprite *sprEffect = nullptr;

	if (effect == EFFECT_NONE) return nullptr;

	std::string effectName = EffectToString(effect);

	int offsetX = 0;
	int offsetY = 0;
	int speed = 1000;
	int repeatMode = 1;
	bool useKey = false;
	SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;

	switch (effect) {
	case EFFECT_FLASHDOWN:
		speed = 300;
		blendMode = SDL_BLENDMODE_ADD;
		break;
	case EFFECT_READY:
		repeatMode = 0;
		break;
	case EFFECT_SPHERE:
	case EFFECT_MIRROR:
		repeatMode = 0;
		break;
	case EFFECT_FAINT:
		speed = 300;
		repeatMode = 0;
		break;
	case EFFECT_POISON:
		useKey = true;
		blendMode = SDL_BLENDMODE_ADD;
		break;
	case EFFECT_THINK:
		repeatMode = 3;
		offsetX = 10;
		offsetY = -100;
		break;
	}

	if (effectName.size() == 0) {
		gClient.addToDebugLog("Failed to get effect name [" + std::to_string(effect) + "] for " + Name);
		return nullptr;
	}
	INI commonIni("INI\\Common.ini", effectName);

	std::vector<Asset> assets;
	std::string frameAmount = commonIni.getEntry("FrameAmount");
	if (frameAmount.size() == 0) {
		gClient.addToDebugLog("Failed to get frame amount [" + effectName + "] for " + Name);
		return nullptr;
	}

	for (int i = 0; i < stoi(frameAmount); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);
		std::string path = commonIni.getEntry(nextFrame);
		if (path[0] == '.') path = "data\\" + path.substr(2);
		else path = "data\\" + path;
		Asset asset;
		if (useKey) asset.reset(new Texture(renderer, path, {0,0,0,255}, true));
		else asset.reset(new Texture(renderer, path, true));
		asset->setBlendMode(blendMode);
		assets.push_back(asset);
	}
	sprEffect = new Sprite(renderer, assets, stEffect);

	if (offsetX != 0) sprEffect->SetX(offsetX);
	if (offsetY != 0) sprEffect->SetY(offsetY);

	sprEffect->speed = speed; // remove?
	sprEffect->setFrameInterval(50);
	sprEffect->repeatMode = repeatMode;
	sprEffect->start();

	return sprEffect;
}


Sprite* Entity::addEffect(int eff) {
	if (eff == EFFECT_NONE) return nullptr;

	Effect effect;
	effect.type = eff;
	effect.sprite = CreateEffectSprite(eff);
	if (effect.sprite) effects.push_back(effect);

	return effect.sprite;
}

Sprite* Entity::findEffect(int eff) {
	Sprite* sprEffect = nullptr;

	for (auto effect : effects) {
		if (effect.type == eff) return effect.sprite;
	}

	return nullptr;
}

std::vector<Effect>::iterator Entity::removeEffect(int effect) {
	std::vector<Effect>::iterator itr;
	for (itr = effects.begin(); itr != effects.end(); itr++) {
		if (itr->type == effect) {
			delete itr->sprite;
			return effects.erase(itr);
		}
	}
}

std::vector<Effect>::iterator Entity::removeEffect(Sprite* sprEffect) {
	std::vector<Effect>::iterator itr;
	for (itr = effects.begin(); itr != effects.end(); itr++) {
		if (itr->sprite == sprEffect) {
			delete itr->sprite;
			return effects.erase(itr);
		}
	}
}

void Entity::clearEffects() {
	for (auto effect : effects) if (effect.sprite) delete effect.sprite;
	effects.clear();
}