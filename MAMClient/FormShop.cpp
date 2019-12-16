#include "stdafx.h"
#include "FormShop.h"
#include "FormMain.h"
#include "FormPrompt.h"
#include "Player.h"

#include "pItemAction.h"

FormShop::FormShop(std::string title, int xPos, int yPos) : SubForm("Shop", title, 604, 335) {
	inventory = player->inventory;
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	pnlShop = new Panel(x + 16, y + 38, 169, 235, "Item List");
	pnlShop->setDepth(-1);
	lvShop = new ListView(x + 20, y + 49, 160, 220);
	lvShop->setRowHeight(18);
	addWidget(pnlShop);
	addWidget(lvShop);

	inventoryBg = new Texture(gClient.renderer);
	inventoryBg->texture = gui->invetoryBg->texture;
	inventoryBg->width = gui->invetoryBg->width;
	inventoryBg->height = gui->invetoryBg->height;

	btnCheckAccount = new Button("Check Account", x + 20, y + 304, 157, 19);
	btnDeposit = new Button("Deposit $", x + 20, y + 280, 75, 19);
	btnWithdraw = new Button("Withdraw $", x + 103, y + 280, 75, 19);

	btnBuySell = new Button("", x + 360, y + 293, 70, 22);
	setBuyButtonLabel(true);
	btnClose = new Button("Close(&X)", x + 432, y + 293, 70, 22);
	btnHelp = new Button("Help(&H)", x + 505, y + 293, 70, 22);

	addWidget(btnCheckAccount);
	addWidget(btnDeposit);
	addWidget(btnWithdraw);
	addWidget(btnBuySell);
	addWidget(btnClose);
	addWidget(btnHelp);

	lblCash = new Label("$", x + 383, y + 270);
	setCashLabel();
	addWidget(lblCash);

	ivItem = new ImageView(x + 199, y + 36, 100, 100);
	lblItemInfo = new Label(" ", x + 199, y + 143);
	lblItemInfo->setTextWidth(150);
	addWidget(ivItem);
	addWidget(lblItemInfo);
}


FormShop::~FormShop() {
	//
}


void FormShop::addShopInventory(std::vector<int> shopInv) {
	//shopInventory = shopInv;

	INI itemINI = *gClient.itemIni;

	for (auto itemId : shopInv) {
		shopInventory[itemId] = nullptr;

		std::string itemSection = "id" + std::to_string(itemId);
		itemINI.setSection(itemSection);

		std::string name = itemINI.getEntry("name");
		int look = stoi(itemINI.getEntry("look"));
		lvShop->addRow(itemId, name);
	}
}


void FormShop::render() {
	SubForm::render();

	SDL_Rect inventoryBgRect{ x + 352, y + 30, inventoryBg->width, inventoryBg->height };
	SDL_RenderCopy(renderer, inventoryBg->texture, NULL, &inventoryBgRect);

	int itemPosCount = 0;
	for (int i = 0; i < inventory->getItemCount(); i++) {
		Item* curItem = inventory->getItemInSlot(i);
		Texture* itemTexture = curItem->getTexture(40);

		SDL_Rect itemRect = { x + firstItemPos.x + ((itemPosCount % rowSize) * cellWidth) , y + firstItemPos.y + ((itemPosCount / rowSize) * cellHeight), itemTexture->width, itemTexture->height };
		SDL_RenderCopy(renderer, itemTexture->texture, NULL, &itemRect);

		itemPosCount++;
	}
}


bool FormShop::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		clickedItem = nullptr;
		if (doesPointIntersect(SDL_Rect{ x,y,width,height }, mx, my)) {
			for (int i = 0; i < inventory->getItemCount(); i++) {
				Item* nextItem = inventory->getItemInSlot(i);
				if (nextItem) {
					SDL_Rect itemRect = { x + firstItemPos.x + ((i % rowSize) * cellWidth) , y + firstItemPos.y + ((i / rowSize) * cellHeight), 40, 40 };
					if (doesPointIntersect(itemRect, mx, my)) {
						clickedItem = nextItem;
						clickItemRect = itemRect;
						break;
					}
				}
			}

			return true;
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (clickedItem && doesPointIntersect(SDL_Rect{ x,y,width,height }, mx, my)) {
			if (doesPointIntersect(clickItemRect, mx, my)) {
				if (focusedItem != clickedItem) {
					focusedItem = clickedItem;
					ivItem->setTexture(focusedItem->getTexture(85));
					lblItemInfo->setText(focusedItem->getShopDetails());
					setBuyButtonLabel(false);
				}

				if (e->button.clicks >= 2 && !prompt) {
					promptSellItem(clickedItem);
				}
			}
			clickedItem = nullptr;
			return true;
		}

		clickedItem = nullptr;
	}

	return false;
}


void FormShop::handleWidgetEvent() {
	if (btnClose->wasPressed()) {
		closeWindow = true;
	}

	if (lastCash != player->cash) {
		lastCash = player->cash;
		setCashLabel();		
	}

	//Focus shop item
	if (lvShop->didSelectionChange()) {
		int itemId = lvShop->getSelection();
		if (shopInventory.find(itemId) == shopInventory.end() || shopInventory[itemId] == nullptr) {
			Item* shopitem = new Item(itemId);
			shopInventory[itemId] = shopitem;
		}
		focusedItem = shopInventory[itemId];
		ivItem->setTexture(focusedItem->getTexture(85));
		lblItemInfo->setText(focusedItem->getShopDetails());
		setBuyButtonLabel(true);
	}

	//Trigger buy item
	if (lvShop->wasDoubleClicked()) {
		int itemId = lvShop->getSelection();
		if (shopInventory.find(itemId) == shopInventory.end() || shopInventory[itemId] == nullptr) {
			Item* shopitem = new Item(itemId);
			shopInventory[itemId] = shopitem;
		}
		Item* bItem = shopInventory[itemId];
		promptBuyItem(bItem);
	}

	if (btnBuySell->wasPressed()) {
		if (isShopItem) promptBuyItem(focusedItem);
		else promptSellItem(focusedItem);
	}

	//Confirm buying item
	if (buyingItem) {
		SubForm* findForm = formMain->getSubform("ItemBuy");
		if (findForm) {
			if (((FormPrompt*)findForm)->submitted) {
				pItemAction *buyPacket = new pItemAction(focusedItem->getId(), iaBuy);
				gClient.addPacket(buyPacket);

				formMain->deleteSubform("ItemBuy");
				buyingItem = false;
			}
		}
		else buyingItem = false;
	}

	//Confirm selling item
	if (sellingItem) {
		SubForm* findForm = formMain->getSubform("ItemSell");
		if (findForm) {
			if (((FormPrompt*)findForm)->submitted) {
				pItemAction *sellPacket = new pItemAction(focusedItem->getId(), iaSell);
				gClient.addPacket(sellPacket);

				formMain->deleteSubform("ItemSell");
				sellingItem = false;
			}
		}
		else sellingItem = false;
	}
}



void FormShop::handleItemDoubleClick() {
	pItemAction *itemAction = nullptr;

	bool found = false;

	switch (doubleClickedItem->getType()) {
	case itWeapon:
	case itArmor:
	case itShoes:
	case itAccessory:
	case itHeadwear:
		if (player->equipment[doubleClickedItem->getType()] && player->equipment[doubleClickedItem->getType()]->getId() == doubleClickedItem->getId()) {
			if (inventory->getItemCount() >= 15) {
				//create a notice popup
			}
			else {
				itemAction = new pItemAction(doubleClickedItem->getId(), iaUnequip);
				player->unequipItem(doubleClickedItem);
			}
			found = true;
			break;
		}
		if (!found) itemAction = new pItemAction(doubleClickedItem->getId(), iaUse);
		break;

	case itMedicine:
		itemAction = new pItemAction(doubleClickedItem->getId(), iaUse);
		clearFocusedItem();
		break;
	}

	if (itemAction) gClient.addPacket(itemAction);
}


void FormShop::clearFocusedItem() {
	focusedItem = nullptr;
	ivItem->setTexture(nullptr);
	lblItemInfo->setText(" ");
}


void FormShop::setCashLabel() {
	std::string playerCash = std::to_string(player->cash);
	std::string cashText;
	int nvCount = 0;
	for (int i = playerCash.size(); i > 0; i--) {
		if (nvCount % 3 == 0 && nvCount > 0)
			cashText.insert(0, ",");
		cashText = playerCash[i - 1] + cashText;
		nvCount++;
	}
	lblCash->setText("$: " + cashText);
}


void FormShop::setBuyButtonLabel(bool buyMode) {
	if (buyMode) {
		btnBuySell->setButtonText("Buy(&B)");
		isShopItem = true;
	}
	else {
		btnBuySell->setButtonText("Sell(&S)");
		isShopItem = false;
	}
}


void FormShop::promptBuyItem(Item* bItem) {
	if (!bItem) return;

	FormPrompt *prompt = new FormPrompt("ItemBuy", "Are you sure?", formMain->width / 2, formMain->height / 2);
	std::string msgPrompt = "Do you want to Buy the " + bItem->getName() + "?";
	prompt->setMessage(msgPrompt);
	formMain->addSubform("ItemBuy", prompt);
	buyingItem = true;
}


void FormShop::promptSellItem(Item* sItem) {
	if (!sItem) return;

	FormPrompt *prompt = new FormPrompt("ItemSell", "Are you sure?", formMain->width / 2, formMain->height / 2);
	std::string msgPrompt = "Do you want to Sell the " + sItem->getName() + "?";
	prompt->setMessage(msgPrompt);
	formMain->addSubform("ItemSell", prompt);
	sellingItem = true;
}