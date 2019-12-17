#ifndef __PET_H
#define __PET_H

#include "pPetInfo.h"

class Item;

class Pet {
public:
	Pet(pPetInfo* packet);
	~Pet();

	int Pet::getId();
	int Pet::getBattleId();
	int Pet::getLook();
	std::string Pet::getElement();
	std::string Pet::getName();
	int Pet::getLevel();
	int Pet::getExperience();
	int Pet::getLevelUpExperience();
	int Pet::getLoyalty();
	int Pet::getCurrentHealth();
	int Pet::getMaxHealth();
	int Pet::getAttack();
	int Pet::getDefence();
	int Pet::getDexterity();

	int Pet::getMedalAttack();
	int Pet::getMedalDefence();
	int Pet::getMedalDexterity();

	Item* Pet::getItem();
	void Pet::useItem(int itemId);
	void Pet::useMedicine(Item* item);

private:
	int id, owner;
	std::string name;

	int type;
	std::string element;

	int monsterClass, look;
	int level, experience, nextLevelExp, generation, loyalty;

	int life_current, life_max;
	int attack, defence, dexterity;
	int medal_attack, medal_defence, medal_dexterity;

	Item *item = nullptr;
	int skillCount;
	int skills[5];

	void Pet::setElement();
};

#endif