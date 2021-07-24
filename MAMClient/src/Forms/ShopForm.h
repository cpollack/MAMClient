#pragma once

#include "Window.h"

class CListBox;
class CImageBox;
class CLabel;
class CButton;

class Item;
class CShop;

class CShopForm : public CWindow {
public:
	CShopForm();

	void handleEvent(SDL_Event& e);
	void SetShop(CShop * aShop);
	void LoadShopInventory();

private:
	void HookWidgets();

	void LoadViewItem(Item* item, bool shopItem);
	void UpdateInventory();
	void SetItemToImageBox(CImageBox* imgBox, Item* item);
	void SetCash();
	void SetBuySell();

	void BuyItem();
	void SellItem();

	CImageBox* CShopForm::createSellImageBox(Item* item);
	CImageBox* createImageBox(std::string name);

	Item *viewItem = nullptr;
	CShop *shop = nullptr;
	std::vector<Item> shopInventory;
	bool BuyMode = false;
	bool Buying = false, Selling = false;

public: //Event Hooks
	void btnBuy_Click(SDL_Event& e);
	void btnSell_Click(SDL_Event& e);

	void btnCloseShop_Click(SDL_Event& e);
	void btnDeposit_Click(SDL_Event& e);
	void btnWithdraw_Click(SDL_Event& e);
	void btnCheckAccount_Click(SDL_Event& e);

	void imgItem_Click(SDL_Event& e);
	void listShop_SelectionChange(SDL_Event& e);
	void listShop_SelectionDblClick(SDL_Event& e);

private: //Widgets
	CListBox *listShop;
	CImageBox *imgItem, *imgInventory;
	CLabel *lblItemDetail, *lblCash;
	CButton *btnBuy, *btnSell, *btnCloseShop;
	CButton *btnDeposit, *btnWithdraw, *btnCheckAccount;

	std::vector<CImageBox*> imgItems;
};