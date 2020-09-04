#pragma once

#include "Define.h"
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

	virtual int GetMaxLife();
	bool IsFullLife() { return GetCurrentLife() == GetMaxLife(); }

	int Pet::getAttack();
	void SetAttack(int iAttack) { attack = iAttack; }

	int Pet::getDefence();
	void SetDefence(int iDef) { defence = iDef; }

	int Pet::getDexterity();
	void SetDexterity(int iDex) { dexterity = iDex; }
	
	int GetGeneration() { return generation; }

	int GetClass() { return Class; }
	int GetElement();
	std::string GetElementText();
	bool IsUnevo() { return (Class / 10000) == 7; }
	bool IsSuper() { return (Class / 1000) == 1; }
	int GetEvoNum() { return (Class % 10000) / 1000; }

	int Pet::getMedalAttack();
	int Pet::getMedalDefence();
	int Pet::getMedalDexterity();

	Item* Pet::getItem();
	void Pet::setItem(Item* pItem) { item = pItem; }
	void Pet::useItem(int itemId);
	void Pet::useMedicine(Item* item);

	std::string getPortraitPath();
	std::string getDetails();
	Asset GetMouseoverTexture(SDL_Renderer *renderer, bool showIcon);

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
