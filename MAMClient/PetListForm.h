#pragma once

#include "Window.h"

class CImageBox;
class CGauge;
class CField;
class Sprite;

class CPromptForm;

class CPetListForm : public CWindow {
public:
	CPetListForm();

	void handleEvent(SDL_Event& e);

private:
	void HookWidgets();
	void LoadPortraits();
	void LoadSprite();
	void SetMarching(int index);
	void LoadPet(int index);

	int GetMarchingPetIndex();
	int selection = -1;

	CPromptForm *promptForm = nullptr;
	void DropPet();
	bool Dropping = false;

public: //Event Hooks
	void imgPet1_Click(SDL_Event& e);
	void imgPet2_Click(SDL_Event& e);
	void imgPet3_Click(SDL_Event& e);
	void imgPet4_Click(SDL_Event& e);
	void imgPet5_Click(SDL_Event& e);

	void btnChangeName_Click(SDL_Event& e);
	void btnEvolve_Click(SDL_Event& e);
	void btnSkills_Click(SDL_Event& e);
	void btnRegister_Click(SDL_Event& e);
	void btnCatalog_Click(SDL_Event& e);
	void btnEvaluate_Click(SDL_Event& e);
	void btnPetStall_Click(SDL_Event& e);
	void btnDrop_Click(SDL_Event& e);
	void btnMarch_Click(SDL_Event& e);
	void btnChangeAccessory_Click(SDL_Event& e);

private: //Widgets
	CImageBox *imgStar, *imgPet1, *imgPet2, *imgPet3, *imgPet4, *imgPet5, *imgSprite;
	CImageBox *imgBraveMedals, *imgCalmnessMedals, *imgSpeedMedals;
	CField *fldName, *fldID, *fldAttack, *fldDefence, *fldDexterity;
	CLabel *lblPetName1, *lblPetName2, *lblPetName3, *lblPetName4, *lblPetName5;
	CLabel *lblSpecies, *lblElement, *lblRegistered;
	CLabel *lblLevel, *lblGeneration, *lblAccessory;
	CGauge *gaugeLoyalty, *gaugeExperience, *gaugeLife;
	
	CButton *btnChangeName, *btnEvolve, *btnSkills, *btnRegister, *btnCatalog;
	CButton *btnEvaluate, *btnPetStall, *btnDrop, *btnMarch, *btnChangeAccessory;
};
