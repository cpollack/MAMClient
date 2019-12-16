#include "stdafx.h"
#include "Pet.h"
#include "Client.h"
#include "MainWindow.h"

#include "Player.h"
#include "Inventory.h"

Pet::Pet(pPetInfo* packet) {
	id = packet->petId;
	owner = packet->ownerId;

	name = packet->name;
	monsterClass = packet->monsterClass;
	look = packet->look;
	setElement();

	level = packet->level;
	experience = packet->experience;
	nextLevelExp = getLevelUpExperience();
	generation = packet->generation;
	loyalty = packet->loyalty;

	life_current = packet->life_current;
	life_max = packet->life_max;
	attack = packet->attack;
	defence = packet->defence;
	dexterity = packet->dexterity;

	medal_attack = packet->medal_attack;
	medal_defence = packet->medal_defence;
	medal_dexterity = packet->medal_dexterity;

	skillCount = packet->skillCount;
	for (int i = 0; i < skillCount; i++)
		skills[i] = packet->skills[i];
}


Pet::~Pet() {

}


void Pet::setElement() {
	type = monsterClass / 10000;
	if (type == 1 || type == 7) {
		type = (monsterClass % 10000) / 1000;
	}

	switch (type) {
	case 2:
		element = "Water";
		break;
	case 3:
		element = "Fire";
		break;
	case 4:
		element = "Metal";
		break;
	case 5:
		element = "Wood";
		break;
	case 6:
		element = "Earth";
		break;
	}
}


std::string Pet::getElement() {
	return element;
}


int Pet::getId() {
	return id;
}

int Pet::getLook() {
	return look;
}

std::string Pet::getName() {
	return std::string(name);
}

int Pet::getLevel() {
	return level;

}
int Pet::getExperience() {
	return experience;
}

int Pet::getLevelUpExperience() {
	int nextLevel = level * (level + 1) * 0.75;
	return nextLevel;
}

int Pet::getLoyalty() {
	return loyalty;
}

int Pet::getCurrentHealth() {
	return life_current;
}

int Pet::getMaxHealth() {
	return life_max;
}

int Pet::getAttack() {
	return attack;
}

int Pet::getDefence() {
	return defence;
}

int Pet::getDexterity() {
	return dexterity;
}

int Pet::getMedalAttack() {
	return medal_attack;
}

int Pet::getMedalDefence() {
	return medal_defence;
}

int Pet::getMedalDexterity() {
	return medal_dexterity;
}

Item* Pet::getItem() {
	return item;
}


void Pet::useItem(int itemId) {
	Item* usedItem = player->inventory->getItem(itemId);
	if (!usedItem) {
		std::cout << "Error: Use Item - Could not find item id " << itemId << std::endl;
		return;
	}

	switch (usedItem->getType()) {
	/*case itWeapon:
	case itArmor:
	case itShoes:
	case itAccessory:
	case itHeadwear:
		equipItem(usedItem);
		break;*/

	case itMedicine:
		useMedicine(usedItem);
		break;
	}
}


void Pet::useMedicine(Item* item) {
	int life = item->getLife();
	if (life != 0) {
		life_current += life;
		mainForm->shiftPetHealthGauge(life);
	}

	player->inventory->removeItem(item->getId());
}