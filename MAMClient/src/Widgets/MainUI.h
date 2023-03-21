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
	CButton* addMenuButton(std::string name, SDL_Point point, std::string imgPath, int btnWidth = menuBtnWidth, int btnHeight = menuBtnHeight);
	CLabel* addLabel(std::string name, SDL_Point point, int w, int h, std::string text, bool bold = false);
	CButton* addButton(std::string name, SDL_Point point, std::string buttonImg, std::string pushedImg, int btnWidth = menuBtnWidth, int btnHeight = menuBtnHeight);
	CGauge* addGauge(std::string name, SDL_Point point, int w, int h, std::string foreground, bool verticle = false);

	bool IsAssetPixel(Asset asset, Uint32 *pixels, SDL_Point point);	

private:
	//Asset PlayerFrame;
	//Asset PetFrame;
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
	void updatePlayerDetails();
	void updatePlayerLevel();
	//void updatePetLevel();

	void setMapCoordLabels(SDL_Point coord);

private: //Team Buttons
	CButton *btnTeamCreate, *btnTeamJoin, *btnTeamManage, *btnTeamLeave, *btnTeamDisband;
	bool ShowingTeamButtons;
	void ShowTeamButtons();
	void HideTeamButtons();

	void btnTeamCreate_Click(SDL_Event& e);
	void btnTeamJoin_Click(SDL_Event& e);
	void btnTeamManage_Click(SDL_Event& e);
	void btnTeamLeave_Click(SDL_Event& e);
	void btnTeamDisband_Click(SDL_Event& e);

private:
	CWindow* window;

	CGauge *gaugePlayerHealth, *gaugePlayerMana, *gaugePlayerExp;
	CGauge *gaugePetHealth, *gaugePetExp;
	CButton* btnBattle, * btnBattleAuto, * btnBattleConfig;
	//CButton *btnBattle, *btnBattleAuto, *btnBattleRepeat, *btnBattleConfig;
	//CLabel* lblPlayerLevel, * lblPetLevel;
	//CLabel *lblPlayerLevel;

	CLabel* lblCoordX, * lblCoordY;
	CLabel* lblName, * lblNickName, * lblLevel;
	CLabel* lblSpouse, * lblCash, * lblReputation;
	CLabel* lblRank, * lblGuild, * lblGuildRank;

	CButton* btnPlayer, * btnPet;
	CButton* btnWuxing, * btnOptions;
	CButton* btnMap, * btnTeam, * btnInventory, * btnKungfu, * btnSocial, * btnFriend;

public:
	static const int menuBtnWidth = 60, menuBtnHeight = 60;
	//const int menuBtnWidth = 30, menuBtnHeight = 21;
	const SDL_Point menuPoint[8] = { {532, 540}, {566, 540}, {600, 540}, {634, 540},
							   {532, 564}, {566, 564}, {600, 564}, {634, 564} };
	/*const SDL_Point menuPoint[8] = {{532, 540}, {566, 540}, {600, 540}, {634, 540},
							   {532, 564}, {566, 564}, {600, 564}, {634, 564} };*/

private:
	void btnBattle_Click(SDL_Event& e);
	void btnBattleAuto_Click(SDL_Event& e);
	void btnBattleRepeat_Toggle(SDL_Event& e);
	void btnBattleConfig_Click(SDL_Event& e);

	void btnPlayer_Click(SDL_Event& e);
	void btnPet_Click(SDL_Event& e);

	void btnMap_Click(SDL_Event& e);
	void btnTeam_Click(SDL_Event& e);
	void btnInventory_Click(SDL_Event& e);	
	void btnKungfu_Click(SDL_Event& e);	
	void btnSocial_Click(SDL_Event& e);
	void btnFriend_Click(SDL_Event& e);

	//No longer exist, what to do?
	void btnWuxing_Click(SDL_Event& e);
	void btnOptions_Click(SDL_Event& e);

	bool MouseOverPlayer, MouseOverPet;
	bool btnPlayer_Down = false;
	bool btnPet_Down = false;
};

const int slotWidth = 48, slotHeight = 48;
const SDL_Point skillSlotPoint[7] = { {430,418}, {478,418}, {526,418}, {574,418}, {622,418}, {670,418}, {718,418} };