#pragma once

#include "User.h"

class pPlayerInfo;
class pItem;

class GameMap;
class Pet;
class Inventory;
class Item;

const int EQUIPMENT_SLOTS = 5;
enum {
	SLOT_WEAPON,
	SLOT_ARMOR,
	SLOT_SHOE,
	SLOT_BODY,
	SLOT_HEAD
};

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
	int point_unused, life, mana, attack, defence, dexterity;
	
	int cash, reputation, virtue;
	int wuxing, kungfu, petRaising, thievery;

	std::string PlayerRole; //Trimmed role used for 

	Inventory* GetInventory() { return inventory; }
	Item* equipment[5] = { nullptr };
	Inventory* inventory;

	DWORD lastPositionPacket = 0; //Player position is sent every 2000ms while walking

	
public:
	void jumpTo(SDL_Point coord);
	void walkTo(SDL_Point coord);

	virtual void SetCoord(SDL_Point coord);
protected: 
	void takeNextStep();
	
public:
	void setPlayerInfo(pPlayerInfo* packet);
	
	int GetExperience() { return experience; }
	void SetExperience(int iExp) { experience = iExp; }
	int GetLevelUpExperience();	

	int GetUnusedPoints()	{ return point_unused; }
	void SetUnusedPoints(int point);

	int GetCultivation()	{ return cultivation + Level; }
	void SetCultivation(int iCult) { cultivation = iCult; }

	int GetLifePoint()		{ return life; }
	void SetLifePoint(int point);

	int GetManaPoint()		{ return mana; }
	void SetManaPoint(int point);

	int GetAttack();
	int GetAttackPoint()	{ return attack; }
	void SetAttackPoint(int point);

	int GetDefence();
	int GetDefencePoint()	{ return defence; }
	void SetDefencePoint(int point);

	int GetDexterity();
	int GetDexterityPoint()	{ return dexterity; }
	void SetDexterityPoint(int point);

	int GetCash() { return cash; }
	void SetCash(int iCash) { cash = iCash; }

	int GetReputation() { return reputation; }
	void SetReputation(int iRep) { reputation = iRep; }

	int GetVirtue() { return virtue; }
	void SetVirtue(int iVirtue) { virtue = iVirtue; }

	int GetWuxing() { return wuxing; }
	void SetWuxing(int iWux) { wuxing = iWux; }
	std::string GetWuxingTitle();

	int GetKungfu() { return kungfu; }
	void SetKungfu(int iKungfu) { kungfu = iKungfu; }
	std::string GetKungfuTitle();

	int GetPetRaising() { return petRaising; }
	void SetPetRaising(int iPetRaising) { petRaising = iPetRaising; }
	std::string GetPetRaisingTitle();

	int GetThievery() { return thievery; }
	void SetThievery(int iThievery) { thievery = iThievery; }
	std::string GetThieveryTitle();

public: // Pets
	void Player::addPet(Pet* newPet);
	Pet* Player::setActivePet(int id);
	Pet* getActivePet();
	Pet* GetMarchingPet() { return getActivePet(); }
	Pet* Player::getPet(int petId);
	std::vector<Pet*> Player::getPetList();
	void Player::removePet(int petId);

public: // Items
	void addItem(pItem* packet);
	void removeItem(Item* item);
	void Player::useItem(int itemId);
	void Player::useMedicine(Item* item);
	void Player::setEquipment(pItem* packet);
	void Player::equipItem(Item* item);
	void Player::unequipItem(int slot);
	void Player::unequipItem(Item* item);
	void Player::sellItem(int itemId);
	void Player::buyItem(int itemId);

	Item* GetWeapon();
	Item* GetArmor();
	Item* GetShoe();
	Item* GetBodyAccessory();
	Item* GetHeadAccessory();
	int GetNextAvailableItemID();

private:
	SDL_Renderer* renderer;

	Pet* petList[5] = { nullptr };
	int petCount = 0;
	Pet* activePet = nullptr;
};

extern Player* player;