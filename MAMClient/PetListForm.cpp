#include "stdafx.h"
#include "PetListForm.h"
#include "GameLibrary.h"
#include "CustomEvents.h"

#include "PromptForm.h"

#include "Client.h"
#include "Player.h"
#include "Pet.h"
#include "INI.h"

#include "pRename.h"

#include "Texture.h"
#include "Sprite.h"
#include "Label.h"
#include "ImageBox.h"
#include "Field.h"
#include "Gauge.h"

CPetListForm::CPetListForm() : CWindow("PetListForm.JSON") {
	Type = FT_PET;

	HookWidgets();
	LoadPortraits();

	selection = GetMarchingPetIndex();
	SetMarching(selection);
	LoadPet(selection);

	registerEvent("btnChangeName", "Click", std::bind(&CPetListForm::btnChangeName_Click, this, std::placeholders::_1));
	registerEvent("btnEvolve", "Click", std::bind(&CPetListForm::btnEvolve_Click, this, std::placeholders::_1));
	registerEvent("btnSkills", "Click", std::bind(&CPetListForm::btnSkills_Click, this, std::placeholders::_1));
	registerEvent("btnRegister", "Click", std::bind(&CPetListForm::btnRegister_Click, this, std::placeholders::_1));
	registerEvent("btnCatalog", "Click", std::bind(&CPetListForm::btnCatalog_Click, this, std::placeholders::_1));
	registerEvent("btnEvaluate", "Click", std::bind(&CPetListForm::btnEvaluate_Click, this, std::placeholders::_1));
	registerEvent("btnPetStall", "Click", std::bind(&CPetListForm::btnPetStall_Click, this, std::placeholders::_1));
	registerEvent("btnDrop", "Click", std::bind(&CPetListForm::btnDrop_Click, this, std::placeholders::_1));
	registerEvent("btnMarch", "Click", std::bind(&CPetListForm::btnMarch_Click, this, std::placeholders::_1));
	registerEvent("btnChangeAccessory", "Click", std::bind(&CPetListForm::btnChangeAccessory_Click, this, std::placeholders::_1));
}

void CPetListForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.window.windowID != windowID) return;
}

void CPetListForm::HookWidgets() {
	imgStar = (CImageBox*)GetWidget("imgStar");
	imgPet1 = (CImageBox*)GetWidget("imgPet1");
	imgPet2 = (CImageBox*)GetWidget("imgPet2");
	imgPet3 = (CImageBox*)GetWidget("imgPet3");
	imgPet4 = (CImageBox*)GetWidget("imgPet4");
	imgPet5 = (CImageBox*)GetWidget("imgPet5");

	lblPetName1 = (CLabel*)GetWidget("lblPetName1");
	lblPetName2 = (CLabel*)GetWidget("lblPetName2");
	lblPetName3 = (CLabel*)GetWidget("lblPetName3");
	lblPetName4 = (CLabel*)GetWidget("lblPetName4");
	lblPetName5 = (CLabel*)GetWidget("lblPetName5");

	imgSprite = (CImageBox*)GetWidget("imgSprite");
	imgBraveMedals = (CImageBox*)GetWidget("imgBraveMedals");
	imgCalmnessMedals = (CImageBox*)GetWidget("imgCalmnessMedals");
	imgSpeedMedals = (CImageBox*)GetWidget("imgSpeedMedals");

	fldName = (CField*)GetWidget("fldName");
	fldID = (CField*)GetWidget("fldID");
	fldAttack = (CField*)GetWidget("fldAttack");
	fldDefence = (CField*)GetWidget("fldDefence");
	fldDexterity = (CField*)GetWidget("fldDexterity");

	gaugeLoyalty = (CGauge*)GetWidget("gaugeLoyalty");
	gaugeExperience = (CGauge*)GetWidget("gaugeExperience");
	gaugeLife = (CGauge*)GetWidget("gaugeLife");

	lblSpecies = (CLabel*)GetWidget("lblSpecies");
	lblElement = (CLabel*)GetWidget("lblElement");
	lblRegistered = (CLabel*)GetWidget("lblRegistered");
	lblLevel = (CLabel*)GetWidget("lblLevel");
	lblGeneration = (CLabel*)GetWidget("lblGeneration");
	lblAccessory = (CLabel*)GetWidget("lblAccessory");

	btnChangeName = (CButton*)GetWidget("btnChangeName");
	btnEvolve = (CButton*)GetWidget("btnEvolve");
	btnSkills = (CButton*)GetWidget("btnSkills");
	btnRegister = (CButton*)GetWidget("btnRegister");
	btnCatalog = (CButton*)GetWidget("btnCatalog");

	btnEvaluate = (CButton*)GetWidget("btnEvaluate");
	btnPetStall = (CButton*)GetWidget("btnPetStall");
	btnDrop = (CButton*)GetWidget("btnDrop");
	btnMarch = (CButton*)GetWidget("btnMarch");
	btnChangeAccessory = (CButton*)GetWidget("btnChangeAccessory");
}

void CPetListForm::LoadPortraits() {
	std::vector<Pet*> pets = player->getPetList();

	for (int i = 0; i < 5; i++) {
		Pet* pet = nullptr;
		if (pets.size() > i) pet = pets[i];

		Texture* portrait = nullptr;
		std::string petName;
		if (pet) {
			std::string look = std::to_string(pet->getLook());
			while (look.size() < 4) look.insert(look.begin(), '0');
			look = "peticon" + look;

			INI ini("INI\\petLook.ini", look);
			std::string iconPath = ini.getEntry("Frame0");

			portrait = new Texture(renderer, iconPath);
			petName = pet->getName();
		}

		switch (i) {
		case 0:
			if (pet) {
				imgPet1->SetVisible(true);
				lblPetName1->SetVisible(true);
				imgPet1->SetImage(portrait);
				lblPetName1->SetText(petName);
			}
			else {
				imgPet1->SetVisible(false);
				lblPetName1->SetVisible(false);
			}
			break;
		case 1:
			if (pet) {
				imgPet2->SetVisible(true);
				lblPetName2->SetVisible(true);
				imgPet2->SetImage(portrait);
				lblPetName2->SetText(petName);
			}
			else {
				imgPet2->SetVisible(false);
				lblPetName2->SetVisible(false);
			}
			break;
		case 2:
			if (pet) {
				imgPet3->SetVisible(true);
				lblPetName3->SetVisible(true);
				imgPet3->SetImage(portrait);
				lblPetName3->SetText(petName);
			}
			else {
				imgPet3->SetVisible(false);
				lblPetName3->SetVisible(false);
			}
			break;
		case 3:
			if (pet) {
				imgPet4->SetVisible(true);
				lblPetName4->SetVisible(true);
				imgPet4->SetImage(portrait);
				lblPetName4->SetText(petName);
			}
			else {
				imgPet4->SetVisible(false);
				lblPetName4->SetVisible(false);
			}
			break;
		case 4:
			if (pet) {
				imgPet5->SetVisible(true);
				lblPetName5->SetVisible(true);
				imgPet5->SetImage(portrait);
				lblPetName5->SetText(petName);
			}
			else {
				imgPet5->SetVisible(false);
				lblPetName5->SetVisible(false);
			}
			break;
		}
	}
}

void CPetListForm::LoadSprite() {
	if (selection == -1) return;
	Pet *pet = player->getPetList()[selection];

	std::string role = getRoleFromLook(pet->getLook());
	int animation = Walk;
	int direction = 1;
	std::string animString = animationTypeToString(animation) + std::to_string(direction);
	Sprite* sprite = new Sprite(renderer, getSpriteFramesFromAni(role, animation, direction), stMonster);
	sprite->start();
	imgSprite->BindSprite(sprite);
}

void CPetListForm::LoadPet(int index) {
	LoadSprite();

	Pet *pet = player->getPetList()[index];
	fldName->SetText(pet->getName());
	lblSpecies->SetText("N/A");
	lblElement->SetText(pet->GetElementText());
	lblRegistered->SetText("");
	fldID->SetText(std::to_string(pet->getId()));

	int loyalty = pet->getLoyalty();
	if (loyalty > 100) loyalty = 100;
	gaugeLoyalty->set(loyalty, 100);

	lblLevel->SetText(formatInt(pet->getLevel()));
	gaugeExperience->set(pet->getExperience(), pet->getLevelUpExperience());
	lblGeneration->SetText("0");
	gaugeLife->set(pet->getCurrentHealth(), pet->getMaxHealth());
	lblAccessory->SetText("");

	//last 0 is placeholder for petitem
	std::string attack = formatInt(pet->getAttack()) + "(+" + std::to_string(pet->getMedalAttack() * 5) + "% + " + formatInt(0) + ")";
	std::string defence = formatInt(pet->getDefence()) + "(+" + std::to_string(pet->getMedalDefence() * 5) + "% + " + formatInt(0) + ")";
	std::string dexterity = formatInt(pet->getDexterity()) + "(+" + std::to_string(pet->getMedalDexterity() * 5) + "% + " + formatInt(0) + ")";
	fldAttack->SetText(attack);
	fldDefence->SetText(defence);
	fldDexterity->SetText(dexterity);

	//medals?
}

void CPetListForm::btnChangeName_Click(SDL_Event& e) {

}

void CPetListForm::btnEvolve_Click(SDL_Event& e) {

}

void CPetListForm::btnSkills_Click(SDL_Event& e) {

}

void CPetListForm::btnRegister_Click(SDL_Event& e) {

}

void CPetListForm::btnCatalog_Click(SDL_Event& e) {

}

void CPetListForm::btnEvaluate_Click(SDL_Event& e) {

}

void CPetListForm::btnPetStall_Click(SDL_Event& e) {

}

void CPetListForm::btnDrop_Click(SDL_Event& e) {

}

void CPetListForm::btnMarch_Click(SDL_Event& e) {

}

void CPetListForm::btnChangeAccessory_Click(SDL_Event& e) {

}

int CPetListForm::GetMarchingPetIndex() {
	int index = -1;
	std::vector<Pet*> pets = player->getPetList();
	Pet* pet = player->getActivePet();
	for (int i = 0; i < pets.size(); i++) {
		if (pet == pets[i]) {
			index = i;
			break;
		}
	}
	return index;
}

void CPetListForm::SetMarching(int index) {
	imgStar->SetX(20 + (100 * index - 1));
}