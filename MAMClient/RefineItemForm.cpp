#include "stdafx.h"
#include "RefineItemForm.h"

#include "ImageBox.h"
#include "Label.h"

#include "CustomEvents.h"
#include "Player.h"
#include "Inventory.h"

#include "pItemAction.h"
#include "pWuxing.h"

CRefineItemForm::CRefineItemForm() : CWindow("RefineItemForm.JSON") {
	Type = FT_REFINE;

	HookWidgets();
	UpdateInventory();
}

void CRefineItemForm::HookWidgets() {
	imgInventory = (CImageBox*)GetWidget("imgInventory");
	imgRefine = (CImageBox*)GetWidget("imgRefine");

	imgRefineSlot = createImageBox("imgRefineSlot");
	imgRefineSlot->SetPosition(imgRefine->GetX() + 9, imgRefine->GetY() + 7);
	imgRefineSlot->SetHoverDelay(0);

	lblCash = (CLabel*)GetWidget("lblCash");
	lblAttr = (CLabel*)GetWidget("lblAttr");

	btnBegin = (CButton*)GetWidget("btnUnequip");
	registerEvent("btnBegin", "Click", std::bind(&CRefineItemForm::btnBegin_Click, this, std::placeholders::_1));

	std::string boxName = "imgItem";
	int toX = imgInventory->GetX() + 24;
	int toY = imgInventory->GetY() + 24;
	while (imgItems.size() < 16) {
		int count = imgItems.size();
		int next = count + 1;
		std::string widgetName = boxName + (next < 10 ? "0" : "") + std::to_string(next);
		CImageBox *nextItem = createImageBox(widgetName);
		nextItem->SetPosition(toX + (count % 4 * 47), toY + (count / 4 * 47));
		nextItem->SetHoverDelay(0);
		imgItems.push_back(nextItem);
		registerEvent(widgetName, "DoubleClick", std::bind(&CRefineItemForm::imgItem_DoubleClick, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverStart", std::bind(&CRefineItemForm::imgItem_OnHoverStart, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverEnd", std::bind(&CRefineItemForm::imgItem_OnHoverEnd, this, std::placeholders::_1));
	}
}

void CRefineItemForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.type == CUSTOMEVENT_ITEM) {
		if (e.user.code == ITEM_ADD) {
			UpdateInventory();
			Item* addItem = (Item*)e.user.data1;
			if (addItem) {
				if (addItem->GetName().compare("TrueTemprStone") == 0 && addItem->GetInventor().compare("¡ùRefinery") == 0) receivedItem = true;
				if (addItem->GetID() == itemId) {
					wuxedItem = true;
					item = addItem;
					SetItemToImageBox(imgRefineSlot, item);
					UpdateAttr();
				}
			}
		}
	}

	if (e.window.windowID != windowID) return;
}

void CRefineItemForm::render() {
	CWindow::render();

	if (hoverTexture) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		SDL_Rect hoverRect = hoverTexture->rect;
		hoverRect.x = mx; hoverRect.y = my;
		if (hoverRect.y + hoverRect.h > Height) hoverRect.y = Height - hoverRect.h;
		SDL_RenderCopy(renderer, hoverTexture->getTexture(), nullptr, &hoverRect);
	}
}

void CRefineItemForm::step() {
	if (!refining) return;

	int currentTime = SDL_GetTicks();
	if (currentTime - lastRefineTick < delayMs) return;

	Inventory* inv = player->GetInventory();
	Item* stone = nullptr;
	
	std::vector<PACKET_WUX_SLOT> packetSlots;
	int itemSlot = 0;
	int stoneSlot = 1;

	pItemAction* itemActMsg;
	pWuxing* msgWux;

	switch (refineMode) {
	case 1: //check for refine item
		std::cout << "Checking for stone." << std::endl;
		stone = inv->findItem("TrueTemprStone", "¡ùRefinery");
		if (!stone) refineMode = 2;
		else refineMode = 4;
		break;
	case 2: //buy item
		std::cout << "Buying stone." << std::endl;
		
		//check if Spirit stones are enough?
		//check for full inv

		receivedItem = false;
		itemActMsg = new pItemAction(refineType, iaBuy);
		gClient.addPacket(itemActMsg);
		refineMode++;
		break;
	case 3: //wait to receive item
		std::cout << "Waiting for stone." << std::endl;
		if (receivedItem) {
			refineMode = 4;
		}
		break;
	case 4: //wux item
		std::cout << "Refining item." << std::endl;
		wuxedItem = false;

		stone = inv->findItem("TrueTemprStone", "¡ùRefinery");
		if (!stone) {
			refineMode = 2;
			//What happened? item didn't exist and we have to restart
		}

		for (int i = 0; i < 8; i++) {
			PACKET_WUX_SLOT pws;
			if (i == itemSlot) {
				pws.type = OBJ_ITEM;
				pws.id = item->GetID();
			}
			else if (i == stoneSlot) {
				pws.type = OBJ_ITEM;
				pws.id = stone->GetID();
			}
			else {
				pws.type = OBJ_NONE;
				pws.id = 0;
			}
			packetSlots.push_back(pws);
		}

		msgWux = new pWuxing(WUX_MODE_START, 0, packetSlots);
		gClient.addPacket(msgWux);
		refineMode++;
		break;
	case 5: //wait for wux confirmation
		std::cout << "Waiting for results." << std::endl;
		if (wuxedItem) {
			refineMode = 6;
		}
		break;
	case 6: //check if item is maxed? always no for now. Restart flow
		std::cout << "Restarting refining process!." << std::endl;
		bool isMaxed = false;

		//if (lastCoreStat != )

		if (!isMaxed) refineMode = 1;
		break;
	}

	lastRefineTick = SDL_GetTicks();
}

CImageBox* CRefineItemForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(40);
	imgBox->SetHeight(40);
	imgBox->UseBlackBackground(true);
	AddWidget(imgBox);
	return imgBox;
}

void CRefineItemForm::UpdateAttr() {
	int val;
	switch (item->getType()) {
	case itWeapon:
		val = item->getAttack();
		break;
	case itArmor:
		val = item->getDefence();
		break;
	case itShoes:
		val = item->getDexterity();
		break;
	case itAccessory:
		val = item->getMana();
		break;
	case itHeadwear:
		val = item->getLife();
		break;
	}
	lblAttr->SetText(std::to_string(val));
}

void CRefineItemForm::UpdateInventory() {
	localInv.clear();
	int slot = 0;
	int nexItem = 0;
	for (int i = 0; i < imgItems.size(); i++) {
		Item* pItem = player->inventory->getItemInSlot(nexItem++);
		if (pItem) {
			if (pItem->GetID() != itemId && pItem->getType() >= itWeapon && pItem->getType() <= itHeadwear) {
				SetItemToImageBox(imgItems[slot++], pItem);
				localInv.push_back(pItem);
			}
		}
		else break;
	}
	for (int i = slot; i < imgItems.size(); i++) {
		SetItemToImageBox(imgItems[i], nullptr);
	}
}

void CRefineItemForm::SetItemToImageBox(CImageBox* imgBox, Item* item) {
	if (!imgBox) return;
	if (!item) {
		imgBox->SetImage(nullptr);
		return;
	}
	Texture *textureItem = new Texture(renderer, item->getTexturePath(40), true);
	imgBox->SetImage(textureItem);
	delete textureItem;
}

void CRefineItemForm::btnBegin_Click(SDL_Event& e) {
	//CloseWindow = true;
	if (!refining) {
		refining = true;
		refineMode = 1;
	}
	else {
		refining = false;
		refineMode = 0;
		lastRefineTick = 0;
	}
}

void CRefineItemForm::imgItem_DoubleClick(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;
	if (pos >= localInv.size()) return;

	Item* clickItem = localInv[pos];
	if (!clickItem) return;

	item = clickItem;
	itemId = item->GetID();
	SetItemToImageBox(imgRefineSlot, item);
	UpdateAttr();
	UpdateInventory();
}

void CRefineItemForm::imgItem_OnHoverStart(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;
	if (pos >= localInv.size()) return;

	Item* hoverItem = localInv[pos];
	if (!hoverItem) return;

	hoverTexture = hoverItem->GetMouseoverTexture(renderer, false, 75);
}

void CRefineItemForm::imgItem_OnHoverEnd(SDL_Event& e) {
	hoverTexture.reset();
}

/*void CRefineItemForm::EquipAccessory(Item* pItem) {
	if (!pItem) return;

	//check level requirement of selected item
	if (pet->GetLevel() < pItem->getLevel()) {
		doPromptError(this, "Error", pet->GetName() + " is not strong enough to equip this accessory.");
		return;
	}

	//An item is already equiped
	if (item) {
		DoUnequip();

		//Unequip, but old item may not have a guaranteed slot if new item wasn't equipable
		//for now lets always assume it can be if it passes level req
	}

	pPetAction *msg = new pPetAction(pet->GetID(), pItem->GetID(), paUseItem);
	gClient.addPacket(msg);
}

void  CRefineItemForm::DoUnequip() {
	pPetAction *msg = new pPetAction(pet->GetID(), 0, paUnequip);
	gClient.addPacket(msg);

	//change item petid to inventory id
	int newId = player->GetNextAvailableItemID();
	if (newId >= 0) item->SetID(newId);

	player->inventory->addItem(item);
	pet->setItem(nullptr);
	if (item->getLife() != 0) customEvent(CUSTOMEVENT_PET, PET_LIFEMAX);
	UpdateAccessory();
	UpdateInventory();

	customEvent(CUSTOMEVENT_PET, PET_UNEQUIP);
}*/