#include "stdafx.h"
#include "MainUI.h"

#include "Global.h"

//Subforms triggered by buttons
#include "BattleConfigForm.h"
#include "CharacterForm.h"
#include "PetListForm.h"
#include "InventoryForm.h"
#include "WuxingForm.h"

#include "Window.h"
#include "Gauge.h"
#include "Label.h"

#include "Options.h"
#include "Battle.h"
#include "GameMap.h"
#include "Player.h"
#include "Pet.h"
#include "Team.h"

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
	SDL_Rect surfaceRect{ 10, 600 - 120 - 10, 0, 0 };

	//LABELS
	lblCoordX = addLabel("lblCoordX", { surfaceRect.x + 220, surfaceRect.y + 10 }, 20, 14, "", true);
	lblCoordY = addLabel("lblCoordY", { surfaceRect.x + 250, surfaceRect.y + 10 }, 20, 14, "", true);

	int col1 = 382, col2 = 518, col3 = 660;
	int row1 = 52, row2 = 76, row3 = 100;
	addLabel("lblStaticName", { surfaceRect.x + col1, surfaceRect.y + row1 }, 40, 14, "Name", true);
	addLabel("lblStaticNickName", { surfaceRect.x + col1, surfaceRect.y + row2 }, 60, 14, "Nickname", true);
	addLabel("lblStaticLevel", { surfaceRect.x + col1, surfaceRect.y + row3 }, 35, 14, "Level", true);
	lblName = addLabel("lblName", { surfaceRect.x + col1 + 45, surfaceRect.y + row1 }, 90, 14, "");
	lblNickName = addLabel("lblNickName", { surfaceRect.x + col1 + 67, surfaceRect.y + row2 }, 75, 14, "");
	lblLevel = addLabel("lblLevel", { surfaceRect.x + col1 + 37, surfaceRect.y + row3 }, 95, 14, "");

	addLabel("lblStaticSpouse", { surfaceRect.x + col2, surfaceRect.y + row1 }, 45, 14, "Spouse", true);
	addLabel("lblStaticCash", { surfaceRect.x + col2, surfaceRect.y + row2 }, 30, 14, "Cash", true);
	addLabel("lblStaticReputation", { surfaceRect.x + col2, surfaceRect.y + row3 }, 65, 14, "Reputation", true);
	lblSpouse = addLabel("lblSpouse", { surfaceRect.x + col2 + 52, surfaceRect.y + row1 }, 90, 14, "None");
	lblCash = addLabel("lblCash", { surfaceRect.x + col2 + 38, surfaceRect.y + row2 }, 105, 14, "1");
	lblReputation = addLabel("lblReputation", { surfaceRect.x + col2 + 73, surfaceRect.y + row3 }, 75, 14, "2");

	lblRank = addLabel("lblRank", { surfaceRect.x + col3, surfaceRect.y + row1 }, 118, 14, "Mortal", false);
	addLabel("lblStaticGuild", { surfaceRect.x + col3, surfaceRect.y + row2 }, 32, 14, "Guild", true);
	addLabel("lblStaticGuildRank", { surfaceRect.x + col3, surfaceRect.y + row3 }, 50, 14, "Position", true);
	lblGuild = addLabel("lblGuild", { surfaceRect.x + col3 + 37, surfaceRect.y + row2 }, 90, 14, "");
	lblGuildRank = addLabel("lblGuildRank", { surfaceRect.x + col3 + 57, surfaceRect.y + row3 }, 70, 14, "");

	//GAUGES
	gaugePlayerHealth = addGauge("gaugePlayerHealth", { surfaceRect.x + 62, surfaceRect.y + 47 }, 110, 16, "yellow_slot.jpg");
	gaugePlayerMana = addGauge("gaugePlayerMana", { surfaceRect.x + 62, surfaceRect.y + 70 }, 110, 16, "green_slot.jpg");
	gaugePlayerExp = addGauge("gaugePlayerExp", { surfaceRect.x + 62, surfaceRect.y + 93 }, 110, 16, "green_slot.jpg");
	gaugePetHealth = addGauge("gaugePetHealth", { surfaceRect.x + 259, surfaceRect.y + 47 }, 110, 16, "yellow_slot.jpg");
	gaugePetExp = addGauge("gaugePetExp", { surfaceRect.x + 259, surfaceRect.y + 70 }, 110, 16, "green_slot.jpg");

	//BUTTONS
	btnPlayer = addButton("btnPlayer", {surfaceRect.x + 17, surfaceRect.y + 11}, "button_basicMsg.jpg", "button_basicMsgDown.jpg", 73, 25);
	btnPet = addButton("btnPet", { surfaceRect.x + 102, surfaceRect.y + 11 }, "button_pet.jpg", "button_petDown.jpg", 73, 25);

	btnMap = addButton("btnMap", { surfaceRect.x + 512, surfaceRect.y + 1 }, "minMap-1.jpg", "minMap-2.jpg", 41, 39);
	btnTeam = addButton("btnTeam", { surfaceRect.x + 554, surfaceRect.y + 1 }, "team-1.jpg", "team-2.jpg", 41, 39);
	btnInventory = addButton("btnInventory", { surfaceRect.x + 596, surfaceRect.y + 1 },  "items-1.jpg", "items-2.jpg", 41, 39);
	btnKungfu = addButton("btnKungfu", { surfaceRect.x + 638, surfaceRect.y + 1 }, "PK-1.jpg", "PK-2.jpg", 41, 39);
	btnSocial = addButton("btnSocial", { surfaceRect.x + 680, surfaceRect.y + 1 }, "record-1.jpg", "record-2.jpg", 41, 39);
	btnFriend = addButton("btnFriend", { surfaceRect.x + 722, surfaceRect.y + 1 },  "email-1.jpg", "email-2.jpg", 41, 39);
	//btnWuxing = addMenuButton("btnWuxing", menuPoint[4], "btnMenuWuxing.png");
	//btnOptions = addMenuButton("btnOptions", menuPoint[7], "btnMenuOptions.png");

	RegisterEvent("btnPlayer", "Click", std::bind(&CMainUI::btnPlayer_Click, this, std::placeholders::_1));
	RegisterEvent("btnPet", "Click", std::bind(&CMainUI::btnPet_Click, this, std::placeholders::_1));

	RegisterEvent("btnMap", "Click", std::bind(&CMainUI::btnMap_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeam", "Click", std::bind(&CMainUI::btnTeam_Click, this, std::placeholders::_1));
	RegisterEvent("btnInventory", "Click", std::bind(&CMainUI::btnInventory_Click, this, std::placeholders::_1));
	RegisterEvent("btnKungfu", "Click", std::bind(&CMainUI::btnKungfu_Click, this, std::placeholders::_1));
	RegisterEvent("btnSocial", "Click", std::bind(&CMainUI::btnSocial_Click, this, std::placeholders::_1));
	RegisterEvent("btnFriend", "Click", std::bind(&CMainUI::btnFriend_Click, this, std::placeholders::_1));

	//TEAM BUTTONS
	int teamBtnY = surfaceRect.y - 25;
	btnTeamCreate = new CButton(window, "btnTeamCreate", 674, teamBtnY);
	btnTeamCreate->SetWidth(83);
	btnTeamCreate->SetHeight(21);
	btnTeamCreate->SetText("Create");
	widgets["btnTeamCreate"] = btnTeamCreate;
	btnTeamJoin = new CButton(window, "btnTeamJoin", 674, teamBtnY);
	btnTeamJoin->SetWidth(83);
	btnTeamJoin->SetHeight(21);
	btnTeamJoin->SetText("Join");
	widgets["btnTeamJoin"] = btnTeamJoin;
	btnTeamManage = new CButton(window, "btnTeamManage", 674, teamBtnY);
	btnTeamManage->SetWidth(83);
	btnTeamManage->SetHeight(21);
	btnTeamManage->SetText("Manage");
	widgets["btnTeamManage"] = btnTeamManage;
	btnTeamLeave = new CButton(window, "btnTeamLeave", 674, teamBtnY);
	btnTeamLeave->SetWidth(83);
	btnTeamLeave->SetHeight(21);
	btnTeamLeave->SetText("Leave");
	widgets["btnTeamLeave"] = btnTeamLeave;
	btnTeamDisband = new CButton(window, "btnTeamDisband", 674, teamBtnY);
	btnTeamDisband->SetWidth(83);
	btnTeamDisband->SetHeight(21);
	btnTeamDisband->SetText("Disband");
	widgets["btnTeamDisband"] = btnTeamDisband;
	HideTeamButtons();

	RegisterEvent("btnTeamCreate", "Click", std::bind(&CMainUI::btnTeamCreate_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeamJoin", "Click", std::bind(&CMainUI::btnTeamJoin_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeamManage", "Click", std::bind(&CMainUI::btnTeamManage_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeamLeave", "Click", std::bind(&CMainUI::btnTeamLeave_Click, this, std::placeholders::_1));
	RegisterEvent("btnTeamDisband", "Click", std::bind(&CMainUI::btnTeamDisband_Click, this, std::placeholders::_1));
}

void CMainUI::Load() {
	updatePlayerDetails();

	//Player
	/*if (!gaugePlayerHealth) gaugePlayerHealth = addGauge("gaugePlayerHealth", 32, 515, 70, 70, "player_life.png", "", true);
	if (!gaugePlayerMana) gaugePlayerMana = addGauge("gaugePlayerMana", 0, 529, 118, 69, "player_mana.png", "", false);
	if (!gaugePlayerExp) gaugePlayerExp = addGauge("gaugePlayerExp", 127, 593, 260, 7, "player_exp.png", "", false);
	updatePlayerLevel();

	//Pet
	if (!gaugePetHealth) gaugePetHealth = addGauge("gaugePetHealth", 413, 593, 260, 7, "pet_exp.png", "", false);
	if (!gaugePetExp) gaugePetExp = addGauge("gaugePetExp", 413, 593, 260, 7, "pet_exp.png", "", false);*/
	//gaugePetExp->SetRightFill(true);
	//updatePetLevel();
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
	//SDL_RenderCopy(renderer, PlayerFrame->texture, NULL, &PlayerFrame->getRect());
	//SDL_RenderCopy(renderer, PetFrame->texture, NULL, &PetFrame->getRect());

	for (auto widget : widgets) widget.second->Render();
}

void CMainUI::HandleEvent(SDL_Event& e) {
	CWidget::HandleEvent(e); //Mouseover

	if (e.type == CUSTOMEVENT_PLAYER) {
		if (e.user.code == PLAYER_TEAM) {
			if (ShowingTeamButtons) ShowTeamButtons(); //updates appropriate visible team buttons
		}
	}

	if (e.type == CUSTOMEVENT_BATTLE) {
		/*if (e.user.code == BATTLE_START) {
			btnBattle->SetVisible(false);
			btnBattleAuto->SetVisible(true);
			btnBattleAuto->Toggle(options.GetAutoBattle(), false);
		}
		if (e.user.code == BATTLE_END) {
			btnBattle->SetVisible(true);
			btnBattleAuto->SetVisible(false);
		}*/
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

CButton* CMainUI::addMenuButton(std::string name, SDL_Point point, std::string imgPath, int btnWidth, int btnHeight) {
	std::string prefix = "data/GUI/Main/MenuButtons/";
	CButton* btnMenu = new CButton(Window, name, point.x, point.y);
	btnMenu->SetUnPressedImage(prefix + imgPath);
	btnMenu->SetWidth(btnWidth);
	btnMenu->SetHeight(btnHeight);
	widgets[name] = btnMenu;
	return btnMenu;
}

CLabel* CMainUI::addLabel(std::string name, SDL_Point point, int w, int h, std::string text, bool bold/*=false*/) {
	CLabel* lbl = new CLabel(Window, name, point.x, point.y);
	lbl->SetWidth(w);
	lbl->SetHeight(h);
	if (bold) lbl->SetBold(true);
	lbl->SetAlignment(laLeft);
	lbl->SetVAlignment(lvaCenter);
	lbl->SetText(text);
	widgets[name] = lbl;
	return lbl;
}

CButton* CMainUI::addButton(std::string name, SDL_Point point, std::string buttonImg, std::string pushedImg, int btnWidth, int btnHeight) {
	CButton* btn = new CButton(Window, name, point.x, point.y);
	btn->SetUnPressedImage(buttonImg);
	if (pushedImg.length() > 0) btn->SetPressedImage(pushedImg);
	btn->SetWidth(btnWidth);
	btn->SetHeight(btnHeight);
	btn->SetUseGUI(true);
	widgets[name] = btn;
	return btn;
}

CGauge* CMainUI::addGauge(std::string name, SDL_Point point, int w, int h, std::string foreground, bool verticle/*=false*/) {
	CGauge* gauge = new CGauge(Window, name, point.x, point.y);
	gauge->SetWidth(w);
	gauge->SetHeight(h);
	gauge->SetForegroundImage(foreground);
	gauge->SetBackgroundImage("bg_slot.jpg");
	gauge->SetVerticle(verticle);
	gauge->SetUseGUI(true);
	widgets[name] = gauge;
	return gauge;
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
	//pBattleState* battlePacket = new pBattleState(0, teamSize, player->GetID(), 0);
	//gClient.addPacket(battlePacket);
}

void CMainUI::btnBattleAuto_Click(SDL_Event& e) {
	options.SetAutoBattle(btnBattleAuto->GetToggled());
}

/*void CMainUI::btnBattleRepeat_Toggle(SDL_Event& e) {
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
}*/

void CMainUI::btnBattleConfig_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	CBattleConfigForm* bcForm = new CBattleConfigForm();
	Windows.push_back(bcForm);
}

void CMainUI::btnPlayer_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	CCharacterForm* charForm = new CCharacterForm();
	Windows.push_back(charForm);
}

void CMainUI::btnPet_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	CPetListForm* petForm = new CPetListForm();
	Windows.push_back(petForm);
}

void CMainUI::btnMap_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	//
}

void CMainUI::btnTeam_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;

	if (ShowingTeamButtons) HideTeamButtons();
	else ShowTeamButtons();
}

void CMainUI::btnInventory_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	CInventoryForm* invForm = new CInventoryForm();
	Windows.push_back(invForm);
}

void CMainUI::btnKungfu_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	//
}

void CMainUI::btnSocial_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	//
}

void CMainUI::btnFriend_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	//
}

void CMainUI::btnWuxing_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	CWuxingForm* wuxForm = new CWuxingForm();
	Windows.push_back(wuxForm);
}

void CMainUI::btnOptions_Click(SDL_Event& e) {
	if (battle || options.GetRepeatBattle()) return;
	//
}

void CMainUI::btnTeamCreate_Click(SDL_Event& e) {
	CTeam::Create();
}

void CMainUI::btnTeamJoin_Click(SDL_Event& e) {
	GameMode = GAMEMODE_SELECTTEAM;
}

void CMainUI::btnTeamManage_Click(SDL_Event& e) {
	//
}

void CMainUI::btnTeamLeave_Click(SDL_Event& e) {
	CTeam::Leave();
}

void CMainUI::btnTeamDisband_Click(SDL_Event& e) {
	CTeam::Leave();
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


void CMainUI::updatePlayerDetails() {
	lblName->SetText(StringToWString(player->GetName()));
	lblNickName->SetText(StringToWString(player->getNickName()));
	lblSpouse->SetText(StringToWString(player->getSpouse()));

	std::string strLevelCult = formatInt(player->GetLevel()) + "/" + formatInt(player->GetCultivation());
	lblLevel->SetText(strLevelCult);

	lblCash->SetText(formatInt(player->GetCash()));
	lblReputation->SetText(formatInt(player->GetReputation()));

	lblRank->SetText(player->GetRankText());
	lblGuild->SetText(StringToWString(player->getGuild()));
	lblGuildRank->SetText(StringToWString(player->getGuildTitle()));
}

void CMainUI::updatePlayerLevel() {
	//lblPlayerLevel->SetText(std::to_string(player->GetLevel()));
}

/*void CMainUI::updatePetLevel() {
	Pet *pPet = player->GetMarchingPet();
	if (pPet) {
		lblPetLevel->SetText(std::to_string(pPet->GetLevel()));
	}
	else lblPetLevel->SetText("");
}*/

void CMainUI::setMapCoordLabels(SDL_Point coord) {
	lblCoordX->SetText(std::to_string(coord.x));
	lblCoordY->SetText(std::to_string(coord.y));
}

void CMainUI::ShowTeamButtons() {
	HideTeamButtons();

	std::vector<CButton*> teamBtns;
	if (player->GetTeam()) {
		if (player->GetTeam()->GetLeader() == player) {
			teamBtns.push_back(btnTeamManage);
			teamBtns.push_back(btnTeamDisband);
		}
		else teamBtns.push_back(btnTeamLeave);
	}
	else {
		teamBtns.push_back(btnTeamCreate);
		teamBtns.push_back(btnTeamJoin);
	}

	if (teamBtns.size() > 1) {
		teamBtns[0]->SetX(432);
		teamBtns[0]->SetVisible(true);

		teamBtns[1]->SetX(516);
		teamBtns[1]->SetVisible(true);
	}
	else {
		teamBtns[0]->SetX(474);
		teamBtns[0]->SetVisible(true);
	}

	ShowingTeamButtons = true;
}

void CMainUI::HideTeamButtons() {
	btnTeamCreate->SetVisible(false);
	btnTeamJoin->SetVisible(false);
	btnTeamManage->SetVisible(false);
	btnTeamLeave->SetVisible(false);
	btnTeamDisband->SetVisible(false);
	ShowingTeamButtons = false;
}