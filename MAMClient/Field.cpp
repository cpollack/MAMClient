#include "stdafx.h"
#include "Field.h"
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
	//
}

void CField::Render() {
	if (!Visible) return;
	if (!fieldTexture) RenderFieldTexture();
	SDL_Texture *priorTarget = SDL_GetRenderTarget(renderer);

	//Render the background UI for the widget
	SDL_Rect fieldRect = { X, Y, Width, Height };
	SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);

	//Render the actual text
	SDL_Rect textRect{ X + 5, Y + ((Height / 2) - (fontRect.h / 2)), fontRect.w, fontRect.h };
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
			if (cursorPos == Text.size()) {
				cursorX = textRect.x + textRect.w;
			}

			if (cursorPos == 0) {
				cursorX = textRect.x;
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

std::string CField::GetText() {
	if (IsPassword) return passwordText;
	return Text;
}

void CField::SetText(std::string text) {
	if (IsPassword) {
		passwordText = text;
		Text = "";
		for (int i = 0; i < passwordText.length(); i++) Text.push_back('*');
	}
	else Text = text;
	RenderText();
}

void CField::OnFocus() {
	Focused = true;
	SDL_StartTextInput();
	cursorPos = Text.size();
	cursorFrame = 0;
}


void CField::OnFocusLost() {
	Focused = false;
	SDL_StopTextInput();
	SDL_Event e;
	OnChange(e);
}


void CField::OnClick(SDL_Event& e) {
	//if (!Focused) SetFocus(true);
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
		Text.pop_back();
		if (IsPassword) passwordText.pop_back();
		RenderText();
		cursorPos--;
		if (cursorPos < 0) cursorPos = 0;
		return;
	}

	//'Enter' or Submit
	if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
		OnSubmit(e);
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
	cursorPos += newText.length();
}


void CField::OnSubmit(SDL_Event& e) {
	auto iter = eventMap.find("Submit");
	if (iter != eventMap.end()) iter->second(e);
	OnChange(e);
}

void CField::OnChange(SDL_Event& e) {
	if (Text.compare(lastValue) == 0) return;
	lastValue = Text;

	auto iter = eventMap.find("OnChange");
	if (iter != eventMap.end()) iter->second(e);
}


//Old implementation, to be deprecated

Field::Field(std::string text, int toX, int toY, int w, int h, bool smallBorder) : Widget(toX, toY) {
	widgetType = wtField;
	loadFont(false);

	fieldText = text;
	//Widget::renderText(fieldText);
	Widget::setText(fieldText);

	width = w;
	height = h;

	//Draw Field texture with borders
	fieldTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(renderer, fieldTexture);

	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 255);
	SDL_RenderClear(renderer);

	hlineRGBA(renderer, 0, width, 0, 0xA0, 0xA0, 0xA0, 0xFF);
	vlineRGBA(renderer, 0, 0, height, 0xA0, 0xA0, 0xA0, 0xFF);

	if (!smallBorder) {
		hlineRGBA(renderer, 1, width - 1, 1, 0x69, 0x69, 0x69, 0xFF);
		vlineRGBA(renderer, 1, 1, height - 1, 0x69, 0x69, 0x69, 0xFF);

		hlineRGBA(renderer, 1, width - 1, height - 2, 0xE3, 0xE3, 0xE3, 0xFF);
		vlineRGBA(renderer, width - 2, 1, height - 2, 0xE3, 0xE3, 0xE3, 0xFF);
	}

	hlineRGBA(renderer, 0, width, height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	vlineRGBA(renderer, width - 1, 0, height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderTarget(renderer, NULL);
}


Field::~Field() {
}


void Field::render() {
	if (!visible) return;

	//Render the background UI for the widget
	SDL_Rect fieldRect = { x, y, width, height };
	SDL_RenderCopy(renderer, fieldTexture, NULL, &fieldRect);

	//Render the actual text
	SDL_Rect textRect{ x + 5, y + ((height / 2) - (fontRect.h / 2)), fontRect.w, fontRect.h };
	SDL_RenderCopy(renderer, fontTexture, NULL, &textRect);

	//Render cursor
	if (focused) {
		cursorFrames++;
		int nextFrame = cursorFrames % 60;
		if (nextFrame < 30) {
			int cursorX = 0;
			if (cursorPos == fieldText.size()) {
				cursorX = textRect.x + textRect.w;
			}

			if (cursorPos == 0) {
				cursorX = textRect.x;
			}

			int cursorY1 = textRect.y;
			int cursorY2 = textRect.y + textRect.h;
			vlineRGBA(renderer, cursorX, cursorY1, cursorY2, 0xE3, 0xE3, 0xE3, 0xFF);
		}
	}
}


bool Field::handleEvent(SDL_Event* e) {
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (mx >= x && mx <= (x + width) && my >= y && my <= (y + height)) {
			clicked = true;
			//return true;
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (focused && !clicked) {
			SDL_StopTextInput();
			focused = false;
			return false;
		}

		if (clicked && mx >= x && mx <= (x + width) && my >= y && my <= (y + height)) {
			clicked = false;
			if (!disabled) {
				focused = true;
				newFocus = true;
				cursorPos = fieldText.size();
				cursorFrames = 0;
				SDL_StartTextInput();
			}
			return true;
		}

		clicked = false;
	}

	if (focused && e->type == SDL_KEYDOWN)
	{
		//Handle backspace
		if (e->key.keysym.sym == SDLK_BACKSPACE && fieldText.length() > 0)
		{
			fieldText.pop_back();
			formatAndRender();
			return true;
		}

		//'Enter' to act like a submit
		if (e->key.keysym.sym == SDLK_RETURN || e->key.keysym.sym == SDLK_KP_ENTER) {
			focused = false;
			submitted = true;
			return true;
		}
	}

	if (focused && e->type == SDL_TEXTINPUT)
	{
		//Not copy or pasting
		/*if (!((e.text.text[0] == 'c' || e.text.text[0] == 'C') && (e.text.text[0] == 'v' || e.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL))
		{
			//Append character
			inputText += e.text.text;
			renderText = true;
		}*/

		fieldText += e->text.text;
		formatAndRender();
		return true;
	}

	return false;
}


void Field::setMaxSize(int size) {
	characterLimit = size;
	formatAndRender();
}


void Field::setMask(bool useMask) {
	isMasked = useMask;
	formatAndRender();
}


void Field::formatAndRender() {
	if (fieldText.size() > characterLimit) {
		fieldText = fieldText.substr(0, characterLimit);
	}
	cursorPos = fieldText.size();

	std::string tempOutput;
	if (isMasked) {
		int strLength = fieldText.size();
		//renderText.clear();
		//for (int i =0; i<strLength; i++)
		//	renderText.push_back(L'\x2022');

		tempOutput.clear();
		for (int i = 0; i<strLength; i++)
			tempOutput.push_back(42);
	}
	else {
		//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		//renderText = converter.from_bytes(fieldText);
		tempOutput = fieldText;
	}
	
	/*std::wstring tempOutput;
	if (renderText.size() == 0) tempOutput = L" ";
	else tempOutput = renderText;
	u16string utext(tempOutput.begin(), tempOutput.end());

	SDL_Surface* lSurface = TTF_RenderUNICODE_Solid(font, utext.c_str(), gui->fontColor);*/

	
	if (!tempOutput.length()) tempOutput = " ";
	//SDL_Surface* lSurface = TTF_RenderUNICODE_Solid(font, utext.c_str(), gui->fontColor);
	SDL_Surface* lSurface = TTF_RenderText_Blended(font, tempOutput.c_str(), gui->fontColor);
	if (lSurface != NULL) {
		fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);
		fontRect = { 0, 0, lSurface->w, lSurface->h };

		SDL_FreeSurface(lSurface);
	}
}


/*void Field::renderText(std::wstring wText) {
	SDL_Surface* lSurface = TTF_RenderText_Solid(wFont, wText.c_str(), gui->fontColor);
	if (lSurface != NULL) {
		fontTexture = SDL_CreateTextureFromSurface(renderer, lSurface);
		fontRect = { 0, 0, lSurface->w, lSurface->h };

		SDL_FreeSurface(lSurface);
	}
}*/


std::string Field::getValue() {
	return fieldText;
}


void Field::setValue(std::string val) {
	cursorPos = val.size();
	fieldText = val;
	if (val == "") Widget::renderText(" ");
	else Widget::renderText(fieldText);
}


bool Field::isSubmitted() {
	if (submitted) {
		submitted = false;
		return true;
	}
	return false;
}


void Field::setFocused() {
	focused = true;
}


void Field::clearFocus() {
	focused = false;
}


bool Field::isNewFocus() {
	bool nf = newFocus;
	newFocus = false;
	return nf;
}


bool Field::isFocused() {
	return focused;
}


void Field::setDisabled(bool disable) {
	disabled = disable;
}