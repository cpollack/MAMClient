#pragma once

#include "Window.h"
#include "Define.h"

class CLoginForm;
class CPromptForm;
class CCharCreateForm;

class CMainUI;
class CGauge;
class CImageBox;
class CVideoFrame;

class User;

enum MainFormMode {
	MFM_INIT = 0,
	MFM_GROUP,
	MFM_SERVER,
	MFM_LOGIN,
	MFM_CREATE_SELECT,
	MFM_CREATE_DETAILS,
	MFM_MAIN
};

class CMainWindow : public CWindow {
public:
	CMainWindow();
	~CMainWindow();

	bool init() override;
	void initUI() override;
	void ReloadAssets();
	void render();
	void handleEvent(SDL_Event& e);
	void step();

	void changeMode(int mode); //Call cleanup of prior mode, then run init of new mode
	int getMode();
	void reloadUI();

	CLabel* addMainLabel(std::string name, int x, int y, int w, int h, std::string text, bool bold=false);
	CButton* addMainButton(std::string name, std::string text, int x, int y, int w, int h);
	CButton* addMainButton(std::string name, int x, int y, int w, int h, std::string pressed, std::string unpressed);
	CButton* addMainButton2(std::string name, int x, int y, int w, int h, std::string pressed, std::string unpressed);

private: //Init
	void init_init();
	void init_cleanup();
	void init_render();
	void init_step();

	int startTime;

private: // Login
	void login_init();
	void login_cleanup();
	void login_render();
	void login_handleEvent(SDL_Event& e);
	void login_step();

public:
	CLoginForm *loginForm;
	

private: // Select
	void select_init();
	void select_cleanup();
	void select_render();
	void select_handleEvent(SDL_Event& e);
	void select_step();

	SDL_Rect blackRect;
	const int selectHDIV = 8;
	const int selectVDIV = 10;
	Texture *portraits[8];
	bool femalePortraits = false;
	Texture *selectedCharacter, *unselectedCharacter;
	int characterIndex;

	void select_setVideo(bool standby);
	void videoFrame_PlaybackComplete(SDL_Event& e);
	CVideoFrame *videoFrame;

	CLabel *lblMaleOn, *lblMaleOff, *lblFemaleOn, *lblFemaleOff;
	void lblMaleOff_Click(SDL_Event &e);
	void lblFemaleOff_Click(SDL_Event &e);

	void btnSelectOk_Click(SDL_Event& e);
	void btnSelectCancel_Click(SDL_Event& e);

private: // Details
	void details_init();
	void details_cleanup();
	void details_render();
	void details_handleEvent(SDL_Event& e);
	void details_step();

	CCharCreateForm *charCreateForm;

private: // Main
	void main_init();
	void main_cleanup();
	void main_render();
	void main_handleEvent(SDL_Event& e);
	void main_step();

	void main_init_widgets();
	void main_setPetPortrait();
	void main_render_ui();

public: //Widget Event hooks
	virtual void btnClose_Click(SDL_Event& e);
private: 
	void btnJump_Click(SDL_Event& e);
	void btnCloud_Click(SDL_Event& e);

	void btnGuild_Click(SDL_Event& e);

	SDL_Texture* gameTexture;
	SDL_Rect gameRect;
	CPromptForm *dcPromptForm = nullptr;
	bool Disconnected = false, RelogReady = false;

	//CPlayerInfoFrame *playerInfoFrame;
	CMainUI *mainUI;
	
	CImageBox *playerPortrait, *petPortrait;
	CImageBox *imgPortrait;

	User* lastMouseoverUser = nullptr;
	int userDetailsStartTime;

public: // Main form hooks
	void setPlayerDetailsLabels();
	void setUserDetailsLabels(User* user);
	void setMapCoordLabels(SDL_Point coord);

	void setPlayerHealthGauge(int val);
	void setPlayerHealthGauge(int val, int max);
	void shiftPlayerHealthGauge(int val);
	void setPlayerManaGauge(int val);
	void setPlayerManaGauge(int val, int max);
	void shiftPlayerManaGauge(int val);
	void setPlayerExpGauge(int val);
	void setPlayerExpGauge(int val, int max);
	void shiftPlayerExpGauge(int val);
	void setPetHealthGauge(int val);
	void setPetHealthGauge(int val, int max);
	void shiftPetHealthGauge(int val);
	void setPetExpGauge(int val);
	void setPetExpGauge(int val, int max);
	void shiftPetExpGauge(int val);

	void openShop(int shopId);

private:
	int Mode;

private: //GUI Texture
	Texture* logo;

	Texture* topCenter;
	Texture* topLeft;
	Texture* topRight;
	Texture* mainWindow;
	Texture* surface;
	Texture* newGui;
	Texture* minimize;

	SDL_Rect surfaceRect;
};

extern CMainWindow *mainForm;