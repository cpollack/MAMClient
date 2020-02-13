#pragma once

#include <locale>
#include <codecvt>

#include "Widget.h"

class CField : public CWidget {
public:
	CField(CWindow* window, std::string name, int x, int y);
	CField(CWindow* window, rapidjson::Value& vWidget);
	~CField();

	void ReloadAssets();
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
	void LoseFocus();

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
