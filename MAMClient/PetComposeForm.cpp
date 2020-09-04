#include "stdafx.h"
#include "PetComposeForm.h"
#include "CustomEvents.h"

#include "ListBox.h"
#include "ImageBox.h"
#include "Label.h"
#include "Field.h"
#include "Gauge.h"
#include "Sprite.h"

#include "Options.h"
#include "INI.h"

#include "Player.h"
#include "Pet.h"

#include "pPetAction.h"


CPetComposeForm::CPetComposeForm() : CWindow("PetComposeForm.JSON") {
	Type = FT_PETCOMPOSE;

	HookWidgets();
	loadListView();
	clearPreview();
}

void CPetComposeForm::handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			//doEvolve();
		}
	}
	CWindow::handleEvent(e);
}

void CPetComposeForm::HookWidgets() {
	imgSrcMain = (CImageBox*)GetWidget("imgSrcMain");
	imgSrcMinor = (CImageBox*)GetWidget("imgSrcMinor");
	imgDstMain = (CImageBox*)GetWidget("imgDstMain");
	imgDstMinor = (CImageBox*)GetWidget("imgDstMinor");
	imgAccessory = (CImageBox*)GetWidget("imgAccessory");
	registerEvent("imgSrcMain", "Click", std::bind(&CPetComposeForm::imgSrcMain_Click, this, std::placeholders::_1));
	registerEvent("imgSrcMain", "DoubleClick", std::bind(&CPetComposeForm::imgSrcMain_DoubleClick, this, std::placeholders::_1));
	registerEvent("imgSrcMinor", "Click", std::bind(&CPetComposeForm::imgSrcMinor_Click, this, std::placeholders::_1));
	registerEvent("imgSrcMinor", "DoubleClick", std::bind(&CPetComposeForm::imgSrcMinor_DoubleClick, this, std::placeholders::_1));
	registerEvent("imgDstMain", "Click", std::bind(&CPetComposeForm::imgDstMain_Click, this, std::placeholders::_1));
	registerEvent("imgDstMinor", "Click", std::bind(&CPetComposeForm::imgDstMinor_Click, this, std::placeholders::_1));
	registerEvent("imgAccessory", "MouseOver", std::bind(&CPetComposeForm::imgAccessory_MouseOver, this, std::placeholders::_1));

	btnSwitch = (CButton*)GetWidget("btnSwitch");
	btnOK = (CButton*)GetWidget("btnOK");
	btnCancel = (CButton*)GetWidget("btnCancel");
	registerEvent("btnSwitch", "Click", std::bind(&CPetComposeForm::btnSwitch_Click, this, std::placeholders::_1));
	registerEvent("btnOK", "Click", std::bind(&CPetComposeForm::btnOK_Click, this, std::placeholders::_1));
	registerEvent("btnCancel", "Click", std::bind(&CPetComposeForm::btnCancel_Click, this, std::placeholders::_1));

	listPets = (CListBox*)GetWidget("listPets");
	registerEvent("listPets", "SelectionDblClick", std::bind(&CPetComposeForm::listPets_SelectionDblClick, this, std::placeholders::_1));
	registerEvent("listPets", "MouseOver", std::bind(&CPetComposeForm::listPets_MouseOver, this, std::placeholders::_1));

	gaugePercMain = (CGauge*)GetWidget("gaugePercMain");
	gaugePercMinor = (CGauge*)GetWidget("gaugePercMinor");

	lblViewPreview = (CLabel*)GetWidget("lblViewPreview");
	lblPetName = (CLabel*)GetWidget("lblPetName");
	lblGeneration = (CLabel*)GetWidget("lblGeneration");
	lblPercMain = (CLabel*)GetWidget("lblPercMain");
	lblPercMinor = (CLabel*)GetWidget("lblPercMinor");

	fldLevel = (CField*)GetWidget("fldLevel");
	fldLife = (CField*)GetWidget("fldLife");
	fldLifeGrowth = (CField*)GetWidget("fldLifeGrowth");
	fldGrowth = (CField*)GetWidget("fldGrowth");
	fldAttack = (CField*)GetWidget("fldAttack");
	fldAttackRate = (CField*)GetWidget("fldAttackRate");
	fldDefense = (CField*)GetWidget("fldDefense");
	fldDefenseRate = (CField*)GetWidget("fldDefenseRate");
	fldDexterity = (CField*)GetWidget("fldDexterity");
	fldDexterityRate = (CField*)GetWidget("fldDexterityRate");
}

void CPetComposeForm::loadSprite(bool isMain, bool isSource) {
	Pet *pet = nullptr;
	CImageBox *ib = nullptr;

	if (isSource) {
		if (isMain) {
			pet = srcMain;
			ib = imgSrcMain;
		}
		else {
			pet = srcMinor;
			ib = imgSrcMinor;
		}
	}
	else {
		if (isMain) {
			pet = dstMain;
			ib = imgDstMain;
		}
		else {
			pet = dstMinor;
			ib = imgDstMinor;
		}
	}

	if (!pet) {
		if (ib) ib->ClearSprite();
		return;
	}

	//if (options.IsColorDisabled(look)) sprBefore = new Sprite(renderer, frames, stCharacter);
	//else sprBefore = new Sprite(renderer, frames, stCharacter, colorShifts);

	//Refactor for HSB color sets via ini

	Sprite* sprite = new Sprite(renderer, getSpriteFramesFromAni(getRoleFromLook(pet->GetLook()), StandBy, 0), stPet);
	sprite->start();
	ib->BindSprite(sprite);
}

void CPetComposeForm::loadListView() {
	pets.clear();
	listPets->ClearList();

	std::vector<Pet*> playerList = player->getPetList();
	Pet* marching = player->GetMarchingPet();
	for (auto pet : playerList) {
		if (pet != marching && pet != srcMain && pet != srcMinor && pet->GetLevel() >= 300) pets.push_back(pet);
	}
	for (auto pet : pets) {
		CImageBox *ib = createPetImageBox(pet);
		if (ib) listPets->AddItem(ib);
	}
}

CImageBox* CPetComposeForm::createPetImageBox(Pet* pet) {
	CImageBox *imgBox = new CImageBox(this, "petList_" + pet->GetName(), 0, 0);
	imgBox->SetWidth(listPets->GetListWidth());
	imgBox->SetHeight(78);
	imgBox->UseBlackBackground(false);

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, listPets->GetListWidth(), 78);
	if (!texture) return nullptr;

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	SDL_Rect rect = { 4, 4, 55, 69 };
	boxRGBA(renderer, 4, 4, 58, 72, 0, 0, 0, 255);

	std::string petPath = pet->getPortraitPath();
	if (petPath.length()) {
		Texture tPet(renderer, petPath, true);
		tPet.setBlendMode(SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(renderer, tPet.texture, NULL, &rect);
	}

	Texture* tName = stringToTexture(renderer, pet->GetName(), gui->font, 0, gui->fontColor, 0);
	if (tName) {
		tName->rect.x = 64;
		tName->rect.y = 32;
		SDL_RenderCopy(renderer, tName->texture, NULL, &tName->rect);
		delete tName;
	}

	SDL_SetRenderTarget(renderer, priorTarget);

	imgBox->SetImage(new Texture(renderer, texture, listPets->GetListWidth(), 78));
	return imgBox;
}

void CPetComposeForm::clearDetails() {
	lblViewPreview->SetText("Select a pet to view its details.");
	lblPetName->SetText(" ");
	lblGeneration->SetText("Generation 0");

	accessory = nullptr;
	imgAccessory->SetImage(nullptr);

	fldLevel->SetText(" ");
	fldLife->SetText(" ");
	fldLifeGrowth->SetText(" ");

	fldAttack->SetText(" ");
	fldAttackRate->SetText(" ");
	fldDefense->SetText(" ");
	fldDefenseRate->SetText(" ");
	fldDexterity->SetText(" ");
	fldDexterityRate->SetText(" ");
	fldGrowth->SetText(" ");

	//medals?
}

void CPetComposeForm::setDetails(Pet* pet) {
	clearDetails();
	detailsPet = pet;
	if (!detailsPet) return;

	if (detailsPet == srcMain) lblViewPreview->SetText("Viewing Main");
	else if (detailsPet == srcMinor) lblViewPreview->SetText("Viewing Minor");
	else if (detailsPet == dstMain) lblViewPreview->SetText("Previewing Main");
	else if (detailsPet == dstMinor) lblViewPreview->SetText("Previewing Minor");
	else lblViewPreview->SetText("Viewing Pet");

	lblPetName->SetText(detailsPet->GetName());
	lblGeneration->SetText("Generation " + std::to_string(detailsPet->GetGeneration()));

	accessory = nullptr;
	imgAccessory->SetImage(nullptr);

	fldLevel->SetText(std::to_string(detailsPet->GetLevel()));
	fldLife->SetText(std::to_string(detailsPet->GetLevel()));
	fldLifeGrowth->SetText(std::to_string(detailsPet->GetLevel()));

	fldAttack->SetText(std::to_string(detailsPet->GetLevel()));
	fldAttackRate->SetText(std::to_string(detailsPet->GetLevel()));
	fldDefense->SetText(std::to_string(detailsPet->GetLevel()));
	fldDefenseRate->SetText(std::to_string(detailsPet->GetLevel()));
	fldDexterity->SetText(std::to_string(detailsPet->GetLevel()));
	fldDexterityRate->SetText(std::to_string(detailsPet->GetLevel()));
	fldGrowth->SetText(std::to_string(detailsPet->GetLevel()));

	//medals?
}

void CPetComposeForm::doPreview() {

}

void CPetComposeForm::clearPreview() {
	if (dstMain) dstMain = nullptr;
	if (dstMinor) dstMinor = nullptr;

	imgDstMain->ClearSprite();
	lblPercMain->SetText("0%");
	gaugePercMain->set(0);

	imgDstMinor->ClearSprite();
	lblPercMinor->SetText("0%");
	gaugePercMinor->set(0);
}

void CPetComposeForm::doCompose() {
	/*pPetAction *msg = new pPetAction(pet->GetID(), 0, paEvolve);
	gClient.addPacket(msg);

	//Informs PetList that it should close itself
	customEvent(CUSTOMEVENT_WINDOW, WINDOW_EVOLVE)*/

	CloseWindow = true;
}

void CPetComposeForm::imgSrcMain_Click(SDL_Event& e) {

}

void CPetComposeForm::imgSrcMain_DoubleClick(SDL_Event& e) {
	if (!srcMain) return;

	srcMain = nullptr;
	imgSrcMain->ClearSprite();
	loadListView();
	clearPreview();
}

void CPetComposeForm::imgSrcMinor_Click(SDL_Event& e) {

}

void CPetComposeForm::imgSrcMinor_DoubleClick(SDL_Event& e) {
	if (!srcMinor) return;

	srcMinor = nullptr;
	imgSrcMinor->ClearSprite();
	loadListView();
	clearPreview();
}

void CPetComposeForm::imgDstMain_Click(SDL_Event& e) {

}

void CPetComposeForm::imgDstMinor_Click(SDL_Event& e) {

}

void CPetComposeForm::imgAccessory_MouseOver(SDL_Event& e) {

}

void CPetComposeForm::listPets_MouseOver(SDL_Event& e) {

}

void CPetComposeForm::listPets_SelectionClick(SDL_Event& e) {
}

void CPetComposeForm::listPets_SelectionDblClick(SDL_Event& e) {
	int idx = listPets->GetSelectedIndex();
	if (idx < 0 || idx >= pets.size()) return;

	Pet *pet = pets[idx];
	if (!pet) return;

	if (!pet->IsFullLife()) {
		doPromptError(this, "Alert", "Your pet is not at full life. Please heal your pet before attempting to compose.");
		return;
	}

	if (!srcMain) {
		srcMain = pet;
		loadSprite(true, true);
		loadListView();
	}
	else {
		if (!srcMinor) {
			srcMinor = pet;
			loadSprite(false, true);
			loadListView();
		}
	}

	if (srcMain && srcMinor) {
		doPreview();
	}
}

void CPetComposeForm::btnSwitch_Click(SDL_Event& e) {
	Pet* temp = srcMain;
	srcMain = srcMinor;
	srcMinor = temp;

	loadSprite(true, true);
	loadSprite(false, true);
	if (srcMain && srcMinor) {
		doPreview();
	}
}

void CPetComposeForm::btnOK_Click(SDL_Event& e) {
	/*if (pet->GetLevel() < 500) {
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
	}*/

	//doEvolve();
}

void CPetComposeForm::btnCancel_Click(SDL_Event& e) {
	CloseWindow = true;
}