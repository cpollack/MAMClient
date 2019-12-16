#ifndef __NPC_H
#define __NPC_H

#include "Sprite.h"
#include "RLE.h"
#include "INI.h"
#include "Label.h"

class pNpcInfo;
class GameMap;

/*
Types:
0 - none
Blacksmith - 100 - weapons / armor / darts
Doctor - 101 - potions / poisons
Boutique - 102 - headware / accessories
PetFeeder - 103
Pawnshop - 106
*/

class NPC {
private:
	SDL_Renderer* renderer;
	Label* nameLabel;
	int id, type, look;
	int x, y;

	Sprite* animations[8] = { nullptr };
	HSBSet hslSets[3] = { 0 };
	int direction, animDir;
	SDL_Point real_position;

public:
	std::string name;

	NPC(pNpcInfo* packet);
	~NPC();

	void NPC::render(int offsetX, int offsetY, bool showName);
	void NPC::loadAnimation(int dir);
	Sprite* NPC::getCurrentSprite();
	int NPC::getId();
	int NPC::getType();
};

#endif