#pragma once

#include "Entity.h"

#include "Sprite.h"
#include "RLE.h"
#include "INI.h"
#include "Label.h"

class pNpcInfo;
class pAiNpcInfo;

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
	NPC(pAiNpcInfo* packet);
	~NPC();

	void render();
	void handleEvent(SDL_Event& e);

private:
	virtual void loadSprite();

	
	int Type;

private:
	SDL_Renderer* renderer;
	Texture* nameLabel;

	int GetShopID();

public:
	int GetType() { return Type; }
	std::string GetTypeText();

	bool isAiNpc{ false };
};
