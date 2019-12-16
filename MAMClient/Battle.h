#pragma once

#include "GameLibrary.h"
#include "INI.h"

#include "Texture.h"
#include "Sprite.h"
#include "Player.h"
#include "Button.h"
#include "Label.h"

class Battle;
class BattleScene;
enum bsType;
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
	player_capture,
	player_item,
	player_run,
	player_switch,
	player_defend,
	pet_attack,
	pet_skill,
	pet_defend
};

enum bActorType {
	PLAYER,
	PET,
	ENEMY
};

enum bEffect {
	beNone,
	beFaint,
	beReady,
	beMirror,
	beSphere
};

typedef struct BattleArray {
	BYTE arrayData[348];
	bool visible = false;
	Texture *texture = nullptr;
	int type;
	std::string name;
	std::string pivot;
	std::string condition;
	int attack;
	int defense;
	int dex;
	bool top;

	~BattleArray() {
		delete texture;
	}
}BattleArray;

class pEnemyInfo;
class pFighterInfo;
class pBattleResponse;
class pBattleRound;
class pBattleResult;
class pColor;

class bMonster {
public:
	SDL_Renderer *renderer;
	Battle* battle;
	std::map<std::string, Sprite*> animation;
	std::vector<std::string> animation_names;
	bEffect effect = beNone;
	bEffect secEffect = beNone;
	Sprite *effectSprite = nullptr;
	Sprite *secEffectSprite = nullptr;
	std::string current_animation_name;
	int current_animation;
	int next_animation;
	int direction;
	int next_direction;
	SDL_Point basePos;
	SDL_Point pos;
	HSBSet hslSets[7] = { 0 };
	int hslSetCount = 0;

	int id;
	int battleId;
	std::string name;
	int look;
	int level;
	int type;

	bool alive = true;
	bool defending = false;
	std::string battleYell = "";
	SDL_Rect* focusRect = nullptr;

	bMonster(Battle* thisBattle) { 
		type = bActorType::ENEMY;
		battle = thisBattle;
	}
	~bMonster() { 
		for (auto &iter : animation_names) {
			delete animation[iter];
		}
		delete effectSprite; 
		delete secEffectSprite;
	}

	void setAnimation(int animType, int animDir);
	void loadAnimationSprite(int animType, int animDir);
	std::string getAniFileName(int aLook);
	void render();
	void setEffect(bEffect ef);
	void setSecondaryEffect(bEffect ef);
	void setHslShifts(int count, BYTE* shifts);
};

class bAlly :public bMonster {
public:
	int life_current;
	int life_max;
	int mana_current;
	int mana_max;

	bAlly(Battle* thisBattle) : bMonster(thisBattle) {
		type = bActorType::PLAYER;
	}
};

struct bAction {
	bMonster* source;
	bMonster* target;
	int action;
	SDL_Point target_position;
};

struct LessThanByY
{
	bool operator()(const bMonster* lhs, const bMonster* rhs) const
	{
		return lhs->pos.y > rhs->pos.y;
	}
};

class Battle {
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
	SDL_Rect battleRect = { 0, 0, 740, 410 };
	SDL_Rect renderRect;
	Texture *bgTexture = nullptr;

	std::priority_queue<bMonster*, std::vector<bMonster*>, LessThanByY> drawActors;
	std::vector<bAlly*>ally;
	int allyCount = 0;
	std::vector<bMonster*>monster;
	bMonster* captureTarget = nullptr;
	int monsterCount = 0;
	bMonster *focusedActor = nullptr; //used with mouseover
	int actorCount = 0;

	BattleArray* allyArray = nullptr;
	BattleArray* enemyArray = nullptr;
	std::map<bEffect,Sprite*> effects;
	std::vector<FloatingLabel*> floatingLabels; //for showing float labels: damage, actions, etc

	//30 second countdown
	int countDown, countDown_last;
	int countDown_start = 0;
	std::vector<Texture*> numbers;
	SDL_Rect tensRect, onesRect;

	//battle buttons are 48x48
	std::map<int, CButton*> battleButtons;
	int playerAction = -1, petAction = -1;
	bool selectTarget = false, playerButton_pressed = false, petButton_pressed = false;
	bMonster *selectedTarget = nullptr;
	std::vector<bMonster*> animateActors, currentlyAnimating;
	
	Sprite *crow = nullptr; //Used to show autobattle delay
	SDL_Texture* chatBubble = NULL;

	//BYTE *arrayData = nullptr;

	std::queue<BattleScene*> scenes;
	BattleScene* currentScene = nullptr;
	std::vector<Packet*> actions;
	BattleScene* responseScene = nullptr;
	int lastActionGroup = 0;

public:
	Battle::Battle(SDL_Renderer *r, int mapDoc, int actorCount);
	Battle::~Battle();
	void Battle::render();
	void Battle::render_battleArray(BattleArray* battleArray);
	void Battle::render_focusBox(bMonster* monster);
	bool Battle::handleEvent(SDL_Event& e);
	void Battle::step();

	void Battle::setAllyFormation(int formation);
	void Battle::setEnemyFormation(int formation);
	void Battle::handlePacket(Packet* packet);
	void Battle::handleEnemyInfoPacket(pEnemyInfo* packet);
	void Battle::handleFighterInfoPacket(pFighterInfo* packet);
	void Battle::handleResponsePacket(pBattleResponse* packet);
	void Battle::handleRoundPacket(pBattleRound* packet);
	void Battle::handleResultPacket(pBattleResult* packet);
	void Battle::handleColorPacket(pColor* packet);
	void Battle::createAttackResponse(pBattleResponse* packet, BattleScene* scene);
	void Battle::createDefendResponse(pBattleResponse* packet);
	void Battle::createCaptureResponse(pBattleResponse* packet);
	void Battle::addAlly(int id, std::string name, int look, int level, int life_current, int life_max, int mana_current, int mana_max);
	void Battle::addMonster(int id, std::string name, int look, int level);

	void Battle::start();
	bool Battle::isOver();
	int Battle::getMode();
	bMonster* Battle::getActorById(int actorId);

	void Battle::loadBattleArray(BattleArray** bArray, int arrayId, int type);
	SDL_Point Battle::getBattlePosFromArray(BattleArray* bArray, int fighterNum, bool isSource);

	bool Battle::doesMouseIntersect(SDL_Rect aRect, int x, int y);
	//bsType Battle::getSceneTypeFromAction(int action);

	void Battle::makeChatBubbleTexture();
	void Battle::loadEffect(bEffect effect);
	std::string Battle::effectTypeToString(int effectType);
	Sprite* Battle::getEffect(bEffect effect);
	FloatingLabel* Battle::createFloatingLabel(std::string top, std::string bottom, bMonster* target);
	FloatingLabel* Battle::createFloatingLabel(int damage, std::string bottom, bMonster* target);

public:
	void btnPlayerAttack_Click(SDL_Event& e);
	void btnPlayerDefend_Click(SDL_Event& e);
	void btnPlayerCapture_Click(SDL_Event& e);
	void btnPlayerRun_Click(SDL_Event& e);
	void btnPetAttack_Click(SDL_Event& e);
	void btnPetDefend_Click(SDL_Event& e);

};

extern Battle *battle;