#include "stdafx.h"
#include "Monster.h"

#include "Define.h"

Monster::Monster(SDL_Renderer *renderer, int id, std::string name, int look) : Entity(renderer, id, name, look) {
	BattleID = id;
	ID = BattleID | _IDMSK_MONSTER;
	Type = OBJ_MONSTER;
	BattleType = OBJTYPE_MONSTER;
}

Monster::~Monster() {
	//
}