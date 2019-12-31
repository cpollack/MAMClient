#pragma once

#include "Client.h"

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
	~CWidget();
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
	bool Visible = true;	
	bool ReadOnly;
	bool mouseOver;
	bool held;

	//Text related properties
	bool Underlined, Bold;
	int TextWrapWidth;
	std::string Text;

public: //Accessors
	void SetX(int x);
	void SetY(int y);
	void SetPosition(SDL_Point p);
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


public:
	virtual void Render();
	virtual void HandleEvent(SDL_Event& e);	

	virtual bool DoesPointIntersect(SDL_Point point);

	virtual void SetFocus(bool focus);
	bool IsFocus();

//Related to Text Handling
public: 
	virtual void SetText(std::string value);
	void RenderText();
protected:
	TTF_Font* font;
	SDL_Texture* fontTexture = NULL;
	SDL_Rect fontRect;
	SDL_Color fontColor, backColor;

protected:
	typedef std::function<void(SDL_Event&)> EventFunc;
	std::map<std::string, EventFunc> eventMap;
public:
	virtual void RegisterEvent(std::string eventName, EventFunc evf);

	virtual void OnFocus();
	virtual void OnFocusLost();
};


class Widget {
public:
	int x, y;
	int width, height;
	int textWidth = 0;
	int priority = 0;
	int depth = 0;
	int widgetType = wtWidget;

	Widget(int toX, int toY);
	~Widget();
	virtual void Widget::render();
	virtual bool Widget::handleEvent(SDL_Event* e);

	void Widget::loadFont();
	void Widget::loadFont(bool isBold );
	virtual void Widget::renderText(std::string sText);
	virtual void Widget::setText(std::string sText);
	void Widget::setTextWidth(int tWidth);

	void Widget::setVisibility(bool setVisible);
	void Widget::setFont(TTF_Font* newFont);
	void Widget::setFontColor(SDL_Color aColor);
	virtual void Widget::setPosition(SDL_Point pos);
	virtual void Widget::setPosition(int px, int py);
	virtual void Widget::offsetPosition(SDL_Point offset);
	virtual void Widget::offsetPosition(int offsetX, int offsetY);
	void Widget::setPriority(int pri);
	void Widget::setDepth(int dep);
	void Widget::setUnderlinded(bool und);

	TTF_Font* font;
	SDL_Texture* fontTexture = NULL;
	SDL_Rect fontRect;
	SDL_Color fontColor;

private:	

protected:
	SDL_Renderer* renderer;
	SDL_Texture* wTexture;

	std::string text;

	bool visible;
	bool underlined = false;
	bool bold = false;
};

bool operator<(const Widget &s1, const Widget &s2);