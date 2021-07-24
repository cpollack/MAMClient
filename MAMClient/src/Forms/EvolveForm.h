#pragma once

#include "Window.h"
#include "Define.h"

class CImageBox;
class CLabel;
class CButton;

class Pet;
class Sprite;
class CEvolveForm : public CWindow {
public:
	CEvolveForm(Pet* pPet);

	virtual void handleEvent(SDL_Event& e);

private:
	void HookWidgets();
	void loadSprites();
	void doEvolve();

	Pet *pet = nullptr;
	Sprite *sprBefore, *sprAfter;

public: //Event Hooks
	void btnOK_Click(SDL_Event& e);
	void btnCancel_Click(SDL_Event& e);

private: //Widgets
	CImageBox *imgBefore, *imgAfter;
	CButton *btnOK, *btnCancel;

};