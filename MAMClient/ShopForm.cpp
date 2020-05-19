#include "stdafx.h"
#include "ShopForm.h"
#include "CustomEvents.h"

#include "ListBox.h"
#include "ImageBox.h"
#include "Button.h"
#include "Label.h"

#include "Player.h"
#include "Pet.h"
#include "Inventory.h"
#include "Item.h"

#include "ShopDataFile.h"
#include "ItemDataFile.h"

#include "pItemAction.h"

#include "PromptForm.h"

CShopForm::CShopForm() : CWindow("ShopForm.JSON") {
	Type = FT_SHOP;

	HookWidgets();
	LoadViewItem(nullptr, false);
	UpdateInventory();
	SetCash();
}

void CShopForm::HookWidgets() {
	listShop = (CListBox*)GetWidget("listShop");
	registerEvent("listShop", "SelectionChange", std::bind(&CShopForm::listShop_SelectionChange, this, std::placeholders::_1));
	registerEvent("listShop", "SelectionDblClick", std::bind(&CShopForm::listShop_SelectionDblClick, this, std::placeholders::_1));
	imgItem = (CImageBox*)GetWidget("imgItem");
	imgInventory = (CImageBox*)GetWidget("imgInventory");

	lblItemDetail = (CLabel*)GetWidget("lblItemDetail");
	lblCash = (CLabel*)GetWidget("lblCash");

	btnBuy = (CButton*)GetWidget("btnBuy");
	btnSell = (CButton*)GetWidget("btnSell");
	registerEvent("btnBuy", "Click", std::bind(&CShopForm::btnBuy_Click, this, std::placeholders::_1));
	registerEvent("btnSell", "Click", std::bind(&CShopForm::btnSell_Click, this, std::placeholders::_1));

	btnCloseShop = (CButton*)GetWidget("btnCloseShop");
	btnDeposit = (CButton*)GetWidget("btnDeposit");
	btnWithdraw = (CButton*)GetWidget("btnWithdraw");
	btnCheckAccount = (CButton*)GetWidget("btnCheckAccount");
	registerEvent("btnCloseShop", "Click", std::bind(&CShopForm::btnCloseShop_Click, this, std::placeholders::_1));
	registerEvent("btnDeposit", "Click", std::bind(&CShopForm::btnDeposit_Click, this, std::placeholders::_1));
	registerEvent("btnWithdraw", "Click", std::bind(&CShopForm::btnWithdraw_Click, this, std::placeholders::_1));
	registerEvent("btnCheckAccount", "Click", std::bind(&CShopForm::btnCheckAccount_Click, this, std::placeholders::_1));

	std::string boxName = "imgItem";
	int toX = imgInventory->GetX() + 24;
	int toY = imgInventory->GetY() + 24;
	while (imgItems.size() < 16) {
		int count = imgItems.size();
		int next = count + 1;
		std::string widgetName = boxName + (next < 10 ? "0" : "") + std::to_string(next);
		CImageBox *nextItem = createImageBox(widgetName);
		nextItem->SetPosition(toX + (count % 4 * 47), toY + (count / 4 * 47));
		imgItems.push_back(nextItem);
		registerEvent(widgetName, "Click", std::bind(&CShopForm::imgItem_Click, this, std::placeholders::_1));
	}
}

void CShopForm::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);

	if (e.type == CUSTOMEVENT_ITEM) {
		if (e.user.code == ITEM_BUY) {
			UpdateInventory();
		}
		if (e.user.code == ITEM_SELL) {
			UpdateInventory();
			LoadViewItem(nullptr, false);
		}
	}

	if (e.type == CUSTOMEVENT_PLAYER) {
		if (e.user.code == PLAYER_MONEY) {
			SetCash();
		}
	}

	if (e.window.windowID != windowID) return;

	if (e.type = CUSTOMEVENT_WINDOW && e.user.data1 == promptForm) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK) {
			if (Buying) BuyItem();
			if (Selling) SellItem();
		}
		Buying = false;
		Selling = false;
	}
}

void CShopForm::SetShop(CShop *aShop) {
	shop = aShop;
	if (!shop) return;

	LoadShopInventory();
}

void CShopForm::LoadShopInventory() {
	shopInventory.clear();
	listShop->ClearList();
	if (!shop) return;

	CItemDataFile itemDataFile;
	for (auto itemId : shop->inventory) {
		ItemData itemData;
		if (itemDataFile.GetItemByID(itemId, itemData)) {
			Item item(itemData);
			shopInventory.push_back(item);
			CImageBox *ib = createSellImageBox(&item);
			if (ib) listShop->AddItem(ib);
		}
	}
}

CImageBox* CShopForm::createSellImageBox(Item* item) {
	CImageBox *imgBox = new CImageBox(this, "shopInv_" + item->GetName(), 0, 0);
	imgBox->SetWidth(listShop->GetListWidth());
	imgBox->SetHeight(50);
	imgBox->UseBlackBackground(false);

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, listShop->GetListWidth(), 50);
	if (!texture) return nullptr;

	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);

	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, gui->backColor.a);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);

	SDL_Rect rect = { 4, 4, 40, 40 };
	boxRGBA(renderer, 4, 4, 44, 44, 0, 0, 0, 255);

	std::string itemPath = item->getTexturePath(40);
	if (itemPath.length()) {
		Texture tItem(renderer, itemPath, true);
		tItem.setBlendMode(SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(renderer, tItem.texture, NULL, &rect);
	}

	Texture* tName = stringToTexture(renderer, item->GetName(), gui->font, 0, gui->fontColor, 0);
	if (tName) {
		tName->rect.x = 52;
		tName->rect.y = 5;
		SDL_RenderCopy(renderer, tName->texture, NULL, &tName->rect);
		delete tName;
	}

	Texture* tCost = stringToTexture(renderer, "$" + formatInt(item->getCost()), gui->font, 0, gui->fontColor, 0);
	if (tCost) {
		tCost->rect.x = 52;
		tCost->rect.y = 23;
		SDL_RenderCopy(renderer, tCost->texture, NULL, &tCost->rect);
		delete tCost;
	}

	SDL_SetRenderTarget(renderer, priorTarget);

	imgBox->SetImage(new Texture(renderer, texture, listShop->GetListWidth(), 50));
	return imgBox;
}

CImageBox* CShopForm::createImageBox(std::string name) {
	CImageBox *imgBox = new CImageBox(this, name, 0, 0);
	imgBox->SetWidth(40);
	imgBox->SetHeight(40);
	imgBox->UseBlackBackground(true);
	AddWidget(imgBox);
	return imgBox;
}

void CShopForm::SetCash() {
	lblCash->SetText(formatInt(player->GetCash()));
}

void CShopForm::SetBuySell() {
	if (BuyMode) {
		btnBuy->SetVisible(true);
		btnSell->SetVisible(false);
	}
	else {
		btnBuy->SetVisible(false);
		btnSell->SetVisible(true);
	}
}

void CShopForm::LoadViewItem(Item* item, bool shopItem) {
	viewItem = item;
	BuyMode = shopItem;
	SetBuySell();
	if (!item) {
		imgItem->SetImage(nullptr);
		lblItemDetail->SetText("");
		return;
	}

	Texture *textureItem = new Texture(renderer, item->getTexturePath(85), true);
	imgItem->SetImage(textureItem);
	delete textureItem;

	if (shopItem) lblItemDetail->SetText(item->getShopDetails());
	else lblItemDetail->SetText(item->getDetails());
}

void CShopForm::UpdateInventory() {
	for (int i = 0; i < imgItems.size(); i++) {
		SetItemToImageBox(imgItems[i], player->inventory->getItemInSlot(i));
	}
}

void CShopForm::SetItemToImageBox(CImageBox* imgBox, Item* item) {
	if (!imgBox) return;
	if (!item) {
		imgBox->SetImage(nullptr);
		return;
	}
	Texture *textureItem = new Texture(renderer, item->getTexturePath(40), true);
	imgBox->SetImage(textureItem);
	delete textureItem;
}

void CShopForm::btnBuy_Click(SDL_Event& e) {
	if (!BuyMode || !viewItem) return;

	//buy > confirm, client send buy, server respond 1:iteminfo and 2:itemaction buy with id of recieved item (and cost to dec)
	promptForm = doPrompt(this, "Purchase Confirmation", "Do you want to buy " + viewItem->GetName() + " for $" + formatInt(viewItem->getCost()) + "?", true);
	Buying = true;
}

void CShopForm::BuyItem() {
	if (!BuyMode || !viewItem) return;

	pItemAction* itemAction = new pItemAction(viewItem->GetID(), iaBuy);
	gClient.addPacket(itemAction);
}

void CShopForm::btnSell_Click(SDL_Event& e) {
	if (BuyMode || !viewItem) return;

	//sell > confirm, client send sell, server respond with sell, cash += 1/2 value, inventory update to remove
	promptForm = doPrompt(this, "Sell Confirmation", "Are you sure you want to sell " + viewItem->GetName() + " for $" + formatInt(viewItem->getCost()) +"?", true);
	Selling = true;
}

void CShopForm::SellItem() {
	if (BuyMode || !viewItem) return;

	pItemAction* itemAction = new pItemAction(viewItem->GetID(), iaSell);
	gClient.addPacket(itemAction);
}

void CShopForm::btnCloseShop_Click(SDL_Event& e) {
	CWindow::btnClose_Click(e);
}

void CShopForm::btnDeposit_Click(SDL_Event& e) {

}

void CShopForm::btnWithdraw_Click(SDL_Event& e) {

}

void CShopForm::btnCheckAccount_Click(SDL_Event& e) {

}

void CShopForm::imgItem_Click(SDL_Event& e) {
	int pos = stoi(eventWidget->Name.substr(7, std::string::npos)) - 1;

	Item* item = player->inventory->getItemInSlot(pos);
	if (!item) return;

	listShop->SetSelectedIndex(-1);
	LoadViewItem(item, false);

	//Double Click
	if (e.button.clicks == 2) btnSell_Click(e);
}

void CShopForm::listShop_SelectionChange(SDL_Event& e) {
	int selected = listShop->GetSelectedIndex();
	if (selected == -1) return;

	Item* item = &shopInventory[selected];
	if (!item) return;
	LoadViewItem(item, true);
}

void CShopForm::listShop_SelectionDblClick(SDL_Event& e) {
	int selected = listShop->GetSelectedIndex();
	if (selected == -1) return;

	Item* item = &shopInventory[selected];
	if (!item) return;
	btnBuy_Click(e);
}