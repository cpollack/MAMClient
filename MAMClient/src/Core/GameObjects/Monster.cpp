#include "stdafx.h"
#include "Monster.h"

#include "Define.h"

Monster::Monster(SDL_Renderer *renderer, int id, std::string name, int look) : Entity(renderer, id, name, look) {
	Type = OBJ_MONSTER;
}

Monster::~Monster() {
	//
}