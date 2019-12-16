#pragma once

#include "Window.h"

class Sprite;
class CField;
class CDropDown;
class CImageBox;

class CCharCreateForm : public CWindow {
public:
	CCharCreateForm();
	~CCharCreateForm();

	void handleEvent(SDL_Event& e);
	void step();

	void setCharacter(int charIndex);

	void btnClose_Click(SDL_Event& e);
	void btnOk_Click(SDL_Event& e);
	void btnBack_Click(SDL_Event& e);

	void ddStyle_OnChange(SDL_Event& e);
	void btnReset_Click(SDL_Event& e);

	void btnLifeDown_Click(SDL_Event& e);
	void btnLifeUp_Click(SDL_Event& e);
	void btnManaDown_Click(SDL_Event& e);
	void btnManaUp_Click(SDL_Event& e);
	void btnAttackDown_Click(SDL_Event& e);
	void btnAttackUp_Click(SDL_Event& e);
	void btnDefenceDown_Click(SDL_Event& e);
	void btnDefenceUp_Click(SDL_Event& e);
	void btnDexterityDown_Click(SDL_Event& e);
	void btnDexterityUp_Click(SDL_Event& e);

private:
	std::string name, nickname;
	int unspent;
	int life, mana, attack, defence, dexterity;

	CField* fldName, *fldNickname;
	CDropDown* ddStyle;
	CImageBox* imgCharacter;
	Sprite* sprite = nullptr;
	
	int style = 1;
	int look;
	char hsbSet[25];

	void setLabel(std::string name, int value);
};