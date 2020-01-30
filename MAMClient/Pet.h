#pragma once

#include "Monster.h"
#include "pPetInfo.h"

class Item;

#define MAX_PET_SKILLS 5

class Pet : public Monster {
public:
	Pet(SDL_Renderer *renderer, int id, std::string name, int look);
	Pet(pPetInfo* packet);
	~Pet();

	void updateInfo(pPetInfo* packet);

	int Pet::getExperience();
	void SetExperience(int iExp) { experience = iExp; }

	int Pet::getLevelUpExperience();

	int Pet::getLoyalty();
	void SetLoyalty(int iLoy) { loyalty = iLoy; }

	int Pet::getAttack();
	void SetAttack(int iAttack) { attack = iAttack; }

	int Pet::getDefence();
	void SetDefence(int iDef) { defence = iDef; }

	int Pet::getDexterity();
	void SetDexterity(int iDex) { dexterity = iDex; }
	
	int GetGeneration() { return generation; }

	int GetElement();
	std::string GetElementText();

	int Pet::getMedalAttack();
	int Pet::getMedalDefence();
	int Pet::getMedalDexterity();

	Item* Pet::getItem();
	void Pet::useItem(int itemId);
	void Pet::useMedicine(Item* item);

private:
	int owner;

	int Element;

	int Class;
	int experience, nextLevelExp, generation, loyalty;

	int attack, defence, dexterity;
	int medal_attack, medal_defence, medal_dexterity;

	Item *item = nullptr;
	int skillCount;
	int skills[5];

	void Pet::setElement();
};
