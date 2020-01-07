#include "stdafx.h"
#include "Entity.h"

#include "INI.h";
#include "Sprite.h"
#include "GameMap.h"

Entity::Entity(SDL_Renderer* r, int id, std::string name, int look) {
	renderer = r;
	ID = id;
	Name = name;
	Look = look;
	setRole(Look);

	Direction = -1;
	Animation = -1;
}

Entity::~Entity() {
	if (sprite) delete sprite;
}

void Entity::render() {
	RenderPos.x = Position.x - map->cameraX;
	RenderPos.y = Position.y - map->cameraY;
	if (!sprite) return;

	if (!sprite->started) sprite->start();
	sprite->x = RenderPos.x;
	sprite->y = RenderPos.y;
	sprite->render();
}

void Entity::renderNameplate() {
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
		SDL_Rect destRect = { RenderPos.x - (w / 2), RenderPos.y - 82 - h, w, h };

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

	SDL_Rect sprRect = getRenderRect();

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

std::string Entity::getRole(int look) {
	INI roleINI("INI/Roles.ini", "RolesInfo");
	std::string entry = "Role" + std::to_string(look);
	std::string role = roleINI.getEntry(entry);
	return role;
}

void Entity::setRole(int look) {
	Role = getRole(look);
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

void Entity::setAnimation(int animation) {
	Animation = animation;
}

void Entity::loadSprite() {
	if (lastSpriteAnimation == Animation && lastSpriteDirection == sprDirection) return;
	lastSpriteAnimation = Animation;
	lastSpriteDirection = sprDirection;

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

	sprite = new Sprite(renderer, textures, stCharacter);
	if (Animation == Walk) sprite->speed = 3;

	//Set sprite location
	sprite->setLocation(Position.x, Position.y);
}


int Entity::getID() {
	return ID;
}

std::string Entity::getName() {
	return Name;
}

SDL_Rect Entity::getRenderRect() {
	SDL_Rect renderRect;
	if (sprite) {
		//Always reload the sprite location in case its location became invalid
		sprite->setLocation(Position.x, Position.y);
		renderRect = sprite->getRenderRect();
	}
	return renderRect;
}

void Entity::setCoord(SDL_Point coord) {
	Coord = coord;
	if (map) {
		Position = map->CenterOfCoord(Coord);
	}
}

SDL_Point Entity::getCoord() {
	return Coord;
}