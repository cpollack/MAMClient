#pragma once

#include "Form.h"
#include "FormClasses.h"

#include "pBattleState.h"
#include "BattleResult.h"

#include "Label.h"
#include "Player.h"
#include "Dialogue.h"

class CChat;
class GameMap;
class pBattleResult;

class FormMain : public Form {
public:
	FormMain::FormMain(int w, int h);
	FormMain::~FormMain();
	void FormMain::initIntro();
	void FormMain::initLogin();
	void FormMain::initMain();

	void FormMain::render();
	void FormMain::render_intro();
	void FormMain::render_login();
	void FormMain::render_main();
	void FormMain::handleEvent(SDL_Event& e);
	void FormMain::handleEvent_login(SDL_Event& e);
	void FormMain::handleEvent_main(SDL_Event& e);
	void FormMain::handleWidgetEvent();
	void FormMain::handleWidgetEvent_intro();
	void FormMain::handleWidgetEvent_login();
	void FormMain::handleWidgetEvent_main();

	void FormMain::setMap(GameMap* gameMap);

	void FormMain::openShop(int shopId);

	void setMapCoordLabels(SDL_Point coord);

	/*void FormMain::setPlayerHealthGauge(int val);
	void FormMain::setPlayerHealthGauge(int val, int max);
	void FormMain::shiftPlayerHealthGauge(int val);
	void FormMain::setPlayerManaGauge(int val);
	void FormMain::setPlayerManaGauge(int val, int max);
	void FormMain::shiftPlayerManaGauge(int val);
	void FormMain::setPlayerExpGauge(int val);
	void FormMain::setPlayerExpGauge(int val, int max);
	void FormMain::shiftPlayerExpGauge(int val);
	void FormMain::setPetHealthGauge(int val);
	void FormMain::setPetHealthGauge(int val, int max);
	void FormMain::shiftPetHealthGauge(int val);
	void FormMain::setPetExpGauge(int val);
	void FormMain::setPetExpGauge(int val, int max);
	void FormMain::shiftPetExpGauge(int val);*/

	void FormMain::setCash(int cash);

	void FormMain::handleLoginResponse(pLoginResponse* packet);

private:
	/* Intro */
	Texture* logo;
	int introStartTime;

	/* Login */
	FormLogin *formLogin = nullptr;

	/* Main */
	/* - Game Area */
	SDL_Texture *gameTexture = NULL;
	SDL_Rect gameRect;

	/* - Everything else */
	/*Button *buttonFly;
	Button *buttonCharacter;
	Button *buttonPet;
	Button *buttonFight;
	Button *buttonChat;
	Button *buttonDetails;
	Button *buttonChat2;
	Button *buttonKungfu;
	Button *buttonMinMap;
	Button *buttonTeam;
	Button *buttonItems;
	Button *buttonPk;
	Button *buttonRecord;
	Button *buttonEmail;
	Gauge *player_health;
	Gauge *player_mana;
	Gauge *player_exp;
	Gauge *pet_health;
	Gauge *pet_exp;*/

	bool lastFlyToggle = false;
	int battleFormation = 0;
	int shopId;
};

extern FormMain* formMain;