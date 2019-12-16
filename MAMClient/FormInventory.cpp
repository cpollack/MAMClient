#include "stdafx.h"
#include "FormInventory.h"
#include "FormMain.h"
#include "FormPrompt.h"
#include "Player.h"
#include "Pet.h"

#include "pItemAction.h"
#include "pPetAction.h"

FormInventory::FormInventory(std::string title, int xPos, int yPos) : SubForm("Inventory", title, 596, 378) {
	//Player has a Equipment and Inventory
	inventory = player->inventory;
	//equipment = &player->equipment;
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	std::string equipBgPath = "GUI/main/" + player->PlayerRole + ".bmp";
	SDL_Color colorKey = { 49, 66, 107, 255 };
	equipmentBg = new Texture(gClient.renderer, equipBgPath, colorKey);
	//equipment will be stored on player

	inventoryBg = new Texture(gClient.renderer);
	inventoryBg->texture = gui->invetoryBg->texture;
	inventoryBg->width = gui->invetoryBg->width;
	inventoryBg->height = gui->invetoryBg->height;

	loadEquipmentSlots();

	btnRenameItem = new Button("Change Name", x + 368, y + 268, 96, 19);
	btnCheckAccount = new Button("Check Account", x + 472, y + 268, 103, 19);
	btnGive = new Button("Give $", x + 369, y + 292, 53, 19);
	btnDeposit = new Button("Deposit $", x + 428, y + 292, 61, 19);
	btnWithdraw = new Button("Withdraw $", x + 496, y + 292, 79, 19);
	btnUseHuman = new Button("Human Use(&R)", x + 369, y + 316, 104, 19);
	btnUsePet = new Button("Pet Use (&P)", x + 480, y + 316, 95, 19);
	btnClose = new Button("Close(&X)", x + 433, y + 340, 70, 22);
	btnHelp = new Button("Help(&H)", x + 505, y + 340, 70, 22);

	addWidget(btnRenameItem);
	addWidget(btnCheckAccount);
	addWidget(btnGive);
	addWidget(btnDeposit);
	addWidget(btnWithdraw);
	addWidget(btnUseHuman);
	addWidget(btnUsePet);
	addWidget(btnClose);
	addWidget(btnHelp);

	lblItemInfo = new Label("$", x + 37, y + 343);
	lblItemInfo->setAlignment(laRight);

	std::string playerCash = std::to_string(player->cash);
	std::string cashText;
	int nvCount = 0;
	for (int i = playerCash.size(); i > 0; i--) {
		if (nvCount % 3 == 0 && nvCount > 0)
			cashText.insert(0, ",");
		cashText = playerCash[i-1] + cashText;
		nvCount++;
	}
	fldCash = new Field(cashText, x + 40, y + 344, 105, 16);
	fldCash->setDisabled(true);
	btnDropItem = new Button("Drop(&D)", x + 148, y + 343, 59, 18);
	addWidget(lblItemInfo);
	addWidget(fldCash);
	addWidget(btnDropItem);

	ivItem = new ImageView(x + 219, y + 32, 100, 100);
	lblItemInfo = new Label(" ", x + 219, y + 139);
	lblItemInfo->setTextWidth(130);
	addWidget(ivItem); 
	addWidget(lblItemInfo);
}


FormInventory::~FormInventory() {
	delete equipmentBg;
}


void FormInventory::render() {
	SubForm::render();

	SDL_Rect equipBgRect{ x + 9, y + 24, equipmentBg->width, equipmentBg->height };
	SDL_RenderCopy(renderer, equipmentBg->texture, NULL, &equipBgRect);
	for (int i = 0; i < 5; i++) {
		if (player->equipment[i]) {
			SDL_Rect itemRect = { equipBgRect.x + eqSlots[i].x, equipBgRect.y + eqSlots[i].y, 36, 36 };
			SDL_RenderCopy(renderer, player->equipment[i]->getTexture(40)->texture, NULL, &itemRect);
		}
	}

	SDL_Rect inventoryBgRect{ x + 353, y + 33, inventoryBg->width, inventoryBg->height };
	SDL_RenderCopy(renderer, inventoryBg->texture, NULL, &inventoryBgRect);
	
	int itemPosCount = 0;
	for (int i = 0; i < inventory->getItemCount(); i++) {
		Item* curItem = inventory->getItemInSlot(i);
		Texture* itemTexture = curItem->getTexture(40);
		//ItemTextureType ittSmall=40 ittLarge85
		
		SDL_Rect itemRect = { x + firstItemPos.x + ((itemPosCount % rowSize) * cellWidth) , y + firstItemPos.y + ((itemPosCount / rowSize) * cellHeight), itemTexture->width, itemTexture->height };
		SDL_RenderCopy(renderer, itemTexture->texture, NULL, &itemRect);

		//inventory[i]->setPosition(x + itemPos.x, y + itemPos.y);
		//inventory[i]->render();
		itemPosCount++;
	}
}


bool FormInventory::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	//if (login->handleEvent(e)) return true;


	if (e->type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		clickedItem = nullptr;
		if (doesPointIntersect(SDL_Rect{ x,y,width,height }, mx, my)) {
			//Try Inventory
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

			//Try Equipment
			if (!clickedItem) {
				SDL_Rect equipBgRect{ x + 9, y + 24, equipmentBg->width, equipmentBg->height };
				for (int i = 0; i < 5; i++) {
					Item* nextItem = player->equipment[i];
					if (nextItem) {
						SDL_Rect itemRect = { equipBgRect.x + eqSlots[i].x, equipBgRect.y + eqSlots[i].y, 40, 40 };
						if (doesPointIntersect(itemRect, mx, my)) {
							clickedItem = nextItem;
							clickItemRect = itemRect;
							break;
						}
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
					lblItemInfo->setText(focusedItem->getDetails());
				}

				if (e->button.clicks >= 2 && !prompt) {
					FormPrompt *prompt = new FormPrompt("ItemDblClick", "Are you sure?", formMain->width / 2, formMain->height / 2);
					std::string msgPrompt;
					if (clickedItem->getType() > 4) msgPrompt = "Do you want to Use the " + clickedItem->getName() + "?";
					else msgPrompt = "Do you want to Equip the " + clickedItem->getName() + "?";
					prompt->setMessage(msgPrompt);
					formMain->addSubform("ItemDblClick", prompt);
					doubleClickedItem = clickedItem;
				}
			}
			clickedItem = nullptr;
			return true;
		}

		clickedItem = nullptr;
	}

	return false;
}


void FormInventory::handleWidgetEvent() {
	if (btnClose->wasPressed()) {
		closeWindow = true;
    }

	if (btnUseHuman->wasPressed()) {
		if (focusedItem) {
			FormPrompt *prompt = new FormPrompt("ItemDblClick", "Are you sure?", formMain->width / 2, formMain->height / 2);
			std::string msgPrompt;
			if (focusedItem->getType() > 4) msgPrompt = "Do you want to Use the " + focusedItem->getName() + "?";
			else msgPrompt = "Do you want to Equip the " + focusedItem->getName() + "?";
			prompt->setMessage(msgPrompt);
			formMain->addSubform("ItemDblClick", prompt);
			doubleClickedItem = focusedItem;
		}
	}

	if (doubleClickedItem) {
		SubForm* findForm = formMain->getSubform("ItemDblClick");
		if (findForm) {
			if (((FormPrompt*)findForm)->submitted) {
				handleItemDoubleClick();

				formMain->deleteSubform("ItemDblClick");
				prompt = nullptr;
			}
		}
		else doubleClickedItem = nullptr;
	}

	if (btnUsePet->wasPressed()) {
		if (focusedItem) {
			if (focusedItem->getType() == itMedicine) {
				pPetAction* itemAction = new pPetAction(player->getActivePet()->getId(), focusedItem->getId(), paUseItem);
				gClient.addPacket(itemAction);
				clearFocusedItem();
			}
			else {
				//invalid item type message
			}
		}
	}

	if (btnDropItem->wasPressed()) {
		if (focusedItem) {
			FormPrompt * prompt = new FormPrompt("ItemDrop", "Are you sure?", formMain->width / 2, formMain->height / 2);
			std::string msgPrompt = "Do you want to Drop the " + focusedItem->getName() + "?";
			prompt->setMessage(msgPrompt);
			formMain->addSubform("ItemDrop", prompt);
			droppingItem = true;
		}
	}
	if (droppingItem) {
		SubForm* findForm = formMain->getSubform("ItemDrop");
		if (findForm) {
			if (((FormPrompt*)findForm)->submitted) {
				pItemAction *dropPacket = new pItemAction(focusedItem->getId(), iaDrop);
				gClient.addPacket(dropPacket);

				formMain->deleteSubform("ItemDrop");
				droppingItem = false;
			}
		}
		else droppingItem = false;
	}
}


void FormInventory::loadEquipmentSlots() {
	std::string eqType = player->PlayerRole;

	if (eqType == "Man01") {
		eqSlots[0] = { 29,133 };
		eqSlots[1] = { 77,73 };
		eqSlots[2] = { 87,239 };
		eqSlots[3] = { 141,15 };
		eqSlots[4] = { 23,20 };
	}
}


void FormInventory::handleItemDoubleClick() {
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


void FormInventory::clearFocusedItem() {
	focusedItem = nullptr;
	ivItem->setTexture(nullptr);
	lblItemInfo->setText(" ");
}