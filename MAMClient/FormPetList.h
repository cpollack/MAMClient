#ifndef __FORMPETLIST_H
#define __FORMPETLIST_H

#include "SubForm.h"
#include "SpriteView.h"

class Player;
class Pet;
class Item;

struct PetButton {
	SubForm* subform;
	int x, y, count;
	SDL_Texture *star_button;
	Label *lblPetName;
	Button *btnPet;

	bool focused = false;

	PetButton::PetButton(SubForm* subform, std::string name, int count, int toX, int toY);
	PetButton::~PetButton();
	void PetButton::render();
	void PetButton::setFocused(bool foc);
};

class FormPetList : public SubForm {
public:
	FormPetList(std::string title, int xPos, int yPos);
	~FormPetList();

	void FormPetList::render();
	bool FormPetList::handleEvent(SDL_Event* e);
	void FormPetList::handleWidgetEvent();

	void FormPetList::setMarchingPet(int petId);

private:
	Pet* pets[5] = { nullptr };
	Pet* activePet = nullptr;
	Item* petItem = nullptr;

	bool droppingPet = false;

	Panel *pnlPetList, *pnlPetStaus, *pnlStats, *pnlMedals;

	PetButton* petButton[5] = { nullptr };
	int activePetPos, marchingPetPos;
	int focusPb = -1;

	SpriteView *svPet;
	Label *lblCategory;
	Button *btnRegister;

	Label *lblName, *lblLevel, *lblLevelVal, *lblEquipment, *lblEquipmentName, *lblLife, *lblExp, *lblLoyalty;
	Field *fldName;
	//Gauge *ggLife, *ggExp, *ggLoyalty;

	Button *btnChangeName, *btnEvolution, *btnPetSkill, *btnEquip, *btnUnequip, *btnCatalog;

	Label *lblAttack, *lblDefence, *lblDexterity, *lblId;
	Field *fldAttack, *fldDefence, *fldDexterity, *fldId;

	Label *lblMedalAttack, *lblMedalDefence, *lblMedalDexterity;
	SDL_Texture *line1, *line2, *txMedalAttack, *txMedalDefence, *txMedalDexterity;

	Button *btnEvaluate, *btnPetVendor, *btnDrop, *btnClose, *btnHelp;

	void FormPetList::addPetsToList();
	void FormPetList::setActivePet(int itr);
	void FormPetList::dropPet(int pos);
};

#endif