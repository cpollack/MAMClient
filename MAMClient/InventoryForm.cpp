#include "stdafx.h"
#include "InventoryForm.h"
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

CInventoryForm::CInventoryForm() : CWindow("InventoryForm.JSON") {
	Type = FT_INVENTORY;

	HookWidgets();
	LoadEquipment();
	LoadViewItem(nullptr);
	UpdateEquipment();
	UpdateInventory();
	SetCash();
}

void CInventoryForm::HookWidgets() {
	imgEquipment = (CImageBox*)GetWidget("imgEquipment");
	imgItem = (CImageBox*)GetWidget("imgItem");
	imgInventory = (CImageBox*)GetWidget("imgInventory");

	lblItemDetail = (CLabel*)GetWidget("lblItemDetail");
	lblCash = (CLabel*)GetWidget("lblCash");

	btnUse = (CButton*)GetWidget("btnUse");
	btnPetUse = (CButton*)GetWidget("btnPetUse");
	btnDrop = (CButton*)GetWidget("btnDrop");
	btnRename = (CButton*)GetWidget("btnRename");
	btnClose = (CButton*)GetWidget("btnClose");
	btnDeposit = (CButton*)GetWidget("btnDeposit");
	btnWithdraw = (CButton*)GetWidget("btnWithdraw");
	btnGive = (CButton*)GetWidget("btnGive");
	registerEvent("btnUse", "Click", std::bind(&CInventoryForm::btnUse_Click, this, std::placeholders::_1));
	registerEvent("btnPetUse", "Click", std::bind(&CInventoryForm::btnPetUse_Click, this, std::placeholders::_1));
	registerEvent("btnDrop", "Click", std::bind(&CInventoryForm::btnDrop_Click, this, std::placeholders::_1));
	registerEvent("btnRename", "Click", std::bind(&CInventoryForm::btnRename_Click, this, std::placeholders::_1));
	registerEvent("btnCloseInv", "Click", std::bind(&CInventoryForm::btnClose_Click, this, std::placeholders::_1));
	registerEvent("btnDeposit", "Click", std::bind(&CInventoryForm::btnDeposit_Click, this, std::placeholders::_1));
	registerEvent("btnWithdraw", "Click", std::bind(&CInventoryForm::btnWithdraw_Click, this, std::placeholders::_1));
	registerEvent("btnGive", "Click", std::bind(&CInventoryForm::btnGive_Click, this, std::placeholders::_1));

	imgWeapon = createImageBox("imgWeapon");
	imgArmor = createImageBox("imgArmor");
	imgShoe = createImageBox("imgShoe");
	imgHeadAccessory = createImageBox("imgHeadAccessory");
	imgBodyAccessory = createImageBox("imgBodyAccessory");
	registerEvent("imgWeapon", "Click", std::bind(&CInventoryForm::imgWeapon_Click, this, std::placeholders::_1));
	registerEvent("imgArmor", "Click", std::bind(&CInventoryForm::imgArmor_Click, this, std::placeholders::_1));
	registerEvent("imgShoe", "Click", std::bind(&CInventoryForm::imgShoe_Click, this, std::placeholders::_1));
	registerEvent("imgHeadAccessory", "Click", std::bind(&CInventoryForm::imgHeadAccessory_Click, this, std::placeholders::_1));
	registerEvent("imgBodyAccessory", "Click", std::bind(&CInventoryForm::imgBodyAccessory_Click, this, std::placeholders::_1));

	std::string boxName = "imgItem";
	int toX = imgInventory->GetX() + 24;
	int toY = imgInventory->GetY() + 24;
	while (imgItems.size() < 16) {
		int count = imgItems.size();
		int next = count + 1;
		std::string widgetName = boxName + (next < 10 ? "0" : "") + std::to_string(next);
		CImageBox *nextItem = createImageBox(widgetName);
		nextItem->SetPosition( toX + (count % 4 * 47), toY + (count / 4 * 47) );
		imgItems.push_back(nextItem);
		registerEvent(widgetName, "Click", std::bind(&CInventoryForm::imgItem_Click, this, std::placeholders::_1));
	}
}

void CInventoryForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.type == CUSTOMEVENT_ITEM) {
		if (e.user.code == ITEM_USE) {
			UpdateInventory();
			if (e.user.data1 && ((Item*)e.user.data1)->getType() <= 4) UpdateEquipment();
		}
		if (e.user.code == ITEM_DROP) {
			UpdateInventory();
		}
	}

	if (e.window.windowID != windowID) return;

	if (e.type = CUSTOMEVENT_WINDOW && e.user.data1 == promptForm) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			if (UsingItem) {
				pItemAction *itemAction = new pItemAction(viewItem->GetID(), iaUse);
				gClient.addPacket(itemAction);
				LoadViewItem(nullptr);
			}
			if (DropItem) {
				pItemAction *itemAction = new pItemAction(viewItem->GetID(), iaDrop);
				gClient.addPacket(itemAction);
				LoadViewItem(nullptr);
			}
		}
		UsingItem = false;
		PetUsingItem = false;
		DropItem = false;
	}
}

CImageBox* CInventoryForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(40);
	imgBox->SetHeight(40);
	imgBox->UseBlackBackground(true);
	AddWidget(imgBox);
	return imgBox;
}

void CInventoryForm::LoadEquipment() {
	std::string strRole = player->PlayerRole;
	if (strRole.length() == 0 || (strRole[0] != 'M' && strRole[0] != 'M')) strRole = "Man01";
	std::string equipBgPath = "GUI/main/" + strRole + ".bmp";
	SDL_Color colorKey = { 49, 66, 107, 255 };
	Texture *equipmentBg = new Texture(renderer, equipBgPath, colorKey, true);
	imgEquipment->SetImage(equipmentBg);
	delete equipmentBg;

	int eqX = imgEquipment->GetX();
	int eqY= imgEquipment->GetY();

	std::vector<SDL_Point> slots = GetEquipmentSlots();
	imgWeapon->SetPosition(eqX + slots[0].x, eqY + slots[0].y);
	imgArmor->SetPosition(eqX + slots[1].x, eqY + slots[1].y);
	imgShoe->SetPosition(eqX + slots[2].x, eqY + slots[2].y);
	imgHeadAccessory->SetPosition(eqX + slots[3].x, eqY + slots[3].y);
	imgBodyAccessory->SetPosition(eqX + slots[4].x, eqY + slots[4].y);
}

std::vector<SDL_Point> CInventoryForm::GetEquipmentSlots() {
	std::string strRole = player->PlayerRole;
	if (strRole.length() == 0 || (strRole[0] != 'M' && strRole[0] != 'M')) strRole = "Man01";

	SDL_Point weapon, armor, shoe, head, body;
	if (player->GetGender() == MALE) {
		int idx = strRole.at(4) - '0';
		switch (idx) {
		case 1:
			weapon = {27,131};
			armor = {75,71};
			shoe = {85,237};
			head = {139,13};
			body = {21,18};
			break;
		case 2:
			weapon = {25,123};
			armor = {83,70};
			shoe = {91,231};
			head = {139,15};
			body = {20,27};
			break;
		case 3:
			weapon = {140,77};
			armor = {46,100};
			shoe = {70,229};
			head = {16,29};
			body = {124,16};
			break;
		case 4:
			weapon = {21,140};
			armor = {89,110};
			shoe = {139,204};
			head = {128,35};
			body = {17,22};
			break;
		case 5:
			weapon = { 107,123 };
			armor = { 77,58 };
			shoe = { 77,243 };
			head = { 140,15 };
			body = { 18,11 };
			break;
		case 6:
			weapon = { 134,137 };
			armor = { 60,91 };
			shoe = { 62,249 };
			head = { 144,13 };
			body = { 7,10 };
			break;
		case 7:
			weapon = { 133,170 };
			armor = { 83,105 };
			shoe = { 43,231 };
			head = { 146,13 };
			body = { 18,15 };
			break;
		case 8:
			weapon = { 8,234 };
			armor = { 100,114 };
			shoe = { 146,245 };
			head = { 149,57 };
			body = { 48,34 };
			break;
		}
	}
	else {
		int idx = strRole.at(6) - '0';
		switch (idx) {
		case 1:
			weapon = { 142,125 };
			armor = { 63,109 };
			shoe = { 53,205 };
			head = { 124,26 };
			body = { 21,41 };
			break;
		case 2:
			weapon = { 149,150 };
			armor = { 71,98 };
			shoe = { 46,223 };
			head = { 128,19 };
			body = { 14,41 };
			break;
		case 3:
			weapon = { 46,75 };
			armor = { 83,128 };
			shoe = { 77,223 };
			head = { 141,32 };
			body = { 26,19 };
			break;
		case 4:
			weapon = { 31,169 };
			armor = { 90,115 };
			shoe = { 92,223 };
			head = { 144,18 };
			body = { 22,24 };
			break;
		case 5:
			weapon = {43,146};
			armor = {80,76};
			shoe = {81,251};
			head = {140,15};
			body = {10,49};
			break;
		case 6:
			weapon = {72,179};
			armor = {104,102};
			shoe = {106,244};
			head = {145,13};
			body = {15,18};
			break;
		case 7:
			weapon = {45,158};
			armor = {88,103};
			shoe = {81,240};
			head = {149,19};
			body = {17,14};
			break;
		case 8:
			weapon = {41,77};
			armor = {90,120};
			shoe = {147,243};
			head = {14,12};
			body = {143,30};
			break;
		}
	}
	std::vector<SDL_Point> slots;
	slots.push_back(weapon);
	slots.push_back(armor);
	slots.push_back(shoe);
	slots.push_back(head);
	slots.push_back(body);
	return slots;
}

void CInventoryForm::SetCash() {
	lblCash->SetText(formatInt(player->GetCash()));
}

void CInventoryForm::LoadViewItem(Item* item) {
	viewItem = item;
	if (!item) {
		imgItem->SetImage(nullptr);
		lblItemDetail->SetText("");
		return;
	}

	Texture *textureItem = new Texture(renderer, item->getTexturePath(85), true);
	imgItem->SetImage(textureItem);
	delete textureItem;

	lblItemDetail->SetText(item->getDetails());
}

void CInventoryForm::UpdateEquipment() {
	SetItemToImageBox(imgWeapon, player->GetWeapon());
	SetItemToImageBox(imgArmor, player->GetArmor());
	SetItemToImageBox(imgShoe, player->GetShoe());
	SetItemToImageBox(imgBodyAccessory, player->GetBodyAccessory());
	SetItemToImageBox(imgHeadAccessory, player->GetHeadAccessory());
}

void CInventoryForm::UpdateInventory() {
	for (int i = 0; i < imgItems.size(); i++) {
		SetItemToImageBox(imgItems[i], player->inventory->getItemInSlot(i));
	}
}

void CInventoryForm::SetItemToImageBox(CImageBox* imgBox, Item* item) {
	if (!imgBox) return;
	if (!item) {
		imgBox->SetImage(nullptr);
		return;
	}
	Texture *textureItem = new Texture(renderer, item->getTexturePath(40), true);
	imgBox->SetImage(textureItem);
	delete textureItem;
}

void CInventoryForm::btnUse_Click(SDL_Event& e) {
	pItemAction *itemAction = new pItemAction(viewItem->GetID(), iaUse);
	gClient.addPacket(itemAction);
	LoadViewItem(nullptr);
}

void CInventoryForm::btnPetUse_Click(SDL_Event& e) {
	if (!viewItem) return;

	if (viewItem->getType() == itMedicine) {
		pPetAction* itemAction = new pPetAction(player->getActivePet()->GetID(), viewItem->GetID(), paUseItem);
		gClient.addPacket(itemAction);
		LoadViewItem(nullptr);
	}
}

void CInventoryForm::btnDrop_Click(SDL_Event& e) {
	if (!viewItem) return;

	std::string message = "Are you sure you wish to Drop ";
	message += "[" + viewItem->getName() + "]?";
	promptForm = doPrompt(this, "Confirm", message, true);

	DropItem = true;
}

void CInventoryForm::btnRename_Click(SDL_Event& e) {

}

void CInventoryForm::btnCloseInv_Click(SDL_Event& e) {
	CWindow::btnClose_Click(e);
}

void CInventoryForm::btnDeposit_Click(SDL_Event& e) {

}

void CInventoryForm::btnWithdraw_Click(SDL_Event& e) {

}

void CInventoryForm::btnGive_Click(SDL_Event& e) {

}

void CInventoryForm::imgWeapon_Click(SDL_Event& e) {
	Item* item = player->GetWeapon();
	if (!item) return;
	LoadViewItem(item);

	if (e.button.clicks == 2) UnequipItem(item);
}

void CInventoryForm::imgArmor_Click(SDL_Event& e) {
	Item* item = player->GetArmor();
	if (!item) return;
	LoadViewItem(item);

	if (e.button.clicks == 2) UnequipItem(item);
}

void CInventoryForm::imgShoe_Click(SDL_Event& e) {
	Item* item = player->GetShoe();
	if (!item) return;
	LoadViewItem(item);

	if (e.button.clicks == 2) UnequipItem(item);
}

void CInventoryForm::imgHeadAccessory_Click(SDL_Event& e) {
	Item* item = player->GetHeadAccessory();
	if (!item) return;
	LoadViewItem(item);

	if (e.button.clicks == 2) UnequipItem(item);
}

void CInventoryForm::imgBodyAccessory_Click(SDL_Event& e) {
	Item* item = player->GetBodyAccessory();
	if (!item) return;
	LoadViewItem(item);

	if (e.button.clicks == 2) UnequipItem(item);
}

void CInventoryForm::UnequipItem(Item* item) {
	if (!item) return;
	if (player->inventory->getItemCount() >= 15) {
		doPromptError(this, "Error", "You cannot unequip your item because your inventory is full!");
		return;
	}

	pItemAction *itemAction = new pItemAction(item->GetID(), iaUnequip);
	gClient.addPacket(itemAction);

	player->unequipItem(item);

	UpdateEquipment();
	UpdateInventory();
}

void CInventoryForm::imgItem_Click(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7,std::string::npos)) - 1;

	Item* item = player->inventory->getItemInSlot(pos);
	if (!item) return;
	LoadViewItem(item);

	//Double Click
	if (e.button.clicks == 2) UseItem(item);
}

void CInventoryForm::UseItem(Item* item) {
	if (!item) return;

	UsingItem = true;

	std::string message = "Are you sure you wish to ";
	if (item->getType() <= 4) message += "Equip ";
	else message += "Use ";
	message += "[" + item->getName() + "]?";
	promptForm = doPrompt(this, "Confirm", message, true);
}
