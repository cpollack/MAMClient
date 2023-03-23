#include "stdafx.h"
#include "WuxingForm.h"
#include "CustomEvents.h"

#include "Sprite.h"
#include "ListBox.h"
#include "ImageBox.h"
#include "Button.h"
#include "Label.h"

#include "Player.h"
#include "Pet.h"
#include "Inventory.h"
#include "Item.h"

#include "pWuxing.h"

#include "PromptForm.h"

CWuxingForm::CWuxingForm() : CWindow("WuxingForm.JSON") {
	Type = FT_WUXING;

	HookWidgets();
	setMainIngredient(0);
	LoadPreviewItem(nullptr);

	LoadItemsList();
	LoadPetsList();
	CreateSprites();
}

void CWuxingForm::render() {
	CWindow::render();

	if (StartAnimation) {
		fire->render();
		diagram->render();

		if (fireSlot >= 0) {
			if (fire->completed) {
				ClearSlot(fireSlot);
				fireSlot = -1;
				if (GetOvenObjectCount() > 0) diagram->start();
				else {
					StartAnimation = false;
					SetPreviewObject((Item*)nullptr);
					LoadItemsList();
					LoadPetsList();
				}
			}
		}
		else {
			if (diagram->completed) {
				fireSlot = GetNextFilledSlot(true);
				if (fireSlot < 0) fireSlot = GetNextFilledSlot(false);
				if (fireSlot >= 0) {
					fire->setLocation(SDL_Point{ imgOven->GetX() + slotPoint[fireSlot].x, imgOven->GetY() + slotPoint[fireSlot].y });
					fire->start();
				}
				else {
					StartAnimation = false;
					SetPreviewObject((Item*)nullptr);
					LoadItemsList();
					LoadPetsList();
				}
			}
		}
	}
	astMainPointer->Render(); //Render ontop of possible spinning diagram

	if (!mouseFocus) return; //Mouse is over window

	int mx, my;
	SDL_GetMouseState(&mx, &my);

	if (dragTexture) {
		SDL_Rect dragRect = dragTexture->rect;
		dragRect.x = mx; dragRect.y = my;
		if (dragRect.y + dragRect.h > Height) dragRect.y = Height - dragRect.h;
		SDL_RenderCopy(renderer, dragTexture->getTexture(), nullptr, &dragRect);
	}

	if (hoverTexture && !dragTexture) {
		SDL_Rect hoverRect = hoverTexture->rect;
		hoverRect.x = mx; hoverRect.y = my;
		if (hoverRect.y + hoverRect.h > Height) hoverRect.y = Height - hoverRect.h;
		SDL_RenderCopy(renderer, hoverTexture->getTexture(), nullptr, &hoverRect);
	}
}

void CWuxingForm::handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_ITEM) {
		if (e.user.code == ITEM_PREVIEW) {
			SetPreviewObject((Item*)e.user.data1);
		}
		if (e.user.code == ITEM_ADD || e.user.code == ITEM_REMOVE) {
			if (!StartAnimation) LoadItemsList();
		}
	}
	if (e.type == CUSTOMEVENT_PET) {
		if (e.user.code == PET_PREVIEW) {
			SetPreviewObject((Pet*)e.user.data1);
		}
		if (e.user.code == PET_ADD || e.user.code == PET_REMOVE) {
			if (!StartAnimation) LoadPetsList();
		}
	}

	CWindow::handleEvent(e);
}

void CWuxingForm::HookWidgets() {
	listItems = (CListBox*)GetWidget("listItems");
	listItems->SetHoverDelay(200);
	listPets = (CListBox*)GetWidget("listPets");
	listPets->SetHoverDelay(200);

	imgOven = (CImageBox*)GetWidget("imgOven");

	imgPreview = (CImageBox*)GetWidget("imgPreview");
	lblPreview = (CLabel*)GetWidget("lblPreview");

	btnStart = (CButton*)GetWidget("btnStart");
	registerEvent("btnStart", "Click", std::bind(&CWuxingForm::btnStart_Click, this, std::placeholders::_1));
	
	std::string boxName = "imgSlot";
	int toX = imgOven->GetX();
	int toY = imgOven->GetY();
	for (int i = 0; i < 8; i++) {
		std::string widgetName = boxName + std::to_string(i);
		CImageBox *nextSlot = createImageBox(widgetName);
		nextSlot->SetPosition(toX + slotPoint[i].x, toY + slotPoint[i].y);
		slots[i] = nextSlot;
		registerEvent(widgetName, "DoubleClick", std::bind(&CWuxingForm::Slot_DoubleClick, this, std::placeholders::_1));
		registerEvent(widgetName, "RightClick", std::bind(&CWuxingForm::Slot_RightClick, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverStart", std::bind(&CWuxingForm::Slot_OnHoverStart, this, std::placeholders::_1));
		registerEvent(widgetName, "OnHoverEnd", std::bind(&CWuxingForm::Slot_OnHoverEnd, this, std::placeholders::_1));
		registerEvent(widgetName, "OnDragStart", std::bind(&CWuxingForm::Slot_OnDragStart, this, std::placeholders::_1));
		registerEvent(widgetName, "OnDragEnd", std::bind(&CWuxingForm::Slot_OnDragEnd, this, std::placeholders::_1));
	}

	registerEvent("listItems", "OnListItemHoverStart", std::bind(&CWuxingForm::listItems_OnListItemHoverStart, this, std::placeholders::_1));
	registerEvent("listItems", "OnListItemHoverEnd", std::bind(&CWuxingForm::listItems_OnListItemHoverEnd, this, std::placeholders::_1));
	registerEvent("listItems", "OnItemDragStart", std::bind(&CWuxingForm::listItems_OnDragStart, this, std::placeholders::_1));
	registerEvent("listItems", "OnItemDragEnd", std::bind(&CWuxingForm::listItems_OnDragEnd, this, std::placeholders::_1));
	registerEvent("listPets", "OnListItemHoverStart", std::bind(&CWuxingForm::listPets_OnListItemHoverStart, this, std::placeholders::_1));
	registerEvent("listPets", "OnListItemHoverEnd", std::bind(&CWuxingForm::listPets_OnListItemHoverEnd, this, std::placeholders::_1));
	registerEvent("listPets", "OnItemDragStart", std::bind(&CWuxingForm::listPets_OnDragStart, this, std::placeholders::_1));
	registerEvent("listPets", "OnItemDragEnd", std::bind(&CWuxingForm::listPets_OnDragEnd, this, std::placeholders::_1));
}

CImageBox* CWuxingForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(50);
	imgBox->SetHeight(50);
	imgBox->SetHoverDelay(200);
	AddWidget(imgBox);
	return imgBox;
}

void CWuxingForm::LoadItemsList() {
	localInventory.clear();
	listItems->ClearList();

	int index = 0;
	for (int i = 0; i < player->GetInventory()->getItemCount(); i++) {
		Item *pItem = player->GetInventory()->getItemInSlot(i);
		if (!pItem) continue;

		bool skipItem = false;
		for (int j = 0; j < 8; j++) {
			if (wuxingSlot[j].object == pItem) {
				skipItem = true;
				break;
			}
		}
		if (skipItem) continue;

		localInventory.push_back(pItem);
		listItems->AddItem(localInventory[index++]->GetName());
	}
}

void CWuxingForm::LoadPetsList() {
	localPets.clear();
	listPets->ClearList();

	int index = 0;
	for (int i = 0; i < player->getPetList().size(); i++) {
		Pet *pPet = player->getPetList().at(i);
		if (!pPet) continue;
		if (player->GetMarchingPet() == pPet) continue;

		bool skipPet = false;
		for (int j = 0; j < 8; j++) {
			if (wuxingSlot[j].object == pPet) {
				skipPet = true;
				break;
			}
		}
		if (skipPet) continue;

		localPets.push_back(pPet);
		listPets->AddItem(localPets[index++]->GetName());
	}
}

void CWuxingForm::CreateSprites() {
	std::vector<std::string> files;

	for (int i = 0; i < 14; i++) {
		std::string fileName = "data/stove/fire" + std::to_string(i) + ".tga";
		files.push_back(fileName);
	}
	fire = new Sprite(renderer, files, stAni);
	fire->setFrameInterval(20);
	fire->repeatMode = 1;

	files.clear();
	for (int i = 0; i < 8; i++) {
		std::string fileName = "data/stove/diagrams" + std::to_string(i) + ".tga";
		files.push_back(fileName);
	}
	diagram = new Sprite(renderer, files, stAni);
	diagram->setFrameInterval(20);
	diagram->repeatMode = 1;
	diagram->setLocation(SDL_Point{ imgOven->GetX() + 75, imgOven->GetY() + 79 });
}

void CWuxingForm::setMainIngredient(int iSlot) {
	if (mainIngredient == iSlot) return;
	if (iSlot < 0 || iSlot > 7) return;

	mainIngredient = iSlot;

	std::string filePath = "data/stove/point" + std::to_string(mainIngredient) + ".tga";
	astMainPointer.reset(new Texture(renderer, filePath, true));
	SDL_Point pointIng = { imgOven->GetX() + centerPoint.x - mainPoint[mainIngredient].x, imgOven->GetY() + centerPoint.y - mainPoint[mainIngredient].y };
	astMainPointer->setPosition(pointIng);
}

void CWuxingForm::LoadPreviewItem(Item* item) {
	if (!item) {
		imgPreview->SetImage(nullptr);
		lblPreview->SetText("");
		return;
	}

	Texture *textureItem = new Texture(renderer, item->getTexturePath(40), true);
	imgPreview->SetImage(textureItem);
	delete textureItem;

	lblPreview->SetText(item->getDetails());
}

int CWuxingForm::GetMouseOverSlot() {
	std::string boxName = "imgSlot";
	for (int i = 0; i < 8; i++) {
		CWidget *widget = GetWidget(boxName + std::to_string(i));
		if (!widget) continue;

		CImageBox *imgSlot = (CImageBox*)widget;
		if (imgSlot->IsMouseOver()) return i;
	}
	return -1;
}

CImageBox* CWuxingForm::GetMouseOverSlotWidget() {
	std::string boxName = "imgSlot";
	for (int i = 0; i < 8; i++) {
		CWidget *widget = GetWidget(boxName + std::to_string(i));
		if (!widget) continue;

		CImageBox *imgSlot = (CImageBox*)widget;
		if (imgSlot->IsMouseOver()) return imgSlot;
	}
	return nullptr;
}

int CWuxingForm::GetSlotByObject(void* object) {
	if (!object) return -1;
	for (int i = 0; i < 8; i++) {
		if (wuxingSlot[i].object == object) return i;
	}
	return -1;
}

CImageBox* CWuxingForm::GetSlotWidgetByPos(int pos) {
	if (pos < 0 || pos >= 8) return nullptr;

	std::string widgetName = "imgSlot" + std::to_string(pos);
	return (CImageBox*)GetWidget(widgetName);
}

int CWuxingForm::GetOvenObjectCount() {
	int count = 0;
	for (int i = 0; i < 8; i++) if (wuxingSlot[i].object) count++;
	return count;
}

int CWuxingForm::GetNextFilledSlot(bool skipMain) {
	for (int i = 0; i < 8; i++) {
		if (wuxingSlot[i].object) {
			if (!skipMain || (skipMain && i != mainIngredient)) return i;
		}
	}
	return -1;
}

void CWuxingForm::ClearSlot(int slot) {
	if (slot < 0 || slot >= 8) return;
	wuxingSlot[slot].type = OBJ_NONE;
	wuxingSlot[slot].object = nullptr;

	CImageBox *imgSlot = GetSlotWidgetByPos(slot);
	if (imgSlot) imgSlot->UseBlackBackground(false);
}

void CWuxingForm::SendWux() {
	std::vector<PACKET_WUX_SLOT> packetSlots;
	for (int i = 0; i < 8; i++) {
		PACKET_WUX_SLOT pws;
		pws.type = wuxingSlot[i].type;
		if (pws.type == OBJ_ITEM) pws.id = ((Item*)wuxingSlot[i].object)->GetID();
		else if (pws.type == OBJ_PET) pws.id = ((Pet*)wuxingSlot[i].object)->GetID();
		else pws.id = 0;

		packetSlots.push_back(pws);
	}

	pWuxing *packet = new pWuxing((int)WUX_MODE_START, mainIngredient, packetSlots);
	gClient.addPacket(packet);
}

void CWuxingForm::RequestPreview() {
	bool doPreview = true;
	if (GetOvenObjectCount() < 2) doPreview = false;
	if (!wuxingSlot[mainIngredient].object) doPreview = false;

	if (!doPreview) {
		SetPreviewObject((Item*)nullptr);
		return;
	}

	//send preview packet
	std::vector<PACKET_WUX_SLOT> packetSlots;
	for (int i = 0; i < 8; i++) {
		PACKET_WUX_SLOT pws;
		pws.type = wuxingSlot[i].type;
		if (pws.type == OBJ_ITEM) pws.id = ((Item*)wuxingSlot[i].object)->GetID();
		else if (pws.type == OBJ_PET) pws.id = ((Pet*)wuxingSlot[i].object)->GetID();
		else pws.id = 0;

		packetSlots.push_back(pws);
	}

	pWuxing *packet = new pWuxing((int)WUX_MODE_PREVIEW, mainIngredient, packetSlots);
	gClient.addPacket(packet);
}

void CWuxingForm::SetPreviewObject(Item* item) {
	if (previewObject.object) {
		if (previewObject.type == OBJ_ITEM) delete (Item*)previewObject.object;
		if (previewObject.type == OBJ_PET) delete (Pet*)previewObject.object;
		previewObject.type = OBJ_NONE;
		previewObject.object = nullptr;

		imgPreview->SetImageFromFile("");
		imgPreview->UseBlackBackground(true);
		lblPreview->SetText(" ");
	}
	previewObject.type = OBJ_ITEM;
	previewObject.object = item;
	if (!item) return;

	imgPreview->UseBlackBackground(true);
	imgPreview->SetImageFromFile(((Item*)previewObject.object)->getTexturePath(85));
	lblPreview->SetText(((Item*)previewObject.object)->getDetails());
}

void CWuxingForm::SetPreviewObject(Pet* pet) {
	if (previewObject.object) {
		if (previewObject.type == OBJ_ITEM) delete (Item*)previewObject.object;
		if (previewObject.type == OBJ_PET) delete (Pet*)previewObject.object;
		
		previewObject.type = OBJ_NONE;
		previewObject.object = nullptr;

		imgPreview->SetImageFromFile("");
		imgPreview->UseBlackBackground(true);
		lblPreview->SetText(" ");
	}
	previewObject.type = OBJ_PET;
	previewObject.object = pet;
	if (!pet) return;

	//Name is not filled by packet, so set it to from main ingredient pet
	pet->SetName(((Pet*)wuxingSlot[mainIngredient].object)->GetName());

	imgPreview->UseBlackBackground(true);
	imgPreview->SetImageFromFile(((Pet*)previewObject.object)->getPortraitPath());
	lblPreview->SetText(((Pet*)previewObject.object)->getDetails());
}

void CWuxingForm::btnStart_Click(SDL_Event& e) {
	int count = GetOvenObjectCount();
	if (count < 2) {
		doPromptError(this, "Warning", "You must have at least two stove slots filled to wux.");
		return;
	}
	if (!wuxingSlot[mainIngredient].object) {
		doPromptError(this, "Warning", "You must select a main ingredient.");
		return;
	}

	diagram->start();
	StartAnimation = true;
	SendWux();
}

void CWuxingForm::listItems_OnListItemHoverStart(SDL_Event& e) {
	int hoverItem = listItems->GetMouseOverItem();
	if (hoverItem < 0 || hoverItem >= localInventory.size()) return;

	hoverObject.type = OBJ_ITEM; 
	hoverObject.object = localInventory[hoverItem];
	hoverTexture = ((Item*)hoverObject.object)->GetMouseoverTexture(renderer, true);
}

void CWuxingForm::listItems_OnListItemHoverEnd(SDL_Event& e) {
	hoverObject.type = OBJ_NONE;
	hoverObject.object = nullptr;
	hoverTexture.reset();
}

void CWuxingForm::listPets_OnListItemHoverStart(SDL_Event& e) {
	int hoverItem = listPets->GetMouseOverItem();
	if (hoverItem < 0 || hoverItem >= localPets.size()) return;

	hoverObject.type = OBJ_PET;
	hoverObject.object = localPets[hoverItem];
	hoverTexture = ((Pet*)hoverObject.object)->GetMouseoverTexture(renderer, true);
}

void CWuxingForm::listPets_OnListItemHoverEnd(SDL_Event& e) {
	hoverObject.type = OBJ_NONE;
	hoverObject.object = nullptr;
	hoverTexture.reset();
}

void CWuxingForm::listItems_OnDragStart(SDL_Event& e) {
	int dragItem = listItems->GetSelectedIndex();
	if (dragItem < 0 || dragItem >= localInventory.size()) return;

	Item* pItem = localInventory[dragItem];
	if (!pItem) return;

	dragObject.type = OBJ_ITEM;
	dragObject.object = pItem;
	dragTexture.reset(new Texture(renderer, pItem->getTexturePath(40), true));
}

void CWuxingForm::listItems_OnDragEnd(SDL_Event& e) {
	CImageBox *imgSlot = GetMouseOverSlotWidget();
	if (imgSlot) {
		int slot = GetMouseOverSlot();
		if (slot >= 0) {
			wuxingSlot[slot] = dragObject;
			if (GetOvenObjectCount() == 1) setMainIngredient(slot);
			RequestPreview();

			imgSlot->UseBlackBackground(true);
			imgSlot->SetImage(new Texture(renderer, ((Item*)dragObject.object)->getTexturePath(40), true));

			LoadItemsList();
		}
	}
	
	dragObject.type = OBJ_NONE;
	dragObject.object = nullptr;
	dragTexture.reset();
}

void CWuxingForm::listPets_OnDragStart(SDL_Event& e) {
	int dragPet = listPets->GetSelectedIndex();
	if (dragPet < 0 || dragPet >= localPets.size()) return;

	Pet* pPet = localPets[dragPet];
	if (!pPet) return;

	dragObject.type = OBJ_PET;
	dragObject.object = pPet;
	dragTexture.reset(new Texture(renderer, pPet->getPortraitPath(), true));
}

void CWuxingForm::listPets_OnDragEnd(SDL_Event& e) {
	CImageBox *imgSlot = GetMouseOverSlotWidget();
	if (imgSlot) {
		int slot = GetMouseOverSlot();
		if (slot >= 0) {
			wuxingSlot[slot] = dragObject;
			if (GetOvenObjectCount() == 1) setMainIngredient(slot);
			RequestPreview();

			imgSlot->UseBlackBackground(true);
			imgSlot->SetImage(new Texture(renderer, ((Pet*)dragObject.object)->getPortraitPath(), true));

			LoadPetsList();
		}
	}

	dragObject.type = OBJ_NONE;
	dragObject.object = nullptr;
	dragTexture.reset();
}

void CWuxingForm::Slot_DoubleClick(SDL_Event& e) {
	int slot = GetMouseOverSlot();
	if (slot >= 0) {
		ClearSlot(slot);
	}

	RequestPreview();
	LoadItemsList();
	LoadPetsList();
}

void CWuxingForm::Slot_RightClick(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos));
	if (wuxingSlot[pos].object) {
		setMainIngredient(pos);
		RequestPreview();
	}
}

void CWuxingForm::Slot_OnHoverStart(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos));
	if (wuxingSlot[pos].object) {
		hoverObject.type = wuxingSlot[pos].type;
		hoverObject.object = wuxingSlot[pos].object;
		if (hoverObject.type == OBJ_ITEM) hoverTexture = ((Item*)hoverObject.object)->GetMouseoverTexture(renderer, true);
		else if (hoverObject.type == OBJ_PET) hoverTexture = ((Pet*)hoverObject.object)->GetMouseoverTexture(renderer, true);
	}
}

void CWuxingForm::Slot_OnHoverEnd(SDL_Event& e) {
	hoverObject.type = OBJ_NONE;
	hoverObject.object = nullptr;
	hoverTexture.reset();
}

void CWuxingForm::Slot_OnDragStart(SDL_Event& e) {
 	int pos = stoi(eventWidget->Name.substr(7, std::string::npos));
	if (wuxingSlot[pos].object) {
		dragObject.type = wuxingSlot[pos].type;
		dragObject.object = wuxingSlot[pos].object;
		if (dragObject.type == OBJ_ITEM) dragTexture.reset(new Texture(renderer, ((Item*)dragObject.object)->getTexturePath(40), true));
		else if (dragObject.type == OBJ_PET) dragTexture.reset(new Texture(renderer, ((Pet*)dragObject.object)->getPortraitPath(), true));
	}
}

void CWuxingForm::Slot_OnDragEnd(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos));
	bool changeMain = false;
	if (pos == mainIngredient) changeMain = true;
	if (listItems->IsMouseOver() || listPets->IsMouseOver()) {
		ClearSlot(pos);
		LoadItemsList();
		LoadPetsList();
		RequestPreview();
	}
	else {
		int overPos = -1;
		std::string boxName = "imgSlot";
		for (int i = 0; i < 8; i++) {
			std::string widgetName = boxName + std::to_string(i);
			CWidget *widget = GetWidget(widgetName);
			if (widget) {
				if (widget->IsMouseOver()) {
					overPos = i;
					break;
				}
			}
		}

		if (overPos >= 0) {
			WuxingSlot tempSlot = wuxingSlot[overPos];
			wuxingSlot[overPos] = wuxingSlot[pos];
			wuxingSlot[pos] = tempSlot;

			CImageBox *oldSlot = GetSlotWidgetByPos(pos);
			if (wuxingSlot[pos].object) {
				oldSlot->UseBlackBackground(true);
				if (wuxingSlot[pos].type == OBJ_ITEM) oldSlot->SetImage(new Texture(renderer, ((Item*)wuxingSlot[pos].object)->getTexturePath(40), true));
				else if (wuxingSlot[pos].type == OBJ_PET) oldSlot->SetImage(new Texture(renderer, ((Pet*)wuxingSlot[pos].object)->getPortraitPath(), true));
			}
			else {
				oldSlot->UseBlackBackground(false);
			}

			CImageBox *newSlot = GetSlotWidgetByPos(overPos);
			if (wuxingSlot[overPos].object) {
				newSlot->UseBlackBackground(true);
				if (wuxingSlot[overPos].type == OBJ_ITEM) newSlot->SetImage(new Texture(renderer, ((Item*)wuxingSlot[overPos].object)->getTexturePath(40), true));
				else if (wuxingSlot[overPos].type == OBJ_PET)newSlot->SetImage(new Texture(renderer, ((Pet*)wuxingSlot[overPos].object)->getPortraitPath(), true));
			}
			else {
				newSlot->UseBlackBackground(false);
			}
			if (changeMain) setMainIngredient(overPos);
			RequestPreview();
		}
	}
	
	dragObject.type = OBJ_NONE;
	dragObject.object = nullptr;
	dragTexture.reset();
}