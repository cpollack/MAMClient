#include "stdafx.h"
#include "FormPetList.h"
#include "FormMain.h"
#include "FormPrompt.h"

#include "Player.h"
#include "Pet.h"
#include "Item.h"

#include "pPetAction.h"

FormPetList::FormPetList(std::string title, int xPos, int yPos) : SubForm("PetList", title, 693, 399) {
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	//Panels
	pnlPetList = new Panel(x + 12, y + 36, 165, 319, "Pet List");
	pnlPetList->setDepth(-2);
	pnlPetStaus = new Panel(x + 182, y + 36, 497, 319, "Pet Status");
	pnlPetStaus->setDepth(-2);
	pnlStats = new Panel(x + 190, y + 234, 210, 113, "");
	pnlStats->setDepth(-1);
	pnlMedals = new Panel(x + 415, y + 234, 256, 113, "");
	pnlMedals->setDepth(-1);
	addWidget(pnlPetList);
	addWidget(pnlPetStaus);
	addWidget(pnlStats);
	addWidget(pnlMedals);

	//Pet View
	svPet = new SpriteView(x + 188, y + 51, 130, 150);
	lblCategory = new Label("Category ", x + 222, y + 209);
	btnRegister = new Button("Register", x + 348, y + 204, 70, 22);
	addWidget(svPet);
	addWidget(lblCategory);
	addWidget(btnRegister);

	//Status Panel
	lblName = new Label("Name:", x + 392, y + 51);
	lblName->setAlignment(laRight);
	fldName = new Field("", x + 394, y + 47, 155, 23);

	lblLevel = new Label("Level:", x + 392, y + 74);
	lblLevel->setAlignment(laRight);
	lblLevelVal = new Label("", x + 398, y + 74);

	lblEquipment = new Label("Equipment:", x + 392, y + 122);
	lblEquipment->setAlignment(laRight);
	lblEquipmentName = new Label("", x + 398, y + 97);

	lblLife = new Label("Life:", x + 392, y + 97);
	lblLife->setAlignment(laRight);
	//ggLife = new Gauge(x + 398, y + 122, 155, 16);

	lblExp = new Label("Experience:", x + 392, y + 146);
	lblExp->setAlignment(laRight);
	//ggExp = new Gauge(x + 398, y + 146, 155, 16);

	lblLoyalty = new Label("Loyalty:", x + 392, y + 170);
	lblLoyalty->setAlignment(laRight);
	//ggLoyalty = new Gauge(x + 398, y + 170, 155, 16);
	//ggLoyalty->set(0, 100);
	//ggLoyalty->setLabel(" ");

	addWidget(lblName);
	addWidget(fldName);
	addWidget(lblLevel);
	addWidget(lblLevelVal);
	addWidget(lblEquipment);
	addWidget(lblEquipmentName);
	addWidget(lblLife);
	//addWidget(ggLife);
	addWidget(lblExp);
	//addWidget(ggExp);
	addWidget(lblLoyalty);
	//addWidget(ggLoyalty);

	//Right Side Buttons
	btnChangeName = new Button("Change Name", x + 581, y + 49, 91, 22);
	btnEvolution = new Button("Evolution", x + 581, y + 80, 91, 22);
	btnPetSkill = new Button("Pet Skills", x + 581, y + 111, 91, 22);
	btnEquip = new Button("Equip", x + 581, y + 141, 91, 22);
	btnUnequip = new Button("Unequip", x + 581, y + 173, 91, 22);
	btnCatalog = new Button("Pet Catalog", x + 581, y + 204, 91, 22);
	addWidget(btnChangeName);
	addWidget(btnEvolution);
	addWidget(btnPetSkill);
	addWidget(btnEquip);
	addWidget(btnUnequip);
	addWidget(btnCatalog);

	//Stats Panel
	lblAttack = new Label("Attack:", x + 283, y + 243);
	lblAttack->setAlignment(laRight);
	fldAttack = new Field("", x + 286, y + 246, 97, 16, true);
	fldAttack->setDisabled(true);
	lblDefence = new Label("Defence:", x + 283, y + 267);
	lblDefence->setAlignment(laRight);
	fldDefence = new Field("", x + 286, y + 270, 97, 16, true);
	fldDefence->setDisabled(true);
	lblDexterity = new Label("Dexterity:", x + 283, y + 289);
	lblDexterity->setAlignment(laRight);
	fldDexterity = new Field("", x + 286, y + 292, 97, 16, true);
	fldDexterity->setDisabled(true);
	lblId = new Label("Id:", x + 283, y + 315);
	lblId->setAlignment(laRight);
	fldId = new Field("", x + 286, y + 318, 97, 16, true);
	fldId->setDisabled(true);
	addWidget(lblAttack);
	addWidget(fldAttack);
	addWidget(lblDefence);
	addWidget(fldDefence);
	addWidget(lblDexterity);
	addWidget(fldDexterity);
	addWidget(lblId);
	addWidget(fldId);

	//Medal Panel
	line1 = gui->line1->texture;
	line2 = gui->line2->texture;
	txMedalAttack = gui->medal_attack->texture;
	txMedalDefence = gui->medal_defence->texture;
	txMedalDexterity = gui->medal_dexterity->texture;
	lblMedalAttack = new Label("Medal of Bravery", x + 543, y + 257);
	lblMedalAttack->setAlignment(laRight);
	lblMedalDefence = new Label("Medal of Calmness", x + 543, y + 281);
	lblMedalDefence->setAlignment(laRight);
	lblMedalDexterity = new Label("Medal of Speed", x + 543, y + 307);
	lblMedalDexterity->setAlignment(laRight);
	addWidget(lblMedalAttack);
	addWidget(lblMedalDefence);
	addWidget(lblMedalDexterity);

	//Bottom buttons
	btnEvaluate = new Button("Evalutaion", x + 16, y + 360, 70, 23);
	btnPetVendor = new Button("Pet Vendor", x + 182, y + 360, 91, 22);
	btnDrop = new Button("Drop(&D)", x + 438, y + 360, 70, 22);
	btnClose = new Button("Close(&X)", x + 523, y + 360, 70, 22);
	btnHelp = new Button("Help(&H)", x + 608, y + 360, 70, 22);
	addWidget(btnEvaluate);
	addWidget(btnPetVendor);
	addWidget(btnDrop);
	addWidget(btnClose);
	addWidget(btnHelp);

	addPetsToList();
}


FormPetList::~FormPetList() {

}


void FormPetList::render() {
	SubForm::render();

	for (int i = 0; i < 5; i++) {
		if (!petButton[i]) break;
		petButton[i]->render();
	}

	SDL_Rect lineRect = { x + 547, y + 251, 109, 25};
	SDL_RenderCopy(renderer, line1, NULL, &lineRect);
	lineRect.y += lineRect.h;
	SDL_RenderCopy(renderer, line2, NULL, &lineRect);
	lineRect.y += lineRect.h;
	SDL_RenderCopy(renderer, line2, NULL, &lineRect);

	//loop through medals and render them
}


bool FormPetList::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	int mx, my;
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mx, &my);

		focusPb = -1;
		for (int i = 0; i < 5; i++) {
			if (!petButton[i]) break;

			int rx = x + 18;
			int ry = y + 49 + (i * 62);
			SDL_Rect pbRect = { rx, ry, 128, 255 };
			if (doesPointIntersect(pbRect, mx, my)) {
				focusPb = i;
			}
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		SDL_GetMouseState(&mx, &my);

		if (focusPb >= 0) {
			int rx = x + 18;
			int ry = y + 49 + (focusPb * 62);
			SDL_Rect pbRect = { rx, ry, 128, 255 };
			if (doesPointIntersect(pbRect, mx, my)) {
				if (petButton[activePetPos]) petButton[activePetPos]->setFocused(false);
				//petButton[activePetPos]->btnPet->setButtonText("Resting");
				setActivePet(focusPb);
			}
			focusPb = -1;
		}
	}

	return false;
}


void FormPetList::handleWidgetEvent() {
	if (btnDrop->wasPressed()) {
		FormPrompt *prompt = new FormPrompt("PromptDrop", "Are you sure?", formMain->width / 2, formMain->height / 2);
		std::string msgPrompt = "Are you sure you want to drop '" + activePet->getName() + "' ?";
		prompt->setMessage(msgPrompt);
		formMain->addSubform("PromptDrop", prompt);
		droppingPet = true;
	}
	if (droppingPet) {
		SubForm* findForm = formMain->getSubform("PromptDrop");
		if (findForm) {
			if (((FormPrompt*)findForm)->submitted) {
				dropPet(activePetPos);

				formMain->deleteSubform("PromptDrop");
				droppingPet = false;
			}
		}
		else droppingPet = false;
	}

	for (int i = 0; i < 5; i++) {
		if (!petButton[i]) break;

		if (petButton[i]->btnPet->wasPressed()) {
			if (i != marchingPetPos) {
				//send change active pet packet
				pPetAction* pAction = new pPetAction(pets[i]->getId(), 0, paSetActive);
				gClient.addPacket(pAction);
				break;
			}
		}
	}

	if (btnClose->wasPressed()) {
		closeWindow = true;
	}
}


void FormPetList::addPetsToList() {
	activePet = player->getActivePet();

	std::vector<Pet*> petList = player->getPetList();
	int petCount = 0;
	for (auto pet : petList) {
		pets[petCount] = pet;
		petButton[petCount] = new PetButton(this, pet->getName(), petCount, x + 153, y + 45 + (petCount*62));

		if (pet->getId() == activePet->getId()) {
			petButton[petCount]->setFocused(true);
			setActivePet(petCount);
			marchingPetPos = petCount;
			petButton[petCount]->btnPet->setButtonText("Marching");
		}

		petCount++;
	}
}


void FormPetList::setMarchingPet(int petId) {
	for (int i = 0; i < 5; i++) {
		if (pets[i]->getId() == petId) {
			petButton[marchingPetPos]->btnPet->setButtonText("Resting");
			petButton[i]->btnPet->setButtonText("Marching");
			marchingPetPos = i;
			break;
		}
	}
}


void FormPetList::setActivePet(int itr) {
	activePetPos = itr;
	activePet = pets[itr];
	petItem = activePet->getItem();
	petButton[activePetPos]->setFocused(true);

	fldName->setValue(activePet->getName());
	lblLevelVal->setText(std::to_string(activePet->getLevel()));
	//ggLife->set(activePet->getCurrentHealth(), activePet->getMaxHealth());
	//ggExp->set(activePet->getExperience(), activePet->getLevelUpExperience());

	int loyalty = (activePet->getLoyalty() > 100) ? 100 : activePet->getLoyalty();
	//ggLoyalty->set(loyalty);
	//ggLoyalty->setLabel(std::to_string(loyalty));

	fldAttack->setValue(std::to_string(activePet->getAttack()) + "(" + std::to_string(5 * activePet->getMedalAttack()) + "% + " + std::to_string((petItem) ? petItem->getAttack() : 0) + ")");
	fldDefence->setValue(std::to_string(activePet->getDefence()) + "(" + std::to_string(5 * activePet->getMedalDefence()) + "% + " + std::to_string((petItem) ? petItem->getDefence() : 0) + ")");
	fldDexterity->setValue(std::to_string(activePet->getDexterity()) + "(" + std::to_string(5 * activePet->getMedalDexterity()) + "% + " + std::to_string((petItem) ? petItem->getDexterity() : 0) + ")");
	fldId->setValue(std::to_string(activePet->getId()));

	svPet->setLook(activePet->getLook());
	lblCategory->setText("Category " + activePet->getElement());
}


void FormPetList::dropPet(int pos) {
	pPetAction *dropPacket = new pPetAction(pets[pos]->getId(), 0, paDrop);
	gClient.addPacket(dropPacket);

	//Shift Pet list over
	for (int i = 0; i < 5; i++) {
		if (i == pos) {
			player->removePet(pets[i]->getId());
			pets[i] = nullptr;
			delete petButton[i];
			petButton[i] = nullptr;
		}
		if (i > pos) {
			pets[i-1] = pets[i];
			pets[i] = nullptr;
			petButton[i-1] = petButton[i];
			petButton[i] = nullptr;
		}
	}

	//Adjust focused row
	int oldFocus = activePetPos;
	int pCount = player->getPetList().size();
	if (activePetPos > pCount) activePetPos = pCount-1;
	if (oldFocus != activePetPos) {
		if (oldFocus != -1 && petButton[oldFocus]) petButton[oldFocus]->setFocused(false);
		setActivePet(activePetPos);
		petButton[activePetPos]->setFocused(true);
	}
}


PetButton::PetButton(SubForm* subform, std::string name, int count, int toX, int toY) {
	this->subform = subform;
	this->count = count;
	x = toX;
	y = toY;

	int rx = subform->x + 18;
	int ry = subform->y + 45 + (count * 62);
	star_button = gui->star_button->texture;
	lblPetName = new Label(name, rx + 34, ry + 10 + 2);
	lblPetName->setDepth(-5);
	subform->addWidget(lblPetName);

	btnPet = new Button("Resting", rx + 72, ry + 34, 73, 20);
	subform->addWidget(btnPet);
}


PetButton::~PetButton() {
	subform->removeWidget(lblPetName);
	delete lblPetName;
	subform->removeWidget(btnPet);
	delete btnPet;
}


void PetButton::render() {
	SDL_Rect starRect = { subform->x + 20, subform->y + 51 + (count * 62), 51, 21 };
	SDL_RenderCopy(gClient.renderer, star_button, NULL, &starRect);
	lblPetName->setPosition(starRect.x + 34, starRect.y);
	lblPetName->render();

	if (focused) {
		int rx = subform->x + 18;
		int ry = subform->y + 45 + (count * 62);
		roundedRectangleRGBA(gClient.renderer, rx, ry, rx + 153, ry + 58, 10, 128, 255, 128, 255);
	}
}


void PetButton::setFocused(bool foc) {
	focused = foc;
}
