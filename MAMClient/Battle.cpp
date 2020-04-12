#include "stdafx.h"
#include "Battle.h"
#include "BattleScene.h"
#include "BattleArray.h"
#include "BattleAI.h"

#include "CustomEvents.h"
#include "Define.h"
#include "Options.h"

#include "GUI.h"
#include "Texture.h"
#include "AssetManager.h"
#include "MainWindow.h"

#include "UserManager.h"
#include "Player.h"
#include "Pet.h"
#include "Monster.h"
#include "Inventory.h"
#include "Item.h"

#include "pBattleAction.h"
#include "pEnemyInfo.h"
#include "pFighterInfo.h"
#include "pNormalAct.h"
#include "pItemAct.h"
#include "pBattleRound.h"
#include "pBattleResult.h"
#include "pColor.h"

bool LessThanByY::operator()(Entity* lhs, Entity* rhs) {
	return (lhs->GetBattlePos().y > rhs->GetBattlePos().y);
}

Battle::Battle(SDL_Renderer *r, int mapDoc, int actorCount) {
	renderer = r;
	doc = mapDoc;
	this->actorCount = actorCount;
	mode = bmInit;
	autoBattle = options.GetAutoBattle();
	battleAI.reset(new BattleAI);

#ifdef NEWGUI
	battleRect = { 0, 0, 800, 600 };

	std::string num = std::to_string(rand() % 48);
	while (num.length() < 3) num.insert(num.begin(), '0');
	std::string backdropPath = "data/fightBG/backdrop/backdrop" + num + ".jpg";

	num = std::to_string(rand() % 18);
	while (num.length() < 3) num.insert(num.begin(), '0');
	std::string scenePath = "data/fightBG/scene/scene" + num + ".tga";
#else
	battleRect = {0, 0, 740, 410};

	INI fightBg("INI\\FIGHTBGDATA.ini", "FIGHTBG" + std::to_string(doc));
	std::string scenePath = fightBg.getEntry("FIGHTBG0");
	std::string backdropPath;
#endif
	renderRect = { 0, 0, battleRect.w, battleRect.h };
	tScene = new Texture(renderer, scenePath);
	if (backdropPath.length() > 0) tBackdrop = new Texture(renderer, backdropPath);

	//Load button textures
	std::string btnPlayerPath = "fight\\menu\\";
	std::string btnPetPath = "fight\\petbutton\\";
	//int y = battleRect.h - 50 - 48 - 6;
	int y = 538;
	for (int i = 0; i <= 9; i++) {
		std::string btnName;
		switch (i) {
		case 0: btnName = "btnPlayerAttack"; break;
		case 1: btnName = "btnPlayerSkill"; break;
		case 2: btnName = "btnPlayerCapture"; break;
		case 3: btnName = "btnPlayerItem"; break;
		case 4: btnName = "btnPlayerRun"; break;
		case 5: btnName = "btnPlayerSwitch"; break;
		case 6: btnName = "btnPlayerDefend"; break;
		case 7: btnName = "btnPetAttack"; break;
		case 8: btnName = "btnPetSkill"; break;
		case 9: btnName = "btnPetDefend"; break;
		}

		battleButtons[i] = new CButton(mainForm, btnName, 0, y);
		battleButtons[i]->SetWidth(48);
		battleButtons[i]->SetHeight(48);
		std::string btnPath;
		if (i < 7) btnPath = btnPlayerPath;
		else btnPath = btnPetPath;
		battleButtons[i]->SetUnPressedImage(btnPath + std::to_string(i) + ".bmp");
		battleButtons[i]->SetPressedImage(btnPath + std::to_string(i) + "-1.bmp");
	}
	battleButtons[auto_battle] = new CButton(mainForm, "btnAutoBattle", 730, y);
	battleButtons[auto_battle]->SetWidth(48);
	battleButtons[auto_battle]->SetHeight(48);
	battleButtons[auto_battle]->SetType(ButtonType::btToggle);
	battleButtons[auto_battle]->SetUnPressedImage("fight/AutoOff.bmp");
	battleButtons[auto_battle]->SetPressedImage("fight/AutoOn.bmp");
	battleButtons[auto_battle]->Load();
	battleButtons[auto_battle]->Toggle(autoBattle);


	//int x = battleRect.w - 300 - 20;
	int x = 396;
	battleButtons[BattleMenu::player_attack]->SetX(x);
	battleButtons[BattleMenu::player_skill]->SetX(x + (48 * 1));
	battleButtons[BattleMenu::player_capture]->SetX(x + (48 * 2));
	battleButtons[BattleMenu::player_defend]->SetX(x + (48 * 3));
	battleButtons[BattleMenu::player_item]->SetX(x + (48 * 4));
	battleButtons[BattleMenu::player_run]->SetX(x + (48 * 5));

	battleButtons[BattleMenu::pet_attack]->SetX(x);
	battleButtons[BattleMenu::pet_skill]->SetX(x + (48 * 1));
	battleButtons[BattleMenu::pet_defend]->SetX(x + (48 * 2));

	battleButtons[BattleMenu::player_attack]->RegisterEvent("Click", std::bind(&Battle::btnPlayerAttack_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::player_defend]->RegisterEvent("Click", std::bind(&Battle::btnPlayerDefend_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::player_capture]->RegisterEvent("Click", std::bind(&Battle::btnPlayerCapture_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::player_item]->RegisterEvent("Click", std::bind(&Battle::btnPlayerItem_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::player_run]->RegisterEvent("Click", std::bind(&Battle::btnPlayerRun_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::pet_attack]->RegisterEvent("Click", std::bind(&Battle::btnPetAttack_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::pet_defend]->RegisterEvent("Click", std::bind(&Battle::btnPetDefend_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::auto_battle]->RegisterEvent("Click", std::bind(&Battle::btnAutoBattle_Click, this, std::placeholders::_1));

	//Load number textures
	SDL_Color colorKey;
	colorKey.r = 255; colorKey.g = 0; colorKey.b = 255;
	std::string numPath = "data\\effect\\num\\";
	for (int i = 0; i < 10; i++) {
		Asset numAsset(new Texture(renderer, numPath + std::to_string(i) + ".bmp", colorKey));
		SDL_SetTextureAlphaMod(numAsset->texture, 150);
		numbers.push_back(numAsset);
	}
	int midX = battleRect.w / 2;
	int midY = battleRect.h / 2;
	int numW = numbers[0]->width;
	int numH = numbers[0]->height;
	tensRect = { midX - numW, midY - (numH / 2), numW, numH };
	onesRect = { midX, midY - (numH / 2), numW, numH };

	//Rework asset manager cache for battle scene optimization?
	//loadEffect(EFFECT_FAINT);
	//loadEffect(EFFECT_READY);
	makeChatBubbleTexture();

	SDL_Event e;
	SDL_zero(e);
	e.type = CUSTOMEVENT_BATTLE;
	e.user.code = BATTLE_START;
	e.user.data1 = this;
	SDL_PushEvent(&e);
}


Battle::~Battle() {
	delete allyArray;
	delete enemyArray;

	delete tScene;
	numbers.clear(); //Not required, but explicitly freeing numbers

	for (auto ally : allies) ally->CleanupBattle();
	for (auto enemy : enemies) enemy->CleanupBattle();

	for (auto ally : allies) {
		if (ally->GetID() != player->GetID()
			&& ally->GetID() != player->getActivePet()->GetID()
			&& !userManager.getUserById(ally->GetID())) delete ally;
	}
	for (auto enemy : enemies) {
		if (!userManager.getUserById(enemy->GetID())) delete enemy;
	}

	std::map<int, CButton*>::iterator it;
	for (it = battleButtons.begin(); it != battleButtons.end(); it++)
	{
		delete it->second;
	}
	delete crow;
	SDL_DestroyTexture(chatBubble);
}


void Battle::setAllyFormation(int formation) {
	//int allyFormation = formation % 0xFFFF;
	loadBattleArray(&allyArray, formation, true);
}


void Battle::setEnemyFormation(int formation) {
	//int enemyFormation = formation / 0xFFFF;
	loadBattleArray(&enemyArray, formation, false);
}


void Battle::render() {
	if (tBackdrop) {
		SDL_Rect srcRect = {200, 100, 800, 600};
		SDL_RenderCopy(renderer, tBackdrop->texture, &srcRect, NULL);
	}
	SDL_RenderCopy(renderer, tScene->texture, NULL, &renderRect);

	if (allyArray) allyArray->Render();
	if (enemyArray) enemyArray->Render();

	//Render timer
	if (!autoBattle && (mode == bmTurnPlayer || mode == bmTurnPet)) {
		if (countDown > 0) {
			int tensPos = countDown / 10;
			int onesPos = countDown % 10;

			SDL_RenderCopy(renderer, numbers.at(tensPos)->texture, NULL, &tensRect);
			SDL_RenderCopy(renderer, numbers.at(onesPos)->texture, NULL, &onesRect);
		}
	}

	// Add all allies and monsters to sorted queue for drawing order
	for (int i = 0; i < allies.size(); i++) 
		drawActors.push(allies.at(i));
	for (int i = 0; i < enemies.size(); i++) 
		drawActors.push(enemies.at(i));
	if (captureTarget) drawActors.push(captureTarget);
	while (!drawActors.empty()) {
		Entity *actor = drawActors.top();
		actor->render_battle();
		drawActors.pop();
	}

	//Draws the mouse over box with fighter details
	if (focusedActor) {
		render_focusBox(focusedActor);
	}

	//Display battle dialogue
	for (int i = 0; i < allies.size(); i++) {
		if (allies[i]->GetBattleYell() != "") {
			SDL_Rect chatBubbleRect = { allies[i]->GetBattlePos().x - 118, allies[i]->GetBattlePos().y - 50, 118, 62 };
			SDL_RenderCopy(renderer, chatBubble, NULL, &chatBubbleRect);

			SDL_Point p = { chatBubbleRect.x + 10, chatBubbleRect.y + 5 };
			Asset aYell(stringToTexture(renderer, allies[i]->GetBattleYell(), gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0));
			aYell->setPosition(p);
			aYell->Render();
		}
	}
	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i]->GetBattleYell() != "") {
			SDL_Rect chatBubbleRect = { enemies[i]->GetBattlePos().x - 118, enemies[i]->GetBattlePos().y - 50, 118, 62 };
			SDL_RenderCopy(renderer, chatBubble, NULL, &chatBubbleRect);

			SDL_Point p = { chatBubbleRect.x + 10, chatBubbleRect.y + 5 };
			Asset aYell(stringToTexture(renderer, enemies[i]->GetBattleYell(), gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0));
			aYell->setPosition(p);
			aYell->Render();
		}
	}
}

void Battle::render_ui() {
	//Draw Battle Buttons
	battleButtons[BattleMenu::auto_battle]->Render();

	if (mode == bmTurnPlayer) {
		if (!selectTarget && !selectItem) {
			battleButtons[BattleMenu::player_attack]->Render();
			battleButtons[BattleMenu::player_skill]->Render();
			battleButtons[BattleMenu::player_capture]->Render();
			battleButtons[BattleMenu::player_item]->Render();
			battleButtons[BattleMenu::player_defend]->Render();
			battleButtons[BattleMenu::player_run]->Render();
		}
		if (selectItem) {
			render_items();
		}
	}

	if (mode == bmTurnPet && !selectTarget) {
		battleButtons[BattleMenu::pet_attack]->Render();
		battleButtons[BattleMenu::pet_skill]->Render();
		battleButtons[BattleMenu::pet_defend]->Render();
	}
}


void Battle::render_focusBox(Entity* entity) {
	SDL_Rect rect = entity->getRenderRect(0, true);
	//each is 4 pixel width
	//255,255,128,128

	Sint16 vx1[] = { rect.x, rect.x + rect.w - 1 - 4, rect.x + rect.w - 1, rect.x + 4 };
	Sint16 vy1[] = { rect.y, rect.y, rect.y + 4, rect.y + 4};
	filledPolygonRGBA(renderer, vx1, vy1, 4, 255, 255, 128, 128);

	Sint16 vx2[] = { rect.x, rect.x + 4, rect.x + 4, rect.x };
	Sint16 vy2[] = { rect.y, rect.y + 4, rect.y + rect.h - 1, rect.y + rect.h - 1 - 4 };
	filledPolygonRGBA(renderer, vx2, vy2, 4, 255, 255, 128, 128);

	Sint16 vx3[] = { rect.x + rect.w - 1 - 4, rect.x + rect.w - 1, rect.x + rect.w - 1, rect.x + rect.w - 1 - 4 };
	Sint16 vy3[] = { rect.y, rect.y + 4, rect.y + rect.h - 1, rect.y + rect.h - 1 - 4 };
	filledPolygonRGBA(renderer, vx3, vy3, 4, 255, 255, 128, 128);

	Sint16 vx4[] = { rect.x, rect.x + rect.w - 1 - 4, rect.x + rect.w - 1, rect.x + 4 };
	Sint16 vy4[] = { rect.y + rect.h - 1 - 4, rect.y + rect.h - 1 - 4, rect.y + rect.h - 1, rect.y + rect.h - 1 };
	filledPolygonRGBA(renderer, vx4, vy4, 4, 255, 255, 128, 128);

	SDL_Point p;
	int heightOffset;
	if (entity->GetBattleType() != OBJTYPE_MONSTER && entity->GetBattleType() != OBJTYPE_VSPLAYER && entity->GetBattleType() != OBJTYPE_VSPET) {
		//render ally specific details: life, mana, life/mana bars

		int barW = 51;
		int barH = 5;
		SDL_Color yellow = { 255,255,0,255 };
		SDL_Color red = { 255,0,0,255 };

		int barX = entity->GetBattlePos().x - (barW / 2);
		int barY = entity->GetBattlePos().y - 80;

		//Life Bar
		lineRGBA(renderer, barX, barY, barX + barW - 1, barY, 255, 255, 0, 255);
		lineRGBA(renderer, barX, barY + barH - 1, barX + barW - 2, barY + barH - 1, 255, 255, 0, 255);
		lineRGBA(renderer, barX, barY, barX, barY + barH - 1, 255, 255, 0, 255);
		lineRGBA(renderer, barX + barW - 1, barY, barX + barW - 1, barY + barH - 2, 255, 255, 0, 255);
		if (entity->GetCurrentLife() > 0) {
			float fillW = ((float)entity->GetCurrentLife() / entity->GetMaxLife()) * barW;
			boxRGBA(renderer, barX + 1, barY + 1, barX + (int)fillW, barY + 3, 255, 0, 0, 255);
		}

		//Mana Bar
		if (entity->GetMaxMana() > 0) {
			barY += 7;
			lineRGBA(renderer, barX, barY, barX + barW - 1, barY, 0, 0, 255, 255);
			lineRGBA(renderer, barX, barY + barH - 1, barX + barW - 2, barY + barH - 1, 0, 0, 255, 255);
			lineRGBA(renderer, barX, barY, barX, barY + barH - 1, 0, 0, 255, 255);
			lineRGBA(renderer, barX + barW - 1, barY, barX + barW - 1, barY + barH - 2, 0, 0, 255, 255);

			if (entity->GetCurrentMana() > 0) {
				float fillW = ((float)entity->GetCurrentMana() / entity->GetMaxMana()) * barW;
				boxRGBA(renderer, barX + 1, barY + 1, barX + (int)fillW, barY + 3, 255, 255, 255, 255);
			}
		}

		std::string name = "Name: " + entity->GetName();
		p = { entity->GetBattlePos().x - 30, entity->GetBattlePos().y - 45 };
		Texture *tName = stringToTexture(renderer, name, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tName->setPosition(p);
		tName->Render();
		heightOffset = tName->rect.h;
		delete tName;

		std::string level = "Level: " + formatInt(entity->GetLevel());
		p = { entity->GetBattlePos().x - 30, entity->GetBattlePos().y - 45 + heightOffset };
		Texture *tLevel = stringToTexture(renderer, level, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tLevel->setPosition(p);
		tLevel->Render();
		heightOffset = tLevel->rect.h;
		delete tLevel;

		std::string life = "Life: " + formatInt(entity->GetCurrentLife()) + "/" + formatInt(entity->GetMaxLife());
		p = { entity->GetBattlePos().x - 30, p.y + heightOffset };
		Texture *tLife = stringToTexture(renderer, life, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tLife->setPosition(p);
		tLife->Render();
		heightOffset = tLife->rect.h;
		delete tLife;

		if (entity->GetMaxMana() > 0) {
			std::string mana = "Mana: " + formatInt(entity->GetCurrentMana()) + "/" + formatInt(entity->GetMaxMana());
			p = { entity->GetBattlePos().x - 30, p.y + heightOffset };
			Texture *tMana = stringToTexture(renderer, mana, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
			tMana->setPosition(p);
			tMana->Render();
			delete tMana;
		}
	}
	else {
		std::string name = "Name: " + entity->GetName();
		p = { entity->GetBattlePos().x - 40, entity->GetBattlePos().y - 45 };
		Asset aName(stringToTexture(renderer, name, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0));
		aName->setPosition(p);
		aName->Render();
		heightOffset = aName->rect.h;

		std::string level = "Level: " + formatInt(entity->GetLevel());
		p = { entity->GetBattlePos().x - 40, entity->GetBattlePos().y - 45 + heightOffset };
		Asset aLevel(stringToTexture(renderer, level, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0));
		aLevel->setPosition(p);
		aLevel->Render();
	}
}

void Battle::render_items() {
	int iW, iH;
	SDL_GetRendererOutputSize(renderer, &iW, &iH);

	int rows = ceil(itemList.size() / 4.0);
	int x = iW - itemBoxOffset.x;
	int y = iH - itemBoxOffset.y;
	for (int i = 0; i < rows; i++) {
		SDL_Point p;

		for (int j = 0; j < 4; j++) {
			SDL_Rect dest = { x + (j * itemBox->width), y - ((rows - i - 1) * itemBox->height), itemBox->width, itemBox->height };

			SDL_RenderCopy(renderer, itemBox->texture, NULL, &dest);
			int itemIndex = (i * 4) + j;
			if (itemIndex < itemList.size()) {
				dest.x += 8;
				dest.y += 6;
				dest.w = itemAssets[itemIndex]->width;
				dest.h = itemAssets[itemIndex]->height;
				SDL_RenderCopy(renderer, itemAssets[itemIndex]->texture, NULL, &dest);
			}
		}
	}

	if (focusedItem) {
		Asset aItem = CreateItemMouseover(focusedItem);
		if (aItem) {
			int mx, my;
			SDL_GetMouseState(&mx, &my);
			mx -= (gui->left->width + 20);
			my -= (gui->topCenter->height + 9);
			SDL_Rect destRect = aItem->rect;
			destRect.x = mx - battleRect.x - destRect.w;
			destRect.y = my - (destRect.h / 2);
			boxRGBA(renderer, destRect.x, destRect.y, destRect.x + destRect.w - 1, destRect.y + destRect.h - 1, 0, 0, 0, 96);
			SDL_RenderCopy(renderer, aItem->texture, NULL, &destRect);
		}
	}
}


bool Battle::handleEvent(SDL_Event& e) {
	int x, y;
	SDL_GetMouseState(&x, &y);

	int mx, my;
#ifdef NEWGUI
	mx = x;
	my = y;
#else
	mx = x - (gui->left->width + 20);
	my = y - (gui->topCenter->height + 9);
#endif

	SDL_Event e2 = e;
	e2.motion.x = mx;
	e2.motion.y = my;

	battleButtons[BattleMenu::auto_battle]->HandleEvent(e2);

	if (mode == bmTurnPlayer) {
		battleButtons[BattleMenu::player_attack]->HandleEvent(e2);
		battleButtons[BattleMenu::player_skill]->HandleEvent(e2);
		battleButtons[BattleMenu::player_capture]->HandleEvent(e2);
		battleButtons[BattleMenu::player_item]->HandleEvent(e2);
		battleButtons[BattleMenu::player_defend]->HandleEvent(e2);
		battleButtons[BattleMenu::player_run]->HandleEvent(e2);
	}

	if (mode == bmTurnPet) {
		battleButtons[BattleMenu::pet_attack]->HandleEvent(e2);
		battleButtons[BattleMenu::pet_skill]->HandleEvent(e2);
		battleButtons[BattleMenu::pet_defend]->HandleEvent(e2);
	}

	if (e.type == SDL_MOUSEMOTION) {
		for (auto entity : enemies) entity->handleEvent_battle(e2);
		for (auto entity : allies) entity->handleEvent_battle(e2);

		Entity *newFocus = nullptr;
		for (auto entity : enemies) {
			if (entity->IsMousedOver()) {
				newFocus = entity;
				break;
			}
		}
		if (!newFocus) {
			for (auto entity : allies) {
				if (entity->IsMousedOver()) {
					newFocus = entity;
					break;
				}
			}
		}
		if (focusedActor != newFocus) focusedActor = newFocus;

		//Items
		if (selectItem) {
			int iW, iH;
			SDL_GetRendererOutputSize(renderer, &iW, &iH);

			focusedItem = nullptr;
			int rows = ceil(itemList.size() / 4.0);
			int x = battleRect.x + battleRect.w - itemBoxOffset.x;
			int y = battleRect.y + battleRect.h - itemBoxOffset.y;
			for (int i = 0; i < rows; i++) {
				SDL_Point p;

				for (int j = 0; j < 4; j++) {
					SDL_Rect boxRect = { x + (j * itemBox->width), y - ((rows - i - 1) * itemBox->height), itemBox->width, itemBox->height };
					if (doesPointIntersect(boxRect, SDL_Point{ mx,my })) {
						int itemIndex = (i * 4) + j;
						if (itemList.size() > itemIndex) focusedItem = itemList[itemIndex];
						break;
					}
				}
				if (focusedItem) break;
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (e.button.button == SDL_BUTTON_RIGHT) {
			playerButton_pressed = false;
			petButton_pressed = false;
			selectTarget = false;
			selectItem = false;
		}

		if (e.button.button == SDL_BUTTON_LEFT) {
			if (selectItem) {
				if (focusedItem) {
					selectedItem = focusedItem;
					selectItem = false;
					selectTarget = true;
					focusedItem = nullptr;
				}
			}
			else if (selectTarget) {
				if (focusedActor && focusedActor->IsAlive() && (focusedActor->IsEnemy() || selectedItem)) {
					selectedTarget = focusedActor;
					selectTarget = false;
				}
			}
			else {
				if (focusedActor && focusedActor->IsAlive() && focusedActor->IsEnemy()) {
					if (mode == bmTurnPlayer && !playerButton_pressed) {
						selectedTarget = focusedActor;
						if (playerAction == -1) clickToAttack = true;
					}

					if (mode == bmTurnPet && !petButton_pressed) {
						selectedTarget = focusedActor;
						clickToAttack = true;
					}
				}
			}

			playerButton_pressed = false;
			petButton_pressed = false;
		}
	}

	return true;
}


void Battle::step() {
	if (mode == bmInit || mode == bmEnd) return;

	if (mode == bmRoundStart) {
		countDown_start = SDL_GetTicks();
		mode = bmTurnPlayer;
		playerAction = -1;
		battleAnimateReady = false;
		for (auto entity : allies) {
			std::vector<Effect>::iterator it2 = entity->removeEffect(EFFECT_READY);
		}
	}

	//update timer here, not in render. Timer may expire and change mode
	int timeElapsed = SDL_GetTicks() - countDown_start;
	timeElapsed /= 1000;
	countDown = 30 - timeElapsed;

	if (countDown <= 0 && (mode == bmTurnPlayer || mode == bmTurnPet)) {
		selectTarget = false;
		selectItem = false;
		
		pBattleAction* battleAct;
		if (mode == bmTurnPlayer) {
			playerAction = baNoAction;
			battleAct = new pBattleAction(playerAction, round, player->GetID(), 0, 0, player->AccountId);
			actions.push_back(battleAct);
			if (player->IsAlive()) player->addEffect(EFFECT_READY);
		}
		petAction = baNoAction;
		battleAct = new pBattleAction(petAction, round, pet->GetBattleId(), 0, 0, player->AccountId);
		actions.push_back(battleAct);
		if (pet->IsAlive()) pet->addEffect(EFFECT_READY);
		mode = bmWait;
	}

	if (mode == bmTurnPlayer) {
		//Button event callbacks set action flags

		if (autoBattle) {
			selectedTarget = battleAI->GetNextTarget(player, enemies);
			if (selectedTarget) playerAction = BattleAction::baAttack;
		}

		if (clickToAttack) {
			SDL_Event e;
			btnPlayerAttack_Click(e);
		}
		
		if (selectedTarget) {
			bool validTarget = false;
			if (selectedItem) validTarget = true;
			else {
				for (auto actor : enemies) {
					if (actor == selectedTarget) {
						validTarget = true;
						break;
					}
				}
			}

			if (validTarget) {
				int useId = (!selectedItem) ? 0 : selectedItem->GetID();
				//set item or skill ID when appropriate
				pBattleAction* battleAct = new pBattleAction(playerAction, round, player->GetID(), selectedTarget->GetBattleId(), useId, player->AccountId);
				actions.push_back(battleAct);
				mode = bmTurnPet;
				selectedTarget = nullptr;
				selectTarget = false;
			}
			playerAction = -1;
		}
		if (mode != bmTurnPlayer) if (player->IsAlive()) player->addEffect(EFFECT_READY);
	}

	if (mode == bmTurnPet) {
		//Button event callbacks set action flags

		if (autoBattle) {
			selectedTarget = battleAI->GetNextTarget(pet, enemies);
			if (selectedTarget) petAction = BattleAction::baAttack;
		}

		if (clickToAttack) {
			SDL_Event e;
			btnPetAttack_Click(e);
		}

		if (!pet || running || !pet->IsAlive()) {
			mode = bmWait;
			petAction = baNoAction;

			pBattleAction* battleAct = new pBattleAction(petAction, round, pet ? pet->GetBattleId() : 0, 0, 0, player->AccountId);
			actions.push_back(battleAct);
		}

		if (selectedTarget) {
			bool validTarget = false;
			for (auto actor : enemies) {
				if (actor == selectedTarget) {
					validTarget = true;
					break;
				}
			}

			if (validTarget) {
				pBattleAction* battleAct = new pBattleAction(petAction, round, pet->GetBattleId(), selectedTarget->GetBattleId(), 0, player->AccountId);
				actions.push_back(battleAct);

				mode = bmWait;
				selectedTarget = nullptr;
				selectTarget = false;
			}
			petAction = -1;
		}
		if (pet && mode != bmTurnPet) if (pet->IsAlive()) pet->addEffect(EFFECT_READY);
	}

	if (mode == bmWait) {
		if (actions.size() > 0) {
			gClient.addPacket(actions);
			actions.clear();
		}

		if (battleAnimateReady) {
			mode = bmAnimate;
			for (auto entity : allies) {
				std::vector<Effect>::iterator itr = entity->removeEffect(EFFECT_READY);
			}
		}
	}

	if (mode == bmVictory) {
		mode = bmAnimate;

		BattleScene *newScene = new BattleScene(allies, enemies);
		for (auto actor : allies) {
			if (actor->IsAlive()) {
				//No difference between these anymore, now that battle yell is removed
				if (actor->GetID() == player->getActivePet()->GetBattleId()) {
					newScene->addAction(actor, bsVictory, AnimType::Laugh, actor->GetTargetingPos());
				}
				else {
					newScene->addAction(actor, bsVictory, AnimType::Laugh, actor->GetTargetingPos());
				}
			}
		}
		scenes.push(newScene);
	}

	if (mode == bmDefeat) {
		mode = bmAnimate;
	}

	if (mode == bmAnimate) {
		if (scenes.size() == 0 && currentScene == nullptr) {
			mode = bmRoundEnd;
		}
		else {
			if (!currentScene) {
				currentScene = scenes.front();
				currentScene->start();
				scenes.pop();
			}
			currentScene->step();
			if (currentScene->isFinished()) {
				delete currentScene;
				currentScene = nullptr;
			}
		}
	}

	if (mode == bmRoundEnd) {
		running = false;

		if (captureTarget) {
			delete captureTarget;
			captureTarget = nullptr;
		}

		if (battleEnd) {
			if (battleEndTimer == 0) {
				if (victory) mode = bmVictory;
				else mode = bmDefeat;
				battleEndTimer = SDL_GetTicks();
			}
			int curEndSecs = (SDL_GetTicks() - battleEndTimer) / 1000;
			if (curEndSecs >= 2) {
				mode = bmEnd;
				SDL_Event e;
				SDL_zero(e);
				e.type = CUSTOMEVENT_BATTLE;
				e.user.code = BATTLE_END;
				e.user.data1 = this;
				SDL_PushEvent(&e);
			}
		}
		else {
			for (auto actor : allies) {
				actor->SetDefending(false);
			}
			for (auto actor : enemies) {
				actor->SetDefending(false);
			}

			mode = bmRoundStart;
			round++;
		}
	}
}

void Battle::btnPlayerAttack_Click(SDL_Event& e) {
	selectTarget = true;
	playerButton_pressed = true;
	playerAction = baAttack;
	clickToAttack = false;
}

void Battle::btnPlayerDefend_Click(SDL_Event& e) {
	mode = bmTurnPet;
	playerButton_pressed = true;
	playerAction = baDefend;

	pBattleAction* battleAct = new pBattleAction(playerAction, round, player->GetID(), 0, 0, player->AccountId);
	actions.push_back(battleAct);
	if (player->IsAlive()) player->addEffect(EFFECT_READY);
}

void Battle::btnPlayerCapture_Click(SDL_Event& e) {
	selectTarget = true;
	playerButton_pressed = true;
	playerAction = baCapture;
}

void Battle::btnPlayerItem_Click(SDL_Event& e) {
	selectTarget = false;
	selectItem = true;
	playerButton_pressed = true;
	playerAction = baUseItem;

	LoadItemBox();
	LoadItemList();
}

void Battle::btnPlayerRun_Click(SDL_Event& e) {
	mode = bmTurnPet;
	playerButton_pressed = true;
	running = true;
	playerAction = baPlayerRun;

	pBattleAction* battleAct = new pBattleAction(playerAction, round, player->GetID(), 0, 0, player->AccountId);
	actions.push_back(battleAct);
	if (player->IsAlive()) player->addEffect(EFFECT_READY);
	if (pet && pet->IsAlive()) pet->addEffect(EFFECT_READY);
}

void Battle::btnPetAttack_Click(SDL_Event& e) {
	selectTarget = true;
	petButton_pressed = true;
	petAction = baAttack;
	clickToAttack = false;
}

void Battle::btnPetDefend_Click(SDL_Event& e) {
	mode = bmWait;
	petButton_pressed = true;
	petAction = baDefend;

	pBattleAction* battleAct = new pBattleAction(petAction, round, pet->GetBattleId(), 0, 0, player->AccountId);
	actions.push_back(battleAct);
	if (pet && pet->IsAlive()) pet->addEffect(EFFECT_READY);
}

void Battle::btnAutoBattle_Click(SDL_Event& e) {
	autoBattle = !autoBattle;
	options.SetAutoBattle(autoBattle);
}

void Battle::handlePacket(Packet* packet) {
	switch (packet->getType()) {
	case ptEnemyInfo:
		handleEnemyInfoPacket((pEnemyInfo*)packet);
		break;
	case ptFighterInfo:
		handleFighterInfoPacket((pFighterInfo*)packet);
		break;
	case ptBattleAction:
		//these are for verification, no reason to implement atm
		break;
	case ptNormalAct:
		handleNormalActPacket((pNormalAct*)packet);
		break;
	case ptItemAct:
		handleItemActPacket((pItemAct*)packet);
		break;
	case ptBattleRound:
		handleRoundPacket((pBattleRound*)packet);
		break;
	case ptBattleResult:
		handleResultPacket((pBattleResult*)packet);
		break;
	}
}


void Battle::handleEnemyInfoPacket(pEnemyInfo* packet) {
	int enemyCount = packet->enemyCount;

	for (int i = 0; i < enemyCount; i++) {
		EnemyInfo nextEnemy = packet->enemy[i];
		addEnemy(nextEnemy.monsterId, std::string(nextEnemy.name), nextEnemy.look, nextEnemy.level);
	}
}


void Battle::handleFighterInfoPacket(pFighterInfo* packet) {
	addAlly(packet->userId, packet->name, packet->look, packet->level, packet->hp_cur, packet->hp_max, packet->mana_cur, packet->mana_max);
	addAlly(packet->petId, packet->petName, packet->petLook, packet->petLevel, packet->pet_hp_cur, packet->pet_hp_max, 0, 0);

	if ((allies.size() + enemies.size()) == actorCount) {
		mode = bmRoundStart;
	}
}


void Battle::handleNormalActPacket(pNormalAct* packet) {
	int action = packet->action;
	int currentGroup = packet->group;
	if (currentGroup != lastActionGroup && responseScene) {
		scenes.push(responseScene);
		responseScene = nullptr;
		lastActionGroup = currentGroup;
	}
	if (!responseScene) responseScene = new BattleScene(allies, enemies);

	switch (action) {
	case baAttack:
		createAttackResponse(packet, responseScene);
		break;
	case baSpecial:
		break;
	case baDefend:
		createDefendResponse(packet);
		break;
	case baNoAction:
		break;
	case baPlayerRun:
		break;
	case baCaptureFail:
	case baCaptureSuccess:
		createCaptureResponse(packet);
		break;
	case baUseItem:
		break;
	}
}


void Battle::handleItemActPacket(pItemAct* packet) {
	int currentGroup = packet->battleGroup;
	if (currentGroup != lastActionGroup && responseScene) {
		scenes.push(responseScene);
		responseScene = nullptr;
		lastActionGroup = currentGroup;
	}
	if (!responseScene) responseScene = new BattleScene(allies, enemies);

	if (packet->sourceId == player->GetID()) {
		if (selectedItem) player->removeItem(selectedItem);
		selectedItem = nullptr;
	}

	//add to scene
	Entity* source = getActorById(packet->sourceId);
	Entity* target = getActorById(packet->targetId);

	SDL_Event e;
	SDL_zero(e);
	for (auto ally : allies) {
		if (ally->GetBattleId() == packet->targetId) {
			if (ally->GetBattleId() == player->GetID()) {
				e.type = CUSTOMEVENT_PLAYER;
				e.user.code = PLAYER_LIFEMANA;
			}
			else if (ally->GetBattleId() == pet->GetBattleId()) {
				e.type = CUSTOMEVENT_PET;
				e.user.code = PET_LIFE;
			}

			switch (packet->itemMode) {
			case 88:
				ally->SetLife(ally->GetCurrentLife() - packet->arLife[0]);
				break;
			case 700:
				ally->SetLife(ally->GetCurrentLife() + packet->arLife[0]);
				ally->SetMana(ally->GetCurrentMana() + packet->arMana[0]);
				break;
			}
			break;
		}
	}

	int reaction;
	std::string top, bottom;
	int effect = EFFECT_HEAL;
	if (packet->arLife[0] > 0) {
		if (packet->itemMode == 88) {
			reaction = Wound;
			effect = EFFECT_POISON;
			top = "-" + std::to_string(packet->arLife[0]) + " Life";
		}
		if (packet->itemMode == 700) {
			reaction = Laugh;
			top += "+" + std::to_string(packet->arLife[0]) + " Life";
		}
	} 
	if (packet->arMana[0] > 0) {
		if (packet->itemMode == 700) reaction = Laugh;
		if (top.length() == 0) top = "+" + std::to_string(packet->arMana[0]) + " Mana";
		else bottom = "+" + std::to_string(packet->arMana[0]) + " Mana";
	}
	responseScene->setReactor(target, reaction);

	std::vector<std::string> labels;
	labels.push_back(top);
	if (bottom.length() > 0) labels.push_back(bottom);

	bsPerform *response = responseScene->addAction(source, bsUseItem, (AnimType)0, target->GetBattleBasePos(), labels);
	response->effect = effect;
	if (e.type != 0) response->boundEvent = e;
}


void Battle::createAttackResponse(pNormalAct* packet, BattleScene* scene) {
	Entity* source = getActorById(packet->sourceId);
	Entity* target = getActorById(packet->targetId);

	int reaction;
	if (target->IsDefending())reaction = (packet->targetState) ? AnimType::Defend : AnimType::Faint;
	else reaction = (packet->targetState) ? AnimType::Wound : AnimType::Faint;
	responseScene->setReactor(target, reaction);

	std::vector<std::string> labels;
	std::string damageLbl;
	if (packet->damage == 0) damageLbl = "Miss";
	else damageLbl = "-" + std::to_string(packet->damage);
	labels.push_back(damageLbl);

	SDL_Point target_position = target->GetTargetingPos();
	responseScene->addAction(source, bsMoveTo, AnimType::Walk, target_position);
	bsPerform *perf = responseScene->addAction(source, bsAction, (AnimType)(AnimType::Attack01 + (rand() % 3)), target_position, labels);
	if (perf && packet->damage != 0) {
		SDL_Event e;
		SDL_zero(e);
		if (packet->targetId == player->GetBattleId()) {
			e.type = CUSTOMEVENT_PLAYER;
			e.user.code = PLAYER_LIFE;
			e.user.data1 = new int(packet->damage * -1);
			perf->boundEvent = e;
		}
		if (player->getActivePet() && packet->targetId == player->getActivePet()->GetBattleId()) {
			e.type = CUSTOMEVENT_PET;
			e.user.code = PET_LIFE;
			e.user.data1 = new int(packet->damage * -1);
			perf->boundEvent = e;
		}
	}
	responseScene->addAction(source, bsMoveTo, AnimType::Walk, source->GetBattleBasePos());
	responseScene->addAction(source, bsStandby, AnimType::StandBy, source->GetBattleBasePos());
}


void Battle::createDefendResponse(pNormalAct* packet) {
	Entity* source = getActorById(packet->sourceId);
	source->SetDefending(true);
	source->addFloatingLabel("Defense");
}


void Battle::createCaptureResponse(pNormalAct* packet) {
	Entity* source = getActorById(packet->sourceId);
	Entity* target = getActorById(packet->targetId);

	responseScene->setReactor(source, AnimType::StandBy);
	responseScene->addAction(target, bsCaptureBegin, (AnimType)0, target->GetBattleBasePos());
	responseScene->addAction(target, bsCaptureDrag, (AnimType)0, target->GetBattleBasePos());

	if (packet->action == baCaptureSuccess) {
		responseScene->addAction(target, bsCaptureSuccess, AnimType::Walk, source->GetBattleBasePos());

		//remove monster from list
		captureTarget = target;
		for (int i = 0; i < enemyCount; i++) {
			if (enemies[i] == target) {
				enemies.erase(enemies.begin() + i);
				break;
			}
		}
		enemyCount--;
	}

	if (packet->action == baCaptureFail) {
		responseScene->addAction(target, bsCaptureAngry, AnimType::StandBy, target->GetBattlePos());
		responseScene->addAction(target, bsCaptureFail, AnimType::Walk, target->GetBattleBasePos());
		responseScene->addAction(target, bsStandby, AnimType::StandBy, target->GetBattleBasePos());
	}
}


void Battle::handleRoundPacket(pBattleRound* packet) {
	//remove all pending Ready effects
	for (auto entity : allies) {
		std::vector<Effect>::iterator it2 = entity->removeEffect(EFFECT_READY);
	}

	//Make sure the latest scene is pushed to the scene queue
	if (responseScene) {
		scenes.push(responseScene);
		responseScene = nullptr;
	}
	lastActionGroup = 0;

	int koAlly = 0;
	for (int i = 0; i < allyCount; i++) {
		if (!packet->alive[i]) {
			allies[i]->SetAlive(false);
			koAlly++;
		}
	}
	if (koAlly == allyCount) {
		battleEnd = true;
		battleAnimateReady = true;
		return;
	}

	int koEnemy = 0;
	for (int i = 0; i < enemyCount; i++) {
		if (!packet->alive[allyCount + i]) {
			enemies[i]->SetAlive(false);
			koEnemy++;
		}
	}

	battleAnimateReady = true;
}


void Battle::handleResultPacket(pBattleResult* packet) {
	battleEnd = true;
	if (scenes.size() == 0 && currentScene == nullptr) {
		mode = bmRoundEnd;
	}

	if (packet->resultMode == brmVictory) victory = true;
	battleAnimateReady = true;
}


void Battle::handleColorPacket(pColor* packet) {
	Entity* entity = nullptr;
	for (auto actor : allies) {
		if (actor->GetBattleId() == packet->sourceId) {
			entity = actor;
			break;
		}
	}

	if (!entity) {
		for (auto actor : enemies) {
			if (actor->GetBattleId() == packet->sourceId) {
				entity = actor;
				//actor->setHslShifts(packet->count, packet->hslSets);
				break;
			}
		}
	}

	if (entity) {
		ColorShifts colorShifts;
		for (int i = 0; i < 25; i += 5) {
			ColorShift shift;
			memcpy(&shift, packet->hslSets + i, 5);
			colorShifts.push_back(shift);
		}
		if (colorShifts.size() > 0) entity->setColorShifts(colorShifts, true);
		entity->loadSprite(true);
	}
}


void Battle::addAlly(DWORD id, std::string name, int look, int level, int life_current, int life_max, int mana_current, int mana_max) {
	Entity *tempAlly = nullptr;
	if (id > _IDMSK_PET) {
		if (player->getActivePet() && id == player->getActivePet()->GetBattleId()) {
			tempAlly = player->getActivePet();
			if (tempAlly) pet = (Pet*)tempAlly;
			tempAlly->SetRenderer(renderer);
		}
		if (!tempAlly) tempAlly = new Pet(renderer, id, name, look);
		tempAlly->SetBattleType(OBJTYPE_FRIENDPET);
	}
	else {
		if (id == player->GetID()) tempAlly = player;
		if (!tempAlly) tempAlly = userManager.getUserById(id);
		if (!tempAlly) tempAlly = new User(renderer, id, name, look);
		tempAlly->SetBattleType(OBJTYPE_FRIENDPLAYER);
	}

	assert(tempAlly);

	tempAlly->SetLevel(level);
	tempAlly->SetLife(life_current);
	tempAlly->SetMaxLife(life_max);
	tempAlly->SetMana(mana_current);
	tempAlly->SetMaxMana(mana_max);

	tempAlly->setAnimation(StandBy, true);
	tempAlly->setDirection(default_ally_dir, true);
	//tempAlly->loadSprite(true);

	int arrayPos = allies.size() / 2;
	if (allies.size() % 2 != 0) {
		arrayPos = 5 + (allies.size() / 2);
	}

	SDL_Point posOffset = allyArray->GetPosition(arrayPos, true);
	//SDL_Point battlePos = { (renderRect.x + renderRect.w - posOffset.x - adjx), (renderRect.y + renderRect.h - posOffset.y - adjy) };
	SDL_Point battlePos = posOffset;
	posOffset = allyArray->GetTargetPosition(arrayPos, true);
	//SDL_Point targetingPos = { (renderRect.x + renderRect.w - posOffset.x - adjx), (renderRect.y + renderRect.h - posOffset.y - adjy) };
	SDL_Point targetingPos = posOffset;

	tempAlly->SetBattleBasePos(battlePos);
	tempAlly->SetBattlePos(battlePos);
	tempAlly->SetTargetingPos(targetingPos);

	allies.push_back(tempAlly);
	allyCount++;
	tempAlly->SetArrayPos(allyCount);
	tempAlly->SetAlive(true);
	if (allyCount == 1) allyArray->SetLeader(tempAlly);
}


void Battle::addEnemy(DWORD id, std::string name, int look, int level) {
	Entity *tempEnemy = nullptr;
	if (id > _IDMSK_MONSTER) {
		tempEnemy = new Monster(renderer, id, name, look);
	}
	else if (id > _IDMSK_PET) {
		tempEnemy = new Pet(renderer, id, name, look);
		tempEnemy->SetBattleType(OBJTYPE_VSPET);
	}
	else {
		tempEnemy = userManager.getUserById(id);
		if (!tempEnemy) tempEnemy = new User(renderer, id, name, look);
		tempEnemy->SetBattleType(OBJTYPE_VSPLAYER);
	}

	assert(tempEnemy);

	tempEnemy->SetLevel(level);

	tempEnemy->setAnimation(StandBy, true);
	tempEnemy->setDirection(default_monster_dir, true);
	//tempEnemy->loadSprite(true);
	
	SDL_Point posOffset = enemyArray->GetPosition(enemies.size(), false);
	SDL_Point battlePos = posOffset;
	//SDL_Point battlePos = { (renderRect.x + renderRect.w - posOffset.x), (renderRect.y + renderRect.h - posOffset.y) };
	posOffset = enemyArray->GetTargetPosition(enemies.size(), false);
	SDL_Point targetingPos = posOffset;
	//SDL_Point targetingPos = { (renderRect.x + renderRect.w - posOffset.x), (renderRect.y + renderRect.h - posOffset.y) };

	tempEnemy->SetBattleBasePos(battlePos);
	tempEnemy->SetBattlePos(battlePos);
	tempEnemy->SetTargetingPos(targetingPos);

	enemies.push_back(tempEnemy);
	enemyCount++;
	if (enemyCount == 1) enemyArray->SetLeader(tempEnemy);
}


void Battle::start() {
	mode = bmTurnPlayer;
	countDown = 30;
	countDown_start = SDL_GetTicks();
}


bool Battle::isOver() {
	if (mode == bmEnd) return true;
	return false;
}


int Battle::getMode() {
	return mode;
}


Entity* Battle::getActorById(int actorId) {
	for (auto actor : allies) {
		if (actor->GetBattleId() == actorId) return actor;
	}

	for (auto actor : enemies) {
		if (actor->GetBattleId() == actorId) return actor;
	}

	return nullptr;
}


void Battle::loadBattleArray(BattleArray** bArray, int arrayId, bool bAlly) {
	if (*bArray) delete *bArray;
	*bArray = new BattleArray(renderer);

	std::string arrayPath = "ArrayData\\";
	INI arrayINI(arrayPath + "array.ini", "ARRAY" + std::to_string(arrayId));
	std::string arrayFilePath = arrayINI.getEntry("ARRAY0");
	if (arrayFilePath.at(0) == '.') arrayFilePath = arrayFilePath.substr(2, std::string::npos);

	bool success = (*bArray)->Load(arrayFilePath.c_str(), arrayId, bAlly);
	if (!success) return;
}


/*SDL_Point Battle::getBattlePosFromArray(BattleArray* bArray, int fighterNum, bool isSource) {
	if (!bArray) return { 0, 0 };
	BYTE* arrayData = bArray->arrayData;

	int *ally15xsets = (int*)(arrayData + 0xE4); // v4[5] - 0xE4
	int *v5 = (int*)(arrayData + 0xF8); // v5[5] - 0xF8
	int *v6 = (int*)(arrayData + 0x10C); // v6[5] - 0x10C
	int *v7 = (int*)(arrayData + 0x120); // v7[5] - 0x120
	int *v8 = (int*)(arrayData + 0x134); // v8[10] - 0x134

	SDL_Point sourcePos = { 0,0 };
	SDL_Point targetPos = { 0,0 };

	if (fighterNum >= 5)
	{
		if (fighterNum >= 10) //Allies
		{
			if (fighterNum >= 15)
			{
				if (fighterNum < 20)
				{
					sourcePos.x = 46 * (15 - v5[fighterNum]) + 46;
					sourcePos.y = 25 * (15 - v6[fighterNum]);
					targetPos.x = 46 * (15 - v5[fighterNum]);
					targetPos.y = 25 * (15 - v6[fighterNum]) + 25;
				}
			}
			else
			{
				sourcePos.x = 46 * (15 - ally15xsets[fighterNum]) + 46;
				sourcePos.y = 25 * (15 - v5[fighterNum]);
				targetPos.x = 46 * (15 - ally15xsets[fighterNum]);
				targetPos.y = 25 * (15 - v5[fighterNum]) + 25;
			}
		}
		else //enemies
		{
			sourcePos.x = 46 * v7[fighterNum];
			sourcePos.y = 25 * v8[fighterNum] + 25;
			targetPos.x = 46 * v7[fighterNum] + 46;
			targetPos.y = 25 * v8[fighterNum];
		}
	}
	else // enemies
	{
		sourcePos.x = 46 * v6[fighterNum];
		sourcePos.y = 25 * v7[fighterNum] + 25;
		targetPos.x = 46 * v6[fighterNum] + 46;
		targetPos.y = 25 * v7[fighterNum];
	}
	
	if (isSource) return sourcePos;
	return targetPos;
}*/


bool Battle::doesMouseIntersect(SDL_Rect aRect, int x, int y) {
	if (x >= aRect.x && x <= (aRect.x + aRect.w) && y >= aRect.y && y <= (aRect.y + aRect.h)) return true;
	return false;
}


/*bsType Battle::getSceneTypeFromAction(int action) {
	switch (action) {
	case player_attack:
	case pet_attack:
		return bsType::bsAction;
	}
}*/

void Battle::makeChatBubbleTexture() {
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	chatBubble = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 118, 62);
	SDL_SetRenderTarget(renderer, chatBubble);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(chatBubble, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	roundedBoxRGBA(renderer, 0, 0, 117, 46, 15, 0, 0, 0, 96);
	roundedRectangleRGBA(renderer, 0, 0, 117, 46, 15, 0, 0, 0, 255);

	filledTrigonRGBA(renderer, 67, 46, 90, 46, 117, 61, 0, 0, 0, 96);
	trigonRGBA(renderer, 67, 46, 90, 46, 117, 61, 0, 0, 0, 255);

	SDL_SetRenderTarget(renderer, priorTarget);
}

void Battle::LoadItemBox() {
	if (itemBox) return;
	itemBox.reset(new Texture(renderer, "data/effect/num/itembox.tga", true));
}

void Battle::LoadItemList() {
	itemList.clear();
	itemAssets.clear();

	int count = player->inventory->getItemCount();
	for (int i = 0; i < count; i++) {
		Item* pItem = player->inventory->getItemInSlot(i);
		if (pItem && (pItem->getSort() == 700 || pItem->getSort() == 600)) { //700:Medicine, 600:Poison
			itemList.push_back(pItem);
			Asset aItem(new Texture(renderer, pItem->getTexturePath(40), true));
			itemAssets.push_back(aItem);
		}
	}
}

Asset Battle::CreateItemMouseover(Item* item) {
	Asset asset;
	if (!item) return asset;

	std::string text = item->getName();
	switch (item->getSort()) {
	case 600:
		text += "\nReduce HP: " + formatInt(item->getLife());
		text += "\nPoison";
		break;
	case 700:
		text += "\nIncrease HP: " + formatInt(item->getLife());
		text += "\nIncrease MP: " + formatInt(item->getMana());
		text += "\nMedicine";
		break;
	}

	int levelReq = item->getLevel();
	if (player->GetLevel() < levelReq) {
		text += "\n[Unqualified]";
	}
	else {
		text += "\n[Suitable]";
	}

	asset.reset(stringToTexture(renderer, text, gui->font, 0, { 255,255,255,255 }, 110));
	return asset;
}