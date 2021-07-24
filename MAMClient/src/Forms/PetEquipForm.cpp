#include "stdafx.h"
#include "PetEquipForm.h"
#include "CustomEvents.h"

#include "ImageBox.h"
#include "Label.h"

#include "Player.h"
#include "Pet.h"
#include "Inventory.h"
#include "Item.h"

#include "pItemAction.h"
#include "pPetAction.h"

#include "PromptForm.h"

CPetEquipForm::CPetEquipForm(Pet* pPet) : CWindow("PetEquipForm.JSON") {
	Type = FT_INVENTORY;
	pet = pPet;

	SetTitle("Change Accessory for " + pet->GetName());

	HookWidgets();
	UpdateAccessory();
	UpdateInventory();
}

void CPetEquipForm::HookWidgets() {
	imgInventory = (CImageBox*)GetWidget("imgInventory");
	imgAccessory = (CImageBox*)GetWidget("imgAccessory");

	lblAccessory = (CLabel*)GetWidget("lblAccessory");

	btnUnequip = (CButton*)GetWidget("btnUnequip");
	btnOK = (CButton*)GetWidget("btnOK");
	registerEvent("btnUnequip", "Click", std::bind(&CPetEquipForm::btnUnequip_Click, this, std::placeholders::_1));
	registerEvent("btnOK", "Click", std::bind(&CPetEquipForm::btnOK_Click, this, std::placeholders::_1));

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
		registerEvent(widgetName, "DoubleClick", std::bind(&CPetEquipForm::imgItem_DoubleClick, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverStart", std::bind(&CPetEquipForm::imgItem_OnHoverStart, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverEnd", std::bind(&CPetEquipForm::imgItem_OnHoverEnd, this, std::placeholders::_1));
	}
}

void CPetEquipForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.type == CUSTOMEVENT_ITEM) {
		if (e.user.code == ITEM_USE) {
			item = pet->getItem();
			if (item) player->inventory->removeItem(item->GetID(), false);
			UpdateInventory();
			UpdateAccessory();
		}
		if (e.user.code == ITEM_ADD) {
			UpdateInventory();
		}
	}

	if (e.window.windowID != windowID) return;
}

void CPetEquipForm::render() {
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

CImageBox* CPetEquipForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(40);
	imgBox->SetHeight(40);
	imgBox->UseBlackBackground(true);
	AddWidget(imgBox);
	return imgBox;
}

void CPetEquipForm::UpdateAccessory() {
	if (!pet) return;

	item = pet->getItem();
	if (!item) {
		imgAccessory->SetImage(nullptr);
		lblAccessory->SetText(" ");
		return;
	}

	Texture *textureItem = new Texture(renderer, item->getTexturePath(85), true);
	imgAccessory->SetImage(textureItem);
	delete textureItem;
	lblAccessory->SetText(item->getDetails());
}

void CPetEquipForm::UpdateInventory() {
	localInv.clear();
	int slot = 0;
	int nexItem = 0;
	for (int i = 0; i < imgItems.size(); i++) {
		Item* pItem = player->inventory->getItemInSlot(nexItem++);
		if (pItem) {
			if (pItem->GetType() == itAccessory) {
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

void CPetEquipForm::SetItemToImageBox(CImageBox* imgBox, Item* item) {
	if (!imgBox) return;
	if (!item) {
		imgBox->SetImage(nullptr);
		return;
	}
	Texture *textureItem = new Texture(renderer, item->getTexturePath(40), true);
	imgBox->SetImage(textureItem);
	delete textureItem;
}

void CPetEquipForm::btnUnequip_Click(SDL_Event& e) {
	if (player->inventory->getItemCount() >= 15) {
		doPromptError(this, "Unable to Unequip", "You are unable to unequip this accessory due to a full inventory");
		return;
	}
	if (!item) {
		doPromptError(this, "Unable to Unequip", pet->GetName() + " does not have an accessory equipped.");
		return;
	}

	DoUnequip();
}

void CPetEquipForm::btnOK_Click(SDL_Event& e) {
	CloseWindow = true;
}

void CPetEquipForm::imgItem_DoubleClick(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;
	if (pos >= localInv.size()) return;

	Item* clickItem = localInv[pos];
	if (!clickItem) return;

	EquipAccessory(clickItem);
}

void CPetEquipForm::imgItem_OnHoverStart(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;
	if (pos >= localInv.size()) return;

	Item* hoverItem = localInv[pos];
	if (!hoverItem) return;

	hoverTexture = hoverItem->GetMouseoverTexture(renderer, false, 75);
}

void CPetEquipForm::imgItem_OnHoverEnd(SDL_Event& e) {
	hoverTexture.reset();
}

void CPetEquipForm::EquipAccessory(Item* pItem) {
	if (!pItem) return;

	//check level requirement of selected item
	if (pet->GetLevel() < pItem->GetLevel()) {
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

void  CPetEquipForm::DoUnequip() {
	pPetAction *msg = new pPetAction(pet->GetID(), 0, paUnequip);
	gClient.addPacket(msg);

	//change item petid to inventory id
	int newId = player->GetNextAvailableItemID();
	if (newId >= 0) item->SetID(newId);

	player->inventory->addItem(item);
	pet->setItem(nullptr);
	if (item->GetLife() != 0) customEvent(CUSTOMEVENT_PET, PET_LIFEMAX);
	UpdateAccessory();
	UpdateInventory();

	customEvent(CUSTOMEVENT_PET, PET_UNEQUIP);
}