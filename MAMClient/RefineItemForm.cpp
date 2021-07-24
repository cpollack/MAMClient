#include "stdafx.h"
#include "RefineItemForm.h"

#include "ImageBox.h"
#include "Label.h"
#include "Button.h"
#include "Dropdown.h"
#include "Gauge.h"

#include "CustomEvents.h"
#include "Player.h"
#include "Inventory.h"

#include "pItemAction.h"
#include "pWuxing.h"

/* Todo:
	timer gauge
	refine detail message
*/

CRefineItemForm::CRefineItemForm() : CWindow("RefineItemForm.JSON") {
	Type = FT_REFINE;

	HookWidgets();
	UpdateInventory();

	refineMethods.push_back({ "Basic Tempering", "RoughTemprStone", 2300, 100,  4, 1000 });
	refineMethods.push_back({ "Skilled Tempering", "PotntTemprStone", 2301, 375,  5, 1500 });
	refineMethods.push_back({ "Master Tempering", "HeavnTemprStone", 2302, 900,  6, 5000 });
	refineMethods.push_back({ "Godly Tempering", "GodlyTemprStone", 2303, 2500, 7, 10000 });
	for (auto rm : refineMethods) ddType->AddRow(rm.method);

	UpdateItemInfo();
	lblCash->SetText(formatInt(player->GetCash()));
	lblMessage->SetText("Please select a tempering method.");
	gaugeProgress->set(0, 100);
	gaugeProgress->UseCustomLabel(true);
	gaugeProgress->SetLabel("0 / 0 seconds");
}

void CRefineItemForm::HookWidgets() {
	imgInventory = (CImageBox*)GetWidget("imgInventory");
	imgRefine = (CImageBox*)GetWidget("imgRefine");

	imgRefineSlot = createImageBox("imgRefineSlot");
	imgRefineSlot->SetPosition(imgRefine->GetX() + 9, imgRefine->GetY() + 7);
	imgRefineSlot->SetHoverDelay(0);
	registerEvent("imgRefineSlot", "DoubleClick", std::bind(&CRefineItemForm::imgRefineSlot_DoubleClick, this, std::placeholders::_1));
	registerEvent("imgRefineSlot", "OnHoverStart", std::bind(&CRefineItemForm::imgRefineSlot_OnHoverStart, this, std::placeholders::_1));
	registerEvent("imgRefineSlot", "OnHoverEnd", std::bind(&CRefineItemForm::imgRefineSlot_OnHoverEnd, this, std::placeholders::_1));

	lblLevel = (CLabel*)GetWidget("lblLevel");
	lblAttr = (CLabel*)GetWidget("lblAttr");
	lblCash = (CLabel*)GetWidget("lblCash");
	lblMessage = (CLabel*)GetWidget("lblMessage");

	ddType = (CDropDown*)GetWidget("ddType");
	registerEvent("ddType", "Change", std::bind(&CRefineItemForm::ddType_Change, this, std::placeholders::_1));

	btnBegin = (CButton*)GetWidget("btnBegin");
	registerEvent("btnBegin", "Click", std::bind(&CRefineItemForm::btnBegin_Click, this, std::placeholders::_1));

	gaugeProgress = (CGauge*)GetWidget("gaugeProgress");

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
				if (addItem->GetName().compare(method.itemName) == 0 && addItem->GetInventor().compare("¡ùRefinery") == 0) receivedItem = true;
				if (addItem->GetID() == itemId) {
					wuxedItem = true;
					item = addItem;
					SetItemToImageBox(imgRefineSlot, item);
					UpdateItemInfo();
				}
			}
		}
	}

	if (e.type == CUSTOMEVENT_PLAYER) {
		if (e.user.code == PLAYER_MONEY) {
			lblCash->SetText(formatInt(player->GetCash()));
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
	CWindow::step();
	if (!refining) return;

	int currentTime = SDL_GetTicks();
	int progress = 0;
	float fProgress = 0.0;
	std::stringstream s;

	Inventory* inv = player->GetInventory();
	Item* stone = nullptr;
	
	std::vector<PACKET_WUX_SLOT> packetSlots;
	int stoneSlot = 1;

	pItemAction* itemActMsg;
	pWuxing* msgWux;

	switch (refineMode) {
	case 1: //check for refine item
		std::cout << "Checking for stone." << std::endl;
		stone = inv->findItem(method.itemName, "¡ùRefinery");
		if (!stone) refineMode = 2;
		else refineMode = 4;
		lastRefineTick = SDL_GetTicks();
		gaugeProgress->set(0);
		break;
	case 2: //buy item
		if (lastRefineTick > 0 && currentTime - lastRefineTick < (method.temperTime * 1000)) {
			progress = ((float)(currentTime - lastRefineTick) * 1.0) / ((float)method.temperTime * 1000) * 100;
			gaugeProgress->set(progress);
			fProgress = (progress * 1.0 / 100) * method.temperTime;
			s << std::setprecision(1) << std::fixed << fProgress << " / " << method.temperTime << " seconds";
			gaugeProgress->SetLabel(s.str());
			break;
		}
		else {
			lastRefineTick = 0;
			gaugeProgress->set(100);
			gaugeProgress->SetLabel(std::to_string(method.temperTime) + " / " + std::to_string(method.temperTime) + " seconds");
		}
			
		std::cout << "Buying stone." << std::endl;
		
		if (player->GetCash() < method.cost) {
			doPrompt(this, "Warning", "You do not have enough spirit stones to refine.");
			refineMode = 0;
			btnBegin->SetText("Begin");
		}

		receivedItem = false;
		itemActMsg = new pItemAction(method.itemId, iaBuy);
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
		if (lastRefineTick > 0 && currentTime - lastRefineTick < (method.temperTime * 1000)) {
			progress = ((float)(currentTime - lastRefineTick) * 1.0) / ((float)method.temperTime * 1000) * 100;
			gaugeProgress->set(progress);
			fProgress = (progress * 1.0 / 100) * method.temperTime;
			s << std::setprecision(1) << std::fixed << fProgress << " / " << method.temperTime << " seconds";
			gaugeProgress->SetLabel(s.str());
			break;
		}
		else {
			lastRefineTick = 0;
			gaugeProgress->set(100);
			gaugeProgress->SetLabel(std::to_string(method.temperTime) + " / " + std::to_string(method.temperTime) + " seconds");
		}

		std::cout << "Refining item." << std::endl;
		wuxedItem = false;

		stone = inv->findItem(method.itemName, "¡ùRefinery");
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

		msgWux = new pWuxing(WUX_MODE_START, itemSlot, packetSlots);
		gClient.addPacket(msgWux);
		refineMode++;
		wuxCount++;
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

		lastRefineTick = 0;
		if (lastCoreStat != GetCoreAttr()) {
			lastCoreStat = GetCoreAttr();
			refineMode = 1;
		}
		else {
			refineMode = 0;
			btnBegin->SetText("Begin");
			//std::string msg = "Refined a total of " + std::to_string(wuxCount) + " times.";
			//doPrompt(this, "Finished Refining", msg);
			doPrompt(this, "Complete", "The refining process has completed.");
		}
		break;
	}
}

CImageBox* CRefineItemForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(40);
	imgBox->SetHeight(40);
	imgBox->UseBlackBackground(true);
	AddWidget(imgBox);
	return imgBox;
}

int CRefineItemForm::GetCoreAttr() {
	int val = 0;
	switch (item->GetType()) {
	case itWeapon:
		val = item->GetAttack();
		break;
	case itArmor:
		val = item->GetDefence();
		break;
	case itShoes:
		val = item->GetDexterity();
		break;
	case itAccessory:
		val = item->GetMana();
		break;
	case itHeadwear:
		val = item->GetLife();
		break;
	}
	return val;
}

void CRefineItemForm::UpdateItemInfo() {
	if (!item) {
		lblAttr->SetText("");
		lblLevel->SetText("");
		return;
	}
	lblAttr->SetText(formatInt(GetCoreAttr()));
	lblLevel->SetText(formatInt(item->GetLevel()));
}

void CRefineItemForm::UpdateInventory() {
	localInv.clear();
	int slot = 0;
	int nexItem = 0;
	for (int i = 0; i < imgItems.size(); i++) {
		Item* pItem = player->inventory->getItemInSlot(nexItem++);
		if (pItem) {
			if (pItem->GetID() != itemId && pItem->GetType() >= itWeapon && pItem->GetType() <= itHeadwear) {
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

RefineMethod CRefineItemForm::GetMethod(std::string strMethod) {
	for (auto nextMethod : refineMethods) {
		if (nextMethod.method.compare(strMethod) == 0) return nextMethod;
	}

	return { "", "", 0, 0, 0 };
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
	if (refining) {
		refining = false;
		refineMode = 0;
		lastRefineTick = 0;
		btnBegin->SetText("Begin");
		return;
	}

	if (player->GetInventory()->getItemCount() >= 14) {
		doPrompt(this, "Warning", "Your inventory is too full to refine. Please make space first.");
		return;
	}

	std::string strMethod = ddType->GetSelection();
	if (strMethod.empty()) {
		doPrompt(this, "Error", "Please select a refining method");
		return;
	}

	method = GetMethod(strMethod);
	if (method.method.empty()) {
		doPrompt(this, "Error", "The selected method (" + strMethod + ") is not valid. Please choose another.");
		return;
	}
	if (player->GetWuxing() < method.minWuxing) {
		doPrompt(this, "Warning", "Your wuxing knowledge is not high enough to use this refining method.");
		return;
	}

	btnBegin->SetText("Stop");
	refining = true;
	refineMode = 1;
	wuxCount = 0;
	lastCoreStat = GetCoreAttr();
}

void CRefineItemForm::ddType_Change(SDL_Event& e) {
	if (ddType->GetSelection().size() == 0) {
		lblMessage->SetText("Please select a tempering method.");
		return;
	}

	RefineMethod tempMethod = GetMethod(ddType->GetSelection());

	std::stringstream s;
	s << "Consume " << std::to_string(tempMethod.cost) << " spirit stones to create a " << tempMethod.itemName;
	s << " for refining your item. Requires a minimum wuxing knowledge of " << std::to_string(tempMethod.minWuxing) << ".";
	lblMessage->SetText(s.str());
}

void CRefineItemForm::imgItem_DoubleClick(SDL_Event& e) {
	if (refining) return;
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;
	if (pos >= localInv.size()) return;

	Item* clickItem = localInv[pos];
	if (!clickItem) return;

	item = clickItem;
	itemId = item->GetID();
	switch (item->GetType()) {
	case itWeapon:
		itemSlot = 2;
		break;
	case itArmor:
		itemSlot = 0;
		break;
	case itShoes:
		itemSlot = 5;
		break;
	case itAccessory:
		itemSlot = 3;
		break;
	case itHeadwear:
		itemSlot = 7;
		break;
	}

	SetItemToImageBox(imgRefineSlot, item);
	UpdateItemInfo();
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

void CRefineItemForm::imgRefineSlot_DoubleClick(SDL_Event& e) {
	if (refining) return;
	if (!item) return;
	SetItemToImageBox(imgRefineSlot, nullptr);
	item = nullptr;
	itemId = 0;
	UpdateInventory();
}

void CRefineItemForm::imgRefineSlot_OnHoverStart(SDL_Event& e) {
	if (!item) return;
	hoverTexture = item->GetMouseoverTexture(renderer, false, 75);
}

void CRefineItemForm::imgRefineSlot_OnHoverEnd(SDL_Event& e) {
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