#pragma once

#include "Window.h"
#include "Define.h"

class CImageBox;
class CLabel;
class CField;
class CButton;
class CListBox;
class CGauge;

class Pet;
class Item;
class Sprite;
class CPetComposeForm : public CWindow {
public:
	CPetComposeForm();

	virtual void handleEvent(SDL_Event& e);
	virtual void render();

private:
	void HookWidgets();
	void loadListView();
	CImageBox* createPetImageBox(Pet* pet);
	void loadSprite(bool isMain, bool isSource);
	void doPreview();
	void clearPreview();
	void doCompose();
	void clearDetails();
	void setDetails(Pet* pet);

	Pet *srcMain = nullptr;
	Pet *srcMinor = nullptr;
	Pet *dstMain = nullptr;
	Pet *dstMinor = nullptr;
	Pet *detailsPet = nullptr;
	Item *accessory = nullptr;

	std::vector<Pet*> pets;
	Asset medalatk, medaldef, medaldex;
	Asset hoverTexture;

public: //Event Hooks
	void imgSrcMain_Click(SDL_Event& e);
	void imgSrcMain_DoubleClick(SDL_Event& e);
	void imgSrcMinor_Click(SDL_Event& e);
	void imgSrcMinor_DoubleClick(SDL_Event& e);
	void imgDstMain_Click(SDL_Event& e);
	void imgDstMinor_Click(SDL_Event& e);
	void imgAccessory_OnHoverStart(SDL_Event& e);
	void imgAccessory_OnHoverEnd(SDL_Event& e);

	void listPets_MouseOver(SDL_Event& e);
	void listPets_SelectionClick(SDL_Event& e);
	void listPets_SelectionDblClick(SDL_Event& e);

	void btnSwitch_Click(SDL_Event& e);
	void btnOK_Click(SDL_Event& e);
	void btnCancel_Click(SDL_Event& e);

private: //Widgets
	CImageBox *imgSrcMain, *imgSrcMinor, *imgDstMain, *imgDstMinor, *imgAccessory;
	CImageBox *imgMedalBrave, *imgMedalCalm, *imgMedalSpeed;
	CButton *btnSwitch, *btnOK, *btnCancel;
	CListBox *listPets;
	CGauge *gaugePercMain, *gaugePercMinor;

	CLabel *lblViewPreview, *lblPetName, *lblGeneration, *lblPercMain, *lblPercMinor;
	CField *fldLevel, *fldLife, *fldLifeGrowth, *fldGrowth;
	CField *fldAttack, *fldAttackRate, *fldDefense, *fldDefenseRate, *fldDexterity, *fldDexterityRate;
};	