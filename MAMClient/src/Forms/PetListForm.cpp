#include "stdafx.h"
#include "PetListForm.h"
#include "GameLibrary.h"
#include "CustomEvents.h"

#include "PromptForm.h"
#include "EvolveForm.h"
#include "PetComposeForm.h"
#include "PetEquipForm.h"

#include "Client.h"
#include "Player.h"
#include "Pet.h"
#include "Item.h"
#include "INI.h"

#include "pRename.h"
#include "pPetAction.h"

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

	registerEvent("imgPet1", "Click", std::bind(&CPetListForm::imgPet1_Click, this, std::placeholders::_1));
	registerEvent("imgPet2", "Click", std::bind(&CPetListForm::imgPet2_Click, this, std::placeholders::_1));
	registerEvent("imgPet3", "Click", std::bind(&CPetListForm::imgPet3_Click, this, std::placeholders::_1));
	registerEvent("imgPet4", "Click", std::bind(&CPetListForm::imgPet4_Click, this, std::placeholders::_1));
	registerEvent("imgPet5", "Click", std::bind(&CPetListForm::imgPet5_Click, this, std::placeholders::_1));

	registerEvent("btnChangeName", "Click", std::bind(&CPetListForm::btnChangeName_Click, this, std::placeholders::_1));
	registerEvent("btnEvolve", "Click", std::bind(&CPetListForm::btnEvolve_Click, this, std::placeholders::_1));
	registerEvent("btnCompose", "Click", std::bind(&CPetListForm::btnCompose_Click, this, std::placeholders::_1));
	registerEvent("btnSkills", "Click", std::bind(&CPetListForm::btnSkills_Click, this, std::placeholders::_1));
	registerEvent("btnRegister", "Click", std::bind(&CPetListForm::btnRegister_Click, this, std::placeholders::_1));
	registerEvent("btnCatalog", "Click", std::bind(&CPetListForm::btnCatalog_Click, this, std::placeholders::_1));
	registerEvent("btnRegister", "Click", std::bind(&CPetListForm::btnRegister_Click, this, std::placeholders::_1));
	registerEvent("btnPetStall", "Click", std::bind(&CPetListForm::btnPetStall_Click, this, std::placeholders::_1));
	registerEvent("btnDrop", "Click", std::bind(&CPetListForm::btnDrop_Click, this, std::placeholders::_1));
	registerEvent("btnMarch", "Click", std::bind(&CPetListForm::btnMarch_Click, this, std::placeholders::_1));
	registerEvent("btnChangeAccessory", "Click", std::bind(&CPetListForm::btnChangeAccessory_Click, this, std::placeholders::_1));

	medalatk.reset(new Texture(renderer, "data/GUI/Main/medalatk.tga"));
	medaldef.reset(new Texture(renderer, "data/GUI/Main/medaldef.tga"));
	medaldex.reset(new Texture(renderer, "data/GUI/Main/medaldex.tga"));

	SetParentFromStack();
}

void CPetListForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.type == CUSTOMEVENT_PET) {
		if (e.user.code == PET_EQUIP || e.user.code == PET_UNEQUIP) {
			ReloadAccessory();
		}
		if (e.user.code == PET_LIFEMAX) {
			Pet *pet = player->getPetList()[selection];
			if (pet) gaugeLife->set(pet->GetCurrentLife(), pet->GetMaxLife());
		}
	}

	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_EVOLVE) {
			CloseWindow = true;
		}
		if (e.user.code == WINDOW_COMPOSE) {
			CloseWindow = true;
		}
	}

	if (e.window.windowID != windowID) return;

	if (e.type = CUSTOMEVENT_WINDOW && e.user.data1 == promptForm) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			if (Dropping) DropPet();
		}
		Dropping = false;
	}
}

void CPetListForm::render() {
	CWindow::render();
	if (selection < 0) return;
	
	if (!player) return;
	Pet *pet = player->getPetList()[selection];
	if (!pet) return;

	SDL_Point p = { imgBraveMedals->GetX() + 3, imgBraveMedals->GetY() + 4 };
	for (int i = 0; i < pet->getMedalAttack(); i++) {
		medalatk->Render(p);
		p.x += 20 + i/2;
	}
	p = { imgCalmnessMedals->GetX() + 3, imgCalmnessMedals->GetY() + 3 };
	for (int i = 0; i < pet->getMedalDefence(); i++) {
		medaldef->Render(p);
		p.x += 20 + i/2;
	}
	p = { imgSpeedMedals->GetX() + 4, imgSpeedMedals->GetY() + 3 };
	for (int i = 0; i < pet->getMedalDexterity(); i++) {
		medaldex->Render(p);
		p.x += 20 + i/2;
	}
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
	btnCompose = (CButton*)GetWidget("btnCompose");
	btnSkills = (CButton*)GetWidget("btnSkills");
	btnCatalog = (CButton*)GetWidget("btnCatalog");

	btnRegister = (CButton*)GetWidget("btnRegister");
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
			std::string look = std::to_string(pet->GetLook());
			while (look.size() < 4) look.insert(look.begin(), '0');
			look = "peticon" + look;

			INI ini("INI\\petLook.ini", look);
			std::string iconPath = ini.getEntry("Frame0");

			portrait = new Texture(renderer, iconPath);
			petName = pet->GetName();
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

		if (portrait) delete portrait;
	}
}

void CPetListForm::LoadSprite() {
	if (selection == -1) {
		imgSprite->BindSprite(nullptr);
		return;
	}
	Pet *pet = player->getPetList()[selection];

	std::string role = getRoleFromLook(pet->GetLook());
	int animation = Walk;
	int direction = 1;
	std::string animString = animationTypeToString(animation) + std::to_string(direction);
	Sprite* sprite = new Sprite(renderer, getSpriteFramesFromAni(role, animation, direction), stMonster);
	sprite->start();
	imgSprite->BindSprite(sprite);
}

void CPetListForm::LoadPet(int index) {
	LoadSprite();

	if (selection == -1) {
		fldName->SetText("");
		lblSpecies->SetText("");
		lblElement->SetText("");
		lblRegistered->SetText("");
		fldID->SetText("");
		gaugeLoyalty->set(0, 100);
		lblLevel->SetText("");
		gaugeExperience->set(0, 0);
		lblGeneration->SetText("");
		gaugeLife->set(0, 0);
		lblAccessory->SetText("");
		fldAttack->SetText("");
		fldDefence->SetText("");
		fldDexterity->SetText("");
		return;
	}

	Pet *pet = player->getPetList()[index];
	fldName->SetText(pet->GetName());
	lblSpecies->SetText("N/A");
	lblElement->SetText(pet->GetElementText());
	lblRegistered->SetText("");
	fldID->SetText(std::to_string(pet->GetID()));

	int loyalty = pet->getLoyalty();
	if (loyalty > 100) loyalty = 100;
	gaugeLoyalty->set(loyalty, 100);

	lblLevel->SetText(formatInt(pet->GetLevel()));
	gaugeExperience->set(pet->getExperience(), pet->getLevelUpExperience());
	lblGeneration->SetText(std::to_string(pet->GetGeneration()));
	gaugeLife->set(pet->GetCurrentLife(), pet->GetMaxLife());

	ReloadAccessory();
	//accessory can also alter life? maxLife may need to factor in pet item too
}

void CPetListForm::ReloadAccessory() {
	if (selection < 0) return;

	Pet *pet = player->getPetList()[selection];
	if (!pet) return;

	Item *pItem = pet->getItem();
	int iAtk = 0, iDef = 0, iDex = 0;
	if (pItem) {
		lblAccessory->SetText(pItem->GetName());
		iAtk = pItem->GetAttack();
		iDef = pItem->GetDefence();
		iDex = pItem->GetDexterity();
	}
	else lblAccessory->SetText(" ");

	std::string attack = formatInt(pet->GetAttack()) + "(+" + std::to_string(pet->getMedalAttack() * 5) + "% + " + formatInt(iAtk) + ")";
	std::string defence = formatInt(pet->GetDefence()) + "(+" + std::to_string(pet->getMedalDefence() * 5) + "% + " + formatInt(iDef) + ")";
	std::string dexterity = formatInt(pet->GetDexterity()) + "(+" + std::to_string(pet->getMedalDexterity() * 5) + "% + " + formatInt(iDex) + ")";
	fldAttack->SetText(attack);
	fldDefence->SetText(defence);
	fldDexterity->SetText(dexterity);
}

void CPetListForm::imgPet1_Click(SDL_Event& e) {
	if (selection == 0) return;
	selection = 0;
	LoadPet(selection);
}

void CPetListForm::imgPet2_Click(SDL_Event& e) {
	if (selection == 1) return;
	selection = 1;
	LoadPet(selection);
}

void CPetListForm::imgPet3_Click(SDL_Event& e) {
	if (selection == 2) return;
	selection = 2;
	LoadPet(selection);
}

void CPetListForm::imgPet4_Click(SDL_Event& e) {
	if (selection == 3) return;
	selection = 3;
	LoadPet(selection);
}

void CPetListForm::imgPet5_Click(SDL_Event& e) {
	if (selection == 4) return;
	selection = 4;
	LoadPet(selection);
}

void CPetListForm::btnChangeName_Click(SDL_Event& e) {
	std::string name = fldName->GetText();
	if (name.length() == 0) {
		doPromptError(this, "Error", "Nickname cannot be blank.");
		return;
	}
	if (name.length() > 16) {
		doPromptError(this, "Error", "Nickname cannot be more than 16 characters.");
		return;
	}

	Pet *pet = player->getPetList()[selection];
	if (!pet) return;

	//Nickname has changed
	if (name.compare(pet->GetName()) == 0) return;

	//send nickname update
	pRename *rename = new pRename(pet->GetID(), rmPet, name);
	gClient.addPacket(rename);

	pet->SetName(name);
	doPrompt(this, "Confirm", "Pet name has been updated successfully!");
	LoadPortraits();
}

void CPetListForm::btnEvolve_Click(SDL_Event& e) {
	Pet *pet = player->getPetList()[selection];
	if (!pet) return;

	if (pet->IsUnevo()) {
		doPromptError(this, "Error", "This pet cannot evolve.");
		return;
	}
	if (pet->GetEvoNum() >= 2) {
		doPromptError(this, "Error", "This pet requires special means to evolve.");
		return;
	}
	if (pet->IsSuper()) {
		doPromptError(this, "Error", "This pet cannot evolve any further.");
		return;
	}

	CEvolveForm *form = new CEvolveForm(pet);
	PushWindow(form);
}

void CPetListForm::btnCompose_Click(SDL_Event& e) {
	CPetComposeForm* compForm = new CPetComposeForm();
	PushWindow(compForm);
}

void CPetListForm::btnSkills_Click(SDL_Event& e) {

}

void CPetListForm::btnCatalog_Click(SDL_Event& e) {

}

void CPetListForm::btnRegister_Click(SDL_Event& e) {

}

void CPetListForm::btnPetStall_Click(SDL_Event& e) {

}

void CPetListForm::btnDrop_Click(SDL_Event& e) {
	if (selection == -1) return;
	int marching = GetMarchingPetIndex();
	if (selection == marching) {
		doPromptError(this, "Error", "You cannot drop your marching pet.");
		return;
	}

	Pet *pet = player->getPetList()[selection];
	if (!pet) return;

	promptForm = new CPromptForm(true);
	promptForm->SetTitle("Are you sure?");
	std::string message = "You are about to drop ";
	message += pet->GetName();
	message += "\n\nLevel " + formatInt(pet->GetLevel());
	message += "\n\nAre you sure you want to continue?";
	promptForm->SetMessage(message);
	promptForm->SetParent(this);
	Windows.push_back(promptForm);

	Dropping = true;
}

void CPetListForm::DropPet() {
	Pet *pet = player->getPetList()[selection];
	pPetAction *dropPacket = new pPetAction(pet->GetID(), 0, paDrop);
	gClient.addPacket(dropPacket);

	player->removePet(pet->GetID());

	LoadPortraits();
	selection = GetMarchingPetIndex();
	LoadPet(selection);
}

void CPetListForm::btnMarch_Click(SDL_Event& e) {
	if (selection == -1) return;
	int currentIndex = GetMarchingPetIndex();
	int petId = 0;
	if (selection == currentIndex) {
		//SetMarching(-1);

		//send change active pet packet
		//pPetAction* pAction = new pPetAction(petId, 0, paSetActive);
		//gClient.addPacket(pAction);
		//You cannot unmarch a pet currently
	}
	else {
		Pet *pet = player->getPetList()[selection];
		petId = pet->GetID();
		SetMarching(selection);

		//send change active pet packet
		pPetAction* pAction = new pPetAction(petId, 0, paSetActive);
		gClient.addPacket(pAction);
	}

	player->setActivePet(selection);
}

void CPetListForm::btnChangeAccessory_Click(SDL_Event& e) {
	Pet *pet = player->getPetList()[selection];
	if (!pet) return;
	CPetEquipForm *form = new CPetEquipForm(pet);
	PushWindow(form);
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
	if (index == -1) imgStar->SetVisible(false);
	else imgStar->SetVisible(true);
	imgStar->SetX(20 + (100 * index - 1));
}
