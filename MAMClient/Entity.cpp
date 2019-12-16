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

void Entity::step() {
	//
}

void Entity::handleEvent(SDL_Event& e) {
	//
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