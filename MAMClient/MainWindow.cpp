#include "stdafx.h"
#include "MainWindow.h"
#include "Global.h"
#include "Define.h"
#include "Options.h"
#include "CustomEvents.h"
#include "SDL_syswm.h"

#include "Client.h"
#include "GUI.h"
#include "GameMap.h"
#include "Battle.h"
#include "Chat.h"

#include "Player.h"
#include "Pet.h"
#include "Team.h"

#include "LoginForm.h"
#include "LogoutForm.h"
#include "PromptForm.h"
#include "CharCreateForm.h"
#include "CharacterForm.h"
#include "PetListForm.h"
#include "InventoryForm.h"
#include "WuxingForm.h"

#include "AssetManager.h"
#include "PlayerInfoFrame.h"
#include "Gauge.h"
#include "ImageBox.h"
#include "VideoFrame.h"
#include "Dialogue.h"

#include "PetMagic.h" //temp, testing

#include "pBattleState.h"
#include "pAction.h"

CMainWindow::CMainWindow() :CWindow() {
	Type = FT_MAIN;
#ifdef SIZE_1024
	Width = 1024;
	Height = 768;
#else
	Width = 800;
	Height = 600;
#endif
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
#ifdef NEWGUI
	Uint32 flags = SDL_WINDOW_SHOWN;
#else
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS;
#endif
	
	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, flags);
	if (window != NULL) {
#ifndef NEWGUI
		SDL_SetWindowBordered(window, SDL_FALSE);
#endif
		mouseFocus = true;
		keyboardFocus = true;

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version); /* initialize info structure with SDL version info */
		if (SDL_GetWindowWMInfo(window, &info)) {
			hwnd = info.info.win.window; //pull hwnd from sdl version info
		}

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
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x07, 0xFF, 0xFF);
			SDL_RenderClear(renderer);

			// Change window type to layered (https://stackoverflow.com/a/3970218/3357935)
			SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
			// Set transparency color
			SetLayeredWindowAttributes(hwnd, RGB(255,7,255), 0, LWA_COLORKEY);

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

#ifdef NEWGUI
	//newGui = gui->getSkinTexture(renderer, "newmain/main.tga", Anchor::aBottomLeft);
#endif

#ifdef SIZE_1024
	topCenter = gui->getSkinTexture(renderer, "1024/TopCenter.bmp", Anchor::aTopLeft);
	mainWindow = gui->getSkinTexture(renderer, "1024/mainwindow.jpg", Anchor::aTopLeft);
	surface = gui->getSkinTexture(renderer, "1024/surface.jpg", Anchor::aTopLeft);
#else
	topCenter = gui->getSkinTexture(renderer, "TopCenter.bmp", Anchor::ANCOR_TOPLEFT);
	mainWindow = gui->getSkinTexture(renderer, "mainwindow.jpg", Anchor::ANCOR_TOPLEFT);
	surface = gui->getSkinTexture(renderer, "surface.jpg", Anchor::ANCOR_TOPLEFT);
#endif

	topLeft = gui->getSkinTexture(renderer, "TopLeft.bmp", Anchor::ANCOR_TOPLEFT);
	topRight = gui->getSkinTexture(renderer, "TopRight.bmp", Anchor::ANCOR_TOPRIGHT);
	
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
	//SDL_SetRenderDrawColor(renderer, 0xFF, 0x07, 0xFF, 0xFF);
	//SDL_RenderClear(renderer);
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

CButton* CMainWindow::addMainButton2(std::string name, int x, int y, int w, int h, std::string unpressed, std::string pressed) {
	CButton *btn = new CButton(this, name, x, y);
	btn->SetWidth(w);
	btn->SetHeight(h);
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
	gauge->SetForegroundImage(foreground);
#ifdef NEWGUI
	std::string background = foreground.substr(0, foreground.find_first_of('.')) + "2" + foreground.substr(foreground.find_first_of('.'), std::string::npos);
	gauge->SetBackgroundImage(background);
	gauge->SetVerticle(true);
#else
	gauge->SetUseGUI(true);
	gauge->SetBackgroundImage("bg_slot.jpg");
	gauge->SetIncreaseImage("orange_slot.jpg");
	gauge->SetDecreaseImage("red_slot.jpg");
#endif
	AddWidget(gauge);
	return gauge;
}

CGauge* CMainWindow::addMainGauge2(std::string name, int x, int y, int w, int h, std::string foreground) {
	CGauge *gauge = new CGauge(this, name, x, y);
	gauge->SetWidth(w);
	gauge->SetHeight(h);
	gauge->SetForegroundImage(foreground);
	std::string background = foreground.substr(0, foreground.find_first_of('.')) + "_Back" + foreground.substr(foreground.find_first_of('.'), std::string::npos);
	gauge->SetBackgroundImage(background);
	//gauge->SetVerticle(true);
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
	selectedCharacter = gui->getSkinTexture(renderer, "BoxFocus.bmp", ANCOR_TOPLEFT);
	unselectedCharacter = gui->getSkinTexture(renderer, "BoxNormal.bmp", ANCOR_TOPLEFT);

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
	//SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 0xFF);
	//SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
	//SDL_RenderClear(renderer);

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
	//SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0xFF, 0xFF);
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
#ifndef NEWGUI
	SetUseClose(true);
	SetUseMinimize(true);
	registerEvent("btnClose", "Click", std::bind(&CMainWindow::btnClose_Click, this, std::placeholders::_1));
	registerEvent("btnMinimize", "Click", std::bind(&CMainWindow::btnMinimize_Click, this, std::placeholders::_1));
#endif // !NEWGUI
	Disconnected = false;
	RelogReady = false;

	//Game Area
#ifdef NEWGUI
	gameRect = { 0, 0, 800, 600 };
#else
	gameRect = { gui->left->width + 20, gui->topCenter->height + 9, 740, 410 };
#endif
	gameTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gameRect.w, gameRect.h);

	chat = new CChat(this);
	//chat->SetRenderer(renderer);
	chat->SetFont(gui->font);
	//chat->SetWidth(gameRect.w);
	//chat->SetWidth(350);
	//chat->SetHeightInLines(7);
#ifdef NEWGUI
	//chat->SetPos(SDL_Point{ 0, Height - chat->GetHeight() - 30 });
#else
	chat->SetPos(SDL_Point{ 0, gameRect.h - chat->GetHeight() });
#endif
	AddWidget(chat);

	// Tell the map the UI boundings are
	map->setMapUiRect(gameRect);

	//Generate all the widgets for this mode
	main_init_widgets();

	//Set Default widget values
#ifndef NEWGUI
	setPlayerDetailsLabels();
#endif // !NEWGUI
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
#ifdef NEWGUI
	std::string portraitPath;
	Texture *txturePortrait;

	playerInfoFrame = new CPlayerInfoFrame(this, "playerInfoFrame", 0, 0);
	AddWidget(playerInfoFrame);

	//gaugePlayerHealth = playerInfoFrame->GetPlayerHealthGauge();
	//gaugePlayerMana = playerInfoFrame->GetPlayerManaGauge();
	//gaugePlayerExp = playerInfoFrame->GetPlayerExpGauge();

	/*petPortrait = new CImageBox(this, "petPortrait", 108, 600);
	petPortrait->SetWidth(55);
	petPortrait->SetHeight(69);
	AddWidget(petPortrait);
	main_setPetPortrait();*/

	//gaugePetHealth = addMainGauge2("gaugePetHealth", 97, 123, 50, 20, "data/GUI/main/Pet_Health.png");
	//gaugePetExp = addMainGauge2("gaugePetExp", 108, 137, 38, 20, "data/GUI/main/Pet_Exp.png");
	//gaugePetHealth = addMainGauge("gaugePetHealth", 164, Height - 90 - 12, 20, 90, "newmain/pethp.tga");
	//gaugePetExp = addMainGauge("gaugePetExp", 170, Height - 79 - 6, 36, 79, "newmain/petexp.tga");
	//gaugePetHealth = playerInfoFrame->GetPetLifeGauge();
	//gaugePetExp = playerInfoFrame->GetPetExpGauge();

	CButton *btnJump = addMainButton2("btnJump", 387, 538, 64, 24, "data/GUI/main/btnWalk.png", "data/GUI/main/btnJump.png");
	addMainButton2("btnCloud", 368, 540, 20, 20, "data/GUI/main/btnCloud.png", "data/GUI/main/btnCloud.png");
	addMainButton2("btnFight", 387, 562, 64, 24, "data/GUI/main/btnFight.png", "data/GUI/main/btnFight.png");
	addMainButton2("btnAutoFight", 368, 562, 20, 20, "data/GUI/main/btnAutoFight.png", "data/GUI/main/btnAutoFight.png");

	//addMainButton2("btnCharacter", 390, 600-60, 60, 60, "button_basicMsg.jpg", "button_basicMsgDown.jpg");
	//addMainButton2("btnPet", 465, 600-35, 60, 60, "button_pet.jpg", "button_petDown.jpg");

	addMainButton2("btnMap", 448, 532, 60, 60, "data/GUI/main/btnMap.png", "data/GUI/main/btnMap.png");
	addMainButton2("btnInventory", 504, 532, 60, 60, "data/GUI/main/btnInventory.png", "data/GUI/main/btnInventory.png");
	addMainButton2("btnWuxing", 562, 532, 60, 60, "data/GUI/main/btnWuxing.png", "data/GUI/main/btnWuxing.png");
	addMainButton2("btnKungfu", 618, 532, 60, 60, "data/GUI/main/btnKungfu.png", "data/GUI/main/btnKungfu.png");
	addMainButton2("btnTeam", 674, 532, 60, 60, "data/GUI/main/btnTeam.png", "data/GUI/main/btnTeam.png");
	//addMainButton2("btnFriend", 684, 532, 60, 60, "data/GUI/main/btnFriend.png", "data/GUI/main/btnFriend.png");
	addMainButton2("btnGuild", 730, 532, 60, 60, "data/GUI/main/btnGuild.png", "data/GUI/main/btnGuild.png");

	addMainButton2("btnTeamCreate", 674, 500, 83, 21, "data/GUI/Team/btnTeamCreate.png", "data/GUI/Team/btnTeamCreate.png")->SetVisible(false);
	addMainButton2("btnTeamJoin", 674, 500, 83, 21, "data/GUI/Team/btnTeamJoin.png", "data/GUI/Team/btnTeamJoin.png")->SetVisible(false);
	addMainButton2("btnTeamManage", 674, 500, 83, 21, "data/GUI/Team/btnTeamManage.png", "data/GUI/Team/btnTeamManage.png")->SetVisible(false);
	addMainButton2("btnTeamLeave", 674, 500, 83, 21, "data/GUI/Team/btnTeamLeave.png", "data/GUI/Team/btnTeamLeave.png")->SetVisible(false);
	addMainButton2("btnTeamDisband", 674, 500, 83, 21, "data/GUI/Team/btnTeamDisband.png", "data/GUI/Team/btnTeamDisband.png")->SetVisible(false);

	MenuBG.reset(new Texture(renderer, "data/GUI/Main/menu_bg.png", true));
	MenuRight.reset(new Texture(renderer, "data/GUI/Main/menu_right.png", true));
	btnMenuCollapse = addMainButton2("btnMenuCollapse", 340, Height - 78, 30, 78, "data/GUI/Main/btnMenuCollapse.png", "");
	btnMenuExpand = addMainButton2("btnMenuExpand", Width - 31, Height - 78, 30, 78, "data/GUI/Main/btnMenuExpand.png", "");
	btnMenuExpand->SetVisible(false);
	registerEvent("btnMenuCollapse", "Click", std::bind(&CMainWindow::btnMenuCollapse_Click, this, std::placeholders::_1));
	registerEvent("btnMenuExpand", "Click", std::bind(&CMainWindow::btnMenuExpand_Click, this, std::placeholders::_1));
#else
	//Buttons
	CButton *btnJump = addMainButton("btnJump", surfaceRect.x + 300, surfaceRect.y + 90, 73, 25, "button_fly_off.jpg", "button_fly_on.jpg");

	addMainButton("btnCharacter", surfaceRect.x + 17, surfaceRect.y + 11, 73, 25, "button_basicMsg.jpg", "button_basicMsgDown.jpg");
	addMainButton("btnPet", surfaceRect.x + 102, surfaceRect.y + 11, 73, 25, "button_pet.jpg", "button_petDown.jpg");

	addMainButton("btnFight", surfaceRect.x + 290, surfaceRect.y + 5, 38, 38, "fight-2.jpg", "fight-1.jpg");
	addMainButton("btnChat", surfaceRect.x + 335, surfaceRect.y + 5, 38, 38, "chat-2.jpg", "chat-1.jpg");

	addMainButton("btnDetails", surfaceRect.x + 377, surfaceRect.y, 44, 46, "HeroStatus.jpg", "HeroStatus.jpg");
	addMainButton("btnChat2", surfaceRect.x + 422, surfaceRect.y, 44, 46, "ChatPanel.jpg", "ChatPanel.jpg");
	addMainButton("btnKungfu", surfaceRect.x + 467, surfaceRect.y, 44, 46, "SelectMag.jpg", "SelectMag.jpg");

	addMainButton("btnMinMap", surfaceRect.x + 512, surfaceRect.y + 1, 41, 39, "minMap-1.jpg", "minMap-2.jpg");
	addMainButton("btnJoinTeam", surfaceRect.x + 553, surfaceRect.y + 1, 41, 39, "team-1.jpg", "team-2.jpg");
	addMainButton("btnInventory", surfaceRect.x + 594, surfaceRect.y + 1, 41, 39, "items-1.jpg", "items-2.jpg");
	addMainButton("btnPK", surfaceRect.x + 635, surfaceRect.y + 1, 41, 39, "PK-1.jpg", "PK-2.jpg");
	addMainButton("btnChatHistory", surfaceRect.x + 676, surfaceRect.y + 1, 41, 39, "record-1.jpg", "record-2.jpg");
	addMainButton("btnFriendList", surfaceRect.x + 717, surfaceRect.y + 1, 41, 39, "email-1.jpg", "email-2.jpg");

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
#endif

	btnJump->SetType(ButtonType::btToggle);
	registerEvent("btnJump", "Click", std::bind(&CMainWindow::btnJump_Click, this, std::placeholders::_1));
	registerEvent("btnCloud", "Click", std::bind(&CMainWindow::btnCloud_Click, this, std::placeholders::_1));
	registerEvent("btnFight", "Click", std::bind(&CMainWindow::btnFight_Click, this, std::placeholders::_1));
	registerEvent("btnAutoFight", "Click", std::bind(&CMainWindow::btnAutoFight_Click, this, std::placeholders::_1));

	registerEvent("btnCharacter", "Click", std::bind(&CMainWindow::btnCharacter_Click, this, std::placeholders::_1));
	registerEvent("btnPet", "Click", std::bind(&CMainWindow::btnPet_Click, this, std::placeholders::_1));

	registerEvent("btnMap", "Click", std::bind(&CMainWindow::btnMap_Click, this, std::placeholders::_1));
	registerEvent("btnInventory", "Click", std::bind(&CMainWindow::btnInventory_Click, this, std::placeholders::_1));
	registerEvent("btnWuxing", "Click", std::bind(&CMainWindow::btnWuxing_Click, this, std::placeholders::_1));
	registerEvent("btnKungfu", "Click", std::bind(&CMainWindow::btnKungfu_Click, this, std::placeholders::_1));
	registerEvent("btnTeam", "Click", std::bind(&CMainWindow::btnTeam_Click, this, std::placeholders::_1));
	registerEvent("btnGuild", "Click", std::bind(&CMainWindow::btnGuild_Click, this, std::placeholders::_1));

	registerEvent("btnTeamCreate", "Click", std::bind(&CMainWindow::btnTeamCreate_Click, this, std::placeholders::_1));
	registerEvent("btnTeamJoin", "Click", std::bind(&CMainWindow::btnTeamJoin_Click, this, std::placeholders::_1));
	registerEvent("btnTeamManage", "Click", std::bind(&CMainWindow::btnTeamManage_Click, this, std::placeholders::_1));
	registerEvent("btnTeamLeave", "Click", std::bind(&CMainWindow::btnTeamLeave_Click, this, std::placeholders::_1));
	registerEvent("btnTeamDisband", "Click", std::bind(&CMainWindow::btnTeamDisband_Click, this, std::placeholders::_1));
}

void CMainWindow::main_setPetPortrait() {
	Pet* activePet = player->getActivePet();
	if (!activePet) return;

	petPortrait->SetImage(nullptr);

	std::string look = std::to_string(activePet->GetLook());
	while (look.size() < 4) look.insert(look.begin(), '0');
	look = "peticon" + look;

	INI ini("INI\\petLook.ini", look);
	std::string portraitPath = ini.getEntry("Frame0");
	Texture *txturePortrait = new Texture(renderer, portraitPath, { 0,0,0,255 }, true);
	SDL_SetTextureBlendMode(txturePortrait->texture, SDL_BLENDMODE_BLEND);
	petPortrait->SetImage(txturePortrait);
	petPortrait->SetY(600 - 77);
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
	/*if (chat) {
		delete chat;
		chat = nullptr;
	}*/ //chat is now a widget
	if (player) {
		delete player;
		player = nullptr;
	}
}

void CMainWindow::main_render() {
	//Render target set to game area
	//SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);
	//SDL_SetRenderTarget(renderer, gameTexture);
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
	//SDL_SetRenderTarget(renderer, NULL);
	//SDL_RenderCopy(renderer, gameTexture, NULL, &gameRect);

	main_render_ui();
}

void CMainWindow::main_render_ui() {
	std::string strTicks = "AvgTick: " + std::to_string(AverageTickLength);
	Asset astTicks(stringToTexture(renderer, strTicks, gui->font, 0, { 255, 255, 255, 255 }, 0));
	boxRGBA(renderer, Width - astTicks->width, 0, Width, astTicks->height, 0, 0, 0, 255);
	astTicks->Render(SDL_Point{ Width - astTicks->width, 0 });

	//Menu
	if (!bMenuHidden || battle) {
		//Stretch the menuBG an extra 20 pixels 
		SDL_Rect menuRect = { Width - (MenuBG->width + 20) - 10, Height - MenuRight->height + 3, MenuBG->width, MenuBG->height };
		SDL_RenderCopy(renderer, MenuBG->texture, NULL, &menuRect);
		menuRect = { Width - MenuRight->width, Height - MenuRight->height, MenuRight->width, MenuRight->height };
		SDL_RenderCopy(renderer, MenuRight->texture, NULL, &menuRect);
	}

	//Widgets after Menu 
	for (auto widget : widgets) {
		widget.second->Render();
	}

	if (battle && battle->getMode() != BattleMode::bmInit) battle->render_ui();

	//Dialogue
	if (dialogue) dialogue->Render();
}

void CMainWindow::main_handleEvent(SDL_Event& e) {
	if (e.type == CUSTOMEVENT_BATTLE) {
		if (e.user.code == BATTLE_START) OnBattle_Start(e);
		if (e.user.code == BATTLE_END) OnBattle_End(e);
	}

	if (e.type == CUSTOMEVENT_PLAYER) {
#ifndef NEWGUI
		if (e.user.code == PLAYER_RENAME) {
			lblNickName->SetText(player->getNickName());
		}

		if (e.user.code == PLAYER_UPDATE) {
			setPlayerDetailsLabels();
		}
#endif

		if (e.user.code == PLAYER_LIFE) {
			playerInfoFrame->adjustPlayerHealthGauge(player->GetCurrentLife());
		}
		if (e.user.code == PLAYER_LIFEMAX) {
			playerInfoFrame->setPlayerHealthGauge(player->GetCurrentLife(), player->GetMaxLife());
		}
		if (e.user.code == PLAYER_MANA) {
			playerInfoFrame->adjustPlayerManaGauge(player->GetCurrentMana());
		}
		if (e.user.code == PLAYER_MANAMAX) {
			playerInfoFrame->setPlayerManaGauge(player->GetCurrentMana(), player->GetMaxMana());
		}
		if (e.user.code == PLAYER_LIFEMANA) {
			playerInfoFrame->adjustPlayerHealthGauge(player->GetCurrentLife());
			playerInfoFrame->adjustPlayerManaGauge(player->GetCurrentMana());
		}
		if (e.user.code == PLAYER_EXP) {
			playerInfoFrame->adjustPlayerExpGauge(player->GetExperience());
		}

		if (e.user.code == PLAYER_LEVEL) {
#ifndef NEWGUI
			setPlayerDetailsLabels();
#endif
			playerInfoFrame->setPlayerExpGauge(player->GetExperience(), player->GetLevelUpExperience());
			playerInfoFrame->updatePlayerLevel();
		}

#ifndef NEWGUI
		if (e.user.code == PLAYER_MONEY) {
			lblCash->SetText(formatInt(player->GetCash()));
		}
		if (e.user.code == PLAYER_REPUTATION) {
			lblReputation->SetText(formatInt(player->GetReputation()));
		}
#endif
		if (e.user.code == PLAYER_TEAM) {
			if (ShowingTeamButtons) ShowTeamButtons(); //updates appropriate visible team buttons
		}
	}

	if (e.type == CUSTOMEVENT_PET) {
		if (e.user.code == PET_MARCHING) {
			playerInfoFrame->Reload();
			Pet* pet = player->getActivePet();
			if (pet) {
				setPetHealthGauge(pet->GetCurrentLife(), pet->GetMaxLife());
				setPetExpGauge(pet->getExperience(), pet->getLevelUpExperience());
			}
		}

		if (e.user.code == PET_LIFE) {
			Pet* pet = player->getActivePet();
			if (pet) playerInfoFrame->adjustPetHealthGauge(pet->GetCurrentLife());
		}
		if (e.user.code == PET_EXP) {
			Pet* pet = player->getActivePet();
			if (pet) playerInfoFrame->adjustPetExpGauge(pet->getExperience());
		}
		if (e.user.code == PET_LEVEL) {
			Pet* pet = player->getActivePet();
			if (pet) {
				playerInfoFrame->setPetHealthGauge(pet->GetCurrentLife(), pet->GetMaxLife());
				playerInfoFrame->setPetExpGauge(pet->getExperience(), pet->getLevelUpExperience());
				playerInfoFrame->updatePetLevel();
			}
		}
	}

	if (e.type == CUSTOMEVENT_USER) {
		if (e.user.code == USER_RIGHTCLICK && GameMode == GAMEMODE_JOINTEAM) {
			CTeam::Join((User*)e.user.data1);
			GameMode = GAMEMODE_NONE;
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

	/*if (chat) {
		SDL_Event e2 = e;
		if (e2.type == SDL_MOUSEMOTION || e2.type == SDL_MOUSEBUTTONDOWN || e2.type == SDL_MOUSEBUTTONUP) {
			e2.motion.x -= gameRect.x;
			e2.motion.y -= gameRect.y;
			chat->handleEvent(e2);
			if (chat->IsMouseOver() && chat->IsBlockMouse()) return;
		}

		chat->handleEvent(e2);
	}*/

	//Dialogue sits ontop of window and widgets
	if (dialogue) {
		dialogue->HandleEvent(e);
		if (dialogue->IsMouseOver()) return;
	}

	//Verify no widgets have taken ownership of the mouse
	//Prevents improper mouse clickthrough when clicking buttons etc
	if (WidgetHasMouse()) return;

	if (battle && battle->getMode() != bmInit) {
		if (battle->handleEvent(e)) return;
	}
	if (!battle && map->handleEvent(e)) return;

	if (e.type == SDL_MOUSEBUTTONDOWN && GameMode == GAMEMODE_SELECTTEAM) {
		if (e.button.button == SDL_BUTTON_LEFT || e.button.button == SDL_BUTTON_RIGHT) {
			GameMode == GAMEMODE_NONE;
		}
	}
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

	chat->step();

	if (battle) {
		battle->step();
		if (battle->isOver()) {
			delete battle;
			battle = nullptr;
			if (options.GetRepeatBattle()) {
				SDL_Event e;
				btnFight_Click(e);
			}
		}
	}
	else if (map) map->step();

	if (dialogue) {
		if (dialogue->selection >= 0) {
			//send npc action packet
			//pNpcAction* actPack = new pNpcAction(dialogue->selection, 100, 0);
			//gClient.addPacket(actPack);
			//Moved to dialogue

			delete dialogue;
			dialogue = nullptr;
		}
	}

	//User details on mouseover revert after 2 seconds
	if (userDetailsStartTime > 0) {
		int curTime = SDL_GetTicks();
		if (curTime - userDetailsStartTime > 2000) setPlayerDetailsLabels();
	}
}

/* Main Form - Events */

void CMainWindow::btnClose_Click(SDL_Event& e) {
	if (Mode == MFM_MAIN) {
		if (Windows.size() > 0) return;
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

void CMainWindow::btnCloud_Click(SDL_Event& e) {
	std::cout << "Button Cloud Clicked!" << std::endl;

	int action;
	if (!player->IsFlying()) {
		//check if player can fly
		action = amFly;
		player->TakeOff();
	}
	else {
		if (!map->isCoordWalkable(player->GetCoord())) {
			messageManager.DoSystemMessage("You can not land here!");
			return;
		}
		action = amLand;
		player->Land();
	}

	pAction *pck = new pAction(player->AccountId, player->GetID(), player->getDirection(), player->GetCoord().x, player->GetCoord().y, action);
	gClient.addPacket(pck);
}

void CMainWindow::btnFight_Click(SDL_Event& e) {
	if (battle) return;
	if (!map || map->IsChangingMap()) return;

	if (!map->isBattleEnabled()) {
		messageManager.DoSystemMessage("You cannot battle in this location");
		return;
	}

	int teamSize = 1;
	pBattleState* battlePacket = new pBattleState(0, teamSize, player->GetID(), 0);
	gClient.addPacket(battlePacket);
}

void CMainWindow::btnAutoFight_Click(SDL_Event& e) {
	CButton *btn = (CButton*)GetWidget("btnAutoFight");
	std::cout << "Button AutoFight Clicked!" << std::endl;
	if (options.GetRepeatBattle()) {
		options.SetRepeatBattle(false);
		if (btn) btn->SetRotate(false);
		return;
	}

	if (!map) return;
	btnFight_Click(e);
	if (!map->isBattleEnabled()) return;

	options.SetRepeatBattle(true);
	if (btn) {
		btn->SetRotate(true);
		btn->SetRotateSpeed(5);
	}
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

void CMainWindow::btnMap_Click(SDL_Event& e) {
	//testing for pet magic evolve. compose is next!
	/*CPetMagic *pm = new CPetMagic(renderer, 0);
	ColorShift cs[3];
	pm->setCoordinate(player->GetCoord());
	pm->addSource(54, cs);
	pm->addDestination(57, cs);
	map->addPetMagic(pm);
	pm->start();*/
}

void CMainWindow::btnInventory_Click(SDL_Event& e) {
	if (battle) return;
	CInventoryForm* invForm = new CInventoryForm();
	Windows.push_back(invForm);
}

void CMainWindow::btnWuxing_Click(SDL_Event& e) {
	if (battle) return;
	CWuxingForm* wuxForm = new CWuxingForm();
	Windows.push_back(wuxForm);
}

void CMainWindow::btnKungfu_Click(SDL_Event& e) {
	//
}

void CMainWindow::btnTeam_Click(SDL_Event& e) {
	if (ShowingTeamButtons) HideTeamButtons();
	else ShowTeamButtons();
}

void CMainWindow::btnGuild_Click(SDL_Event& e) {
	//
}

void CMainWindow::btnMenuCollapse_Click(SDL_Event& e) {
	if (battle) return;
	bMenuHidden = true;
	btnMenuCollapse->SetVisible(false);
	btnMenuExpand->SetVisible(true);

	((CButton*)widgets["btnJump"])->SetVisible(false);
	((CButton*)widgets["btnFight"])->SetVisible(false);
	((CButton*)widgets["btnMap"])->SetVisible(false);
	((CButton*)widgets["btnInventory"])->SetVisible(false);
	((CButton*)widgets["btnWuxing"])->SetVisible(false);
	((CButton*)widgets["btnKungfu"])->SetVisible(false);
	((CButton*)widgets["btnTeam"])->SetVisible(false);
	//((CButton*)widgets["btnFriend"])->SetVisible(false);
	((CButton*)widgets["btnGuild"])->SetVisible(false);

	if (ShowingTeamButtons) HideTeamButtons();
}

void CMainWindow::btnMenuExpand_Click(SDL_Event& e) {
	bMenuHidden = false;
	btnMenuCollapse->SetVisible(true);
	btnMenuExpand->SetVisible(false);

	((CButton*)widgets["btnJump"])->SetVisible(true);
	((CButton*)widgets["btnFight"])->SetVisible(true);
	((CButton*)widgets["btnMap"])->SetVisible(true);
	((CButton*)widgets["btnInventory"])->SetVisible(true);
	((CButton*)widgets["btnWuxing"])->SetVisible(true);
	((CButton*)widgets["btnKungfu"])->SetVisible(true);
	((CButton*)widgets["btnTeam"])->SetVisible(true);
	//((CButton*)widgets["btnFriend"])->SetVisible(true);
	((CButton*)widgets["btnGuild"])->SetVisible(true);
}

void CMainWindow::btnTeamCreate_Click(SDL_Event& e) {
	CTeam::Create();
}

void CMainWindow::btnTeamJoin_Click(SDL_Event& e) {
	GameMode = GAMEMODE_SELECTTEAM;
}

void CMainWindow::btnTeamManage_Click(SDL_Event& e) {
	//
}

void CMainWindow::btnTeamLeave_Click(SDL_Event& e) {
	CTeam::Leave();
}

void CMainWindow::btnTeamDisband_Click(SDL_Event& e) {
	CTeam::Leave();
}

void CMainWindow::OnBattle_Start(SDL_Event& e) {
	//bPriorMenuState = bMenuHidden;
	if (bMenuHidden) btnMenuExpand_Click(e);

	((CButton*)widgets["btnJump"])->SetVisible(false);
	((CButton*)widgets["btnCloud"])->SetVisible(false);
	((CButton*)widgets["btnFight"])->SetVisible(false);
	((CButton*)widgets["btnMap"])->SetVisible(false);
	((CButton*)widgets["btnInventory"])->SetVisible(false);
	((CButton*)widgets["btnWuxing"])->SetVisible(false);
	((CButton*)widgets["btnKungfu"])->SetVisible(false);
	((CButton*)widgets["btnTeam"])->SetVisible(false);
	((CButton*)widgets["btnGuild"])->SetVisible(false);
}

void CMainWindow::OnBattle_End(SDL_Event& e) {
	//if (bPriorMenuState != bMenuHidden) { //it was hidden
	//	bMenuHidden = false;
	//	btnMenuCollapse_Click(e);
	//}

	((CButton*)widgets["btnJump"])->SetVisible(true);
	((CButton*)widgets["btnCloud"])->SetVisible(true);
	((CButton*)widgets["btnFight"])->SetVisible(true);
	((CButton*)widgets["btnMap"])->SetVisible(true);
	((CButton*)widgets["btnInventory"])->SetVisible(true);
	((CButton*)widgets["btnWuxing"])->SetVisible(true);
	((CButton*)widgets["btnKungfu"])->SetVisible(true);
	((CButton*)widgets["btnTeam"])->SetVisible(true);
	((CButton*)widgets["btnGuild"])->SetVisible(true);
}

/* Main Form - Hooks */

void CMainWindow::ShowTeamButtons() {
	HideTeamButtons();

	std::vector<CButton*> teamBtns;
	if (player->GetTeam()) {
		if (player->GetTeam()->GetLeader() == player) {
			teamBtns.push_back((CButton*)GetWidget("btnTeamManage"));
			teamBtns.push_back((CButton*)GetWidget("btnTeamDisband"));
		}
		else teamBtns.push_back((CButton*)GetWidget("btnTeamLeave"));
	}
	else {
		teamBtns.push_back((CButton*)GetWidget("btnTeamCreate"));
		teamBtns.push_back((CButton*)GetWidget("btnTeamJoin"));
	}

	if (teamBtns.size() > 1) {
		teamBtns[0]->SetX(632);
		teamBtns[0]->SetVisible(true);

		teamBtns[1]->SetX(716);
		teamBtns[1]->SetVisible(true);
	}
	else {
		teamBtns[0]->SetX(674);
		teamBtns[0]->SetVisible(true);
	}

	ShowingTeamButtons = true;
}

void CMainWindow::HideTeamButtons() {
	((CButton*)GetWidget("btnTeamCreate"))->SetVisible(false);
	((CButton*)GetWidget("btnTeamJoin"))->SetVisible(false);
	((CButton*)GetWidget("btnTeamManage"))->SetVisible(false);
	((CButton*)GetWidget("btnTeamLeave"))->SetVisible(false);
	((CButton*)GetWidget("btnTeamDisband"))->SetVisible(false);
	ShowingTeamButtons = false;
}

void CMainWindow::setPlayerDetailsLabels() {
	if (!player) return;
	if (Mode != MFM_MAIN) return;
	lastMouseoverUser = nullptr;
	userDetailsStartTime = 0;

#ifdef NEWGUI
	return;
#endif
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
#ifndef NEWGUI
	lblCoordX->SetText(std::to_string(coord.x));
	lblCoordY->SetText(std::to_string(coord.y));
#endif
}

void CMainWindow::setPlayerHealthGauge(int val) {
	playerInfoFrame->setPlayerHealthGauge(val);
}


void CMainWindow::setPlayerHealthGauge(int val, int max) {
	playerInfoFrame->setPlayerHealthGauge(val, max);
}


void CMainWindow::shiftPlayerHealthGauge(int val) {
	playerInfoFrame->shiftPlayerHealthGauge(val);
}


void CMainWindow::setPlayerManaGauge(int val) {
	playerInfoFrame->setPlayerManaGauge(val);
}


void CMainWindow::setPlayerManaGauge(int val, int max) {
	playerInfoFrame->setPlayerManaGauge(val, max);
}


void CMainWindow::shiftPlayerManaGauge(int val) {
	playerInfoFrame->shiftPlayerManaGauge(val);
}


void CMainWindow::setPlayerExpGauge(int val) {
	playerInfoFrame->setPlayerExpGauge(val);
}


void CMainWindow::setPlayerExpGauge(int val, int max) {
	playerInfoFrame->setPlayerExpGauge(val, max);
}


void CMainWindow::shiftPlayerExpGauge(int val) {
	playerInfoFrame->shiftPlayerExpGauge(val);
}


void CMainWindow::setPetHealthGauge(int val) {
	playerInfoFrame->setPetHealthGauge(val);
}


void CMainWindow::setPetHealthGauge(int val, int max) {
	playerInfoFrame->setPetHealthGauge(val, max);
}


void CMainWindow::shiftPetHealthGauge(int val) {
	playerInfoFrame->shiftPetHealthGauge(val);
}


void CMainWindow::setPetExpGauge(int val) {
	playerInfoFrame->setPetExpGauge(val);
}


void CMainWindow::setPetExpGauge(int val, int max) {
	playerInfoFrame->setPetExpGauge(val, max);
}


void CMainWindow::shiftPetExpGauge(int val) {
	playerInfoFrame->shiftPetExpGauge(val);
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