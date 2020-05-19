#pragma once

#include "Client.h"
#include "Define.h"

#include "include/rapidjson/document.h"
#include <functional>

class CWindow;

enum WidgetType {
	wtWidget = 0,
	wtLabel,
	wtCheckBox,
	wtRadioButton,
	wtButton,
	wtPanel,
	wtField,
	wtDropDown,
	wtImageBox,
	wtTabControl,
	wtGauge,
	wtListBox,
	wtGridBox,
	wtSpriteBox
};

class CWidget {
public:
	CWidget(CWindow* window);
	CWidget(CWindow* window, std::string name, int x, int y);
	CWidget(CWindow* window, rapidjson::Value& vWidget);
	virtual ~CWidget();
	//virtual void SetRenderer(SDL_Renderer* rend);

protected:
	SDL_Renderer* renderer;
	bool Loaded = false;
	bool Focused = false;	

public: //Properties
	int WidgetType;
	std::string Name;
protected: 
	int Width, Height;
	int X, Y;
	SDL_Rect widgetRect;
	CWindow* Window = nullptr;
	CWidget* Parent = nullptr;
	std::vector<CWidget*> Children;
	int TabItem;
	int Depth;
	bool Visible = true;	
	bool ReadOnly;
	bool MouseOver;
	bool MouseDown = false;
	bool Hovering = false;
	bool held;
	
	Uint32 MouseOverStart = 0;
	Uint32 HoverStart = 0;
	SDL_Point HoverPoint = { -1,-1 };
	int HoverDelay = 1000;

	const int DragDelta = 5;
	bool Dragging;
	SDL_Rect DragStart;

	//Text related properties
	bool Underlined, Bold, Multiline;
	int TextWrapWidth;
	std::string Text;
	std::wstring wText;

public: //Accessors
	int GetX() { return X; }
	int GetY() { return Y; }
	void SetX(int x);
	void SetY(int y);

	void SetPosition(int toX, int toY);
	void SetPosition(SDL_Point p);

	int GetWidth() { return Width; }
	int GetHeight() { return Height; }
	void SetWidth(int w);
	void SetHeight(int h);

	void SetWindow(CWindow* win);
	void SetParent(CWidget* widget);
	void AddChild(CWidget* widget);
	void SetVisible(bool visible);

	void SetFontColor(SDL_Color fc);
	void SetBackColor(SDL_Color bc);

	CWidget* GetParent();

	int GetTabItem();
	int GetDepth() { return Depth; }

	bool IsMouseOver() { return MouseOver; }
	void SetHoverDelay(int ms) { HoverDelay = ms; }

public:
	virtual void Render();
	virtual void HandleEvent(SDL_Event& e);	
	virtual void Step();
	virtual void Load() {}
	virtual void ReloadAssets() {}

	virtual bool DoesPointIntersect(SDL_Point point);

	virtual void SetFocus(bool focus);
	bool IsFocus();

//Related to Text Handling
public: 
	virtual void SetText(std::string value);
	virtual void SetText(std::wstring value);
	virtual void RenderText();
protected:
	TTF_Font* font, *fontUni;	
	SDL_Texture* fontTexture = NULL;
	SDL_Rect fontRect;
	SDL_Color fontColor, backColor;

protected:
	std::map<std::string, CWidget*> widgets;
	typedef std::function<void(SDL_Event&)> EventFunc;
	std::map<std::string, EventFunc> eventMap;
public:
	virtual void RegisterEvent(std::string eventName, EventFunc evf);
	virtual void RegisterEvent(std::string widgetName, std::string eventName, EventFunc evf);

	virtual void OnFocus();
	virtual void OnFocusLost();

	virtual void OnHoverStart(SDL_Event &e);
	virtual void OnHoverEnd(SDL_Event &e);
};
