#include "stdafx.h"
#include "FormMain.h"

#include "Client.h"
#include "AssetManager.h"
#include "Battle.h"
#include "Chat.h"

#include "GameMap.h"
#include "Player.h"
#include "Pet.h"

#include "FormError.h"

FormMain::FormMain(int w, int h) : Form(w, h) {

	/*
	Main flow:

	1) Intro screen (load assets) rand GUI\rand_LogoPic\\logo01-10.jpg
	2) Server Group (possibly skip as there are no groups)
	3) Server Select
	4) login screen
	5) route to character creation or main

	*/
	setFormType(ftIntro);
	setFormStyle(fsCustom);
	initIntro();

	/*chat = new CChat();
	chat->SetRenderer(renderer);
	chat->SetFont(gui->font);*/
}


void FormMain::initIntro() {
	/*std::string logoPath = "GUI\\rand_LogoPic\\";
	std::string imgNum = std::to_string((rand() % 10) + 1);
	if (imgNum != "10") imgNum = "0" + imgNum;
	logoPath += "logo" + imgNum + ".jpg";
	logo = new Texture(renderer);
	logo->loadTexture(logoPath);*/

	introStartTime = SDL_GetTicks();

	gClient.configIni = new INI("INI\\config.ini");
	if (!gClient.configIni->setSection("Default")) {
		gClient.configIni->addSection("Default");
		gClient.configIni->setSection("Default");
	}
}


void FormMain::initLogin() {
	setFormType(ftLogin);
	std::string loginTitle = "Login - ";
	//get selected server name
	loginTitle += "MythOfOrient";
	//formLogin = new FormLogin(loginTitle, width/2, height/2);
}


void FormMain::initMain() {
	//Cleanup Intro
	delete logo;
	//delete formLogin;

	//Start Main
	setFormType(ftMain);
	setFormStyle(FormStyle::fsMain);
	setCanMinimize(true);

	//Game Area
	/*gameRect = { gui->left->width + 20, gui->topCenter->height + 9, 740, 410 };
	gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gameRect.w, gameRect.h);

	chat->SetWidth(gameRect.w);
	chat->SetHeightInLines(10);
	chat->SetPos(SDL_Point{ 0, gameRect.h - chat->GetHeight() });

	//gui stuff
	map->setMapUiRect(gameRect);*/

	int surfacePosX = gui->left->width;
	int surfacePosY = height - gui->bottomCenter->height - gui->surface->height;
	createLabel("lblCoordX", std::string(" "), surfacePosX + 220, surfacePosY + 10, true);
	createLabel("lblCoordY", std::string(" "), surfacePosX + 250, surfacePosY + 10, true);

	/*buttonFly = new Button(gui->button_fly_off->texture, gui->button_fly_on->texture, surfacePosX + 300, surfacePosY + 90);
	buttonFly->buttonType = btToggle;

	buttonCharacter = new Button(gui->button_basicMsg->texture, gui->button_basicMsgDown->texture, surfacePosX + 17, surfacePosY + 11);
	buttonCharacter->buttonType = btPress;

	buttonPet = new Button(gui->button_pet->texture, gui->button_petDown->texture, surfacePosX + 102, surfacePosY + 11);
	buttonPet->buttonType = btPress;

	buttonFight = new Button(gui->fight2->texture, gui->fight1->texture, surfacePosX + 290, surfacePosY + 5);
	buttonFight->buttonType = btPress;
	buttonChat = new Button(gui->chat2->texture, gui->chat1->texture, surfacePosX + 335, surfacePosY + 5);
	buttonChat->buttonType = btPress;

	buttonDetails = new Button(gui->heroStatus->texture, gui->heroStatus->texture, surfacePosX + 377, surfacePosY);
	buttonDetails->buttonType = btPress;
	buttonChat2 = new Button(gui->chatPanel->texture, gui->chatPanel->texture, surfacePosX + 422, surfacePosY);
	buttonChat2->buttonType = btPress;
	buttonKungfu = new Button(gui->selectMag->texture, gui->selectMag->texture, surfacePosX + 467, surfacePosY);
	buttonKungfu->buttonType = btPress;

	buttonMinMap = new Button(gui->minMap1->texture, gui->minMap2->texture, surfacePosX + 512, surfacePosY + 1);
	buttonMinMap->buttonType = btPress;
	buttonTeam = new Button(gui->team1->texture, gui->team2->texture, surfacePosX + 553, surfacePosY + 1);
	buttonTeam->buttonType = btPress;
	buttonItems = new Button(gui->items1->texture, gui->items2->texture, surfacePosX + 594, surfacePosY + 1);
	buttonItems->buttonType = btPress;
	buttonPk = new Button(gui->pk1->texture, gui->pk2->texture, surfacePosX + 635, surfacePosY + 1);
	buttonPk->buttonType = btPress;
	buttonRecord = new Button(gui->record1->texture, gui->record2->texture, surfacePosX + 676, surfacePosY + 1);
	buttonRecord->buttonType = btPress;
	buttonEmail = new Button(gui->email1->texture, gui->email2->texture, surfacePosX + 717, surfacePosY + 1);
	buttonEmail->buttonType = btPress;*/

	//Character Detail Labels
	createLabel("name", "Name", surfacePosX + 382, surfacePosY + 52, true);
	createLabel("name_value", player->GetName(), surfacePosX + 430, surfacePosY + 52, false);
	createLabel("nickname", "Nickname", surfacePosX + 382, surfacePosY + 76, true);
	createLabel("nickname_value", player->getNickName(), surfacePosX + 455, surfacePosY + 76, false);
	createLabel("level", "Level", surfacePosX + 382, surfacePosY + 100, true);
	createLabel("level_value", std::to_string(player->GetLevel()), surfacePosX + 427, surfacePosY + 100, false);

	createLabel("spouse", "Spouse", surfacePosX + 520, surfacePosY + 52, true);
	createLabel("spouse_value", player->getSpouse(), surfacePosX + 580, surfacePosY + 52, false);
	createLabel("cash", "Cash", surfacePosX + 520, surfacePosY + 76, true);
	createLabel("cash_value", std::to_string(player->cash), surfacePosX + 560, surfacePosY + 76, false);
	createLabel("reputation", "Reputation", surfacePosX + 520, surfacePosY + 100, true);
	createLabel("reputation_value", std::to_string(player->reputation), surfacePosX + 597, surfacePosY + 100, false);

	createLabel("rank_value", "Basic Warrior", surfacePosX + 664, surfacePosY + 52, false);
	createLabel("guild", "Guild", surfacePosX + 664, surfacePosY + 76, true);
	createLabel("guild_value", player->getGuild(), surfacePosX + 704, surfacePosY + 76, false);
	createLabel("guildRank", "Position", surfacePosX + 664, surfacePosY + 100, true);
	createLabel("guildRank_value", player->getGuildTitle(), surfacePosX + 724, surfacePosY + 100, false);

	/*player_health = new Gauge(surfacePosX + 62, surfacePosY + 47, gui->bg_slot, gui->yellow_slot);
	player_mana = new Gauge(surfacePosX + 62, surfacePosY + 70, gui->bg_slot, gui->green_slot);
	player_exp = new Gauge(surfacePosX + 62, surfacePosY + 93, gui->bg_slot, gui->yellow_slot);
	pet_health = new Gauge(surfacePosX + 259, surfacePosY + 47, gui->bg_slot, gui->green_slot);
	pet_exp = new Gauge(surfacePosX + 259, surfacePosY + 70, gui->bg_slot, gui->green_slot);*/

	/*setPlayerHealthGauge(player->life_current, player->life_max);
	setPlayerManaGauge(player->mana_current, player->mana_max);
	setPlayerExpGauge(player->experience, player->getLevelUpExperience());

	Pet* activePet = player->getActivePet();
	if (activePet) {
		setPetHealthGauge(activePet->getCurrentHealth(), activePet->getMaxHealth());
		setPetExpGauge(activePet->getExperience(), activePet->getLevelUpExperience());
	}*/
}

FormMain::~FormMain() {
	for (auto &labelKey : labelKeys) {
		delete labels[labelKey];
	}
	if (battle) delete battle;
	if (player) delete player;
	if (map) delete map;
}

void FormMain::render() {
	Form::render();

	switch (getFormType()) {
	case OldFormType::ftIntro:
		render_intro();
		break;
	case OldFormType::ftLogin:
		render_login();
		break;
	case OldFormType::ftMain:
		render_main();
		break;
	}

	SDL_RenderPresent(renderer);
}

void FormMain::render_intro() {
	SDL_RenderCopy(renderer, logo->texture, NULL, &logo->rect);
}


void FormMain::render_login() {
	SDL_RenderCopy(renderer, logo->texture, NULL, &logo->rect);
	//formLogin->render();

	for (auto subform : subforms) {
		subform->render();
	}
}


void FormMain::render_main() {
	//Render focus set to game area
	/*SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, gameTexture); {
		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(renderer);

		//Battle & Map
		if (battle) {
			if (battle->getMode() != BattleMode::bmInit) battle->render();
			else map->render();
		}
		else {
			if (map) map->render();
		}

		//Chat
		if (chat) chat->render();
	} SDL_SetRenderTarget(renderer, priorTarget);
	SDL_RenderCopy(renderer, gameTexture, NULL, &gameRect);*/

	for (auto &labelKey : labelKeys) {
		labels[labelKey]->render();
	}

	/*buttonFly->render();
	buttonCharacter->render();
	buttonPet->render();
	buttonFight->render();
	buttonChat->render();

	player_health->render();
	player_mana->render();
	player_exp->render();
	pet_health->render();
	pet_exp->render();

	buttonDetails->render();
	buttonChat2->render();
	buttonKungfu->render();

	buttonMinMap->render();
	buttonTeam->render();
	buttonItems->render();
	buttonPk->render();
	buttonRecord->render();
	buttonEmail->render();*/

	for (auto subform : subforms) {
		subform->render();
	}
}


void FormMain::handleEvent(SDL_Event& e) {
	if (e.window.windowID != windowID && !(e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT)) return;
	Form::handleEvent(e);

	switch (getFormType()) {
	case OldFormType::ftLogin:
		handleEvent_login(e);
		break;
	case OldFormType::ftMain:
		handleEvent_main(e);
		break;
	}
}


void FormMain::handleEvent_login(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	for (int i = subforms.size(); i > 0; i--) {
		if (subforms[i - 1]->handleEvent(&e)) return;
		SDL_Rect sfRect = { subforms[i - 1]->x, subforms[i - 1]->y, subforms[i - 1]->width, subforms[i - 1]->height };
		if (doesPointIntersect(sfRect, mx, my)) return;
	}

	//formLogin->handleEvent(&e);
}


void FormMain::handleEvent_main(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	for (int i = subforms.size(); i > 0; i--) {
		if (subforms[i-1]->handleEvent(&e)) return;
		SDL_Rect sfRect = { subforms[i - 1]->x, subforms[i - 1]->y, subforms[i - 1]->width, subforms[i - 1]->height };
		if (doesPointIntersect(sfRect, mx, my)) return;
	}

	/*if (buttonFly->handleEvent(&e)) return;
	if (buttonCharacter->handleEvent(&e)) return;
	if (buttonPet->handleEvent(&e)) return;
	if (buttonFight->handleEvent(&e)) return;
	if (buttonChat->handleEvent(&e)) return;
	if (buttonMinMap->handleEvent(&e)) return;
	if (buttonTeam->handleEvent(&e)) return;
	if (buttonItems->handleEvent(&e)) return;
	if (buttonPk->handleEvent(&e)) return;
	if (buttonRecord->handleEvent(&e)) return;
	if (buttonEmail->handleEvent(&e)) return;*/

	//For now, any subforms should disable events outside UI button clicks (such as opening character stats and items)
	//To be changed, maybe message forms shouldn't? TBD
	if (subforms.size() > 0) return;

	//Below events are specific to clicking the map, do not implement when in battle.
	/*if (battle && battle->getMode() != bmInit) {
		if (battle->handleEvent(&e)) return;
	}
	if (chat) chat->handleEvent(e);
	if (map->handleEvent(e)) return;*/
}


void FormMain::handleWidgetEvent() {
	if (gClient.wasDisconnected()) {
		FormError* errorForm = new FormError("Error", "Error", formMain->width / 2, formMain->height / 2);
		errorForm->setMessage("Error: Connection to the server has been interrupted. Please re-log in.");
		addSubform("Error", errorForm);
	}

	switch (getFormType()) {
	case OldFormType::ftIntro:
		handleWidgetEvent_intro();
		break;
	case OldFormType::ftLogin:
		handleWidgetEvent_login();
		break;
	case OldFormType::ftMain:
		handleWidgetEvent_main();
		break;
	}
}


void FormMain::handleWidgetEvent_intro() {
	int elapsedTime = (SDL_GetTicks() - introStartTime) / 1000;
	if (elapsedTime >= 1) {
		initLogin();
	}
}


void FormMain::handleWidgetEvent_login() {
	if (gClient.mainReady) {
		initMain();
		return;
	}

	int size = subforms.size();
	for (int i = 0; i < size; i++) {
		if (i < subforms.size()) subforms[i]->handleWidgetEvent();
	}

	//formLogin->handleWidgetEvent();

	std::vector<SubForm*>::iterator itr;
	for (itr = subforms.begin(); itr != subforms.end();)
	{
		SubForm* sf = *itr;
		if (sf->closeWindow) {
			itr = Form::deleteSubform(sf->getName());
		}
		else ++itr;
	}
}


void FormMain::handleWidgetEvent_main() {
	/*if (battle && battle->getMode() != bmInit) {
		if (battle->handleWidgetEvent()) {
			if (battle->getMode() == bmEnd) {
				delete battle;
				battle = nullptr;
			}
		}
		else return;
	}*/

	/*std::vector<SubForm*>::iterator itr = subforms.begin();
	while (itr != subforms.end()) {
		(*itr)->handleWidgetEvent();
		itr++;
	}*/
	int size = subforms.size();
	for (int i = 0; i < size; i++) {
		if (i < subforms.size()) subforms[i]->handleWidgetEvent();
	}

	SubForm* findForm = getSubform("Exit");
	if (findForm) {
		/*if (((FormExit*)findForm)->submitted) {
			deleteSubform("Exit");
			SDL_Event quitEvent;
			quitEvent.type = SDL_QUIT;
			SDL_PushEvent(&quitEvent);
		}*/
	}

	//These events happen on every 'step'
	map->step();

	//Temporary event handlers, these will be moved to function pointer callbacks
	/*if (buttonPet->wasPressed() && !battle) {
		SubForm* findForm = Form::getSubform("PetList");
		if (findForm) {
			Form::deleteSubform("PetList");
		}
		else {
			FormPetList* petForm = new FormPetList("Pet List", width / 2, height / 2);
			Form::addSubform("PetList", petForm);
		}
	}*/

	if (battle && battle->isOver()) {
		delete battle;
	}

	//Send battle request
	/*if (buttonFight->wasPressed() && !battle) {
		int teamSize = 1;
		pBattleState* battlePacket = new pBattleState(0, teamSize, player->getID(), 0);
		gClient.addPacket(battlePacket);
	}*/

	/*if (buttonFly->isToggled() != lastFlyToggle) {
		map->toggleJumpMode();
		lastFlyToggle = buttonFly->isToggled();
	}

	if (buttonChat->wasPressed()) {
		//
	}

	if (buttonMinMap->wasPressed()) {
		//
	}

	if (buttonMinMap->wasPressed()) {
		//
	}

	if (buttonTeam->wasPressed()) {
		//
	}

	if (buttonItems->wasPressed() && !battle) {
		SubForm* findForm = Form::getSubform("Inventory");
		if (findForm) {
			Form::deleteSubform("Inventory");
		}
		else {
			FormInventory* invForm = new FormInventory("Inventory", width/2, height/2);
			Form::addSubform("Inventory", invForm);
		}
	}*/

	std::vector<SubForm*>::iterator itr;
	for (itr = subforms.begin(); itr != subforms.end();)
	{
		SubForm* sf = *itr;
		if (sf->closeWindow) {
			itr = Form::deleteSubform(sf->getName());
		}
		else ++itr;
	}
}


void FormMain::setMap(GameMap* gameMap) {
	//deprecate
	//if (buttonFly && buttonFly->isToggled()) map->toggleJumpMode();
}


void FormMain::openShop(int sId) {
	shopId = sId;
	if (shopId == 0) return;
	
	//read shopini
	INI shopIni = *gClient.shopIni;
	std::string shopSection = "ShopId" + std::to_string(shopId);
	shopIni.setSection(shopSection);

	int itemCount = stoi(shopIni.getEntry("Count"));
	std::vector<int> shopItems;
	for (int i = 0; i < itemCount; i++) {
		std::string nextItem = "ItemType" + std::to_string(i);
		shopItems.push_back(stoi(shopIni.getEntry(nextItem)));
	}

	//create shopform
	//FormShop* shopForm = new FormShop("Shop", width / 2, height / 2);
	//shopForm->addShopInventory(shopItems);
	//Form::addSubform("Shop", shopForm);
}


void FormMain::setMapCoordLabels(SDL_Point coord) {
	labels["lblCoordX"]->renderText(std::to_string(coord.x));
	labels["lblCoordY"]->renderText(std::to_string(coord.y));
}

/*void FormMain::setPlayerHealthGauge(int val) {
	player_health->set(val);
}


void FormMain::setPlayerHealthGauge(int val, int max) {
	player_health->set(val, max);
}


void FormMain::shiftPlayerHealthGauge(int val) {
	if (val >= 0) player_health->add(val);
	else player_health->subtract(val);
}


void FormMain::setPlayerManaGauge(int val) {
	player_mana->set(val);
}


void FormMain::setPlayerManaGauge(int val, int max) {
	player_mana->set(val, max);
}


void FormMain::shiftPlayerManaGauge(int val) {
	if (val >= 0) player_mana->add(val);
	else player_mana->subtract(val);
}


void FormMain::setPlayerExpGauge(int val) {
	player_exp->set(val);
}


void FormMain::setPlayerExpGauge(int val, int max) {
	player_exp->set(val, max);
}


void FormMain::shiftPlayerExpGauge(int val) {
	if (val >= 0) player_exp->add(val);
	else player_exp->subtract(val);
}


void FormMain::setPetHealthGauge(int val) {
	pet_health->set(val);
}


void FormMain::setPetHealthGauge(int val, int max) {
	pet_health->set(val, max);
}


void FormMain::shiftPetHealthGauge(int val) {
	if (val >= 0) pet_health->add(val);
	else pet_health->subtract(val);
}


void FormMain::setPetExpGauge(int val) {
	pet_exp->set(val);
}


void FormMain::setPetExpGauge(int val, int max) {
	pet_exp->set(val, max);
}


void FormMain::shiftPetExpGauge(int val) {
	if (val >= 0) pet_exp->add(val);
	else pet_exp->subtract(val);
}*/


void FormMain::setCash(int cash) {
	//Label* lblCash = getLabel("cash_value");
	//if (lblCash) lblCash->setText(std::to_string(cash));
}


void FormMain::handleLoginResponse(pLoginResponse* packet) {
	//formLogin->handleLoginResponse(packet);
}
