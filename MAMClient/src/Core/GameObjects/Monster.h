#pragma once

#include "Entity.h"

class Monster : public Entity {
public:
	Monster(SDL_Renderer *renderer, int id, std::string name, int look);
	~Monster();
};