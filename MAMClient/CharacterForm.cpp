#include "stdafx.h"
#include "CharacterForm.h"
#include "GameLibrary.h"
#include "CustomEvents.h"

#include "PromptForm.h"

#include "Client.h"
#include "Player.h"
#include "pUserPoint.h"
#include "pRename.h"

#include "Texture.h"
#include "Sprite.h"
#include "TabControl.h"
#include "Label.h"
#include "ImageBox.h"
#include "Field.h"
#include "Gauge.h"

CCharacterForm::CCharacterForm() : CWindow("CharacterForm.JSON") {
	Type = FT_CHARACTER;
	SetParentFromStack();

	HookWidgets();
	LoadPortrait();
	LoadSprite();
	LoadOverviewTab();

	registerEvent("tabMain", "OnTabChange", std::bind(&CCharacterForm::tabMain_OnTabChange, this, std::placeholders::_1));

	registerEvent("btnAddLife", "Click", std::bind(&CCharacterForm::btnAddLife_Click, this, std::placeholders::_1));
	registerEvent("btnSubLife", "Click", std::bind(&CCharacterForm::btnSubLife_Click, this, std::placeholders::_1));
	registerEvent("fldPointLife", "OnChange", std::bind(&CCharacterForm::fldPointLife_OnChange, this, std::placeholders::_1));
	registerEvent("btnAddMana", "Click", std::bind(&CCharacterForm::btnAddMana_Click, this, std::placeholders::_1));
	registerEvent("btnSubMana", "Click", std::bind(&CCharacterForm::btnSubMana_Click, this, std::placeholders::_1));
	registerEvent("fldPointMana", "OnChange", std::bind(&CCharacterForm::fldPointMana_OnChange, this, std::placeholders::_1));
	registerEvent("btnAddAttack", "Click", std::bind(&CCharacterForm::btnAddAttack_Click, this, std::placeholders::_1));
	registerEvent("btnSubAttack", "Click", std::bind(&CCharacterForm::btnSubAttack_Click, this, std::placeholders::_1));
	registerEvent("fldPointAttack", "OnChange", std::bind(&CCharacterForm::fldPointAttack_OnChange, this, std::placeholders::_1));
	registerEvent("btnAddDefence", "Click", std::bind(&CCharacterForm::btnAddDefence_Click, this, std::placeholders::_1));
	registerEvent("btnSubDefence", "Click", std::bind(&CCharacterForm::btnSubDefence_Click, this, std::placeholders::_1));
	registerEvent("fldPointDefence", "OnChange", std::bind(&CCharacterForm::fldPointDefence_OnChange, this, std::placeholders::_1));
	registerEvent("btnAddDexterity", "Click", std::bind(&CCharacterForm::btnAddDexterity_Click, this, std::placeholders::_1));
	registerEvent("btnSubDexterity", "Click", std::bind(&CCharacterForm::btnSubDexterity_Click, this, std::placeholders::_1));
	registerEvent("fldPointDexterity", "OnChange", std::bind(&CCharacterForm::fldPointDexterity_OnChange, this, std::placeholders::_1));
	registerEvent("btnReset", "Click", std::bind(&CCharacterForm::btnReset_Click, this, std::placeholders::_1));
	registerEvent("btnApply", "Click", std::bind(&CCharacterForm::btnApply_Click, this, std::placeholders::_1));

	registerEvent("btnUpdateNickname", "Click", std::bind(&CCharacterForm::btnUpdateNickname_Click, this, std::placeholders::_1));
}

void CCharacterForm::ReloadAssets() {
	CWindow::ReloadAssets();
	LoadPortrait();
}

void CCharacterForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	//Events not bound by a window ID
	if (e.type == CUSTOMEVENT_PLAYER) {
		if (e.user.code == PLAYER_RENAME) {
			//Server rename response confirmation received
			LoadAdditionalTab();
			doPrompt(this, "Confirm", "Nickname has been updated successfully!");
		}
	}

	if (e.window.windowID != windowID) return;
}

void CCharacterForm::HookWidgets() {
	tabMain = (CTabControl*)GetWidget("tabMain");
	imgPortrait = (CImageBox*)GetWidget("imgPortrait");

	//Overview Tab
	imgSprite = (CImageBox*)GetWidget("imgSprite");
	lblName = (CLabel*)GetWidget("lblName");
	lblLevelOverview = (CLabel*)GetWidget("lblLevelOverview");
	lblRank = (CLabel*)GetWidget("lblRank");
	lblMainCult = (CLabel*)GetWidget("lblMainCult");
	lblMainLife = (CLabel*)GetWidget("lblMainLife");
	lblMainMana = (CLabel*)GetWidget("lblMainMana");
	lblMainAttack = (CLabel*)GetWidget("lblMainAttack");
	lblMainDefence = (CLabel*)GetWidget("lblMainDefence");
	lblMainDexterity = (CLabel*)GetWidget("lblMainDexterity");

	//Attributes Tab
	lblLevel = (CLabel*)GetWidget("lblLevel");
	lblAttack = (CLabel*)GetWidget("lblAttack");
	lblDefence = (CLabel*)GetWidget("lblDefence");
	lblDexterity = (CLabel*)GetWidget("lblDexterity");

	gaugeExp = (CGauge*)GetWidget("gaugeExp");
	gaugeLife = (CGauge*)GetWidget("gaugeLife");
	gaugeMana = (CGauge*)GetWidget("gaugeMana");

	fldLife = (CField*)GetWidget("fldLife");
	fldMana = (CField*)GetWidget("fldMana");
	fldAttack = (CField*)GetWidget("fldAttack");
	fldDefence = (CField*)GetWidget("fldDefence");
	fldDexterity = (CField*)GetWidget("fldDexterity");
	fldPointLife = (CField*)GetWidget("fldPointLife");
	fldPointMana = (CField*)GetWidget("fldPointMana");
	fldPointAttack = (CField*)GetWidget("fldPointAttack");
	fldPointDefence = (CField*)GetWidget("fldPointDefence");
	fldPointDexterity = (CField*)GetWidget("fldPointDexterity");
	fldPoints = (CField*)GetWidget("fldPoints");
	
	btnAddLife = (CButton*)GetWidget("btnAddLife");
	btnSubLife = (CButton*)GetWidget("btnSubLife");
	btnAddMana = (CButton*)GetWidget("btnAddMana");
	btnSubMana = (CButton*)GetWidget("btnSubMana");
	btnAddAttack = (CButton*)GetWidget("btnAddAttack");
	btnSubAttack = (CButton*)GetWidget("btnSubAttack");
	btnAddDefence = (CButton*)GetWidget("btnAddDefence");
	btnSubDefence = (CButton*)GetWidget("btnSubDefence");
	btnAddDexterity = (CButton*)GetWidget("btnAddDexterity");
	btnSubDexterity = (CButton*)GetWidget("btnSubDexterity");
	btnReset = (CButton*)GetWidget("btnReset");
	btnApply = (CButton*)GetWidget("btnApply");

	//Additional Tab
	fldNickname = (CField*)GetWidget("fldNickname");
	btnUpdateNickname = (CButton*)GetWidget("btnUpdateNickname");

	lblGuild = (CLabel*)GetWidget("lblGuild");
	lblGuildRank = (CLabel*)GetWidget("lblGuildRank");
	lblReputation = (CLabel*)GetWidget("lblReputation");
	lblVirtue = (CLabel*)GetWidget("lblVirtue");

	lblThievery = (CLabel*)GetWidget("lblThievery");
	lblThieveryTitle = (CLabel*)GetWidget("lblThieveryTitle");
	lblKungfu = (CLabel*)GetWidget("lblKungfu");
	lblKungfuTitle = (CLabel*)GetWidget("lblKungfuTitle");
	lblWuxing = (CLabel*)GetWidget("lblWuxing");
	lblWuxingTitle = (CLabel*)GetWidget("lblWuxingTitle");
	lblPetRaising = (CLabel*)GetWidget("lblPetRaising");
	lblPetRaisingTitle = (CLabel*)GetWidget("lblPetRaisingTitle");
}

void CCharacterForm::LoadPortrait() {
	int face = player->GetFace();
	std::string path = "GUI\\player_icon\\";
	if (player->GetGender() == MALE) path += "Man0";
	else path += "Woman0";
	path += std::to_string(face) + ".bmp";
	Texture *portrait = new Texture(renderer, path);
	imgPortrait->SetImage(portrait);
}

void CCharacterForm::tabMain_OnTabChange(SDL_Event& e) {
	switch (tabMain->GetVisibleTab()) {
		case 0:
			LoadOverviewTab();
			break;
		case 1:
			LoadAttributeTab();
			break;
		case 2:
			LoadAdditionalTab();
			break;
	}
}

void CCharacterForm::LoadSprite() {
	std::string role = getRoleFromLook(player->GetLook());
	int animation = StandBy;
	int direction = 0;
	sprite = new Sprite(renderer, getSpriteFramesFromAni(role, animation, direction), stCharacter);
	sprite->start();

	imgSprite->BindSprite(sprite);
}

void CCharacterForm::LoadOverviewTab() {
	lblName->SetText(player->GetName());
	std::string sLevel = "Level " + formatInt(player->GetLevel());
	lblLevelOverview->SetText(sLevel);
	lblRank->SetText(player->GetRankText());
	lblMainCult->SetText(formatInt(player->GetCultivation()));

	lblMainLife->SetText(formatInt(player->GetMaxLife()));
	lblMainMana->SetText(formatInt(player->GetMaxMana()));
	lblMainAttack->SetText(formatInt(player->GetAttack()));
	lblMainDefence->SetText(formatInt(player->GetDefence()));
	lblMainDexterity->SetText(formatInt(player->GetDexterity()));
}

void CCharacterForm::LoadAttributeTab() {
	lblLevel->SetText(formatInt(player->GetLevel()));
	gaugeExp->set(player->GetExperience(), player->GetLevelUpExperience());

	gaugeLife->set(player->GetCurrentLife(), player->GetMaxLife());
	gaugeMana->set(player->GetCurrentMana(), player->GetMaxMana());

	lblAttack->SetText(formatInt(player->GetAttack()));
	lblDefence->SetText(formatInt(player->GetDefence()));
	lblDexterity->SetText(formatInt(player->GetDexterity()));

	ResetPoints();
}

void CCharacterForm::LoadAdditionalTab() {
	fldNickname->SetText(player->getNickName());

	lblGuild->SetText(player->getGuild());
	lblGuildRank->SetText(player->getGuildTitle());
	lblReputation->SetText(formatInt(player->GetReputation()));
	lblVirtue->SetText(formatInt(player->GetVirtue()));

	lblThievery->SetText(formatInt(player->GetThievery()));
	lblThieveryTitle->SetText(player->GetThieveryTitle());
	lblKungfu->SetText(formatInt(player->GetKungfu()));
	lblKungfuTitle->SetText(player->GetKungfuTitle());
	lblWuxing->SetText(formatInt(player->GetWuxing()));
	lblWuxingTitle->SetText(player->GetWuxingTitle());
	lblPetRaising->SetText(formatInt(player->GetPetRaising()));
	lblPetRaisingTitle->SetText(player->GetPetRaisingTitle());
}

void CCharacterForm::ResetPoints() {
	points = player->GetUnusedPoints();
	points_rem = points;
	point_life = 0;
	point_mana = 0;
	point_attack = 0;
	point_defence = 0;
	point_dexterity = 0;

	fldPoints->SetText(std::to_string(points_rem));
	fldPointLife->SetText(std::to_string(point_life));
	fldPointMana->SetText(std::to_string(point_mana));
	fldPointAttack->SetText(std::to_string(point_attack));
	fldPointDefence->SetText(std::to_string(point_defence));
	fldPointDexterity->SetText(std::to_string(point_dexterity));

	fldLife->SetText(std::to_string(player->GetLifePoint()));
	fldMana->SetText(std::to_string(player->GetManaPoint()));
	fldAttack->SetText(std::to_string(player->GetAttackPoint()));
	fldDefence->SetText(std::to_string(player->GetDefencePoint()));
	fldDexterity->SetText(std::to_string(player->GetDexterityPoint()));
}

void CCharacterForm::CalcPointsRem() {
	points_rem = points - point_life - point_mana - point_attack - point_defence - point_dexterity;
	fldPoints->SetText(std::to_string(points_rem));
}

//Life
void CCharacterForm::btnAddLife_Click(SDL_Event& e) {
	if (points_rem <= 0) return;
	fldPoints->SetText(std::to_string(--points_rem));
	fldPointLife->SetText(std::to_string(++point_life));
}

void CCharacterForm::btnSubLife_Click(SDL_Event& e) {
	if (points_rem >= points) return;
	if (point_life <= 0) return;
	fldPoints->SetText(std::to_string(++points_rem));
	fldPointLife->SetText(std::to_string(--point_life));
}

void CCharacterForm::fldPointLife_OnChange(SDL_Event& e) {
	int newPoint = stoi(fldPointLife->GetText());
	if (newPoint < 0) newPoint = 0;

	int dif = stoi(fldPointLife->GetText()) - point_life;
	if (dif > 0 && dif > points_rem) {
		newPoint = points_rem;
	}
	point_life = newPoint;
	fldPointLife->SetText(std::to_string(newPoint));
	CalcPointsRem();
}

//Mana
void CCharacterForm::btnAddMana_Click(SDL_Event& e) {
	if (points_rem <= 0) return;
	fldPoints->SetText(std::to_string(--points_rem));
	fldPointMana->SetText(std::to_string(++point_mana));
}

void CCharacterForm::btnSubMana_Click(SDL_Event& e) {
	if (points_rem >= points) return;
	if (point_mana <= 0) return;
	fldPoints->SetText(std::to_string(++points_rem));
	fldPointMana->SetText(std::to_string(--point_mana));
}

void CCharacterForm::fldPointMana_OnChange(SDL_Event& e) {
	int newPoint = stoi(fldPointMana->GetText());
	if (newPoint < 0) newPoint = 0;

	int dif = stoi(fldPointMana->GetText()) - point_mana;
	if (dif > 0 && dif > points_rem) {
		newPoint = points_rem;
	}
	point_mana = newPoint;
	fldPointMana->SetText(std::to_string(newPoint));
	CalcPointsRem();
}

//Attack
void CCharacterForm::btnAddAttack_Click(SDL_Event& e) {
	if (points_rem <= 0) return;
	fldPoints->SetText(std::to_string(--points_rem));
	fldPointAttack->SetText(std::to_string(++point_attack));
}

void CCharacterForm::btnSubAttack_Click(SDL_Event& e) {
	if (points_rem >= points) return;
	if (point_attack <= 0) return;
	fldPoints->SetText(std::to_string(++points_rem));
	fldPointAttack->SetText(std::to_string(--point_attack));
}

void CCharacterForm::fldPointAttack_OnChange(SDL_Event& e) {
	int newPoint = stoi(fldPointAttack->GetText());
	if (newPoint < 0) newPoint = 0;

	int dif = stoi(fldPointAttack->GetText()) - point_attack;
	if (dif > 0 && dif > points_rem) {
		newPoint = points_rem;
	}
	point_attack = newPoint;
	fldPointAttack->SetText(std::to_string(newPoint));
	CalcPointsRem();
}

//Defence
void CCharacterForm::btnAddDefence_Click(SDL_Event& e) {
	if (points_rem <= 0) return;
	fldPoints->SetText(std::to_string(--points_rem));
	fldPointDefence->SetText(std::to_string(++point_defence));
}

void CCharacterForm::btnSubDefence_Click(SDL_Event& e) {
	if (points_rem >= points) return;
	if (point_defence <= 0) return;
	fldPoints->SetText(std::to_string(++points_rem));
	fldPointDefence->SetText(std::to_string(--point_defence));
}

void CCharacterForm::fldPointDefence_OnChange(SDL_Event& e) {
	int newPoint = stoi(fldPointDefence->GetText());
	if (newPoint < 0) newPoint = 0;

	int dif = stoi(fldPointDefence->GetText()) - point_defence;
	if (dif > 0 && dif > points_rem) {
		newPoint = points_rem;
	}
	point_defence = newPoint;
	fldPointDefence->SetText(std::to_string(newPoint));
	CalcPointsRem();
}

//Dexterity
void CCharacterForm::btnAddDexterity_Click(SDL_Event& e) {
	if (points_rem <= 0) return; 
	fldPoints->SetText(std::to_string(--points_rem));
	fldPointDexterity->SetText(std::to_string(++point_dexterity));
}

void CCharacterForm::btnSubDexterity_Click(SDL_Event& e) {
	if (points_rem >= points) return;
	if (point_dexterity <= 0) return;
	fldPoints->SetText(std::to_string(++points_rem));
	fldPointDexterity->SetText(std::to_string(--point_dexterity));
}

void CCharacterForm::fldPointDexterity_OnChange(SDL_Event& e) {
	int newPoint = stoi(fldPointDexterity->GetText());
	if (newPoint < 0) newPoint = 0;

	int dif = stoi(fldPointDexterity->GetText()) - point_dexterity;
	if (dif > 0 && dif > points_rem) {
		newPoint = points_rem;
	}
	point_dexterity = newPoint;
	fldPointDexterity->SetText(std::to_string(newPoint));
	CalcPointsRem();
}

void CCharacterForm::btnReset_Click(SDL_Event& e) {
	ResetPoints();
}

void CCharacterForm::btnApply_Click(SDL_Event& e) {
	//Validate
	if (points_rem < 0) {
		doPromptError(this, "Error", "You cannot add more attribute points then you have available.");
		return;
	}
	if (points_rem > points) {
		doPromptError(this, "Error", "You cannot reallocate attribute points.");
		return;
	}

	//Send attribute update packet
	pUserPoint *pPoint = new pUserPoint(point_life, point_mana, point_attack, point_defence, point_dexterity);
	gClient.addPacket(pPoint);

	//Update player data
	player->SetLifePoint(player->GetLifePoint() + point_life);
	player->SetManaPoint(player->GetManaPoint() + point_mana);
	player->SetAttackPoint(player->GetAttackPoint() + point_attack);
	player->SetDefencePoint(player->GetDefencePoint() + point_defence);
	player->SetDexterityPoint(player->GetDexterityPoint() + point_dexterity);
	player->SetUnusedPoints(points_rem);

	//Reload Widget data
	LoadAttributeTab();
}

void CCharacterForm::btnUpdateNickname_Click(SDL_Event& e) {
	std::string nickname = fldNickname->GetText();
	if (nickname.length() == 0) {
		doPromptError(this, "Error", "Nickname cannot be blank.");
		return;
	}
	if (nickname.length() > 16) {
		doPromptError(this, "Error", "Nickname cannot be more than 16 characters.");
		return;
	}

	//Nickname has changed
	if (nickname.compare(player->getNickName()) == 0) return;

	//send nickname update
	pRename *rename = new pRename(player->GetID(), rmNickname, nickname);
	gClient.addPacket(rename);
}
