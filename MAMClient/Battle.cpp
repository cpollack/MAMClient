#include "stdafx.h"
#include "Battle.h"
#include "BattleScene.h"

#include "GUI.h"
#include "MainWindow.h"
#include "Player.h"
#include "Pet.h"
#include "Texture.h"

#include "pBattleAction.h"
#include "pEnemyInfo.h"
#include "pFighterInfo.h"
#include "pBattleResponse.h"
#include "pBattleRound.h"
#include "pBattleResult.h"
#include "pColor.h"

Battle::Battle(SDL_Renderer *r, int mapDoc, int actorCount) {
	renderer = r;
	doc = mapDoc;
	this->actorCount = actorCount;
	mode = bmInit;

	INI fightBg("INI\\FIGHTBGDATA.ini", "FIGHTBG" + std::to_string(doc));
	std::string bgPath = fightBg.getEntry("FIGHTBG0");

	renderRect = { 0, 0, battleRect.w, battleRect.h };
	bgTexture = new Texture(renderer, bgPath);

	//Load button textures
	std::string btnPlayerPath = "fight\\menu\\";
	std::string btnPetPath = "fight\\petbutton\\";
	int y = battleRect.h - 48 - 6;
	for (int i = 0; i <= 9; i++) {
		//Texture *offTexture = new Texture(renderer, btnPath + std::to_string(i) + ".bmp");
		//offTexture->setAnchor(Anchor::BOTTOM_LEFT);
		//Texture *onTexture = new Texture(renderer, btnPath + std::to_string(i) + "-1.bmp");
		//onTexture->setAnchor(Anchor::BOTTOM_LEFT);

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
		//battleButtons[i] = new CButton(offTexture->texture, onTexture->texture, 0, y);
		//battleButtons[i]->buttonType = btPress;
	}

	int x = battleRect.w - 300 - 20;
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
	battleButtons[BattleMenu::player_run]->RegisterEvent("Click", std::bind(&Battle::btnPlayerRun_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::pet_attack]->RegisterEvent("Click", std::bind(&Battle::btnPetAttack_Click, this, std::placeholders::_1));
	battleButtons[BattleMenu::pet_defend]->RegisterEvent("Click", std::bind(&Battle::btnPetDefend_Click, this, std::placeholders::_1));

	//Load number textures
	SDL_Color colorKey;
	colorKey.r = 255; colorKey.g = 0; colorKey.b = 255;
	std::string numPath = "data\\effect\\num\\";
	for (int i = 0; i < 10; i++) {
		Texture *tempNum = new Texture(renderer, numPath + std::to_string(i) + ".bmp", colorKey);
		SDL_SetTextureAlphaMod(tempNum->texture, 150);
		numbers.push_back(tempNum);
	}
	int midX = battleRect.w / 2;
	int midY = battleRect.h / 2;
	int numW = numbers[0]->width;
	int numH = numbers[0]->height;
	tensRect = { midX - numW, midY - (numH / 2), numW, numH };
	onesRect = { midX, midY - (numH / 2), numW, numH };

	loadEffect(beFaint);
	loadEffect(beReady);
	makeChatBubbleTexture();
}


Battle::~Battle() {
	//what is deleted versus what is kept in cache?
	delete allyArray;
	delete enemyArray;

	delete bgTexture;
	for (auto num : numbers) {
		delete num;
	}

	for (auto actor : ally) delete actor;
	for (auto actor : monster) delete actor;

	std::map<int, CButton*>::iterator it;
	for (it = battleButtons.begin(); it != battleButtons.end(); it++)
	{
		delete it->second;
	}
	delete crow;
	SDL_DestroyTexture(chatBubble);
}


void Battle::setAllyFormation(int formation) {
	loadBattleArray(&allyArray, formation, 0);
}


void Battle::setEnemyFormation(int formation) {
	int enemyFormation = formation / 0xFFFF - allyArray->type;
	loadBattleArray(&enemyArray, enemyFormation, 1);
}


void Battle::render() {
	SDL_RenderCopy(renderer, bgTexture->texture, NULL, &renderRect);

	if (allyArray && allyArray->visible) {
		render_battleArray(allyArray);
	}
	if (enemyArray && enemyArray->visible) {
		render_battleArray(enemyArray);
	}

	//Render timer
	if (mode == bmTurnPlayer || mode == bmTurnPet) {
		if (countDown > 0) {
			int tensPos = countDown / 10;
			int onesPos = countDown % 10;

			SDL_RenderCopy(renderer, numbers.at(tensPos)->texture, NULL, &tensRect);
			SDL_RenderCopy(renderer, numbers.at(onesPos)->texture, NULL, &onesRect);
		}
	}

	// Add all allies and monsters to sorted queue for drawing order
	for (int i = 0; i < ally.size(); i++) 
		drawActors.push(ally.at(i));
	for (int i = 0; i < monster.size(); i++) 
		drawActors.push(monster.at(i));
	if (captureTarget) drawActors.push(captureTarget);
	while (!drawActors.empty()) {
		bMonster *curMon = drawActors.top();
		curMon->render();
		drawActors.pop();
	}

	//Draws the mouse over box with fighter details
	if (focusedActor) {
		render_focusBox(focusedActor);
	}

	//Draw Battle Buttons
	if (mode == bmTurnPlayer && !selectTarget) {
		battleButtons[BattleMenu::player_attack]->Render();
		battleButtons[BattleMenu::player_skill]->Render();
		battleButtons[BattleMenu::player_capture]->Render();
		battleButtons[BattleMenu::player_item]->Render();
		battleButtons[BattleMenu::player_defend]->Render();
		battleButtons[BattleMenu::player_run]->Render();
	}

	if (mode == bmTurnPet && !selectTarget) {
		battleButtons[BattleMenu::pet_attack]->Render();
		battleButtons[BattleMenu::pet_skill]->Render();
		battleButtons[BattleMenu::pet_defend]->Render();
	}

	//Display battle dialogue
	for (int i = 0; i < ally.size(); i++) {
		if (ally[i]->battleYell != "") {
			SDL_Rect chatBubbleRect = { ally[i]->pos.x - 118, ally[i]->pos.y - 50, 118, 62 };
			SDL_RenderCopy(renderer, chatBubble, NULL, &chatBubbleRect);
			Label lblYell(ally[i]->battleYell, chatBubbleRect.x + 10, chatBubbleRect.y + 5);
			lblYell.render();
		}
	}
	for (int i = 0; i < monster.size(); i++) {
		if (monster[i]->battleYell != "") {
			SDL_Rect chatBubbleRect = { monster[i]->pos.x - 118, monster[i]->pos.y - 50, 118, 62 };
			SDL_RenderCopy(renderer, chatBubble, NULL, &chatBubbleRect);
			Label lblYell(monster[i]->battleYell, chatBubbleRect.x + 10, chatBubbleRect.y + 5);
			lblYell.render();
		}
	}


	//Draw floating labels
	if (floatingLabels.size() > 0) {
		for (auto fLbl : floatingLabels) {
			if (!fLbl->started) {
				fLbl->started = true;
				fLbl->startTime = SDL_GetTicks();
			}
			else {
				SDL_Point p;
				p = fLbl->top->getPosition();
				p.x -= 1; p.y -= 1;
				fLbl->top->setPosition(p);
				if (fLbl->bottom) {
					p = fLbl->bottom->getPosition();
					p.x -= 1; p.y -= 1;
					fLbl->bottom->setPosition(p);
				}
			}
			fLbl->top->Render();
			if (fLbl->bottom) fLbl->bottom->Render();
		}

		//Remove expired labels
		std::vector<FloatingLabel*>::iterator itr;
		for (itr = floatingLabels.begin(); itr != floatingLabels.end();)
		{
			FloatingLabel* fLbl = *itr;
			int timeElapsed = (SDL_GetTicks() - fLbl->startTime) / 1000;
			if (timeElapsed >= 2)
			{
				delete fLbl;
				itr = floatingLabels.erase(itr);
			}
			else ++itr;
		}
	}
}


void Battle::render_battleArray(BattleArray* battleArray) {
	if (battleArray->type == 0) return;

	SDL_RenderCopy(renderer, battleArray->texture->texture, NULL, &battleArray->texture->rect);

	std::string topText = "Battle Formation: " + battleArray->name + ", Pivot: " + battleArray->pivot + ", Condition: " + battleArray->condition;
	Label topLabel(topText, 0, 0);
	topLabel.setFontColor(SDL_Color{ 255,255,255,255 });

	std::string bottomText = "Attack: " + std::to_string(battleArray->attack) + "%, Defense: " + std::to_string(battleArray->defense) + "%, Dexterity: " + std::to_string(battleArray->dex) + "%";
	Label bottomLabel(bottomText, 0, 0);
	bottomLabel.setFontColor(SDL_Color{ 255,255,255,255 });

	int toX, toY;
	if (battleArray->top) {
		toX = renderRect.x + renderRect.w - 400;
		toY = renderRect.y + 5;
	}
	else {
		toX = renderRect.x + 5;
		toY = renderRect.y + renderRect.h - topLabel.fontRect.h - bottomLabel.fontRect.h - 5;
	}

	//topLabel.x = toX;
	//topLabel.y = toY;
	topLabel.setPosition(toX, toY);
	topLabel.render();

	//bottomLabel.x = toX;
	//bottomLabel.y = toY + topLabel.fontRect.h + 2;
	bottomLabel.setPosition(toX, toY + topLabel.fontRect.h + 2);
	bottomLabel.render();
}


void Battle::render_focusBox(bMonster* monster) {
	SDL_Rect rect = *monster->focusRect;
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
	if (monster->type != bActorType::ENEMY) {
		bAlly* ally = (bAlly*)monster;
		//render ally specific details: life, mana, life/mana bars

		int barW = 51;
		int barH = 5;
		SDL_Color yellow = { 255,255,0,255 };
		SDL_Color red = { 255,0,0,255 };

		int barX = monster->pos.x - (barW / 2);
		int barY = monster->pos.y - 80;

		//Life Bar
		lineRGBA(renderer, barX, barY, barX + barW - 1, barY, 255, 255, 0, 255);
		lineRGBA(renderer, barX, barY + barH - 1, barX + barW - 2, barY + barH - 1, 255, 255, 0, 255);
		lineRGBA(renderer, barX, barY, barX, barY + barH - 1, 255, 255, 0, 255);
		lineRGBA(renderer, barX + barW - 1, barY, barX + barW - 1, barY + barH - 2, 255, 255, 0, 255);
		if (ally->life_current > 0) {
			float fillW = ((float)ally->life_current / ally->life_max) * barW;
			boxRGBA(renderer, barX + 1, barY + 1, barX + (int)fillW, barY + 3, 255, 0, 0, 255);
		}

		//Mana Bar
		if (ally->mana_max > 0) {
			barY += 7;
			lineRGBA(renderer, barX, barY, barX + barW - 1, barY, 0, 0, 255, 255);
			lineRGBA(renderer, barX, barY + barH - 1, barX + barW - 2, barY + barH - 1, 0, 0, 255, 255);
			lineRGBA(renderer, barX, barY, barX, barY + barH - 1, 0, 0, 255, 255);
			lineRGBA(renderer, barX + barW - 1, barY, barX + barW - 1, barY + barH - 2, 0, 0, 255, 255);

			if (ally->mana_current > 0) {
				float fillW = ((float)ally->mana_current / ally->mana_max) * barW;
				boxRGBA(renderer, barX + 1, barY + 1, barX + (int)fillW, barY + 3, 255, 255, 255, 255);
			}
		}

		std::string name = "Name: " + monster->name;
		p = { monster->pos.x - 30, monster->pos.y - 45 };
		Texture *tName = stringToTexture(renderer, name, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tName->setPosition(p);
		tName->Render();
		heightOffset = tName->rect.h;
		delete tName;

		std::string level = "Level: " + std::to_string(monster->level);
		p = { monster->pos.x - 30, monster->pos.y - 45 + heightOffset };
		Texture *tLevel = stringToTexture(renderer, level, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tLevel->setPosition(p);
		tLevel->Render();
		heightOffset = tLevel->rect.h;
		delete tLevel;

		std::string life = "Life: " + std::to_string(ally->life_current) + "/" + std::to_string(ally->life_max);
		p = { monster->pos.x - 30, p.y + heightOffset };
		Texture *tLife = stringToTexture(renderer, life, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tLife->setPosition(p);
		tLife->Render();
		heightOffset = tLife->rect.h;
		delete tLife;

		if (ally->mana_max > 0) {
			std::string mana = "Mana: " + std::to_string(ally->mana_current) + "/" + std::to_string(ally->mana_max);
			p = { monster->pos.x - 30, p.y + heightOffset };
			Texture *tMana = stringToTexture(renderer, mana, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
			tMana->setPosition(p);
			tMana->Render();
			delete tMana;
		}
	}
	else {
		std::string name = "Name: " + monster->name;
		p = { monster->pos.x - 40, monster->pos.y - 45 };
		Texture *tName = stringToTexture(renderer, name, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tName->setPosition(p);
		tName->Render();
		heightOffset = tName->rect.h;
		delete tName;

		std::string level = "Level: " + std::to_string(monster->level);
		p = { monster->pos.x - 40, monster->pos.y - 45 + heightOffset };
		Texture *tLevel = stringToTexture(renderer, level, gui->font_battleResult, 0, SDL_Color{ 255,255,255,255 }, 0);
		tLevel->setPosition(p);
		tLevel->Render();
		delete tLevel;
	}
}


bool Battle::handleEvent(SDL_Event& e) {
	int x, y;
	SDL_GetMouseState(&x, &y);

	int mx, my;
	mx = x - (gui->left->width + 20);
	my = y - (gui->topCenter->height + 9);
	if (mode == bmTurnPlayer) {
		battleButtons[BattleMenu::player_attack]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::player_skill]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::player_capture]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::player_item]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::player_defend]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::player_run]->HandleEvent(e, mx, my);
	}

	if (mode == bmTurnPet) {
		battleButtons[BattleMenu::pet_attack]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::pet_skill]->HandleEvent(e, mx, my);
		battleButtons[BattleMenu::pet_defend]->HandleEvent(e, mx, my);
	}

	if (e.type == SDL_MOUSEMOTION) {
		bMonster *newFocus = nullptr;
		//focusedActor = nullptr;
		for (auto actor : monster) {
			Sprite* curSprite = actor->animation[animationTypeToString(actor->current_animation) + std::to_string(actor->direction)];
			SDL_Rect renderRect = curSprite->getRenderRect();
			if (doesMouseIntersect(renderRect, mx, my)) {
				//Only focus a monster when its 'solid' pixels are moused over
				SDL_Point getPixel = { mx - renderRect.x, my - renderRect.y };
				
				Asset currentTexture = curSprite->getCurrentTexture();
				Uint32 pixel = currentTexture->getPixel(getPixel);
				Uint8 alpha = currentTexture->getPixelAlpha(pixel);

				//std::cout << "Alpha: " << std::to_string(alpha) << "\n";
				
				if (alpha >= 64) {
					newFocus = actor;
					//std::cout << "Mousing over: " << actor->name << "\n";
					break;
				}
			}
		}

		if (!newFocus) {
			for (auto actor : ally) {
				Sprite* curSprite = actor->animation[animationTypeToString(actor->current_animation) + std::to_string(actor->direction)];
				SDL_Rect renderRect = curSprite->getRenderRect();
				if (doesMouseIntersect(renderRect, mx, my)) {
					SDL_Point getPixel = { mx - renderRect.x, my - renderRect.y };

					Asset currentTexture = curSprite->getCurrentTexture();
					Uint32 pixel = currentTexture->getPixel(getPixel);
					Uint8 alpha = currentTexture->getPixelAlpha(pixel);
					
					if (alpha >= 64) {
						newFocus = actor;
						//std::cout << "Mousing over: " << actor->name << "\n";
						break;
					}
				}
			}
		}
		if (focusedActor != newFocus) {
			//trigger update of drawable data for focus. Ie: rectangle, name, lvl etc
			//if (newFocus) 

			focusedActor = newFocus;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		if (e.button.button == SDL_BUTTON_RIGHT) {
			playerButton_pressed = false;
			petButton_pressed = false;
			selectTarget = false;
		}

		if (e.button.button == SDL_BUTTON_LEFT) {
			if (focusedActor && focusedActor->alive) {
				if (selectTarget) {
					selectedTarget = focusedActor;
					selectTarget = false;
				}

				if (mode == bmTurnPlayer && !playerButton_pressed) {
					selectedTarget = focusedActor;
					if (playerAction == -1) clickToAttack = true;
				}

				if (mode == bmTurnPet && !petButton_pressed) {
					selectedTarget = focusedActor;
					clickToAttack = true;
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
	}

	//update timer here, not in render. Timer may expire and change mode
	int timeElapsed = SDL_GetTicks() - countDown_start;
	timeElapsed /= 1000;
	countDown = 30 - timeElapsed;

	if (countDown <= 0 && (mode == bmTurnPlayer || mode == bmTurnPet)) {
		selectTarget = false;
		
		pBattleAction* battleAct;
		if (mode == bmTurnPlayer) {
			playerAction = baNoAction;
			battleAct = new pBattleAction(playerAction, round, ally[0]->id, 0, 0, player->AccountId);
			actions.push_back(battleAct);
			if (ally[0]->alive) ally[0]->setEffect(beReady);
		}
		petAction = baNoAction;
		battleAct = new pBattleAction(petAction, round, ally[1]->id, 0, 0, player->AccountId);
		actions.push_back(battleAct);
		if (ally[1]->alive) ally[1]->setEffect(beReady);
		mode = bmWait;
	}

	if (mode == bmTurnPlayer) {
		//Button event callbacks set action flags

		if (clickToAttack) {
			SDL_Event e;
			btnPlayerAttack_Click(e);
		}
		
		if (selectedTarget) {
			bool validTarget = false;
			for (auto actor : monster) {
				if (actor == selectedTarget) {
					validTarget = true;
					break;
				}
			}

			if (validTarget) {
				int useId = 0;
				//set item or skill ID when appropriate
				pBattleAction* battleAct = new pBattleAction(playerAction, round, ally[0]->id, selectedTarget->id, useId, player->AccountId);
				actions.push_back(battleAct);
				mode = bmTurnPet;
				selectedTarget = nullptr;
				selectTarget = false;
			}
			playerAction = -1;
		}
		if (mode != bmTurnPlayer) if (ally[0]->alive) ally[0]->setEffect(beReady);
	}

	if (mode == bmTurnPet) {
		//Button event callbacks set action flags

		if (clickToAttack) {
			SDL_Event e;
			btnPetAttack_Click(e);
		}

		if (running) {
			mode = bmWait;
			petAction = baNoAction;

			pBattleAction* battleAct = new pBattleAction(petAction, round, ally[1]->id, 0, 0, player->AccountId);
			actions.push_back(battleAct);
		}

		if (selectedTarget) {
			bool validTarget = false;
			for (auto actor : monster) {
				if (actor == selectedTarget) {
					validTarget = true;
					break;
				}
			}

			if (validTarget) {
				pBattleAction* battleAct = new pBattleAction(petAction, round, ally[1]->id, selectedTarget->id, 0, player->AccountId);
				actions.push_back(battleAct);

				mode = bmWait;
				selectedTarget = nullptr;
				selectTarget = false;
			}
			petAction = -1;
		}
		if (mode != bmTurnPet) if (ally[1]->alive) ally[1]->setEffect(beReady);
	}

	if (mode == bmWait) {
		if (actions.size() > 0) {
			gClient.addPacket(actions);
			actions.clear();
		}

		if (battleAnimateReady) {
			mode = bmAnimate;
			if (ally[0]->alive) ally[0]->setEffect(beNone);
			if (ally[1]->alive) ally[1]->setEffect(beNone);
		}
	}

	if (mode == bmVictory) {
		mode = bmAnimate;

		BattleScene *newScene = new BattleScene(ally, monster, &floatingLabels);
		for (auto actor : ally) {
			if (actor->alive) {
				if (actor->id == player->getActivePet()->getId()) {
					SDL_Point targetPoint = getBattlePosFromArray(allyArray, actor->battleId, false);
					newScene->addAction(actor, bsVictory, AnimType::Laugh, targetPoint, "Victory!");
				}
				else {
					SDL_Point targetPoint = getBattlePosFromArray(allyArray, actor->battleId, false);
					newScene->addAction(actor, bsVictory, AnimType::Laugh, targetPoint);
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
			}
		}
		else {
			for (auto actor : ally) {
				actor->defending = false;
			}
			for (auto actor : monster) {
				actor->defending = false;
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

	pBattleAction* battleAct = new pBattleAction(playerAction, round, ally[0]->id, 0, 0, player->AccountId);
	actions.push_back(battleAct);
}

void Battle::btnPlayerCapture_Click(SDL_Event& e) {
	selectTarget = true;
	playerButton_pressed = true;
	playerAction = baCapture;
}

void Battle::btnPlayerRun_Click(SDL_Event& e) {
	mode = bmTurnPet;
	playerButton_pressed = true;
	running = true;
	playerAction = baPlayerRun;

	pBattleAction* battleAct = new pBattleAction(playerAction, round, ally[0]->id, 0, 0, player->AccountId);
	actions.push_back(battleAct);
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

	pBattleAction* battleAct = new pBattleAction(petAction, round, ally[1]->id, 0, 0, player->AccountId);
	actions.push_back(battleAct);
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
	case ptBattleResponse:
		handleResponsePacket((pBattleResponse*)packet);
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
		addMonster(nextEnemy.monsterId, std::string(nextEnemy.name), nextEnemy.look, nextEnemy.level);
	}
}


void Battle::handleFighterInfoPacket(pFighterInfo* packet) {
	addAlly(packet->userId, packet->name, packet->look, packet->level, packet->hp_cur, packet->hp_max, packet->mana_cur, packet->mana_max);
	addAlly(packet->petId, packet->petName, packet->petLook, packet->petLevel, packet->pet_hp_cur, packet->pet_hp_max, 0, 0);

	if ((ally.size() + monster.size()) == actorCount) {
		mode = bmRoundStart;
	}
}


void Battle::handleResponsePacket(pBattleResponse* packet) {
	
	int action = packet->action;
	int currentGroup = packet->group;
	if (currentGroup != lastActionGroup && responseScene) {
		scenes.push(responseScene);
		responseScene = nullptr;
		lastActionGroup = currentGroup;
	}
	if (!responseScene) responseScene = new BattleScene(ally, monster, &floatingLabels);

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
	}
}


void Battle::createAttackResponse(pBattleResponse* packet, BattleScene* scene) {
	bMonster* source = getActorById(packet->sourceId);
	bMonster* target = getActorById(packet->targetId);

	int reaction = (packet->targetState) ? AnimType::Wound : AnimType::Faint;
	//defend?
	responseScene->setReactor(target, reaction);

	SDL_Point target_position;
	if (target->type == bActorType::ENEMY) target_position = getBattlePosFromArray(enemyArray, target->battleId, false);
	else target_position = getBattlePosFromArray(allyArray, target->battleId, false);
	target_position = { (renderRect.x + renderRect.w - target_position.x), (renderRect.y + renderRect.h - target_position.y) };

	responseScene->addAction(source, bsMoveTo, AnimType::Walk, target_position);
	responseScene->addAction(source, bsAction, (AnimType)(AnimType::Attack01 + (rand() % 3)), target_position, createFloatingLabel(packet->damage, "", target));
	responseScene->addAction(source, bsMoveTo, (AnimType)(AnimType::Attack01 + (rand() % 3)), source->basePos);
	responseScene->addAction(source, bsStandby, (AnimType)(AnimType::Attack01 + (rand() % 3)), source->basePos);
}


void Battle::createDefendResponse(pBattleResponse* packet) {
	bMonster* source = getActorById(packet->sourceId);
	source->defending = true;

	floatingLabels.push_back(createFloatingLabel("Defense", "", source));
}


void Battle::createCaptureResponse(pBattleResponse* packet) {
	bMonster* source = getActorById(packet->sourceId);
	bMonster* target = getActorById(packet->targetId);

	responseScene->setReactor(source, AnimType::StandBy);
	responseScene->addAction(target, bsCaptureBegin, (AnimType)0, target->basePos);
	responseScene->addAction(target, bsCaptureDrag, (AnimType)0, target->basePos);

	if (packet->action == baCaptureSuccess) {
		responseScene->addAction(target, bsCaptureSuccess, AnimType::Walk, source->basePos);

		//remove monster from list
		captureTarget = target;
		for (int i = 0; i < monsterCount; i++) {
			if (monster[i] == target) {
				monster.erase(monster.begin() + i);
				break;
			}
		}
		monsterCount--;
	}

	if (packet->action == baCaptureFail) {
		responseScene->addAction(target, bsCaptureAngry, AnimType::StandBy, target->pos, "Test...");
		responseScene->addAction(target, bsCaptureFail, AnimType::Walk, target->basePos);
		responseScene->addAction(target, bsStandby, AnimType::StandBy, target->basePos);
	}
}


void Battle::handleRoundPacket(pBattleRound* packet) {
	//Make sure the latest scene is pushed to the scene queue
	if (responseScene) {
		scenes.push(responseScene);
		responseScene = nullptr;
	}
	lastActionGroup = 0;

	int koAlly = 0;
	for (int i = 0; i < allyCount; i++) {
		if (!packet->alive[i]) {
			ally[i]->alive = false;
			koAlly++;
		}
	}
	if (koAlly == allyCount) {
		battleEnd = true;
		battleAnimateReady = true;
		return;
	}

	int koEnemy = 0;
	for (int i = 0; i < monsterCount; i++) {
		if (!packet->alive[allyCount + i]) {
			monster[i]->alive = false;
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
	for (auto actor : ally) {
		if (actor->id == packet->sourceId) {
			actor->setHslShifts(packet->count, packet->hslSets);
			return;
		}
	}

	for (auto actor : monster) {
		if (actor->id == packet->sourceId) {
			actor->setHslShifts(packet->count, packet->hslSets);
			return;
		}
	}
}


void Battle::addAlly(int id, std::string name, int look, int level, int life_current, int life_max, int mana_current, int mana_max) {
	bAlly *tempAlly = new bAlly(this);
	tempAlly->renderer = renderer;
	tempAlly->id = id;
	tempAlly->name = name;
	tempAlly->look = look;
	tempAlly->level = level;
	tempAlly->life_current = life_current;
	tempAlly->life_max = life_max;
	tempAlly->mana_current = mana_current;
	tempAlly->mana_max = mana_max;

	tempAlly->current_animation = AnimType::StandBy;
	tempAlly->direction = 5;

	tempAlly->setAnimation(tempAlly->current_animation, tempAlly->direction);
	tempAlly->animation[tempAlly->current_animation_name]->start();

	int arrayPos = 10 + (ally.size() / 2);
	if (ally.size() % 2 != 0) {
		tempAlly->type = bActorType::PET;
		arrayPos = 15 + (ally.size() / 2);
	}
	tempAlly->battleId = arrayPos;

	SDL_Point posOffset = getBattlePosFromArray(allyArray, arrayPos, true);
	tempAlly->basePos = { (renderRect.x + renderRect.w - posOffset.x), (renderRect.y + renderRect.h - posOffset.y) };
	tempAlly->pos = tempAlly->basePos;
	//tempAlly->destPos = tempAlly->basePos;
	tempAlly->animation[tempAlly->current_animation_name]->setLocation(tempAlly->pos.x, tempAlly->pos.y);

	ally.push_back(tempAlly);
	allyCount++;
}


void Battle::addMonster(int id, std::string name, int look, int level) {
	bMonster *tempMon = new bMonster(this);
	tempMon->renderer = renderer;
	tempMon->id = id;
	tempMon->battleId = monster.size();
	tempMon->name = name;
	tempMon->look = look;
	tempMon->level = level;

	tempMon->current_animation = AnimType::StandBy;
	tempMon->direction = 1;

	tempMon->setAnimation(tempMon->current_animation, tempMon->direction);
	tempMon->animation[tempMon->current_animation_name]->start();
	
	SDL_Point posOffset = getBattlePosFromArray(enemyArray, monster.size(), true);
	tempMon->basePos = { (renderRect.x + renderRect.w - posOffset.x), (renderRect.y + renderRect.h - posOffset.y) };
	tempMon->pos = tempMon->basePos;
	//tempMon->destPos = tempMon->basePos;
	tempMon->animation[tempMon->current_animation_name]->setLocation(tempMon->pos.x, tempMon->pos.y);

	monster.push_back(tempMon);
	monsterCount++;
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


bMonster* Battle::getActorById(int actorId) {
	for (auto actor : ally) {
		if (actor->id == actorId) return actor;
	}

	for (auto actor : monster) {
		if (actor->id == actorId) return actor;
	}

	return nullptr;
}


void Battle::loadBattleArray(BattleArray** bArray, int arrayId, int type) {
	if (*bArray) delete *bArray;
	*bArray = new BattleArray;

	std::string arrayPath = "ArrayData\\";
	INI arrayINI(arrayPath + "array.ini", "ARRAY" + std::to_string(arrayId));
	std::string arrayFilePath = arrayINI.getEntry("ARRAY0");
	if (arrayFilePath.at(0) == '.') arrayFilePath = arrayFilePath.substr(2, std::string::npos);

	std::vector<BYTE> test;
	std::ifstream ifs(arrayFilePath, std::ios::binary);
	if (ifs) {
		ifs.read((char*)(*bArray)->arrayData, 348);
	}
	ifs.close();

	//0 is 'Normal' - type should be 0(ally) or 1(enemy)
	if (arrayId > 0) {
		std::string arrayImgPath = arrayINI.getEntry("Pic" + std::to_string(type));
		if (arrayImgPath.at(0) == '.') arrayImgPath = arrayImgPath.substr(2, std::string::npos);
		(*bArray)->texture = new Texture(renderer, arrayImgPath, SDL_Color{ 0,0,0,255 });
		(*bArray)->texture->rect.x = renderRect.x;
		(*bArray)->texture->rect.y = renderRect.y;
		(*bArray)->visible = true;
	}

	(*bArray)->type = arrayId;
	switch (arrayId) {
	case 0:
		break;
	case 1:
		(*bArray)->name = "Phoenix";
		break;
	case 2:
		(*bArray)->name = "Tiger";
		break;
	case 3:
		(*bArray)->name = "Turtle";
		break;
	case 4:
		(*bArray)->name = "Kylin";
		break;
	case 5:
		(*bArray)->name = "Dragon";
		break;
	}
	(*bArray)->pivot = "Fine";
	(*bArray)->condition = "Normal";
	(*bArray)->attack = 0;
	(*bArray)->defense = 0;
	(*bArray)->dex = 0;

	if (type) (*bArray)->top = true;
	else (*bArray)->top = false;
}


SDL_Point Battle::getBattlePosFromArray(BattleArray* bArray, int fighterNum, bool isSource) {
	if (!bArray) return { 0, 0 };
	BYTE* arrayData = bArray->arrayData;

	int *v4 = (int*)(arrayData + 0xE4); // v4[5] - 0xE4
	int *v5 = (int*)(arrayData + 0xF8); // v5[5] - 0xF8
	int *v6 = (int*)(arrayData + 0x10C); // v6[5] - 0x10C
	int *v7 = (int*)(arrayData + 0x120); // v7[5] - 0x120
	int *v8 = (int*)(arrayData + 0x134); // v8[10] - 0x134

	SDL_Point sourcePos = { 0,0 };
	SDL_Point targetPos = { 0,0 };

	if (fighterNum >= 5)
	{
		if (fighterNum >= 10)
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
				sourcePos.x = 46 * (15 - v4[fighterNum]) + 46;
				sourcePos.y = 25 * (15 - v5[fighterNum]);
				targetPos.x = 46 * (15 - v4[fighterNum]);
				targetPos.y = 25 * (15 - v5[fighterNum]) + 25;
			}
		}
		else
		{
			sourcePos.x = 46 * v7[fighterNum];
			sourcePos.y = 25 * v8[fighterNum] + 25;
			targetPos.x = 46 * v7[fighterNum] + 46;
			targetPos.y = 25 * v8[fighterNum];
		}
	}
	else
	{
		sourcePos.x = 46 * v6[fighterNum];
		sourcePos.y = 25 * v7[fighterNum] + 25;
		targetPos.x = 46 * v6[fighterNum] + 46;
		targetPos.y = 25 * v7[fighterNum];
	}
	
	if (isSource) return sourcePos;
	return targetPos;
}


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

	SDL_SetRenderTarget(renderer, NULL);
}


void bMonster::setAnimation(int animType, int animDir) {
	current_animation = animType;
	current_animation_name = animationTypeToString(animType) + std::to_string(animDir);
	
	if (animation.find(current_animation_name) == animation.end()) {
		loadAnimationSprite(animType, animDir);
	}
	Sprite* currentAnim = animation[current_animation_name];
	currentAnim->start();
}


void bMonster::loadAnimationSprite(int animType, int animDir) {
	std::string aniFile = getAniFileName(look);
	std::string aniSection = animationTypeToString(animType) + std::to_string(animDir);

	INI ani(aniFile, aniSection);
	std::vector<std::string> frames;
	for (int i = 0; i < stoi(ani.getEntry("FrameAmount")); i++) {
		std::string nextFrame = "Frame" + std::to_string(i);

		std::string framePath = ani.getEntry(nextFrame);
		//if (framePath.substr(framePath.length() - 4) == ".rle") framePath = framePath.substr(0, framePath.length() - 4) + ".png";
		frames.push_back(framePath);
	}

	Sprite* tempSprite = new Sprite(renderer, frames, stCharacter);
	
	tempSprite->speed = 16;
	if (animType >= AnimType::Attack01 && animType <= AnimType::Attack03) tempSprite->speed = 9;
	if (animType == AnimType::Faint) {
		tempSprite->repeatMode = 1;
		tempSprite->speed = 3;
	}

	tempSprite->setLocation(pos.x, pos.y);
	if (look > 32) tempSprite->setHsbShifts(hslSets, hslSetCount);
	animation[aniSection] = tempSprite;
	animation_names.push_back(aniSection);
	tempSprite = nullptr;
}


std::string bMonster::getAniFileName(int aLook) {
	INI roles("INI\\Roles.ini", "RolesInfo");
	//roles.setSection("RolesInfo");
	std::string entry = "Role" + std::to_string(aLook);
	std::string aniPath = roles.getEntry(entry);
	aniPath = "ANI\\" + aniPath + ".ani";
	return aniPath;
}


void bMonster::render() {
	animation[current_animation_name]->setLocation(pos.x, pos.y);
	animation[current_animation_name]->render();
	if (!focusRect) {
		focusRect = new SDL_Rect;
		//*focusRect = animation[current_animation_name]->getCurrentTexture()->rect;
		*focusRect = animation[current_animation_name]->getRenderRect();
	}

	if (effect != beNone && effectSprite) {
		/*if (!effectSprite) {
			//load the sprite
			effectSprite = new Sprite(renderer);
			effectSprite = battle->getEffect(effect)->copy();
			effectSprite->start();
		}*/
		effectSprite->setLocation(pos.x, pos.y);
		effectSprite->render();
	}

	if (secEffect != beNone && secEffectSprite) {
		/*if (!secEffectSprite) {
			//load the sprite
			secEffectSprite = new Sprite(renderer);
			secEffectSprite = battle->getEffect(secEffect)->copy();
			secEffectSprite->start();
		}*/
		secEffectSprite->setLocation(pos.x, pos.y);
		secEffectSprite->render();
	}
}


void bMonster::setHslShifts(int count, BYTE* shifts) {
	if (look <= 32) return;
	hslSetCount = count;
	memcpy(hslSets, shifts, count * 5);
	for (auto sprite : animation) {
		sprite.second->setHsbShifts(hslSets, count);
	}
}


void bMonster::setEffect(bEffect ef) {
	if (effect == ef) return;

	if (effectSprite) {
		delete effectSprite;
		effectSprite = nullptr;
	}

	effect = ef;
	if (effect == beNone) return;
	effectSprite = new Sprite(renderer);
	effectSprite = battle->getEffect(effect)->copy();
	effectSprite->start();
}


void bMonster::setSecondaryEffect(bEffect ef) {
	if (secEffect == ef) return;
	
	if (secEffectSprite) {
		delete secEffectSprite;
		secEffectSprite = nullptr;
	}

	secEffect = ef;
	if (secEffect == beNone) return;
	secEffectSprite = new Sprite(renderer);
	secEffectSprite = battle->getEffect(secEffect)->copy();
	secEffectSprite->start();
}


void Battle::loadEffect(bEffect effect) {
	INI effectINI("ini\\Common.ini", effectTypeToString(effect));
	std::vector<std::string> frames;
	for (int i = 0; i < stoi(effectINI.getEntry("FrameAmount")); i++) {
		std::string filePath = effectINI.getEntry("Frame" + std::to_string(i));

		if (filePath.at(0) == '.') {
			filePath = filePath.substr(2, std::string::npos);
		}
		filePath = "data\\" + filePath;

		frames.push_back(filePath);
	}
	Sprite* tempEffect = new Sprite(renderer, frames, stEffect);

	if (effect == beReady) tempEffect->speed = 6;

	effects[effect] = tempEffect;
}


std::string Battle::effectTypeToString(int effectType) {
	switch (effectType) {
	case bEffect::beFaint: return "Faint";
	case bEffect::beReady: return "Ready";
	case bEffect::beMirror: return "Mirror";
	case bEffect::beSphere: return "Sphere";
	}

	return "";
}


Sprite* Battle::getEffect(bEffect effect) {
	if (effects.find(effect) == effects.end()) loadEffect(effect);
	return effects[effect];
}


FloatingLabel* Battle::createFloatingLabel(std::string top, std::string bottom, bMonster* target) {
	FloatingLabel *newLabel = new FloatingLabel();
	SDL_Point point;
	point.x = target->pos.x;
	point.y = target->pos.y - 50;

	newLabel->top = stringToTexture(renderer, top, gui->font, 0, { 255, 255, 255, 255 }, 0);
	newLabel->top->setPosition(point);
	if (!bottom.empty()) {
		newLabel->bottom = stringToTexture(renderer, bottom, gui->font, 0, { 0, 0, 0, 255 }, 0);
		point.y += newLabel->top->height;
		newLabel->bottom->setPosition(point);
	}

	return newLabel;
}


FloatingLabel* Battle::createFloatingLabel(int damage, std::string bottom, bMonster* target) {
	std::string top;
	if (damage == 0) top = "Miss";
	else top = "-" + std::to_string(damage);
	return createFloatingLabel(top, bottom, target);
}