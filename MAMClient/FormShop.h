#ifndef __FORMSHOP_H
#define __FORMSHOP_H

#include "SubForm.h"
#include "Inventory.h"
#include "ImageView.h"

class Item;
class FormPrompt;

class FormShop : public SubForm {
private:
	Inventory* inventory;
	int lastCash = -1;

	FormPrompt * prompt = nullptr;

	const SDL_Point firstItemPos = { 374,54 };
	const int rowSize = 4;
	const int cellWidth = 47;
	const int cellHeight = 48;

	//std::vector<int> shopInventory;
	std::map<int, Item*> shopInventory;

	void FormShop::handleItemDoubleClick();
	void FormShop::clearFocusedItem();
	void FormShop::setCashLabel();
	void FormShop::setBuyButtonLabel(bool buyMode);
	void FormShop::promptBuyItem(Item* bItem);
	void FormShop::promptSellItem(Item* sItem);

public:
	Texture *inventoryBg = nullptr;

	Panel *pnlShop = nullptr;
	ListView *lvShop = nullptr;

	Label *lblCash = nullptr;
	Button *btnCheckAccount = nullptr, *btnDeposit = nullptr, *btnWithdraw = nullptr;
	Button *btnBuySell = nullptr, *btnClose = nullptr, *btnHelp = nullptr;

	ImageView* ivItem = nullptr;
	Label *lblItemInfo = nullptr;

	Item *clickedItem = nullptr, *focusedItem = nullptr;
	Item *doubleClickedItem = nullptr;
	SDL_Rect clickItemRect;
	bool buyingItem = false, sellingItem = false, isShopItem = false;

	FormShop(std::string title, int xPos, int yPos);
	~FormShop();

	void FormShop::render();
	bool FormShop::handleEvent(SDL_Event* e);
	void FormShop::handleWidgetEvent();

	void FormShop::addShopInventory(std::vector<int> shopInv);
};

#endif