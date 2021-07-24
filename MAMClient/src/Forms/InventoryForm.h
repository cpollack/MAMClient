#pragma once

#include "Window.h"

class CImageBox;
class CLabel;
class CButton;

class Item;

class CInventoryForm : public CWindow {
public:
	CInventoryForm();

	void handleEvent(SDL_Event& e);

private:
	void HookWidgets();
	void LoadEquipment();
	std::vector<SDL_Point> GetEquipmentSlots();

	void LoadViewItem(Item* item);
	void UpdateEquipment();
	void UpdateInventory();
	void SetItemToImageBox(CImageBox* imgBox, Item* item);
	void SetCash();

	CImageBox* createImageBox(std::string name);
	void UnequipItem(Item* item);
	void UseItem(Item* item);

	int UsingItem = false, PetUsingItem = false, DropItem = false;
	Item *viewItem = nullptr;

public: //Event Hooks
	void btnUse_Click(SDL_Event& e);
	void btnPetUse_Click(SDL_Event& e);
	void btnDrop_Click(SDL_Event& e);
	void btnRename_Click(SDL_Event& e);
	void btnCloseInv_Click(SDL_Event& e);
	void btnDeposit_Click(SDL_Event& e);
	void btnWithdraw_Click(SDL_Event& e);
	void btnGive_Click(SDL_Event& e);

	void imgWeapon_Click(SDL_Event& e);
	void imgArmor_Click(SDL_Event& e);
	void imgShoe_Click(SDL_Event& e);
	void imgHeadAccessory_Click(SDL_Event& e);
	void imgBodyAccessory_Click(SDL_Event& e);
	void imgItem_Click(SDL_Event& e);

private: //Widgets
	CImageBox *imgEquipment, *imgItem, *imgInventory;
	CLabel *lblItemDetail, *lblCash;
	CButton *btnUse, *btnPetUse, *btnDrop, *btnRename, *btnClose;
	CButton *btnDeposit, *btnWithdraw, *btnGive;

	CImageBox *imgWeapon, *imgArmor, *imgShoe, *imgHeadAccessory, *imgBodyAccessory;
	std::vector<CImageBox*> imgItems;
};