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
	void UpdateLifeGrowth();
	float GetLifeGrowth() { return lifeGrowth; }

	int GetAttack();
	void SetAttack(int iAttack) { attack = iAttack; }
	float GetAttackRate() { return attackRate; }

	int GetDefence();
	void SetDefence(int iDef) { defence = iDef; }
	float GetDefenceRate() { return defenceRate; }

	int GetDexterity();
	void SetDexterity(int iDex) { dexterity = iDex; }
	float GetDexterityRate() { return dexterityRate; }

	void UpdateRates();
	void UpdateGrowth();
	float GetGrowth() { return growth; }

	int GetGeneration() { return generation; }

	int GetClass() { return Class; }
	int GetElement();
	std::string GetElementText();
	bool IsUnevo() { return (Class / 10000) == UNEVOPET; }
	bool IsSuper() { return (Class / 1000) == SUPERPET; }
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

	//Specific to compose
	int GetComposeChance() { return composeChance; }

private:
	int owner;

	int Element;

	int Class;
	int experience, nextLevelExp, generation, loyalty;
	int composeChance;

	int attack, defence, dexterity;
	float attackRate, defenceRate, dexterityRate;
	int medal_attack, medal_defence, medal_dexterity;

	float growth, lifeGrowth;

	Item *item = nullptr;
	int skillCount;
	int skills[5];

	void setElement();
};
