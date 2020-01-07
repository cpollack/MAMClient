#pragma once

#include "Entity.h"

#include "Sprite.h"
#include "RLE.h"
#include "INI.h"
#include "Label.h"

class pNpcInfo;

/*
Types:
0 - none
Blacksmith - 100 - weapons / armor / darts
Doctor - 101 - potions / poisons
Boutique - 102 - headware / accessories
PetFeeder - 103
Pawnshop - 106
*/

class NPC : public Entity {
public:
	NPC(pNpcInfo* packet);
	~NPC();

	void render();
	void handleEvent(SDL_Event& e);

private:
	void loadSprite();

	int Type;

private:
	SDL_Renderer* renderer;
	Texture* nameLabel;
	//int id, type, look;
	//int x, y;

	//Sprite* animations[8] = { nullptr };
	HSBSet hslSets[3] = { 0 };
	//int direction, animDir;
	//SDL_Point real_position;

	int GetShopID();

public:
	//std::string name;

	//Sprite* getCurrentSprite();
	int GetType() { return Type; }
	std::string GetTypeText();
};
