#pragma once

#include "../include/rapidjson/document.h"

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
	FT_PETEVOLVE,
	FT_PETCOMPOSE,
	FT_PETACCESSORY,
	FT_INVENTORY,
	FT_SHOP,
	FT_WUXING,
	FT_REFINE,
	FT_BATTLECONFIG,
};

enum RendererD3D9Mode {
	D3D9_NONE,
	D3D9_PRIMITIVE,
	D3D9_TEXTURE,
};

class Texture;
class CWidget;
class CLabel;
class CButton;

class CPromptForm;

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
	virtual void ReloadAssets();
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

protected: //Window Properties
	SDL_Window* window;
	CWindow* parent = nullptr;
	void SetParentFromStack();
	CPromptForm* promptForm = nullptr;

public:	SDL_Renderer* renderer;
protected:	
	HWND hwnd;
	int windowID;
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
	bool ShowClose = true;
	bool ShowMinimize = false;

	bool Draggable = true;
	bool dragging;
	int drag_start_x, drag_start_y;

	SDL_Rect getDstRect(Texture* texture, int x, int y);
	
public: //Accessors
	SDL_Window* GetWindow();
	int GetWindowID();
	void PushWindow(CWindow *pWindow);

	int GetType();
	int GetWidth();
	int GetHeight();

	void SetParent(CWindow* wParent);
	void SetTitle(std::string title);
	void SetUseClose(bool close);
	void SetUseMinimize(bool min);

public:
	bool CloseWindow = false;

protected: // Widgets and Events
	std::map<std::string, CWidget*> widgets;
	std::map<std::string, CWidget*>::iterator widgetsItr;
	CWidget* eventWidget = nullptr; //The current widget being processed for events
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
	void FocusWidget(std::string widgetName);
	void FocusWidget(CWidget* widget);

	bool WidgetHasMouse();

private:
	std::list<CWidget*> widgetsByDepth;

protected: //GUI textures
	Texture* topCenter_s;
	Texture* bottomCenter;
	Texture* left;
	Texture* right;
	Texture* topLeft_s;
	Texture* topRight_s;
	Texture* bottomLeft;
	Texture* bottomRight;

	CButton *btnClose;
	CButton *btnMinimize;
};

extern CWindow* mainWindow;
extern std::vector<CWindow*> Windows;
extern std::vector<CWindow*> QueueWindows;