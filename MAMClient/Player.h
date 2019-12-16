#pragma once

#include "User.h"

class pPlayerInfo;
class pItem;

class GameMap;
class Pet;
class Inventory;
class Item;


class Player : public User {
public:
	Player(int acctId, int look, int face, char* name);
	Player::~Player();

	void render();
	void step();

public:
	std::string rankDesc;

	int AccountId;
	int experience, cultivation;
	int life_current, life_max, mana_current, mana_max;
	int point_unused, life, mana, attack, defence, dexterity;
	
	int cash, reputation, virtue;
	int wuxing, kungfu, petRaising, thievery;

	std::string PlayerRole; //Trimmed role used for 

	Item* equipment[5] = { nullptr };
	Inventory* inventory;

	DWORD lastPositionPacket = 0; //Player position is sent every 2000ms while walking

	
public:
	void jumpTo(SDL_Point coord);
	void walkTo(SDL_Point coord);

	void setCoord(SDL_Point coord);
protected: 
	void takeNextStep();
	
public:
	void setPlayerInfo(pPlayerInfo* packet);
	
	int getLevelUpExperience();

	void Player::addPet(Pet* newPet);
	Pet* Player::setActivePet(int id);
	Pet* Player::getActivePet();
	Pet* Player::getPet(int petId);
	std::vector<Pet*> Player::getPetList();
	void Player::removePet(int petId);

	void addItem(pItem* packet);
	void Player::useItem(int itemId);
	void Player::useMedicine(Item* item);
	void Player::setEquipment(pItem* packet);
	void Player::equipItem(Item* item);
	void Player::unequipItem(int slot);
	void Player::unequipItem(Item* item);
	void Player::sellItem(int itemId);
	void Player::buyItem(int itemId);

private:
	SDL_Renderer* renderer;

	Pet* petList[5] = { nullptr };
	int petCount = 0;
	Pet* activePet = nullptr;
};

extern Player* player;