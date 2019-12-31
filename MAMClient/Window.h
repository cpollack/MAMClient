#pragma once

#include "include/rapidjson/document.h"

enum FormType {
	FT_DEFAULT = 0,
	FT_MAIN,

	//Common Forms
	FT_MESSAGE,
	FT_PROMPT,

	//Custom Forms
	FT_LOGIN,
	FT_LOGOUT,
	FT_CHARCREATE,
	FT_CHARACTER,
	FT_PET,
	FT_INVENTORY,
};

class Texture;
class CWidget;
class CLabel;
class CButton;

class CWindow {
public:
	CWindow();
	CWindow(int w, int h);
	CWindow::CWindow(std::string jsonFile);
	~CWindow();

	void loadDocument(rapidjson::Document& document);
	//CWidget* LoadWidget(rapidjson::Value& vWidget);
	CWidget* LoadWidgetByType(rapidjson::Value& vWidget);

	virtual bool init();
	virtual void initUI();
	virtual void handleEvent(SDL_Event& e);
	virtual void step();

	void focus();
	void raise();
	virtual void render();
	void renderWidgets();
	void renderPresent();
	//void free();

	//bool hasMouseFocus();
	//bool hasKeyboardFocus();
	//bool Minimized();
	//bool Shown();

protected:
	//Window data
	SDL_Window* window;
public:	SDL_Renderer* renderer;
protected:	int windowID;
	int Type;
	//Window dimensions
	int Width;
	int Height;
	std::string Title;

	//Window focus
	bool mouseFocus;
	bool keyboardFocus;
	bool fullScreen;
	bool minimized;
	bool shown;

	//Window properties
	bool Dragable;
	bool ShowClose = true;
	bool ShowMinimize = false;

	SDL_Rect getDstRect(Texture* texture, int x, int y);
	
public: //Accessors
	SDL_Window* GetWindow();
	int GetWindowID();
	int GetType();
	int GetWidth();
	int GetHeight();

	void SetTitle(std::string title);
	void SetUseClose(bool close);
	void SetUseMinimize(bool min);

public:
	bool CloseWindow = false;

protected: // Widgets and Events
	std::map<std::string, CWidget*> widgets;
public: CWidget *focusedWidget = nullptr;
protected: typedef std::function<void(SDL_Event&)> EventFunc;
	void registerEvent(std::string widgetName, std::string eventName, EventFunc evf);

	void btnMinimize_Click(SDL_Event& e);
	void btnClose_Click(SDL_Event& e);

public:
	void AddWidget(CWidget* widget);
	void RemoveWidget(std::string widgetName);
	void ClearWidgets();
	CWidget* GetWidget(std::string widgetName);
	void SetFocus(std::string widgetName);

protected: //GUI textures
	Texture* topCenter_s;
	Texture* bottomCenter;
	Texture* left;
	Texture* right;
	Texture* topLeft_s;
	Texture* topRight_s;
	Texture* bottomLeft;
	Texture* bottomRight;
	//Texture* close;
	//Texture* minimize;

	CButton *btnClose;
	CButton *btnMinimize;
};

extern std::vector<CWindow*> Windows;