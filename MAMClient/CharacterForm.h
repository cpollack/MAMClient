#pragma once

#include "Window.h"

class CTabControl;
class CImageBox;
class CGauge;
class CField;
class Sprite;

class CCharacterForm : public CWindow {
public:
	CCharacterForm();

	void ReloadAssets();
	void handleEvent(SDL_Event& e);

private:
	void HookWidgets();
	void LoadPortrait();
	void LoadSprite();
	void LoadOverviewTab();
	
	void LoadAttributeTab();
	void ResetPoints();

	void LoadAdditionalTab();
	void CalcPointsRem();

private:
	int points, points_rem;
	int point_life, point_mana, point_attack, point_defence, point_dexterity;

public: //Event Hooks
	void tabMain_OnTabChange(SDL_Event& e);

	void btnAddLife_Click(SDL_Event& e);
	void btnSubLife_Click(SDL_Event& e);
	void fldPointLife_OnChange(SDL_Event& e);

	void btnAddMana_Click(SDL_Event& e);
	void btnSubMana_Click(SDL_Event& e);
	void fldPointMana_OnChange(SDL_Event& e);

	void btnAddAttack_Click(SDL_Event& e);
	void btnSubAttack_Click(SDL_Event& e);
	void fldPointAttack_OnChange(SDL_Event& e);

	void btnAddDefence_Click(SDL_Event& e);
	void btnSubDefence_Click(SDL_Event& e);
	void fldPointDefence_OnChange(SDL_Event& e);

	void btnAddDexterity_Click(SDL_Event& e);
	void btnSubDexterity_Click(SDL_Event& e);
	void fldPointDexterity_OnChange(SDL_Event& e);

	void btnReset_Click(SDL_Event& e);
	void btnApply_Click(SDL_Event& e);

	void btnUpdateNickname_Click(SDL_Event& e);

private: //Widgets
	CTabControl *tabMain;
	CImageBox *imgPortrait;

	//Overview tab
	CImageBox *imgSprite;
	CLabel *lblName, *lblLevelRank, *lblMainCult, *lblMainLife, *lblMainMana, *lblMainAttack, *lblMainDefence, *lblMainDexterity;
	Sprite* sprite;

	//Attribute tab
	CLabel *lblLevel, *lblAttack, *lblDefence, *lblDexterity;
	CGauge *gaugeExp, *gaugeLife, *gaugeMana;
	CField *fldLife, *fldMana, *fldAttack, *fldDefence, *fldDexterity;
	CField *fldPointLife, *fldPointMana, *fldPointAttack, *fldPointDefence, *fldPointDexterity, *fldPoints;

	CButton *btnAddLife, *btnSubLife, *btnAddMana, *btnSubMana, *btnAddAttack, *btnSubAttack, *btnAddDefence, *btnSubDefence, *btnAddDexterity, *btnSubDexterity;
	CButton *btnReset, *btnApply;

	//Additional tab
	CField *fldNickname;
	CButton *btnUpdateNickname;
	CLabel *lblGuild, *lblGuildRank, *lblReputation, *lblVirtue;
	CLabel *lblThievery, *lblThieveryTitle, *lblKungfu, *lblKungfuTitle, *lblWuxing, *lblWuxingTitle, *lblPetRaising, *lblPetRaisingTitle;
};