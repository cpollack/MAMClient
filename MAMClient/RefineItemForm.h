#pragma once

#include "Window.h"
#include "Define.h"

class CImageBox;
class CLabel;
class CButton;

class Item;
class CRefineItemForm : public CWindow {
public:
	CRefineItemForm();

	void handleEvent(SDL_Event& e);
	void render();
	void step();

private:
	void HookWidgets();
	CImageBox* createImageBox(std::string name);

	void UpdateAttr();
	void UpdateInventory();
	void SetItemToImageBox(CImageBox* imgBox, Item* item);

	//void EquipAccessory(Item* pItem);
	//void DoUnequip();

	Item* item = nullptr;
	int itemId = 0;
	int lastCoreStat = 0;
	std::vector<Item*> localInv;
	Asset hoverTexture;

	bool refining = false;
	bool receivedItem = false, wuxedItem = false;
	int refineMode = 0;
	int delayMs = 250;
	int lastRefineTick = 0;
	int refineType = 2302; //5k poison equivalent

public: //Event Hooks
	void btnBegin_Click(SDL_Event& e);

	void imgItem_DoubleClick(SDL_Event& e);
	void imgItem_OnHoverStart(SDL_Event& e);
	void imgItem_OnHoverEnd(SDL_Event& e);

private: //Widgets
	CImageBox *imgRefine, *imgRefineSlot, *imgInventory;
	CLabel *lblAttr, *lblCash;
	CButton *btnBegin;

	std::vector<CImageBox*> imgItems;
};