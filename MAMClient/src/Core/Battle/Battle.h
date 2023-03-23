#pragma once

#include "GameLibrary.h"
#include "INI.h"


#include "Texture.h"
#include "Sprite.h"
#include "Button.h"
#include "Label.h"

class Battle;
class BattleScene;
class BattleAI;

class Fighter;
class Player;
class Pet;

//enum bsType;
struct FloatingLabel;

enum BattleMode {
	bmInit,
	bmWait,
	bmRoundStart,
	bmRoundEnd,
	bmTurnPlayer,
	bmTurnPet,
	bmAnimate,
	bmVictory,
	bmDefeat,
	bmEnd
};

enum BattleMenu {
	player_attack,
	player_skill,
	player_defend,
	player_item,
	player_run,
	player_capture,
	player_switch,
	pet_attack,
	pet_skill,
	pet_defend,
};

class BattleArray;
class pEnemyInfo;
class pFighterInfo;
class pNormalAct;
class pItemAct;
class pBattleRound;
class pBattleResult;
class pColor;
class Item;

struct bAction {
	Fighter* source;
	Fighter* target;
	int action;
	SDL_Point target_position;
};

struct LessThanByY {
	bool operator()(Fighter* lhs, Fighter* rhs);
};

class Battle {
public:
	Battle::Battle(SDL_Renderer *r, int mapDoc, int actorCount);
	Battle::~Battle();
	CButton* CreateButton(std::string btnName, BattleMenu menuPos, int imageIndex, bool isPlayer, SDL_Point point);
	void render();
	void render_ui();
	void render_focusBox(Fighter* fighter);
	void render_items();
	bool handleEvent(SDL_Event& e);
	void step();
	void setButtonVisibility();

private:
	SDL_Renderer *renderer;
	int mode;
	int round = 0;
	bool clickToAttack = false;
	bool battleAnimateReady = false;
	bool battleEnd = false;
	bool victory = false;
	bool running = false;
	int battleEndTimer;

	int doc;
	SDL_Rect battleRect;// = { 0, 0, 740, 410 };
	SDL_Rect renderRect;
	Texture *tBattleBG = nullptr;
	std::shared_ptr<BattleAI> battleAI;

	std::priority_queue<Fighter*, std::vector<Fighter*>, LessThanByY> drawActors;
	std::vector<Fighter*> allies;
	int allyCount = 0;
	std::vector<Fighter*> enemies;
	Fighter* captureTarget = nullptr;
	int enemyCount = 0;
	Fighter* focusedActor = nullptr; //used with mouseover
	int actorCount = 0;

	BattleArray* allyArray = nullptr;
	BattleArray* enemyArray = nullptr;
	std::vector<FloatingLabel*> floatingLabels; //for showing float labels: damage, actions, etc

	//30 second countdown
	int countDown, countDown_last;
	int countDown_start = 0;
	std::vector<Asset> numbers;
	SDL_Rect tensRect, onesRect;

	std::map<int, CButton*> battleButtons;
	int playerAction = -1, petAction = -1;
	bool playerButton_pressed = false, petButton_pressed = false;
	bool selectTarget = false, selectItem = false;
	Fighter* playerFighter = nullptr;
	Fighter* petFighter = nullptr;
	Fighter* selectedTarget = nullptr;
	Item* selectedItem = nullptr;
	std::vector<Fighter*> animateActors, currentlyAnimating;
	
	Sprite *crow = nullptr; //Used to show autobattle delay
	SDL_Texture* chatBubble = NULL;

	//BYTE *arrayData = nullptr;

	std::queue<BattleScene*> scenes;
	BattleScene* currentScene = nullptr;
	std::vector<Packet*> actions;
	BattleScene* responseScene = nullptr;
	int lastActionGroup = 0;

	//Items
	Asset itemBox;
	std::vector<Asset> itemAssets;
	std::vector<Item*> itemList;
	const SDL_Point itemBoxOffset = { 230,100 };
	Item* focusedItem = nullptr;
private:
	void LoadItemBox();
	void LoadItemList();
	Asset CreateItemMouseover(Item* item);

public:
	void Battle::setAllyFormation(int formation);
	void Battle::setEnemyFormation(int formation);
	void Battle::handlePacket(Packet* packet);
	void Battle::handleEnemyInfoPacket(pEnemyInfo* packet);
	void Battle::handleFighterInfoPacket(pFighterInfo* packet);
	void Battle::handleNormalActPacket(pNormalAct* packet);
	void Battle::handleItemActPacket(pItemAct* packet);
	void Battle::handleRoundPacket(pBattleRound* packet);
	void Battle::handleResultPacket(pBattleResult* packet);
	void Battle::handleColorPacket(pColor* packet);
	void Battle::createAttackResponse(pNormalAct* packet, BattleScene* scene);
	void Battle::createDefendResponse(pNormalAct* packet);
	void Battle::createCaptureResponse(pNormalAct* packet);
	void Battle::addAlly(DWORD id, std::string name, int look, int level, int life_current, int life_max, int mana_current, int mana_max);
	void Battle::addEnemy(DWORD id, std::string name, int look, int level);

	void Battle::start();
	bool Battle::isOver();
	int Battle::getMode();
	bool IsReady() { return mode != BattleMode::bmInit; }
	Fighter* Battle::getActorById(int actorId);

	void Battle::loadBattleArray(BattleArray** bArray, int arrayId, bool bAlly);
	//SDL_Point Battle::getBattlePosFromArray(BattleArray* bArray, int fighterNum, bool isSource);

	bool Battle::doesMouseIntersect(SDL_Rect aRect, int x, int y);

	void Battle::makeChatBubbleTexture();

public:
	void btnPlayerAttack_Click(SDL_Event& e);
	void btnPlayerSkill_Click(SDL_Event& e);
	void btnPlayerDefend_Click(SDL_Event& e);
	void btnPlayerItem_Click(SDL_Event& e);
	void btnPlayerRun_Click(SDL_Event& e);
	void btnPlayerCapture_Click(SDL_Event& e);
	void btnPlayerSwitch_Click(SDL_Event& e);
	void btnPetAttack_Click(SDL_Event& e);
	void btnPetSkill_Click(SDL_Event& e);
	void btnPetDefend_Click(SDL_Event& e);

};

extern Battle *battle;