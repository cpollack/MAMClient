#ifndef __FORMINVENTORY_H
#define __FORMINVENTORY_H

#include "SubForm.h"
#include "Inventory.h"
#include "ImageView.h"

class Item;
class FormPrompt;

class FormInventory : public SubForm {
private:
	Inventory* inventory;
	//Item** equipment;
	SDL_Point eqSlots[5];

	FormPrompt * prompt = nullptr;

	void FormInventory::loadEquipmentSlots();

	const SDL_Point firstItemPos = { 375,57 };
	const int rowSize = 4;
	const int cellWidth = 47;
	const int cellHeight = 48;

	void FormInventory::handleItemDoubleClick();
	void FormInventory::clearFocusedItem();

public:
	Texture *equipmentBg = nullptr, *inventoryBg = nullptr;

	Field *fldCash = nullptr;
	Button *btnRenameItem = nullptr, *btnCheckAccount = nullptr, *btnGive = nullptr, *btnDeposit = nullptr, *btnWithdraw = nullptr;
	Button *btnUseHuman = nullptr, *btnUsePet = nullptr, *btnClose = nullptr, *btnHelp = nullptr, *btnDropItem = nullptr;

	ImageView* ivItem = nullptr;
	Label *lblItemInfo = nullptr;


	Item *clickedItem = nullptr, *focusedItem = nullptr;
	Item *doubleClickedItem = nullptr;
	SDL_Rect clickItemRect;
	bool droppingItem = false;

	FormInventory(std::string title, int xPos, int yPos);
	~FormInventory();

	void FormInventory::render();
	bool FormInventory::handleEvent(SDL_Event* e);
	void FormInventory::handleWidgetEvent();
};

#endif