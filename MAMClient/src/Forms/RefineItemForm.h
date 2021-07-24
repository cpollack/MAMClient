#pragma once

#include "Window.h"
#include "Define.h"

struct RefineMethod {
	std::string method;
	std::string itemName;
	int itemId;
	int cost;
	int temperTime;
	int minWuxing;
};

class CImageBox;
class CLabel;
class CButton;
class CDropDown;
class CGauge;
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

	int GetCoreAttr();
	void UpdateItemInfo();
	void UpdateInventory();
	void SetItemToImageBox(CImageBox* imgBox, Item* item);

	//void EquipAccessory(Item* pItem);
	//void DoUnequip();

	RefineMethod GetMethod(std::string strMethod);
	std::vector<RefineMethod> refineMethods;
	RefineMethod method;

	Item* item = nullptr;
	int itemId = 0;
	int itemSlot = 0;
	int lastCoreStat = 0;
	std::vector<Item*> localInv;
	Asset hoverTexture;

	bool refining = false;
	bool receivedItem = false, wuxedItem = false;
	int refineMode = 0;
	int delayMs = 250;
	int lastRefineTick = 0;

	int wuxCount = 0;

public: //Event Hooks
	void btnBegin_Click(SDL_Event& e);
	void ddType_Change(SDL_Event& e);

	void imgItem_DoubleClick(SDL_Event& e);
	void imgItem_OnHoverStart(SDL_Event& e);
	void imgItem_OnHoverEnd(SDL_Event& e);
	void imgRefineSlot_DoubleClick(SDL_Event& e);
	void imgRefineSlot_OnHoverStart(SDL_Event& e);
	void imgRefineSlot_OnHoverEnd(SDL_Event& e);

private: //Widgets
	CImageBox *imgRefine, *imgRefineSlot, *imgInventory;
	CLabel *lblMessage, *lblLevel, *lblAttr, *lblCash;
	CButton *btnBegin;
	CDropDown* ddType;
	CGauge* gaugeProgress;

	std::vector<CImageBox*> imgItems;
};