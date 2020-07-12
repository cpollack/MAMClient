#pragma once

#include "Window.h"
#include "Define.h"

class CImageBox;
class CLabel;
class CButton;

class Pet;
class Item;
class CPetEquipForm : public CWindow {
public:
	CPetEquipForm(Pet* pPet);

	void handleEvent(SDL_Event& e);
	void render();

private:
	void HookWidgets();
	CImageBox* createImageBox(std::string name);

	void UpdateAccessory();
	void UpdateInventory();
	void SetItemToImageBox(CImageBox* imgBox, Item* item);

	void EquipAccessory(Item* pItem);
	void DoUnequip();

	Pet *pet = nullptr;
	Item *item = nullptr;
	std::vector<Item*> localInv;
	Asset hoverTexture;

public: //Event Hooks
	void btnUnequip_Click(SDL_Event& e);
	void btnOK_Click(SDL_Event& e);

	void imgItem_DoubleClick(SDL_Event& e);
	void imgItem_OnHoverStart(SDL_Event& e);
	void imgItem_OnHoverEnd(SDL_Event& e);

private: //Widgets
	CImageBox *imgAccessory, *imgInventory;
	CLabel *lblAccessory;
	CButton *btnUnequip, *btnOK;

	std::vector<CImageBox*> imgItems;
};