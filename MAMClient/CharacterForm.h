#pragma once

#include "Window.h"

class CImageBox;
class Sprite;

class CCharacterForm : public CWindow {
public:
	CCharacterForm();

private:
	void HookWidgets();
	void LoadPortrait();
	void LoadSprite();
	void LoadOverviewTab();
	void LoadAttributeTab();
	void LoadAdditionalTab();

public: //Event Hooks
	//void btnOk_Click(SDL_Event& e);

private: //Widgets
	CImageBox *imgPortrait, *imgSprite;
	CLabel *lblName, *lblLevelRank, *lblMainCult, *lblMainLife, *lblMainAttack, *lblMainDefence, *lblMainDexterity;

	Sprite* sprite;
};