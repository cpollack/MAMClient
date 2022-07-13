#include "stdafx.h"
#include "Field.h"
#include "Window.h"
#include "CustomEvents.h"

CField::CField(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;

	fontColor = gui->fontColor;
}

CField::CField(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	fontColor = gui->fontColor;
	if (vWidget.HasMember("Text")) SetText(vWidget["Text"].GetString());
	if (vWidget.HasMember("MaxLength")) MaxLength = vWidget["MaxLength"].GetInt();
	if (vWidget.HasMember("ThickBorder")) ThickBorder = vWidget["ThickBorder"].GetBool();
	if (vWidget.HasMember("IsPassword")) IsPassword = vWidget["IsPassword"].GetBool();
	if (vWidget.HasMember("Numeric")) Numeric = vWidget["Numeric"].GetBool();
}

CField::~CField() {
	if (fieldTexture) SDL_DestroyTexture(fieldTexture);
	if (hintTexture) SDL_DestroyTexture(hintTexture);
}

void CField::ReloadAssets() {
	if (fieldTexture) {
		RenderFieldTexture();
		RenderText();
	}
}

void CField::Render() {
	if (!Visible) return;
	if (!fieldTexture) RenderFieldTexture();
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);

	//Render the background UI for the widget
	SDL_Rect fieldRect = { X, Y, Width, Height };
	SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);

	SDL_Rect textRect{ X + 5, Y + ((Height / 2) - (fontRect.h / 2)), fontRect.w, fontRect.h };

	//Render Highlight Box
	if (highlightRange.first != highlightRange.second) {
		SDL_Point hpointA, hpointB;
		TTF_SizeText(font, Text.substr(0, highlightRange.first).c_str(), &hpointA.x, &hpointA.y);
		TTF_SizeText(font, Text.substr(highlightRange.first, highlightRange.second - highlightRange.first).c_str(), &hpointB.x, &hpointB.y);
		boxRGBA(renderer, textRect.x + hpointA.x, textRect.y, textRect.x + hpointA.x + hpointB.x, textRect.y + textRect.h, 38, 79, 120, 255);
	}

	//Render the actual text	
	if (Text.length() == 0) {
		if (hintTexture) {
			SDL_Rect hintRect2 = { X + 5, Y + ((Height / 2) - (hintRect.h / 2)), hintRect.w, hintRect.h };
			SDL_RenderCopy(renderer, hintTexture, NULL, &hintRect2);
		}
	}
	else {
		SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);
	}

	//Render cursor
	if (Focused) {
		cursorFrame++;
		int nextFrame = cursorFrame % 60;
		if (nextFrame < 30) {
			int cursorX = 0;
			
			if (cursorPos == 0) {
				cursorX = textRect.x;
			}
			else if (cursorPos == Text.size()) {
				cursorX = textRect.x + textRect.w;
			}
			else {
				std::string subStr = Text.substr(0, cursorPos);
				TTF_SizeText(font, subStr.c_str(), &cursorX, nullptr);
				cursorX += textRect.x;
			}

			int fontHeight = TTF_FontHeight(font);
			int cursorY1 = Y + (Height / 2) - (fontHeight / 2);
			int cursorY2 = cursorY1 + fontHeight;
			vlineRGBA(renderer, cursorX, cursorY1, cursorY2, 0xE3, 0xE3, 0xE3, 0xFF);
		}
	}

	SDL_SetRenderTarget(renderer, priorTarget);
}

void CField::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN && !ReadOnly) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (DoesPointIntersect(SDL_Point{ mx, my })) {
			held = true;
		}
		else {
			if (Focused) {
				if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
					SDL_Event event;
					SDL_zero(event);
					event.type = CUSTOMEVENT_WIDGET;
					event.window.windowID = e.window.windowID;
					event.user.code = WIDGET_FOCUS_LOST;
					event.user.data1 = this;
					event.user.data2 = Window;
					SDL_PushEvent(&event);
				}
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (DoesPointIntersect(SDL_Point{ mx, my })) {
			if (held) {
				OnClick(e);
				return;
			}
		}
		held = false;
	}

	if (Focused && e.type == SDL_KEYDOWN)
	{
		OnKeyDown(e);
	}

	if (Focused && e.type == SDL_TEXTINPUT)
	{
		//Not copy or pasting
		/*if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
		{
		//Append character
		inputText += e.text.text;
		renderText = true;
		}*/

		OnTextInput(e);
		return;
	}
}

void CField::RenderFieldTexture() {
	if (renderer == NULL) return;

	//Draw Field texture with borders
	if (fieldTexture) SDL_DestroyTexture(fieldTexture);
	fieldTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, fieldTexture);

	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 255);
	SDL_RenderClear(renderer);

	hlineRGBA(renderer, 0, Width, 0, 0xA0, 0xA0, 0xA0, 0xFF);
	vlineRGBA(renderer, 0, 0, Height, 0xA0, 0xA0, 0xA0, 0xFF);

	if (ThickBorder) {
		hlineRGBA(renderer, 1, Width - 1, 1, 0x69, 0x69, 0x69, 0xFF);
		vlineRGBA(renderer, 1, 1, Height - 1, 0x69, 0x69, 0x69, 0xFF);

		hlineRGBA(renderer, 1, Width - 1, Height - 2, 0xE3, 0xE3, 0xE3, 0xFF);
		vlineRGBA(renderer, Width - 2, 1, Height - 2, 0xE3, 0xE3, 0xE3, 0xFF);
	}

	hlineRGBA(renderer, 0, Width, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	vlineRGBA(renderer, Width - 1, 0, Height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderTarget(renderer, NULL);
}


void CField::RenderHintTexture() {
	if (hintTexture) SDL_DestroyTexture(hintTexture);
	SDL_Surface* lSurface;

	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	SDL_Color c = { 128, 128, 128, 192 };
	lSurface = TTF_RenderText_Blended(font, Hint.c_str(), c);

	if (lSurface != NULL) {
		hintTexture = SDL_CreateTextureFromSurface(renderer, lSurface);
		//SDL_SetTextureBlendMode(hintTexture, SDL_BlendMode::SDL_BLENDMODE_BLEND);
		hintRect = { 0, 0, lSurface->w, lSurface->h };

		SDL_FreeSurface(lSurface);
	}
}


/*void CField::SetRenderer(SDL_Renderer* rend) {
	CWidget::SetRenderer(rend);
	RenderFieldTexture();
}*/

void CField::RegisterEvent(std::string eventName, EventFunc evf) {
	eventMap[eventName] = evf;
}

void CField::SetX(int x) {
	X = x;
	RenderFieldTexture();
}

void CField::SetY(int y) {
	Y = y;
	RenderFieldTexture();
}

void CField::SetWidth(int w) {
	Width = w;
	RenderFieldTexture();
}

void CField::SetHeight(int h) {
	Height = h;
	RenderFieldTexture();
}


void CField::SetFocus(bool focus) {
	/*CWidget::SetFocus(focus);
	if (Focused) OnFocus();
	else OnFocusLost();*/
}

void CField::SetHint(std::string h) {
	Hint = h;
	if (Hint.length() == 0) return;
	RenderHintTexture();
}

void CField::LoseFocus() {
	if (!Focused) return;
	Focused = false;

	if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WIDGET;
		event.window.windowID = Window->GetWindowID();
		event.user.code = WIDGET_FOCUS_LOST;
		event.user.data1 = this;
		event.user.data2 = Window;
		SDL_PushEvent(&event);
	}
}

std::string CField::GetText() {
	if (IsPassword) return passwordText;
	if (Numeric && Text.length() == 0) return "0";
	return Text;
}

void CField::SetText(std::string text) {
	if (IsPassword) {
		passwordText = text;
		Text = "";
		for (int i = 0; i < passwordText.length(); i++) Text.push_back('*');
	}
	else Text = text;
	if (Numeric) {
		for (int i = Text.length(); i > 0; i--) {
			char c = Text[i - 1];
			if (!(c >= '0' && c <= '9')) Text.erase(Text.begin() + i - 1);
		}
		if (Text.length() == 0) Text = "0";
	}
	RenderText();
}

void CField::OnFocus() {
	std::cout << "Field " << Name << " start text input." << std::endl;
	Focused = true;
	Window->focus();
	SDL_StartTextInput();
	cursorPos = Text.size();
	cursorFrame = 0;
	highlightRange = { cursorPos , cursorPos };
}


void CField::OnFocusLost() {
	Focused = false;
	highlightRange = { cursorPos , cursorPos };
	
	SDL_StopTextInput();

	SDL_Event e;
	OnChange(e);	
}


void CField::OnClick(SDL_Event& e) {
	if (!Focused) {
		if (CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
			SDL_Event event;
			SDL_zero(event);
			event.type = CUSTOMEVENT_WIDGET;
			event.window.windowID = e.window.windowID;
			event.user.code = WIDGET_FOCUS_GAINED;
			event.user.data1 = this;
			event.user.data2 = Window;
			SDL_PushEvent(&event);
		}
	}

	//click to set cursor position?
}


void CField::OnKeyDown(SDL_Event& e) {
	//Backspace
	if (e.key.keysym.sym == SDLK_BACKSPACE && Text.length() > 0)
	{
		if (highlightRange.first == highlightRange.second) {
			if (cursorPos == Text.length()) {
				Text.pop_back();
				if (IsPassword) passwordText.pop_back();
			}
			else {
				if (cursorPos > 0) {
					Text.erase(cursorPos - 1, 1);
					if (IsPassword) passwordText.erase(cursorPos - 1, 1);
				}
			}

			cursorPos--;
		}
		else {
			Text.erase(highlightRange.first, highlightRange.second - highlightRange.first);
			if (IsPassword) passwordText.erase(highlightRange.first, highlightRange.second - highlightRange.first + 1);
			cursorPos = highlightRange.first;			
		}

		if (cursorPos < 0) cursorPos = 0;
		highlightRange = { cursorPos, cursorPos };

		RenderText();		
		return;
	}

	//Delete
	if (e.key.keysym.sym == SDLK_DELETE && Text.length() > 0)
	{		
		Text.erase(highlightRange.first, max(highlightRange.second - highlightRange.first,1));
		if (IsPassword) passwordText.erase(highlightRange.first, max(highlightRange.second - highlightRange.first,1));
		cursorPos = highlightRange.first;
		highlightRange = { cursorPos, cursorPos };

		RenderText();
		return;
	}

	//'Enter' or Submit
	if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
		OnSubmit(e);
		return;
	}

	//'Tab'
	if (e.key.keysym.sym == SDLK_TAB) {
		OnTab(e);
		return;
	}

	if (e.key.keysym.sym == SDLK_LEFT) {
		int oldPos = cursorPos--;
		if (cursorPos < 0) cursorPos = 0;

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) {
			if (highlightRange.first == oldPos) highlightRange.first = cursorPos;
			else highlightRange.second = cursorPos;
		}
		else {
			highlightRange = { cursorPos, cursorPos };
		}
		std::cout << "Highlight Range: " << highlightRange.first << "," << highlightRange.second << std::endl;
		return;
	}

	if (e.key.keysym.sym == SDLK_RIGHT) {
		int oldPos = cursorPos++;
		if (cursorPos > Text.size()) cursorPos = Text.size();

		const Uint8* keystate = SDL_GetKeyboardState(NULL);
		if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) {
			if (highlightRange.second == oldPos) highlightRange.second = cursorPos;
			else highlightRange.first = cursorPos;
		}
		else {
			highlightRange = { cursorPos, cursorPos };
		}
		std::cout << "Highlight Range: " << highlightRange.first << "," << highlightRange.second << std::endl;
		return;
	}
}

void CField::OnTextInput(SDL_Event& e) {
	std::string newText = e.text.text;
	if (IsPassword) {
		passwordText += newText;
		Text = "";
		for (int i = 0; i < passwordText.length(); i++) Text.push_back('*');
	}
	else Text += newText;

	if (MaxLength > 0 && Text.length() > MaxLength) {
		Text = Text.substr(0, MaxLength);
	}
	if (Numeric) {
		for (int i = Text.length(); i > 0; i--) {
			char c = Text[i - 1];
			if (!(c >= '0' && c <= '9')) Text.erase(Text.begin() + i - 1);
		}
	}

	RenderText();
	if (cursorPos == Text.length() - 1)
		cursorPos += newText.length();
}


void CField::OnSubmit(SDL_Event& e) {
	auto iter = eventMap.find("Submit");
	if (iter != eventMap.end()) iter->second(e);
	OnChange(e);
}

void CField::OnTab(SDL_Event &e) {
	auto iter = eventMap.find("OnTab");
	if (iter != eventMap.end()) iter->second(e);
}

void CField::OnChange(SDL_Event& e) {
	if (Numeric && Text.length() == 0) {
		Text = "0";
		RenderText();
	}
	if (Text.compare(lastValue) == 0) return;
	lastValue = Text;

	auto iter = eventMap.find("OnChange");
	if (iter != eventMap.end()) iter->second(e);
}
