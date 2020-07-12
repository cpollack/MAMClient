#include "stdafx.h"
#include "EvolveForm.h"
#include "CustomEvents.h"

#include "ImageBox.h"
#include "Sprite.h"
#include "Options.h"
#include "INI.h"

#include "Pet.h"

#include "pPetAction.h"


CEvolveForm::CEvolveForm(Pet* pPet) : CWindow("EvolveForm.JSON") {
	Type = FT_INVENTORY;
	pet = pPet;

	SetTitle("Evolve " + pet->GetName());

	HookWidgets();
	loadSprites();
}

void CEvolveForm::handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			doEvolve();
		}
	}
	CWindow::handleEvent(e);
}

void CEvolveForm::HookWidgets() {
	imgBefore = (CImageBox*)GetWidget("imgBefore");
	imgAfter = (CImageBox*)GetWidget("imgAfter");

	btnOK = (CButton*)GetWidget("btnOK");
	btnCancel = (CButton*)GetWidget("btnCancel");
	registerEvent("btnOK", "Click", std::bind(&CEvolveForm::btnOK_Click, this, std::placeholders::_1));
	registerEvent("btnCancel", "Click", std::bind(&CEvolveForm::btnCancel_Click, this, std::placeholders::_1));
}

void CEvolveForm::loadSprites() {
	int look = pet->GetLook();
	std::string role = pet->getRole(look);

	//if (options.IsColorDisabled(look)) sprBefore = new Sprite(renderer, frames, stCharacter);
	//else sprBefore = new Sprite(renderer, frames, stCharacter, colorShifts);
	
	//Refactor for HSB color sets via ini
	sprBefore = new Sprite(renderer, getSpriteFramesFromAni(role, StandBy, 0), stCharacter);
	sprBefore->start();
	imgBefore->BindSprite(sprBefore);

	int petClass = pet->GetClass();
	int newClass = (petClass / 10000 * 10000) + ((petClass % 1000 + 1) * 1000) + (petClass % 1000);

	INI ini("ini/PetClass.ini", "Look");
	std::string strLook;
	if (ini.currentSection != -1) strLook = ini.getEntry(std::to_string(newClass));
	if (!strLook.length()) strLook = "0";
	role = pet->getRole(stoi(strLook));

	sprAfter = new Sprite(renderer, getSpriteFramesFromAni(role, StandBy, 0), stCharacter);
	sprAfter->start();
	imgAfter->BindSprite(sprAfter);
}

void CEvolveForm::doEvolve() {
	pPetAction *msg = new pPetAction(pet->GetID(), 0, paEvolve);
	gClient.addPacket(msg);

	//Informs PetList that it should close itself
	customEvent(CUSTOMEVENT_WINDOW, WINDOW_EVOLVE);

	CloseWindow = true;
}

void CEvolveForm::btnOK_Click(SDL_Event& e) {
	if (pet->GetLevel() < 500) {
		doPromptError(this, "Error", "Your pet must reach level 500 to evolve.");
		return;
	}
	if (pet->getLoyalty() < 100) {
		doPromptError(this, "Error", "Your pet is not loyal enough to evolve.");
		return;
	}

	if (pet->GetCurrentLife() < pet->GetMaxLife()) {
		doPrompt(this, "Warning", "Your pet is not at full life, do you wish to continue with evolution?", true);
		return;
	}

	doEvolve();
}

void CEvolveForm::btnCancel_Click(SDL_Event& e) {
	CloseWindow = true;
}