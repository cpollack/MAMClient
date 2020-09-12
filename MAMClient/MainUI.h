#pragma once

#include "Widget.h"

class CWindow;
class CGauge;
class CLabel;
class CButton;
class CMainUI : public CWidget {
public:
	CMainUI(CWindow* w);
	~CMainUI();

	void CreateWidgets();
	void Load();
	void Reload() { Load(); }
	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);
	void Step();

private:
	std::string getPetElementType();
	CGauge* addGauge(std::string name, int x, int y, int w, int h, std::string foreground, std::string background, bool verticle);
	CButton* addMenuButton(std::string name, SDL_Point point, std::string imgPath);

	bool IsAssetPixel(Asset asset, Uint32 *pixels, SDL_Point point);	

private:
	Asset PlayerFrame;
	Asset PetFrame;
	//const int PetFrameWidth = 150;
	//const int PetFrameHeight = 60;
	//Uint32 *playerPixels = nullptr, *petPixels = nullptr;

public: //Gauges
	void setPlayerHealthGauge(int val);
	void setPlayerHealthGauge(int val, int max);
	void shiftPlayerHealthGauge(int val);
	void adjustPlayerHealthGauge(int val);
	void setPlayerManaGauge(int val);
	void setPlayerManaGauge(int val, int max);
	void shiftPlayerManaGauge(int val);
	void adjustPlayerManaGauge(int val);
	void setPlayerExpGauge(int val);
	void setPlayerExpGauge(int val, int max);
	void shiftPlayerExpGauge(int val);
	void adjustPlayerExpGauge(int val);
	void setPetHealthGauge(int val);
	void setPetHealthGauge(int val, int max);
	void shiftPetHealthGauge(int val);
	void adjustPetHealthGauge(int val);
	void setPetExpGauge(int val);
	void setPetExpGauge(int val, int max);
	void shiftPetExpGauge(int val);
	void adjustPetExpGauge(int val);

public: //Labels
	void updatePlayerLevel();
	void updatePetLevel();

private:
	CWindow* window;
	CGauge *gaugePlayerHealth, *gaugePlayerMana, *gaugePlayerExp;
	CGauge *gaugePetHealth, *gaugePetExp;
	CButton *btnBattle, *btnBattleAuto, *btnBattleRepeat, *btnBattleConfig;
	CLabel *lblPlayerLevel, *lblPetLevel;

	CButton* btnPlayer, * btnPet, * btnInventory, * btnWuxing;
	CButton* btnKungfu, * btnTeam, * btnSocial, * btnOptions;

public:
	const int menuBtnWidth = 30, menuBtnHeight = 21;
	const SDL_Point menuPoint[8] = { {532, 540}, {566, 540}, {600, 540}, {634, 540}, 
							   {532, 564}, {566, 564}, {600, 564}, {634, 564} };

private:
	void btnBattle_Click(SDL_Event& e);
	void btnBattleAuto_Click(SDL_Event& e);
	void btnBattleRepeat_Toggle(SDL_Event& e);
	void btnBattleConfig_Click(SDL_Event& e);

	void btnPlayer_Click(SDL_Event& e);
	void btnPet_Click(SDL_Event& e);
	void btnInventory_Click(SDL_Event& e);
	void btnWuxing_Click(SDL_Event& e);
	void btnKungfu_Click(SDL_Event& e);
	void btnTeam_Click(SDL_Event& e);
	void btnSocial_Click(SDL_Event& e);
	void btnOptions_Click(SDL_Event& e);

	bool MouseOverPlayer, MouseOverPet;
	bool btnPlayer_Down = false;
	bool btnPet_Down = false;
};

const int slotWidth = 46, slotHeight = 47;
const SDL_Point skillSlotPoint[5] = { {132,538}, {179,538}, {226,538}, {273,538}, {320,538} };
const SDL_Point itemSlotPoint[2] = { {432,538}, {480,538} };