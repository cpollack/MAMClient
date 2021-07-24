#pragma once

#include "Define.h"
#include "Window.h"

typedef struct WuxingSlot {
	int type = OBJ_NONE;
	void* object = nullptr;
};

class CListBox;
class CImageBox;
class CLabel;
class CButton;

class Sprite;
class Inventory;
class Item;
class Pet;
class CWuxingForm : public CWindow {
public:
	CWuxingForm();

	void render();
	void handleEvent(SDL_Event& e);

private:
	void HookWidgets();

	//void SetItemToImageBox(CImageBox* imgBox, Item* item);

	CImageBox* createImageBox(std::string name);

	void LoadItemsList();
	void LoadPetsList();
	void CreateSprites();
	void setMainIngredient(int iSlot);
	void LoadPreviewItem(Item* item);

	int GetMouseOverSlot();
	CImageBox* GetMouseOverSlotWidget();
	int GetSlotByObject(void* object);
	CImageBox* GetSlotWidgetByPos(int pos);
	int GetOvenObjectCount();
	int GetNextFilledSlot(bool skipMain=false);
	void ClearSlot(int slot);

	void SendWux();
	void RequestPreview();
	void SetPreviewObject(Item* item);
	void SetPreviewObject(Pet* pet);

	SDL_Point slotPoint[8] = { {100, 3}, {18, 18}, {3, 102}, {18, 181}, {181, 18}, {197, 102}, {180, 181}, {100, 197} };
	SDL_Point centerPoint = {123, 126}; //Center of stove
	// Offset origin for main ingredient pointer
	SDL_Point mainPoint[8] = { {65, 121}, {102, 105}, {120, 69}, {103, 28}, {25, 104}, {10, 69}, {25, 29}, {65, 12} };

	int mainIngredient = -1;
	Asset astMainPointer;
	Asset dragTexture;
	WuxingSlot dragObject;
	Asset hoverTexture;
	WuxingSlot hoverObject;

	bool StartAnimation = false;
	int fireSlot = -1;
	Sprite *fire;
	Sprite *diagram;

	std::vector<Item*> localInventory;
	std::vector<Pet*> localPets;
	typedef WuxingSlot PreviewObject;
	PreviewObject previewObject;
	WuxingSlot wuxingSlot[8];

public: //Event Hooks
	void btnStart_Click(SDL_Event& e);

	void listItems_OnListItemHoverStart(SDL_Event& e);
	void listItems_OnListItemHoverEnd(SDL_Event& e);
	void listPets_OnListItemHoverStart(SDL_Event& e);
	void listPets_OnListItemHoverEnd(SDL_Event& e);

	void listItems_OnDragStart(SDL_Event& e);
	void listItems_OnDragEnd(SDL_Event& e);
	void listPets_OnDragStart(SDL_Event& e);
	void listPets_OnDragEnd(SDL_Event& e);

	void Slot_DoubleClick(SDL_Event& e);
	void Slot_RightClick(SDL_Event& e);
	void Slot_OnHoverStart(SDL_Event& e);
	void Slot_OnHoverEnd(SDL_Event& e);
	void Slot_OnDragStart(SDL_Event& e);
	void Slot_OnDragEnd(SDL_Event& e);


private: //Widgets
	CListBox *listItems, *listPets;
	CImageBox *imgOven, *imgPreview;
	CImageBox* slots[8];
	CLabel *lblPreview;
	CButton *btnStart;

};