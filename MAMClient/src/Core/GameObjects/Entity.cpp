#include "stdafx.h"
#include "Entity.h"

#include "Define.h"
#include "INI.h";
#include "Sprite.h"
#include "GameMap.h"
#include "Battle.h"
#include "FloatingLabel.h"
#include "Options.h"

Entity::Entity(SDL_Renderer* r, int id, std::string name, int look) : GameObj(r) {
	ID = id;
	BattleID = id;
	Name = name;
	Look = look;
	setRole(Look);

	Direction = -1;
	Animation = -1;
}

Entity::~Entity() {
	CleanupBattle();
	if (sprite) delete sprite; sprite = nullptr;
	for (auto effect : effects) if (effect.sprite) delete effect.sprite;
}

void Entity::CleanupBattle() {
	if (BattleSprite) delete BattleSprite; BattleSprite = nullptr;
	ArrayPos = -1;
	BattlePos_Base = { 0,0 };
	BattlePos = { 0,0 };
	TargetingPos = { 0,0 };
	alive = true;
	defending = false;
	floatingLabels.clear();
	clearEffects();
}

void Entity::render() {
	GameObj::render();

	render_effects(RenderPos);

	//render_aura()?
}

void Entity::render_battle() {
	if (!BattleSprite) return;

	if (!BattleSprite->started) BattleSprite->start();

	BattleSprite->setLocation(BattlePos);
	if (!Invisible) BattleSprite->render();

	render_effects(BattlePos);
	render_floatingLabels();
}

void Entity::render_effects(SDL_Point point) {
	std::vector<Effect>::iterator itr = effects.begin();
	while (itr != effects.end()) {
		Sprite* effect = itr->sprite;
		effect->render(point.x, point.y);
		if (effect->isFinished) {
			itr = removeEffect(effect);
			if (itr != effects.end()) itr++;
		}
		else itr++;
	}
}

void Entity::render_floatingLabels() {
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
	if (!sprite) return;

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

void Entity::handleEvent_battle(SDL_Event& e) {
	if (!BattleSprite) return;

	SDL_Rect sprRect = GetRenderRect(true);

	if (e.type == SDL_MOUSEMOTION) {
		if (doesPointIntersect(sprRect, e.motion.x, e.motion.y)) {
			//Only focus a npc when its 'solid' pixels are moused over
			SDL_Point getPixel = { e.motion.x - sprRect.x, e.motion.y - sprRect.y };

			Asset currentTexture = BattleSprite->getCurrentTexture();
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

void Entity::setDirection(int direction, bool forBattle) {
	if (!forBattle) {
		Direction = direction;

		sprDirection = direction + 1;
		if (sprDirection > 7) sprDirection -= 8;
	}
	else {
		BattleSprDirection = direction + 1;
		if (BattleSprDirection > 7) BattleSprDirection -= 8;
	}
}

void Entity::setDirectionToCoord(SDL_Point coordinate, bool forBattle) {
	int newDirection = getDirectionToCoord(coordinate);
	setDirection(newDirection, forBattle);
}

int Entity::getDirection(bool forBattle) {
	if (!forBattle) return Direction;
	else return BattleSprDirection;
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

void Entity::setAnimation(int animation, bool forBattle) {
	if (!forBattle) Animation = animation;
	else BattleAnimation = animation;
}

void Entity::setColorShifts(ColorShifts shifts, bool forBattle) {
	//Set last anim to -1 to force a reload
	if (!forBattle) {
		colorShifts.clear();
		colorShifts = shifts;
		lastSpriteAnimation = -1;
	}
	else {
		BattleColorShifts.clear();
		BattleColorShifts = shifts;
		lastBattleAnimation = -1;
	}
}

void Entity::loadSprite(bool forBattle) {
	Sprite *destSprite = nullptr;
	int DestAnimation = -1;
	if (!forBattle) {
		if (lastSpriteAnimation == Animation && lastSpriteDirection == sprDirection) return;
		lastSpriteAnimation = Animation;
		lastSpriteDirection = sprDirection;
		DestAnimation = Animation;

		if (sprite) delete sprite; sprite = nullptr;
		if (Animation < 0 || sprDirection < 0) return;

		if (Role.length() == 0) setRole(Look);

		std::vector<std::string> frames = getSpriteFramesFromAni(Role, Animation, sprDirection);
		if (!frames.size()) return;

		std::vector<Asset> textures;
		for (auto frame : frames) {
			Asset texture(new Texture(renderer, frame));
			textures.push_back(texture);
		}

		if (options.IsColorDisabled(Look)) sprite = new Sprite(renderer, textures, stCharacter);
		else sprite = new Sprite(renderer, textures, stCharacter, colorShifts);
		sprite->setLocation(Position.x, Position.y);
		destSprite = sprite;
	}
	else {
		if (lastBattleAnimation == BattleAnimation && lastBattleDirection == BattleSprDirection) return;
		lastBattleAnimation = BattleAnimation;
		lastBattleDirection = BattleSprDirection;
		DestAnimation = BattleAnimation;

		if (BattleSprite) delete BattleSprite; BattleSprite = nullptr;
		if (BattleAnimation < 0 || BattleSprDirection < 0) return;

		if (Role.length() == 0) setRole(Look);

		std::vector<std::string> frames = getSpriteFramesFromAni(Role, BattleAnimation, BattleSprDirection);
		if (!frames.size()) return;

		std::vector<Asset> textures;
		for (auto frame : frames) {
			Asset texture(new Texture(renderer, frame));
			textures.push_back(texture);
		}

		if (options.IsColorDisabled(Look)) BattleSprite = new Sprite(renderer, textures, stCharacter);
		else BattleSprite = new Sprite(renderer, textures, stCharacter, BattleColorShifts);
		BattleSprite->setLocation(BattlePos.x, BattlePos.y);
		destSprite = BattleSprite;
	}

	
	switch (DestAnimation) {
	case Attack01:
	case Attack02:
	case Attack03:
		destSprite->setFrameInterval(150);
		break;
	case Faint:
		destSprite->setFrameInterval(40);
		break;
	case Walk:
		destSprite->speed = 750;
		destSprite->setFrameInterval(50);
		break;
	default:
		destSprite->setFrameInterval(350);
	}
}

SDL_Rect Entity::GetRenderRect(bool forBattle) {
	if (!forBattle) return GameObj::GetRenderRect();

	SDL_Rect renderRect;
	if (BattleSprite) {
		//Always reload the sprite location in case its location became invalid
		BattleSprite->setLocation(BattlePos.x, BattlePos.y);
		renderRect = BattleSprite->getRenderRect();
	}
	return renderRect;
}

SDL_Rect Entity::GetRenderRect(int frame, bool forBattle) {
	if (!forBattle) return GameObj::GetRenderRect(frame);

	SDL_Rect renderRect;
	if (BattleSprite) {
		//Always reload the sprite location in case its location became invalid
		BattleSprite->setLocation(BattlePos.x, BattlePos.y);
		renderRect = BattleSprite->getRenderRect(frame);
	}
	return renderRect;
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

bool Entity::IsEnemy() {
	return (BattleType == OBJTYPE_MONSTER || BattleType == OBJTYPE_VSPLAYER || BattleType == OBJTYPE_VSPET) ? true : false;
}

void Entity::addFloatingLabel(std::string text) {
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