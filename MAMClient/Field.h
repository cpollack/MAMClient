#pragma once

#include <locale>
#include <codecvt>

#include "Widget.h"

class CField : public CWidget {
public:
	CField(CWindow* window, std::string name, int x, int y);
	CField(CWindow* window, rapidjson::Value& vWidget);
	~CField();

	virtual void Render();
	virtual void HandleEvent(SDL_Event& e);
	virtual void RegisterEvent(std::string eventName, EventFunc evf);

	std::string GetText();
	void SetText(std::string text);

protected:
	bool IsPassword = false;
	bool Numeric = false;
	std::string passwordText;
	std::string lastValue;
	bool ThickBorder = false;
	int MaxLength = 0;

	bool held = false;
	int cursorPos = 0;
	int cursorFrame = 0;

	SDL_Texture *fieldTexture = NULL;
	void RenderFieldTexture();

	std::string Hint = "";
	const SDL_Color hintColor = { 128, 128, 128, 128 };
	SDL_Rect hintRect;
	SDL_Texture *hintTexture = NULL;
	void RenderHintTexture();

public:
	//virtual void SetRenderer(SDL_Renderer* rend);
	void SetX(int x);
	void SetY(int y);
	void SetWidth(int w);
	void SetHeight(int h);
	virtual void SetFocus(bool focus);
	void SetHint(std::string h);

protected:
	void OnFocus();
	void OnFocusLost();

	void OnClick(SDL_Event& e);
	void OnKeyDown(SDL_Event& e);
	void OnTextInput(SDL_Event& e);
	void OnSubmit(SDL_Event& e);
	void OnTab(SDL_Event &e);
	void OnChange(SDL_Event& e);
};

//Old implementation, to be deprecated

//typedef std::basic_string<Uint16, std::char_traits<Uint16>, std::allocator<Uint16> > u16string;

class Field : public Widget {
private:
	SDL_Texture* fieldTexture;
	bool focused = false, newFocus = false;
	bool clicked = false;
	bool isMasked = false;
	bool submitted = false;
	bool disabled = false;

	int cursorPos;
	int cursorFrames;
	int characterLimit;

public:
	Field(std::string sLine, int toX, int toY, int w, int h, bool smallBorder = false);
	~Field();
	void Field::render();
	bool Field::handleEvent(SDL_Event* e);
	void Field::setMaxSize(int size);
	void Field::setMask(bool useMask);
	void Field::formatAndRender();
	std::string Field::getValue();
	void Field::setValue(std::string val);
	void Field::setFocused();
	void Field::clearFocus();
	bool Field::isNewFocus();
	bool Field::isFocused();
	void Field::setDisabled(bool disable);
	bool Field::isSubmitted();
	//void Field::renderText(std::wstring wText);

	std::string fieldText;
	std::wstring renderText;
};
