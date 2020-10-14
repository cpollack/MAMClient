#include "stdafx.h"
#include "CharCreateForm.h"
#include "MainWindow.h"
#include "PromptForm.h"

#include "GameLibrary.h"
#include "CustomEvents.h"
#include "INI.h"
#include "MessageManager.h"

#include "Label.h"
#include "Field.h"
#include "Dropdown.h"
#include "ImageBox.h"
#include "Sprite.h"

#include "pRole.h"

CCharCreateForm::CCharCreateForm() : CWindow("CharCreateForm.JSON") {
	Type = FT_CHARCREATE;
	registerEvent("btnClose", "Click", std::bind(&CCharCreateForm::btnClose_Click, this, std::placeholders::_1));

	fldName = (CField*)GetWidget("fldName");
	fldNickname = (CField*)GetWidget("fldNickname");
	imgCharacter = (CImageBox*)GetWidget("imgCharacter");

	ddStyle = (CDropDown*)GetWidget("ddStyle");
	if (ddStyle) {
		ddStyle->AddRow("Style 1");
		ddStyle->AddRow("Style 2");
		ddStyle->AddRow("Style 3");
	}
	ddStyle->SetValue(0);
	registerEvent("ddStyle", "Change", std::bind(&CCharCreateForm::ddStyle_Change, this, std::placeholders::_1));

	registerEvent("btnOK", "Click", std::bind(&CCharCreateForm::btnOk_Click, this, std::placeholders::_1));
	registerEvent("btnBack", "Click", std::bind(&CCharCreateForm::btnBack_Click, this, std::placeholders::_1));

	registerEvent("btnReset", "Click", std::bind(&CCharCreateForm::btnReset_Click, this, std::placeholders::_1));

	registerEvent("btnLifeDown", "Click", std::bind(&CCharCreateForm::btnLifeDown_Click, this, std::placeholders::_1));
	registerEvent("btnLifeUp", "Click", std::bind(&CCharCreateForm::btnLifeUp_Click, this, std::placeholders::_1));
	registerEvent("btnManaDown", "Click", std::bind(&CCharCreateForm::btnManaDown_Click, this, std::placeholders::_1));
	registerEvent("btnManaUp", "Click", std::bind(&CCharCreateForm::btnManaUp_Click, this, std::placeholders::_1));
	registerEvent("btnAttackDown", "Click", std::bind(&CCharCreateForm::btnAttackDown_Click, this, std::placeholders::_1));
	registerEvent("btnAttackUp", "Click", std::bind(&CCharCreateForm::btnAttackUp_Click, this, std::placeholders::_1));
	registerEvent("btnDefenceDown", "Click", std::bind(&CCharCreateForm::btnDefenceDown_Click, this, std::placeholders::_1));
	registerEvent("btnDefenceUp", "Click", std::bind(&CCharCreateForm::btnDefenceUp_Click, this, std::placeholders::_1));
	registerEvent("btnDexterityDown", "Click", std::bind(&CCharCreateForm::btnDexterityDown_Click, this, std::placeholders::_1));
	registerEvent("btnDexterityUp", "Click", std::bind(&CCharCreateForm::btnDexterityUp_Click, this, std::placeholders::_1));


	SDL_Event e;
	btnReset_Click(e);
}

CCharCreateForm::~CCharCreateForm() {
	delete sprite;
}

void CCharCreateForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);
	if (e.window.windowID != windowID) return;

	if (e.type == CUSTOMEVENT_WINDOW) {
		//Character Successfully Created
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			if (CUSTOMEVENT_WINDOW != ((Uint32)-1)) {
				SDL_Event event;
				SDL_zero(event);
				event.type = CUSTOMEVENT_WINDOW;
				event.window.windowID = mainForm->GetWindowID();
				event.user.code = WINDOW_CLOSE_OK;
				event.user.data1 = this;
				event.user.data2 = nullptr;
				SDL_PushEvent(&event);
			}
			CloseWindow = true;
		}
		if (e.user.code == WINDOW_CLOSE_PROMPT_ERROR) {
			//Stay in character creator
		}
	}
}

void CCharCreateForm::step() {
	Message message = messageManager.Poll();
	if (!message.get()) return;

	CPromptForm *prompt = doPrompt(this, "Character Creation", message->message);

	if (message->message.compare("Character has been created. Please log back in to begin.") != 0) prompt->SetType(WINDOW_CLOSE_PROMPT_ERROR);
}

void CCharCreateForm::setCharacter(int charIndex) {
	character = charIndex;
	std::string role = "Role" + std::to_string(charIndex);
	INI colorInfo("INI/ColorInfo.ini", role);

	int color = style;
	look = stoi(colorInfo.getEntry("Look" + std::to_string(color)));

	int h[5];
	std::string hue = colorInfo.getEntry("Hue" + std::to_string(color));
	sscanf_s(hue.c_str(), "%d %d %d %d %d", &h[0], &h[1], &h[2], &h[3], &h[4]);

	int n[5];
	std::string newhue = colorInfo.getEntry("NewHue" + std::to_string(color));
	sscanf_s(newhue.c_str(), "%d %d %d %d %d", &n[0], &n[1], &n[2], &n[3], &n[4]);

	int s[5];
	std::string sat = colorInfo.getEntry("Stau" + std::to_string(color));
	sscanf_s(sat.c_str(), "%d %d %d %d %d", &s[0], &s[1], &s[2], &s[3], &s[4]);

	int l[5];
	std::string light = colorInfo.getEntry("Light" + std::to_string(color));
	sscanf_s(light.c_str(), "%d %d %d %d %d", &l[0], &l[1], &l[2], &l[3], &l[4]);

	role = getRoleFromLook(look);
	int animation = Walk;
	int direction = 1;
	std::string animString = animationTypeToString(animation) + std::to_string(direction);
	sprite = new Sprite(renderer, getSpriteFramesFromAni(role, animation, direction), stCharacter);
	sprite->start();
	
	shifts.clear();
	for (int i = 0; i < 5; i++) {
		ColorShift shift = { h[i], n[i], 8, s[i], l[i] };
		shifts.push_back(shift);
	}
	if ((charIndex >= 6 && charIndex <= 7) || charIndex >= 13) sprite->setHsbShifts(shifts);

	imgCharacter->BindSprite(sprite);
}

void CCharCreateForm::btnClose_Click(SDL_Event& e) {
	//Force an application close
	applicationClose();
}

void CCharCreateForm::btnOk_Click(SDL_Event& e) {
	//validate
	if (fldName->GetText().length() == 0) {
		doPromptError(this, "Character Creation Error", "Name cannot be empty.");
		return;
	}
	if (fldNickname->GetText().length() == 0) {
		doPromptError(this, "Character Creation Error", "Nickname cannot be empty.");
		return;
	}

	if (unspent > 0) {
		doPromptError(this, "Character Creation Error", "Please allocate all unspent points.");
		return;
	}

	//submit
	int face = 1;
	char hsbSet[25];
	for (int i = 0; i < 5; i++) {
		hsbSet[0 + (i * 5)] = shifts[i].hue;
		hsbSet[1 + (i * 5)] = shifts[i].newHue;
		hsbSet[2 + (i * 5)] = shifts[i].range;
		hsbSet[3 + (i * 5)] = shifts[i].sat;
		hsbSet[4 + (i * 5)] = shifts[i].bright;
	}
	pRole* rolePacket = new pRole(look, face, 100001, life, mana, attack, defence, dexterity, (char*)fldName->GetText().c_str(), (char*)fldNickname->GetText().c_str(), hsbSet);
	gClient.addPacket(rolePacket);
}

void CCharCreateForm::btnBack_Click(SDL_Event& e) {
	CloseWindow = true;
	if (CUSTOMEVENT_WINDOW != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WINDOW;
		event.window.windowID = mainForm->GetWindowID();
		event.user.code = WINDOW_CLOSE_BACK;
		event.user.data1 = this;
		event.user.data2 = nullptr;
		SDL_PushEvent(&event);
	}
}

void CCharCreateForm::ddStyle_Change(SDL_Event& e) {
	style = ddStyle->GetSelectedRow();
	setCharacter(character);
}

void CCharCreateForm::btnReset_Click(SDL_Event& e) {
	setLabel("lblLifePoint", life = 0);
	setLabel("lblManaPoint", mana = 0);
	setLabel("lblAttackPoint", attack = 5);
	setLabel("lblDefencePoint", defence = 10);
	setLabel("lblDexterityPoint", dexterity = 5);
	setLabel("lblUnspent", unspent = 0);
}

void CCharCreateForm::btnLifeDown_Click(SDL_Event& e) {
	if (life == 0) return;
	life--;
	unspent++;
	setLabel("lblLifePoint", life);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::CCharCreateForm::btnLifeUp_Click(SDL_Event& e) {
	if (unspent == 0) return;
	life++;
	unspent--;
	setLabel("lblLifePoint", life);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnManaDown_Click(SDL_Event& e) {
	if (mana == 0) return;
	mana--;
	unspent++;
	setLabel("lblManaPoint", mana);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnManaUp_Click(SDL_Event& e) {
	if (unspent == 0) return;
	mana++;
	unspent--;
	setLabel("lblManaPoint", mana);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnAttackDown_Click(SDL_Event& e) {
	if (attack == 0) return;
	attack--;
	unspent++;
	setLabel("lblAttackPoint", attack);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnAttackUp_Click(SDL_Event& e) {
	if (unspent == 0) return;
	attack++;
	unspent--;
	setLabel("lblAttackPoint", attack);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnDefenceDown_Click(SDL_Event& e) {
	if (defence == 0) return;
	defence--;
	unspent++;
	setLabel("lblDefencePoint", defence);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnDefenceUp_Click(SDL_Event& e) {
	if (unspent == 0) return;
	defence++;
	unspent--;
	setLabel("lblDefencePoint", defence);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnDexterityDown_Click(SDL_Event& e) {
	if (dexterity == 0) return;
	dexterity--;
	unspent++;
	setLabel("lblDexterityPoint", dexterity);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::btnDexterityUp_Click(SDL_Event& e) {
	if (unspent == 0) return;
	dexterity++;
	unspent--;
	setLabel("lblDexterityPoint", dexterity);
	setLabel("lblUnspent", unspent);
}

void CCharCreateForm::setLabel(std::string name, int value) {
	CLabel* lbl = (CLabel*)GetWidget(name);
	if (lbl) {
		lbl->SetText(std::to_string(value));
	}
}