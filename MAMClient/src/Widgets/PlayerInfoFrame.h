#pragma once

#include "Widget.h"

class CGauge;
class CLabel;
class CButton;
class CPlayerInfoFrame : public CWidget {
public:
	CPlayerInfoFrame(CWindow* window, std::string name, int x, int y);
	~CPlayerInfoFrame();
	void Cleanup();

	void Load();
	void Reload() { Load(); }
	void ReloadAssets();
	void Render();
	void HandleEvent(SDL_Event& e);

private:
	void MergePiece(Asset asset, std::string fileName, int x, int y);
	void MergePiece(Asset asset, Asset piece, int x, int y);
	CGauge* addGauge(std::string name, int x, int y, int w, int h, std::string foreground);
	void LoadPortrait(Asset &portrait, std::string path, int width, bool bFlip);

	bool IsAssetPixel(Asset asset, Uint32 *pixels, SDL_Point point);

private:
	Asset PlayerFrame;
	Asset PetFrame;
	const int PetFrameWidth = 150;
	const int PetFrameHeight = 60;
	Uint32 *playerPixels = nullptr, *petPixels = nullptr;

	Asset playerPortrait, petPortait;
	const int PlayerWidth = 80;
	const int PetWidth = 40;
	const SDL_Point petOffset = { 69, 114 };
	SDL_Rect PlayerBtnRect;
	SDL_Rect PetBtnRect;

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
	CGauge *gaugePlayerHealth, *gaugePlayerMana, *gaugePlayerExp;
	CGauge *gaugePetHealth, *gaugePetExp;
	CLabel *lblPlayerLevel, *lblPetLevel;
	CButton *btnHide;

private:
	void btnCharacter_Click(SDL_Event& e);
	void btnPet_Click(SDL_Event& e);
	void btnHide_Toggle(SDL_Event& e);

	bool Expanded = true;
	bool MouseOverPlayer, MouseOverPet;
	bool btnPlayer_Down = false;
	bool btnPet_Down = false;
};