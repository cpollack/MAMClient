#include "stdafx.h"
#include "MainUI.h"

//Subforms triggered by buttons
#include "BattleConfigForm.h"
#include "CharacterForm.h"
#include "PetListForm.h"
#include "InventoryForm.h"
#include "WuxingForm.h"

//TEMP
#include "PetMagic.h" 
#include "PetComposeForm.h"
//ENDTEMP

#include "Window.h"
#include "Gauge.h"
#include "Label.h"

#include "Options.h"
#include "Battle.h"
#include "GameMap.h"
#include "Player.h"
#include "Pet.h"

#include "CustomEvents.h"
#include "MessageManager.h"

#include "pBattleState.h"

CMainUI::CMainUI(CWindow* w) : CWidget(w) {
	Name = "MainUI";
	window = w;

	//Fixed rectangle of main UI
	SetX(0);
	SetY(477);
	SetWidth(800);
	SetHeight(123);

	CreateWidgets();
	Load();

	window->AddWidget(this);
}

CMainUI::~CMainUI() {
	for (auto widget : widgets) delete widget.second;
}

void CMainUI::CreateWidgets() {
	lblPlayerLevel = new CLabel(window, "lblPlayerLevel", 29, 501);
	lblPlayerLevel->SetAlignment(laCenter);
	lblPlayerLevel->SetVAlignment(lvaCenter);
	lblPlayerLevel->SetWidth(22);
	lblPlayerLevel->SetHeight(11);
	lblPlayerLevel->SetFontSize(10);
	widgets["lblPlayerLevel"] = lblPlayerLevel;
	lblPetLevel = new CLabel(window, "lblPetLevel", 767, 570);
	lblPetLevel->SetAlignment(laCenter);
	lblPetLevel->SetVAlignment(lvaCenter);
	lblPetLevel->SetWidth(22);
	lblPetLevel->SetHeight(11);
	lblPetLevel->SetFontSize(10);
	widgets["lblPetLevel"] = lblPetLevel;

	btnBattle = new CButton(window, "btnBattle", 370, 532);
	btnBattle->SetWidth(60);
	btnBattle->SetHeight(30);
	btnBattle->SetUnPressedImage("data/GUI/Main/btnBattle.png");
	widgets["btnBattle"] = btnBattle;
	RegisterEvent("btnBattle", "Click", std::bind(&CMainUI::btnBattle_Click, this, std::placeholders::_1));
	btnBattleAuto = new CButton(window, "btnBattleAuto", 370, 532);
	btnBattleAuto->SetWidth(60);
	btnBattleAuto->SetHeight(30);
	btnBattleAuto->SetUnPressedImage("data/GUI/Main/btnBattle_AutoOff.png");
	btnBattleAuto->SetPressedImage("data/GUI/Main/btnBattle_AutoOn.png");
	btnBattleAuto->SetType(ButtonType::btToggle);
	btnBattleAuto->Toggle(options.GetAutoBattle(), false);
	btnBattleAuto->SetVisible(false);
	widgets["btnBattleAuto"] = btnBattleAuto;
	RegisterEvent("btnBattleAuto", "Click", std::bind(&CMainUI::btnBattleAuto_Click, this, std::placeholders::_1));
	btnBattleRepeat = new CButton(window, "btnBattleRepeat", 372, 562);
	btnBattleRepeat->SetWidth(28);
	btnBattleRepeat->SetHeight(23);
	btnBattleRepeat->SetUnPressedImage("data/GUI/Main/btnBattleRepeat_off.png");
	btnBattleRepeat->SetPressedImage("data/GUI/Main/btnBattleRepeat_on.png");
	btnBattleRepeat->SetType(ButtonType::btToggle);
	widgets["btnBattleRepeat"] = btnBattleRepeat;
	RegisterEvent("btnBattleRepeat", "Toggle", std::bind(&CMainUI::btnBattleRepeat_Toggle, this, std::placeholders::_1));
	btnBattleConfig = new CButton(window, "btnBattle", 402, 564);
	btnBattleConfig->SetWidth(22);
	btnBattleConfig->SetHeight(19);
	btnBattleConfig->SetUnPressedImage("data/GUI/Main/btnBattleConfig.png");
	widgets["btnBattleConfig"] = btnBattleConfig;
	RegisterEvent("btnBattleConfig", "Click", std::bind(&CMainUI::btnBattleConfig_Click, this, std::placeholders::_1));

	btnPlayer = addMenuButton("btnPlayer", menuPoint[0], "btnMenuPlayer.png");
	btnPet = addMenuButton("btnPet", menuPoint[1], "btnMenuPet.png");
	btnInventory = addMenuButton("btnInventory", menuPoint[2], "btnMenuInventory.png");
	btnKungfu = addMenuButton("btnKungfu", menuPoint[3], "btnMenuKungfu.png");
	btnWuxing = addMenuButton("btnWuxing", menuPoint[4], "btnMenuWuxing.png");
	btnTeam = addMenuButton("btnTeam", menuPoint[5], "btnMenuTeam.png");
	btnSocial = addMenuButton("btnSocial", menuPoint[6], "btnMenuSocial.png");
	btnOptions = addMenuButton("btnOptions", menuPoint[7], "btnMenuOptions.png");

	RegisterEvent("btnPlayer", "Click", std::bind(&CMainUI::btnPlayer_Click, this, std::placeholders::_1));
	RegisterEvent("btnPet", "Click", std::bind(&CMainUI::btnPet_Click, this, std::placeholders::_1));
	RegisterEvent("btnInventory", "Click", std::bind(&CMainUI::btnInventory_Click, this, std::placeholders::_1));
	RegisterEvent("btnKungfu", "Click", std::bind(&CMainUI::btnKungfu_Click, this, std::placeholders::_1));
	RegisterEvent("btnWuxing", "Click", std::bind(&CMainUI::btnWuxing_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeam", "Click", std::bind(&CMainUI::btnTeam_Click, this, std::placeholders::_1));
	RegisterEvent("btnSocial", "Click", std::bind(&CMainUI::btnSocial_Click, this, std::placeholders::_1));
	RegisterEvent("btnOptions", "Click", std::bind(&CMainUI::btnOptions_Click, this, std::placeholders::_1));
}

void CMainUI::Load() {
	//Player Frame
	PlayerFrame.reset(new Texture(renderer, "data/GUI/Main/panel_player.png"));
	PlayerFrame->setPosition({ 0, 477 });

	if (!gaugePlayerHealth) gaugePlayerHealth = addGauge("gaugePlayerHealth", 32, 515, 70, 70, "player_life.png", "", true);
	if (!gaugePlayerMana) gaugePlayerMana = addGauge("gaugePlayerMana", 0, 529, 118, 69, "player_mana.png", "", false);
	if (!gaugePlayerExp) gaugePlayerExp = addGauge("gaugePlayerExp", 127, 593, 260, 7, "player_exp.png", "", false);
	updatePlayerLevel();

	//Pet Frame
	std::string element = getPetElementType();

	std::string petPanel = "data/GUI/Main/panel_" + element + ".png";
	PetFrame.reset(new Texture(renderer, petPanel));
	PetFrame->setPosition({ 413, 477 });

	std::string petForeground = "pet_life_" + element + ".png";
	std::string petBackground = "pet_life_" + element + "_back.png";
	if (!gaugePetHealth) gaugePetHealth = addGauge("gaugePetHealth", 697, 515, 70, 70, petForeground, petBackground, true);
	else {
		gaugePetHealth->SetForegroundImage("data/GUI/Main/" + petForeground);
		gaugePetHealth->SetBackgroundImage("data/GUI/Main/" + petBackground);
		gaugePetHealth->ReloadAssets();
	}
	if (!gaugePetExp) gaugePetExp = addGauge("gaugePetExp", 413, 593, 260, 7, "pet_exp.png", "", false);
	gaugePetExp->SetRightFill(true);
	updatePetLevel();
}

std::string CMainUI::getPetElementType() {
	std::string element = "";
	Pet *pPet = player->GetMarchingPet();
	if (pPet) {
		element = pPet->GetElementText();
	}
	if (element.length() == 0) element = "Metal";
	return element;
}

void CMainUI::ReloadAssets() {
	Load();
}

void CMainUI::Render() {
	SDL_RenderCopy(renderer, PlayerFrame->texture, NULL, &PlayerFrame->getRect());
	SDL_RenderCopy(renderer, PetFrame->texture, NULL, &PetFrame->getRect());

	for (auto widget : widgets) widget.second->Render();
}

void CMainUI::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e); //Mouseover

	if (e.type == CUSTOMEVENT_BATTLE) {
		if (e.user.code == BATTLE_START) {
			btnBattle->SetVisible(false);
			btnBattleAuto->SetVisible(true);
			btnBattleAuto->Toggle(options.GetAutoBattle(), false);
		}
		if (e.user.code == BATTLE_END) {
			btnBattle->SetVisible(true);
			btnBattleAuto->SetVisible(false);
		}
	}

	for (auto widget : widgets) widget.second->HandleEvent(e);

	SDL_Event e2 = e;
	e2.motion.x -= X;
	e2.motion.y -= Y;

	if (e.type == SDL_MOUSEMOTION) {
		MouseOverPlayer = false;
		MouseOverPet = false;

		/*if (MouseOver) {
			MouseOver = false;
			SDL_Point point = { e.motion.x - X, e.motion.y - Y };
			SDL_Point petPoint = { point.x - petOffset.x, point.y - petOffset.y };
			if (IsAssetPixel(PlayerFrame, playerPixels, point)) {
				MouseOver = true;
				MouseOverPlayer = true;
			}
			else if (IsAssetPixel(PetFrame, petPixels, petPoint)) {
				MouseOver = true;
				MouseOverPet = true;
			}
		}*/
	}

	if (e.type == SDL_MOUSEBUTTONDOWN && MouseOver) {
		//held = true;
		btnPlayer_Down = false;
		btnPet_Down = false;

		/*if (MouseOverPlayer && doesPointIntersect(PlayerBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnPlayer_Down = true;
		}
		else if (MouseOverPet && doesPointIntersect(PetBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnPet_Down = true;
		}*/
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		/*if (MouseOverPlayer && doesPointIntersect(PlayerBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnCharacter_Click(e);
		}
		else if (MouseOverPet && doesPointIntersect(PetBtnRect, SDL_Point{ e2.motion.x, e2.motion.y })) {
			btnPet_Click(e);
		}*/

		//held = false;
		btnPlayer_Down = false;
		btnPet_Down = false;
	}

	//Override Mouse over so empty space doesn't steal clicks
	MouseOver = false;
}

void CMainUI::Step() {
	if (battle && battle->isOver()) {
		delete battle;
		battle = nullptr;
		if (options.GetRepeatBattle()) {
			SDL_Event e;
			btnBattle_Click(e);
		}
	}
}

CGauge* CMainUI::addGauge(std::string name, int x, int y, int w, int h, std::string foreground, std::string background, bool verticle) {
	std::string prefix = "data/GUI/Main/";

	CGauge *gauge = new CGauge(Window, name, x, y);
	gauge->SetWidth(w);
	gauge->SetHeight(h);
	gauge->SetForegroundImage(prefix + foreground);
	if (background.length()) gauge->SetBackgroundImage(prefix + background);
	gauge->SetVerticle(verticle);
	widgets[name] = gauge;
	return gauge;
}

CButton* CMainUI::addMenuButton(std::string name, SDL_Point point, std::string imgPath) {
	std::string prefix = "data/GUI/Main/MenuButtons/";
	CButton* btnMenu = new CButton(Window, name, point.x, point.y);
	btnMenu->SetUnPressedImage(prefix + imgPath);
	btnMenu->SetWidth(menuBtnWidth);
	btnMenu->SetHeight(menuBtnHeight);
	widgets[name] = btnMenu;
	return btnMenu;
}

bool CMainUI::IsAssetPixel(Asset asset, Uint32 *pixels, SDL_Point point) {
	if (!asset || !pixels) return false;
	if (point.x < 0 || point.y < 0) return false;
	if (point.x > asset->width || point.y > asset->height) return false;

	int idx = asset->width * point.y + point.x;
	if (pixels[idx] > 0) return true;
	return false;
}

void CMainUI::btnBattle_Click(SDL_Event& e) {
	if (battle) return;
	if (!map || map->IsChangingMap()) return;

	if (!map->isBattleEnabled()) {
		messageManager.DoSystemMessage("You cannot battle in this location");
		return;
	}

	int teamSize = 1;
	pBattleState* battlePacket = new pBattleState(0, teamSize, player->GetID(), 0);
	gClient.addPacket(battlePacket);
}

void CMainUI::btnBattleAuto_Click(SDL_Event& e) {
	options.SetAutoBattle(btnBattleAuto->GetToggled());
}

void CMainUI::btnBattleRepeat_Toggle(SDL_Event& e) {
	if (!btnBattleRepeat) return;
	if (options.GetRepeatBattle()) {
		options.SetRepeatBattle(false);
		return;
	}

	if (!map) return;
	btnBattle_Click(e);
	if (!map->isBattleEnabled()) {
		btnBattleRepeat->Toggle(false, false);
		return;
	}

	options.SetRepeatBattle(true);
}

void CMainUI::btnBattleConfig_Click(SDL_Event& e) {
	if (battle) return;
	CBattleConfigForm* bcForm = new CBattleConfigForm();
	Windows.push_back(bcForm);
}

void CMainUI::btnPlayer_Click(SDL_Event& e) {
	if (battle) return;
	CCharacterForm* charForm = new CCharacterForm();
	Windows.push_back(charForm);
}

void CMainUI::btnPet_Click(SDL_Event& e) {
	if (battle) return;
	CPetListForm* petForm = new CPetListForm();
	Windows.push_back(petForm);
}

void CMainUI::btnInventory_Click(SDL_Event& e) {
	if (battle) return;
	CInventoryForm* invForm = new CInventoryForm();
	Windows.push_back(invForm);
}

void CMainUI::btnKungfu_Click(SDL_Event& e) {
	if (battle) return;

	CPetComposeForm* compForm = new CPetComposeForm();
	Windows.push_back(compForm);
	//testing for pet magic evolve. compose is next!
	/*CPetMagic *pm = new CPetMagic(renderer, 0);
	ColorShift cs[3];
	pm->setCoordinate(player->GetCoord());
	pm->addSource(54, cs);
	pm->addDestination(57, cs);
	map->addPetMagic(pm);
	pm->start();*/
}

void CMainUI::btnWuxing_Click(SDL_Event& e) {
	if (battle) return;
	CWuxingForm* wuxForm = new CWuxingForm();
	Windows.push_back(wuxForm);
}

void CMainUI::btnTeam_Click(SDL_Event& e) {
	if (battle) return;
	//
}

void CMainUI::btnSocial_Click(SDL_Event& e) {
	if (battle) return;
	//
}

void CMainUI::btnOptions_Click(SDL_Event& e) {
	if (battle) return;
	//
}

void CMainUI::setPlayerHealthGauge(int val) {
	gaugePlayerHealth->set(val);
}


void CMainUI::setPlayerHealthGauge(int val, int max) {
	gaugePlayerHealth->set(val, max);
}


void CMainUI::shiftPlayerHealthGauge(int val) {
	if (val >= 0) gaugePlayerHealth->add(val);
	else gaugePlayerHealth->subtract(val);
}


void  CMainUI::adjustPlayerHealthGauge(int val) {
	gaugePlayerHealth->AdjustTo(val);
}


void CMainUI::setPlayerManaGauge(int val) {
	gaugePlayerMana->set(val);
}


void CMainUI::setPlayerManaGauge(int val, int max) {
	gaugePlayerMana->set(val, max);
}


void CMainUI::shiftPlayerManaGauge(int val) {
	if (val >= 0) gaugePlayerMana->add(val);
	else gaugePlayerMana->subtract(val);
}


void CMainUI::adjustPlayerManaGauge(int val) {
	gaugePlayerMana->AdjustTo(val);
}


void CMainUI::setPlayerExpGauge(int val) {
	gaugePlayerExp->set(val);
}


void CMainUI::setPlayerExpGauge(int val, int max) {
	gaugePlayerExp->set(val, max);
}


void CMainUI::shiftPlayerExpGauge(int val) {
	if (val >= 0) gaugePlayerExp->add(val);
	else gaugePlayerExp->subtract(val);
}


void CMainUI::adjustPlayerExpGauge(int val) {
	gaugePlayerExp->AdjustTo(val);
}


void CMainUI::setPetHealthGauge(int val) {
	gaugePetHealth->set(val);
}


void CMainUI::setPetHealthGauge(int val, int max) {
	gaugePetHealth->set(val, max);
}


void CMainUI::shiftPetHealthGauge(int val) {
	if (val >= 0) gaugePetHealth->add(val);
	else gaugePetHealth->subtract(val);
}


void  CMainUI::adjustPetHealthGauge(int val) {
	gaugePetHealth->AdjustTo(val);
}


void CMainUI::setPetExpGauge(int val) {
	gaugePetExp->set(val);
}


void CMainUI::setPetExpGauge(int val, int max) {
	gaugePetExp->set(val, max);
}


void CMainUI::shiftPetExpGauge(int val) {
	if (val >= 0) gaugePetExp->add(val);
	else gaugePetExp->subtract(val);
}


void CMainUI::adjustPetExpGauge(int val) {
	gaugePetExp->AdjustTo(val);
}


void CMainUI::updatePlayerLevel() {
	lblPlayerLevel->SetText(std::to_string(player->GetLevel()));
}

void CMainUI::updatePetLevel() {
	Pet *pPet = player->GetMarchingPet();
	if (pPet) {
		lblPetLevel->SetText(std::to_string(pPet->GetLevel()));
	}
	else lblPetLevel->SetText("");
}