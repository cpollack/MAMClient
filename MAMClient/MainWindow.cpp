#include "stdafx.h"
#include "MainWindow.h"
#include "Global.h"
#include "Define.h"
#include "CustomEvents.h"

#include "Client.h"
#include "GUI.h"
#include "GameMap.h"
#include "Battle.h"
#include "Chat.h"

#include "Player.h"
#include "Pet.h"

#include "LoginForm.h"
#include "LogoutForm.h"
#include "PromptForm.h"
#include "CharCreateForm.h"
#include "CharacterForm.h"
#include "PetListForm.h"
#include "InventoryForm.h"

#include "Gauge.h"
#include "VideoFrame.h"

#include "pBattleState.h"

CMainWindow::CMainWindow() :CWindow() {
	Type = FT_MAIN;
	Width = 800;
	Height = 600;
	init();
	initUI();

	SDL_SetWindowTitle(window, "Monster & Me: Reborn");

	//Load Config
	gClient.configIni = new INI("INI\\config.ini");
	if (!gClient.configIni->setSection("Default")) {
		gClient.configIni->addSection("Default");
		gClient.configIni->setSection("Default");
	}

	Mode = MFM_INIT;
	init_init();
}

CMainWindow::~CMainWindow() {
	//
	if (map) delete map;
}

bool CMainWindow::init()
{
	//Create window
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS;
	
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, flags);
	if (window != NULL) {
		SDL_SetWindowBordered(window, SDL_FALSE);
		mouseFocus = true;
		keyboardFocus = true;

		//Create renderer for window
		//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL) {
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			window = NULL;
		}
		else {
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

			//Grab window identifier
			windowID = SDL_GetWindowID(window);

			//Flag as opened
			shown = true;
		}
	}
	else printf("Main Window could not be created! SDL Error: %s\n", SDL_GetError());

	return window != NULL && renderer != NULL;
}

void CMainWindow::initUI() {
	CWindow::initUI();
	topCenter = gui->getSkinTexture(renderer, "TopCenter.bmp", Anchor::aTopLeft);
	topLeft = gui->getSkinTexture(renderer, "TopLeft.bmp", Anchor::aTopLeft);
	topRight = gui->getSkinTexture(renderer, "TopRight.bmp", Anchor::aTopRight);

	mainWindow = gui->getSkinTexture(renderer, "mainwindow.jpg", Anchor::aTopLeft);

	surface = gui->getSkinTexture(renderer, "surface.jpg", Anchor::aTopLeft);
	surfaceRect = surface->rect;
	surfaceRect.x = left->width;
	surfaceRect.y = (Height - surface->height - bottomCenter->height);
	
	//minimize = gui->getSkinTexture(renderer, "Min.bmp", Anchor::TOP_LEFT);
}

void CMainWindow::ReloadAssets() {
	CWindow::ReloadAssets();
	if (map) map->ReloadAssets();
}

void CMainWindow::render() {
	switch (Mode) {
	case MFM_INIT: init_render(); break;
	case MFM_LOGIN: login_render(); break;
	case MFM_CREATE_SELECT: select_render(); break;
	case MFM_CREATE_DETAILS: details_render(); break;
	case MFM_MAIN: main_render(); break;
	}
}

void CMainWindow::handleEvent(SDL_Event& e) {
	CWindow::handleEvent(e);
	switch (Mode) {
	case MFM_LOGIN: login_handleEvent(e); break;
	case MFM_CREATE_SELECT: select_handleEvent(e); break;
	case MFM_CREATE_DETAILS: details_handleEvent(e); break;
	case MFM_MAIN: main_handleEvent(e); break;
	}
}

void CMainWindow::step() {
	CWindow::step();
	switch (Mode) {
	case MFM_INIT: init_step(); break;
	case MFM_LOGIN: login_step(); break;
	case MFM_CREATE_SELECT: select_step(); break;
	case MFM_MAIN: main_step(); break;
	}
}

void CMainWindow::changeMode(int mode) {
	switch (Mode) {
	case MFM_INIT: init_cleanup(); break;
	case MFM_LOGIN: login_cleanup(); break;
	case MFM_CREATE_SELECT: select_cleanup(); break;
	case MFM_CREATE_DETAILS: details_cleanup(); break;
	case MFM_MAIN: main_cleanup(); break;
	}

	Mode = mode;
	switch (Mode) {
	case MFM_INIT: init_init(); break;
	case MFM_LOGIN: login_init(); break;
	case MFM_CREATE_SELECT: select_init(); break;
	case MFM_CREATE_DETAILS: details_init(); break;
	case MFM_MAIN: main_init(); break;
	}
}

int CMainWindow::getMode() {
	return Mode;
}

void CMainWindow::reloadUI() {
	//cleanup and reload ui textures from GUI
}

CLabel* CMainWindow::addMainLabel(std::string name, int x, int y, int w, int h, std::string text, bool bold) {
	CLabel* lbl = new CLabel(this, name, x, y);
	//lbl->SetRenderer(renderer);
	lbl->SetWidth(w);
	lbl->SetHeight(h);
	if (bold) lbl->SetBold(true);
	lbl->SetAlignment(laLeft);
	lbl->SetVAlignment(lvaCenter);
	lbl->SetText(text);
	AddWidget(lbl);
	return lbl;
}

CButton* CMainWindow::addMainButton(std::string name, std::string text, int x, int y, int w, int h) {
	CButton *btn = new CButton(this, name, x, y);
	//btn->SetRenderer(renderer);
	btn->SetWidth(w);
	btn->SetHeight(h);
	btn->SetText(text);
	AddWidget(btn);
	return btn;
}

CButton* CMainWindow::addMainButton(std::string name, int x, int y, int w, int h, std::string unpressed, std::string pressed) {
	CButton *btn = new CButton(this, name, x, y);
	//btn->SetRenderer(renderer);
	btn->SetWidth(w);
	btn->SetHeight(h);
	btn->SetUseGUI(true);
	btn->SetPressedImage(pressed);
	btn->SetUnPressedImage(unpressed);
	AddWidget(btn);
	return btn;
}

CGauge* CMainWindow::addMainGauge(std::string name, int x, int y, int w, int h, std::string foreground) {
	CGauge *gauge = new CGauge(this, name, x, y);
	//gauge->SetRenderer(renderer);
	gauge->SetWidth(w);
	gauge->SetHeight(h);
	gauge->SetUseGUI(true);
	gauge->SetForegroundImage(foreground);
	gauge->SetBackgroundImage("bg_slot.jpg");
	gauge->SetIncreaseImage("orange_slot.jpg");
	gauge->SetDecreaseImage("red_slot.jpg");
	AddWidget(gauge);
	return gauge;
}

/*
  Main Window Flow:
  Initialize
  (Group / Server Select)
  Login
  Character Creation 1
  Character Creation 2
  Main
*/


/* Main Form i Init Begin */

void CMainWindow::init_init() {
	SetUseClose(false);

	//Load a random logo
	std::string logoPath = "GUI\\rand_LogoPic\\";
	std::string imgNum = std::to_string((rand() % 10) + 1);
	if (imgNum != "10") imgNum = "0" + imgNum;
	logoPath += "logo" + imgNum + ".jpg";
	logo = new Texture(renderer);
	logo->loadTexture(logoPath);

	startTime = SDL_GetTicks();
}

void CMainWindow::init_render() {
	SDL_RenderCopy(renderer, logo->texture, NULL, &logo->rect);
}

void CMainWindow::init_step() {
	int elapsedTime = (SDL_GetTicks() - startTime) / 1000;
	if (elapsedTime >= 1) {
		changeMode(MFM_LOGIN);
	}
}

void CMainWindow::init_cleanup() {
	//Logo does not change through modes
}

/* Main Form - Init End */

/* Main Form - Login Begin */

void CMainWindow::login_init() {
	gClient.mainReady = false;
	loginForm = new CLoginForm();
	loginForm->SetParent(this);
	QueueWindows.push_back(loginForm);
}

void CMainWindow::login_render() {
	SDL_RenderCopy(renderer, logo->texture, NULL, &logo->rect);
}

void CMainWindow::login_handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE && e.user.data1 == loginForm) {
			applicationClose();
		}
	}
}

void CMainWindow::login_step() {
	//Login successful, do main or character creation
	if (gClient.mainReady) {
		loginForm->loginSuccess();

		if (player) changeMode(MFM_MAIN);
		else changeMode(MFM_CREATE_SELECT);
	}

	//
}

void CMainWindow::login_cleanup() {
	loginForm = nullptr;
}

/* Main Form - Login End */

/* Main Form - Select Begin */

void CMainWindow::select_init() {
	SetTitle("Select a Character");

	SetUseClose(true);
	registerEvent("btnClose", "Click", std::bind(&CMainWindow::btnClose_Click, this, std::placeholders::_1));

	addMainButton("btnSelectOk", "OK(&O)", 540, 545, 72, 24);
	addMainButton("btnSelectCancel", "Cancel(&X)", 625, 545, 72, 24);
	registerEvent("btnSelectOk", "Click", std::bind(&CMainWindow::btnSelectOk_Click, this, std::placeholders::_1));
	registerEvent("btnSelectCancel", "Click", std::bind(&CMainWindow::btnSelectCancel_Click, this, std::placeholders::_1));
	
	blackRect = { 20, 33, 760, 500 };
	characterIndex = 0;
	femalePortraits = false;
	for (int i = 0; i < 8; i++) {
		std::string portaitPath;
		portaitPath = "GUI/player_icon/Man0" + std::to_string(i+1) + ".bmp";
		portraits[i] = new Texture(renderer, portaitPath, true);
	}
	selectedCharacter = gui->getSkinTexture(renderer, "BoxFocus.bmp", aTopLeft);
	unselectedCharacter = gui->getSkinTexture(renderer, "BoxNormal.bmp", aTopLeft);

	videoFrame = new CVideoFrame(this, "videoFrame", 20, 33);
	videoFrame->SetWidth(500);
	videoFrame->SetHeight(500);
	videoFrame->SetRepeat(false);
	AddWidget(videoFrame);
	registerEvent("videoFrame", "PlaybackComplete", std::bind(&CMainWindow::videoFrame_PlaybackComplete, this, std::placeholders::_1));
	select_setVideo(false);

	//create labels
	SDL_Color white{ 255, 255, 255, 255 };
	SDL_Color black{ 0, 0, 0, 255 };
	SDL_Color grey{ 128, 128, 128, 255 };

	lblMaleOff = addMainLabel("lblMaleOff", blackRect.x + 500 + 15, blackRect.y + 8, 50, 16, "Male");
	lblMaleOff->SetFontColor(black);
	lblMaleOff->SetBackColor(grey);
	lblMaleOff->SetVisible(false);
	lblMaleOn = addMainLabel("lblMaleOn", blackRect.x + 500 + 15, blackRect.y + 8, 50, 16, "Male");
	lblMaleOn->SetFontColor(white);
	lblFemaleOff = addMainLabel("lblFemaleOff", blackRect.x + 500 + 67, blackRect.y + 8, 75, 16, "Female");
	lblFemaleOff->SetFontColor(black);
	lblFemaleOff->SetBackColor(grey);
	lblFemaleOn = addMainLabel("lblFemaleOn", blackRect.x + 500 + 67, blackRect.y + 8, 75, 16, "Female");
	lblFemaleOn->SetFontColor(white);
	lblFemaleOn->SetVisible(false);
	registerEvent("lblMaleOff", "Click", std::bind(&CMainWindow::lblMaleOff_Click, this, std::placeholders::_1));
	registerEvent("lblFemaleOff", "Click", std::bind(&CMainWindow::lblFemaleOff_Click, this, std::placeholders::_1));
}

void CMainWindow::select_cleanup() {
	delete selectedCharacter; 
	selectedCharacter = nullptr;
	delete unselectedCharacter;
	unselectedCharacter = nullptr;
	ClearWidgets();
}

void CMainWindow::select_render() {
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 0xFF);
	SDL_RenderClear(renderer);

	//Draw background and Widgets	
	SDL_RenderCopy(renderer, topCenter_s->texture, NULL, &getDstRect(topCenter_s, 0, 0));

	SDL_RenderCopy(renderer, bottomCenter->texture, NULL, &getDstRect(bottomCenter, 0, Height));
	SDL_RenderCopy(renderer, left->texture, NULL, &getDstRect(left, 0, 0));
	SDL_RenderCopy(renderer, right->texture, NULL, &getDstRect(right, Width, 0));

	SDL_RenderCopy(renderer, topLeft_s->texture, NULL, &getDstRect(topLeft_s, 0, 0));
	SDL_RenderCopy(renderer, topRight_s->texture, NULL, &getDstRect(topRight_s, Width, 0));

	SDL_RenderCopy(renderer, bottomLeft->texture, NULL, &getDstRect(bottomLeft, 0, Height));
	SDL_RenderCopy(renderer, bottomRight->texture, NULL, &getDstRect(bottomRight, Width, Height));

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderFillRect(renderer, &blackRect);

	//Render Box frames
	int xBound = blackRect.x + 505;
	int yBound = blackRect.y + 33;
	SDL_Rect frameRect = { xBound, yBound, 77, 110 };
	SDL_Rect portraitRect;
	for (int i = 0; i < 8; i++) {
		SDL_Texture *renderCharacter;
		if (i == characterIndex) renderCharacter = selectedCharacter->texture;
		else renderCharacter = unselectedCharacter->texture;

		frameRect.x = xBound + ((i % 3) * (frameRect.w + selectHDIV));
		frameRect.y = yBound + ((i / 3) * (frameRect.h + selectVDIV));
		SDL_RenderCopy(renderer, renderCharacter, NULL, &frameRect);

		Texture* portraitTexture = portraits[i];
		portraitRect = portraitTexture->rect;
		portraitRect.x = frameRect.x + (frameRect.w / 2) - (portraitTexture->rect.w / 2);
		portraitRect.y = frameRect.y + (frameRect.h / 2) - (portraitTexture->rect.h / 2);
		SDL_RenderCopy(renderer, portraitTexture->texture, NULL, &portraitRect);
	}

	for (auto widget : widgets) {
		widget.second->Render();
	}
}

void CMainWindow::select_handleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		int xBound = blackRect.x + 505;
		int yBound = blackRect.y + 33;
		SDL_Rect frameRect = { xBound, yBound, 77, 110 };
		for (int i = 0; i < 8; i++) {
			frameRect.x = xBound + ((i % 3) * (frameRect.w + selectHDIV));
			frameRect.y = yBound + ((i / 3) * (frameRect.h + selectVDIV));
			if (doesPointIntersect(frameRect, mx, my)) {
				if (characterIndex != i) {
					characterIndex = i;
					select_setVideo(false);
				}
				break;
			}
		}
	}
}

void CMainWindow::select_step() {
	//
}

void CMainWindow::select_setVideo(bool standby) {
	std::string aviPath = "GUI\\select\\avi\\";
	if (femalePortraits) aviPath += "woman0";
	else aviPath += "man0";
	aviPath += std::to_string(characterIndex + 1) + "\\";
	if (standby) aviPath += "1.avi";
	else aviPath += "2.avi";

	videoFrame->SetRepeat(standby);
	videoFrame->SetVideo(aviPath);
}

void CMainWindow::videoFrame_PlaybackComplete(SDL_Event& e) {
	select_setVideo(true);
}

void CMainWindow::lblMaleOff_Click(SDL_Event &e) {
	lblMaleOn->SetVisible(true);
	lblMaleOff->SetVisible(false);
	lblFemaleOn->SetVisible(false);
	lblFemaleOff->SetVisible(true);
	femalePortraits = false;
	characterIndex = 0;
	select_setVideo(false);

	for (int i = 0; i < 8; i++) {
		std::string portaitPath;
		portaitPath = "GUI/player_icon/Man0" + std::to_string(i + 1) + ".bmp";
		delete portraits[i];
		portraits[i] = new Texture(renderer, portaitPath, true);
	}
}

void CMainWindow::lblFemaleOff_Click(SDL_Event &e) {
	lblMaleOn->SetVisible(false);
	lblMaleOff->SetVisible(true);
	lblFemaleOn->SetVisible(true);
	lblFemaleOff->SetVisible(false);
	femalePortraits = true;
	characterIndex = 0;
	select_setVideo(false);

	for (int i = 0; i < 8; i++) {
		std::string portaitPath;
		portaitPath = "GUI/player_icon/Woman0" + std::to_string(i + 1) + ".bmp";
		delete portraits[i];
		portraits[i] = new Texture(renderer, portaitPath, true);
	}
}

void CMainWindow::btnSelectOk_Click(SDL_Event& e) {
	changeMode(MFM_CREATE_DETAILS);
}

void CMainWindow::btnSelectCancel_Click(SDL_Event& e) {
	changeMode(MFM_LOGIN);
}

/* Main Form - Select End */

/* Main Form - Details Begin */

void CMainWindow::details_init() {
	charCreateForm = new CCharCreateForm();
	Windows.push_back(charCreateForm);

	int sel = characterIndex;
	if (femalePortraits) sel += 8;
	charCreateForm->setCharacter(sel);
}

void CMainWindow::details_cleanup() {
	//
}

void CMainWindow::details_render() {
	SDL_RenderCopy(renderer, logo->texture, NULL, &logo->rect);
}

void CMainWindow::details_handleEvent(SDL_Event& e) {
	if (e.window.windowID != windowID) return;

	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE_OK) {
			charCreateForm = nullptr;
			changeMode(MFM_LOGIN);
		}
		if (e.user.code == WINDOW_CLOSE_BACK) {
			charCreateForm = nullptr;
			changeMode(MFM_CREATE_SELECT);
		}
	}
}

void CMainWindow::details_step() {
	//
}

/* Main Form - Details End */

/* Main Form - Main Begin */

void CMainWindow::main_init() {
	gClient.addToDebugLog("Initializing Main");
	assert(map != nullptr);

	SetTitle("Monster & Me - " + strServer + " - " + std::string(version) + " (" + versionDate + ")");
	SetUseClose(true);
	SetUseMinimize(true);
	Disconnected = false;
	RelogReady = false;

	registerEvent("btnClose", "Click", std::bind(&CMainWindow::btnClose_Click, this, std::placeholders::_1));
	registerEvent("btnMinimize", "Click", std::bind(&CMainWindow::btnMinimize_Click, this, std::placeholders::_1));

	//Game Area
	gameRect = { gui->left->width + 20, gui->topCenter->height + 9, 740, 410 };
	gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gameRect.w, gameRect.h);

	chat = new CChat(this);
	chat->SetRenderer(renderer);
	chat->SetFont(gui->font);
	chat->SetWidth(gameRect.w);
	chat->SetHeightInLines(10);
	chat->SetPos(SDL_Point{ 0, gameRect.h - chat->GetHeight() });

	// Tell the map the UI boundings are
	map->setMapUiRect(gameRect);

	//Generate all the widgets for this mode
	main_init_widgets();

	//Set Default widget values
	setPlayerDetailsLabels();
	setPlayerHealthGauge(player->GetCurrentLife(), player->GetMaxLife());
	setPlayerManaGauge(player->GetCurrentMana(), player->GetMaxMana());
	setPlayerExpGauge(player->experience, player->GetLevelUpExperience());

	Pet* activePet = player->getActivePet();
	if (activePet) {
		setPetHealthGauge(activePet->GetCurrentLife(), activePet->GetMaxLife());
		setPetExpGauge(activePet->getExperience(), activePet->getLevelUpExperience());
	}
}

void CMainWindow::main_init_widgets() {
	//Buttons
	CButton *btnJump = addMainButton("btnJump", surfaceRect.x + 300, surfaceRect.y + 90, 73, 25, "button_fly_off.jpg", "button_fly_on.jpg");
	btnJump->SetType(ButtonType::btToggle);
	registerEvent("btnJump", "Click", std::bind(&CMainWindow::btnJump_Click, this, std::placeholders::_1));

	CButton *btnCharacter = addMainButton("btnCharacter", surfaceRect.x + 17, surfaceRect.y + 11, 73, 25, "button_basicMsg.jpg", "button_basicMsgDown.jpg");
	CButton *btnPet = addMainButton("btnPet", surfaceRect.x + 102, surfaceRect.y + 11, 73, 25, "button_pet.jpg", "button_petDown.jpg");
	registerEvent("btnCharacter", "Click", std::bind(&CMainWindow::btnCharacter_Click, this, std::placeholders::_1));
	registerEvent("btnPet", "Click", std::bind(&CMainWindow::btnPet_Click, this, std::placeholders::_1));

	addMainButton("btnFight", surfaceRect.x + 290, surfaceRect.y + 5, 38, 38, "fight-2.jpg", "fight-1.jpg");
	addMainButton("btnChat", surfaceRect.x + 335, surfaceRect.y + 5, 38, 38, "chat-2.jpg", "chat-1.jpg");
	registerEvent("btnFight", "Click", std::bind(&CMainWindow::btnFight_Click, this, std::placeholders::_1));

	addMainButton("btnDetails", surfaceRect.x + 377, surfaceRect.y, 44, 46, "HeroStatus.jpg", "HeroStatus.jpg");
	addMainButton("btnChat2", surfaceRect.x + 422, surfaceRect.y, 44, 46, "ChatPanel.jpg", "ChatPanel.jpg");
	addMainButton("btnKungfu", surfaceRect.x + 467, surfaceRect.y, 44, 46, "SelectMag.jpg", "SelectMag.jpg");

	addMainButton("btnMinMap", surfaceRect.x + 512, surfaceRect.y + 1, 41, 39, "minMap-1.jpg", "minMap-2.jpg");
	addMainButton("btnJoinTeam", surfaceRect.x + 553, surfaceRect.y + 1, 41, 39, "team-1.jpg", "team-2.jpg");
	addMainButton("btnInventory", surfaceRect.x + 594, surfaceRect.y + 1, 41, 39, "items-1.jpg", "items-2.jpg");
	addMainButton("btnPK", surfaceRect.x + 635, surfaceRect.y + 1, 41, 39, "PK-1.jpg", "PK-2.jpg");
	addMainButton("btnChatHistory", surfaceRect.x + 676, surfaceRect.y + 1, 41, 39, "record-1.jpg", "record-2.jpg");
	addMainButton("btnFriendList", surfaceRect.x + 717, surfaceRect.y + 1, 41, 39, "email-1.jpg", "email-2.jpg");
	registerEvent("btnInventory", "Click", std::bind(&CMainWindow::btnInventory_Click, this, std::placeholders::_1));

	//Labels
	lblCoordX = addMainLabel("lblCoordX", surfaceRect.x + 220, surfaceRect.y + 10, 20, 14, "", true);
	lblCoordY = addMainLabel("lblCoordY", surfaceRect.x + 250, surfaceRect.y + 10, 20, 14, "", true);

	int col1 = 382, col2 = 518, col3 = 660;
	int row1 = 52, row2 = 76, row3 = 100;
	addMainLabel("lblStaticName", surfaceRect.x + col1, surfaceRect.y + row1, 40, 14, "Name", true);
	addMainLabel("lblStaticNickName", surfaceRect.x + col1, surfaceRect.y + row2, 60, 14, "Nickname", true);
	addMainLabel("lblStaticLevel", surfaceRect.x + col1, surfaceRect.y + row3, 35, 14, "Level", true);
	lblName = addMainLabel("lblName", surfaceRect.x + col1 + 45, surfaceRect.y + row1, 90, 14, "");
	lblNickName = addMainLabel("lblNickName", surfaceRect.x + col1 + 67, surfaceRect.y + row2, 75, 14, "");
	lblLevel = addMainLabel("lblLevel", surfaceRect.x + col1 + 37, surfaceRect.y + row3, 95, 14, "");

	addMainLabel("lblStaticSpouse", surfaceRect.x + col2, surfaceRect.y + row1, 45, 14, "Spouse", true);
	addMainLabel("lblStaticCash", surfaceRect.x + col2, surfaceRect.y + row2, 30, 14, "Cash", true);
	addMainLabel("lblStaticReputation", surfaceRect.x + col2, surfaceRect.y + row3, 65, 14, "Reputation", true);
	lblSpouse = addMainLabel("lblSpouse", surfaceRect.x + col2 + 52, surfaceRect.y + row1, 90, 14, "None");
	lblCash = addMainLabel("lblCash", surfaceRect.x + col2 + 38, surfaceRect.y + row2, 105, 14, "1");
	lblReputation = addMainLabel("lblReputation", surfaceRect.x + col2 + 73, surfaceRect.y + row3, 75, 14, "2");
	
	lblRank = addMainLabel("lblRank", surfaceRect.x + col3, surfaceRect.y + row1, 118, 14, "Mortal", false);
	addMainLabel("lblStaticGuild", surfaceRect.x + col3, surfaceRect.y + row2, 32, 14, "Guild", true);
	addMainLabel("lblStaticGuildRank", surfaceRect.x + col3, surfaceRect.y + row3, 50, 14, "Position", true);
	lblGuild = addMainLabel("lblGuild", surfaceRect.x + col3 + 37, surfaceRect.y + row2, 90, 14, "");
	lblGuildRank = addMainLabel("lblGuildRank", surfaceRect.x + col3 + 57, surfaceRect.y + row3, 70, 14, "");

	//Guages
	gaugePlayerHealth = addMainGauge("gaugePlayerHealth", surfaceRect.x + 62, surfaceRect.y + 47, 110, 16, "yellow_slot.jpg");
	gaugePlayerMana = addMainGauge("gaugePlayerMana", surfaceRect.x + 62, surfaceRect.y + 70, 110, 16, "green_slot.jpg");
	gaugePlayerExp = addMainGauge("gaugePlayerExp", surfaceRect.x + 62, surfaceRect.y + 93, 110, 16, "green_slot.jpg");
	gaugePetHealth = addMainGauge("gaugePetHealth", surfaceRect.x + 259, surfaceRect.y + 47, 110, 16, "yellow_slot.jpg");
	gaugePetExp = addMainGauge("gaugePetExp", surfaceRect.x + 259, surfaceRect.y + 70, 110, 16, "green_slot.jpg");
}

void CMainWindow::main_cleanup() {
	ClearWidgets();
	dcPromptForm = nullptr;
	if (map) {
		delete map;
		map = nullptr;
	}
	if (battle) {
		delete battle;
		battle = nullptr;
	}
	if (chat) {
		delete chat;
		chat = nullptr;
	}
	if (player) {
		delete player;
		player = nullptr;
	}
}

void CMainWindow::main_render() {
	main_render_ui();

	//Render target set to game area
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, gameTexture); {
		SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(renderer);

		//Battle & Map
		if (battle) {
			if (battle->getMode() != BattleMode::bmInit) battle->render();
			else map->render();
		}
		else {
			if (map) map->render();
		}

		//Chat
		if (chat) chat->render();
	} SDL_SetRenderTarget(renderer, priorTarget);
	SDL_RenderCopy(renderer, gameTexture, NULL, &gameRect);

	for (auto widget : widgets) {
		widget.second->Render();
	}
}

void CMainWindow::main_render_ui() {
	//render all UI pieces in order
	SDL_RenderCopy(renderer, topCenter->texture, NULL, &getDstRect(topCenter, 0, 0));

	SDL_RenderCopy(renderer, bottomCenter->texture, NULL, &getDstRect(bottomCenter, 0, Height));
	SDL_RenderCopy(renderer, left->texture, NULL, &getDstRect(left, 0, 0));
	SDL_RenderCopy(renderer, right->texture, NULL, &getDstRect(right, Width, 0));

	SDL_RenderCopy(renderer, topLeft->texture, NULL, &getDstRect(topLeft, 0, 0));
	SDL_RenderCopy(renderer, topRight->texture, NULL, &getDstRect(topRight, Width, 0));

	SDL_RenderCopy(renderer, bottomLeft->texture, NULL, &getDstRect(bottomLeft, 0, Height));
	SDL_RenderCopy(renderer, bottomRight->texture, NULL, &getDstRect(bottomRight, Width, Height));

	SDL_RenderCopy(renderer, mainWindow->texture, NULL, &getDstRect(mainWindow, left->width, topCenter->height));
	SDL_RenderCopy(renderer, surface->texture, NULL, &getDstRect(surface, left->width, (Height - surface->height - bottomCenter->height)));

	//Move to image buttons
	//SDL_RenderCopy(renderer, minimize->texture, NULL, &getDstRect(minimize, (Width - 45), 10));
	//SDL_RenderCopy(renderer, close->texture, NULL, &getDstRect(close, (Width - 25), 10));
}

void CMainWindow::main_handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_PLAYER) {
		if (e.user.code == PLAYER_RENAME) {
			lblNickName->SetText(player->getNickName());
		}

		if (e.user.code == PLAYER_UPDATE) {
			setPlayerDetailsLabels();
		}

		if (e.user.code == PLAYER_LIFE) {
			gaugePlayerHealth->AdjustTo(player->GetCurrentLife());
		}
		if (e.user.code == PLAYER_LIFEMAX) {
			gaugePlayerHealth->set(player->GetCurrentLife(), player->GetMaxLife());
		}
		if (e.user.code == PLAYER_MANA) {
			gaugePlayerMana->AdjustTo(player->GetCurrentMana());
		}
		if (e.user.code == PLAYER_MANAMAX) {
			gaugePlayerMana->set(player->GetCurrentMana(), player->GetMaxMana());
		}
		if (e.user.code == PLAYER_LIFEMANA) {
			gaugePlayerHealth->AdjustTo(player->GetCurrentLife());
			gaugePlayerMana->AdjustTo(player->GetCurrentMana());
		}
		if (e.user.code == PLAYER_EXP) {
			gaugePlayerExp->AdjustTo(player->GetExperience());
		}

		if (e.user.code == PLAYER_LEVEL) {
			setPlayerDetailsLabels();
			gaugePlayerExp->set(player->GetExperience(), player->GetLevelUpExperience());
		}

		if (e.user.code == PLAYER_MONEY) {
			lblCash->SetText(formatInt(player->GetCash()));
		}
		if (e.user.code == PLAYER_REPUTATION) {
			lblReputation->SetText(formatInt(player->GetReputation()));
		}
	}

	if (e.type == CUSTOMEVENT_PET) {
		if (e.user.code == PET_MARCHING) {
			Pet* pet = player->getActivePet();
			if (pet) {
				setPetHealthGauge(pet->GetCurrentLife(), pet->GetMaxLife());
				setPetExpGauge(pet->getExperience(), pet->getLevelUpExperience());
			}
		}

		if (e.user.code == PET_LIFE) {
			Pet* pet = player->getActivePet();
			if (pet) gaugePetHealth->AdjustTo(pet->GetCurrentLife());
		}
		if (e.user.code == PET_EXP) {
			Pet* pet = player->getActivePet();
			if (pet) gaugePetExp->AdjustTo(pet->getExperience());
		}
		if (e.user.code == PET_LEVEL) {
			Pet* pet = player->getActivePet();
			if (pet) {
				gaugePetHealth->set(pet->GetCurrentLife(), pet->GetMaxLife());
				gaugePetExp->set(pet->getExperience(), pet->getLevelUpExperience());
			}
		}
	}

	//After Disconnect Prompt
	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE_PROMPT_OK && e.user.data1 == dcPromptForm) {
			dcPromptForm = nullptr;
			RelogReady = true;
			return;
		}
	}

	if (battle && battle->getMode() != bmInit) {
		if (battle->handleEvent(e)) return;
	}
	if (chat) chat->handleEvent(e);
	if (!battle && map->handleEvent(e)) return;
}

void CMainWindow::main_step() {
	if (gClient.wasDisconnected()) {
		Disconnected = true;
		RelogReady = false;
		if (!dcPromptForm) {
			for (auto window : Windows) window->CloseWindow = true;
			dcPromptForm = new CPromptForm();
			dcPromptForm->SetMessage("Error: Connection to the server has been interrupted. Please re-log in.");
			dcPromptForm->SetParent(this);
			Windows.push_back(dcPromptForm);
		}
	}
	if (RelogReady) {
		RelogReady = false;
		changeMode(MFM_LOGIN);
	}

	if (battle) {
		battle->step();
		if (battle->isOver()) {
			delete battle;
			battle = nullptr;
		}
	}
	else if (map) map->step();

	//User details on mouseover revert after 2 seconds
	if (userDetailsStartTime > 0) {
		int curTime = SDL_GetTicks();
		if (curTime - userDetailsStartTime > 2000) setPlayerDetailsLabels();
	}
}

/* Main Form - Events */

void CMainWindow::btnClose_Click(SDL_Event& e) {
	if (Mode == MFM_MAIN) {
		CLogoutForm *logoutForm = new CLogoutForm();
		Windows.push_back(logoutForm);
	}
	else {
		applicationClose();
	}
}

void CMainWindow::btnJump_Click(SDL_Event& e) {
	CButton *btnJump = (CButton*)GetWidget("btnJump");
	if (btnJump) {
		map->toggleJumpMode();
	}
}

void CMainWindow::btnFight_Click(SDL_Event& e) {
	if (battle) return;
	if (!map || map->IsChangingMap()) return;

	int teamSize = 1;
	pBattleState* battlePacket = new pBattleState(0, teamSize, player->GetID(), 0);
	gClient.addPacket(battlePacket);
}

void CMainWindow::btnCharacter_Click(SDL_Event& e) {
	if (battle) return;
	CCharacterForm* charForm = new CCharacterForm();
	Windows.push_back(charForm);
}

void CMainWindow::btnPet_Click(SDL_Event& e) {
	if (battle) return;
	CPetListForm* petForm = new CPetListForm();
	Windows.push_back(petForm);
}

void CMainWindow::btnInventory_Click(SDL_Event& e) {
	if (battle) return;
	CInventoryForm* invForm = new CInventoryForm();
	Windows.push_back(invForm);
}

/* Main Form - Hooks */

void CMainWindow::setPlayerDetailsLabels() {
	if (!player) return;
	if (Mode != MFM_MAIN) return;
	lastMouseoverUser = nullptr;
	userDetailsStartTime = 0;

	lblName->SetText(StringToWString(player->GetName()));
	lblNickName->SetText(StringToWString(player->getNickName()));
	lblSpouse->SetText(StringToWString(player->getSpouse()));

	std::string strLevelCult = formatInt(player->GetLevel()) + "/" + formatInt(player->GetCultivation());
	lblLevel->SetText(strLevelCult);

	lblCash->SetText(formatInt(player->GetCash()));
	lblReputation->SetText(formatInt(player->GetReputation()));

	lblRank->SetText(player->GetRankText());
	lblGuild->SetText(StringToWString(player->getGuild()));
	lblGuildRank->SetText(StringToWString(player->getGuildTitle()));
}

void CMainWindow::setUserDetailsLabels(User* user) {
	if (!user) return;
	userDetailsStartTime = SDL_GetTicks();
	if (user == lastMouseoverUser) return;
}

void CMainWindow::setMapCoordLabels(SDL_Point coord) {
	lblCoordX->SetText(std::to_string(coord.x));
	lblCoordY->SetText(std::to_string(coord.y));
}

void CMainWindow::setPlayerHealthGauge(int val) {
	gaugePlayerHealth->set(val);
}


void CMainWindow::setPlayerHealthGauge(int val, int max) {
	gaugePlayerHealth->set(val, max);
}


void CMainWindow::shiftPlayerHealthGauge(int val) {
	if (val >= 0) gaugePlayerHealth->add(val);
	else gaugePlayerHealth->subtract(val);
}


void CMainWindow::setPlayerManaGauge(int val) {
	gaugePlayerMana->set(val);
}


void CMainWindow::setPlayerManaGauge(int val, int max) {
	gaugePlayerMana->set(val, max);
}


void CMainWindow::shiftPlayerManaGauge(int val) {
	if (val >= 0) gaugePlayerMana->add(val);
	else gaugePlayerMana->subtract(val);
}


void CMainWindow::setPlayerExpGauge(int val) {
	gaugePlayerExp->set(val);
}


void CMainWindow::setPlayerExpGauge(int val, int max) {
	gaugePlayerExp->set(val, max);
}


void CMainWindow::shiftPlayerExpGauge(int val) {
	if (val >= 0) gaugePlayerExp->add(val);
	else gaugePlayerExp->subtract(val);
}


void CMainWindow::setPetHealthGauge(int val) {
	gaugePetHealth->set(val);
}


void CMainWindow::setPetHealthGauge(int val, int max) {
	gaugePetHealth->set(val, max);
}


void CMainWindow::shiftPetHealthGauge(int val) {
	if (val >= 0) gaugePetHealth->add(val);
	else gaugePetHealth->subtract(val);
}


void CMainWindow::setPetExpGauge(int val) {
	gaugePetExp->set(val);
}


void CMainWindow::setPetExpGauge(int val, int max) {
	gaugePetExp->set(val, max);
}


void CMainWindow::shiftPetExpGauge(int val) {
	if (val >= 0) gaugePetExp->add(val);
	else gaugePetExp->subtract(val);
}

void CMainWindow::openShop(int sId) {
	/*shopId = sId;
	if (shopId == 0) return;

	//read shopini
	INI shopIni = *gClient.shopIni;
	std::string shopSection = "ShopId" + std::to_string(shopId);
	shopIni.setSection(shopSection);

	int itemCount = stoi(shopIni.getEntry("Count"));
	std::vector<int> shopItems;
	for (int i = 0; i < itemCount; i++) {
		std::string nextItem = "ItemType" + std::to_string(i);
		shopItems.push_back(stoi(shopIni.getEntry(nextItem)));
	}

	//create shopform
	FormShop* shopForm = new FormShop("Shop", width / 2, height / 2);
	shopForm->addShopInventory(shopItems);
	Form::addSubform("Shop", shopForm);*/
}

/* Main Form - Main End */