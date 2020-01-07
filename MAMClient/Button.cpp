#include "stdafx.h"
#include "Button.h"
#include "Define.h"

CButton::CButton(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	SetPosition(SDL_Point{ x,y });

	fontColor = gui->buttonFontColor;
}

CButton::CButton(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	fontColor = gui->buttonFontColor;
	if (vWidget.HasMember("Text")) SetText(vWidget["Text"].GetString());
}

CButton::~CButton() {
	if (buttonTexture) SDL_DestroyTexture(buttonTexture);
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (pressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (mouseoverTexture) SDL_DestroyTexture(unpressedTexture);
}

void CButton::ReloadAssets() {
	if (buttonTexture) {
		CreateButtonTexture();
	}
}

void CButton::Render() {
	if (!Visible) return;
	if (!buttonTexture) CreateButtonTexture();
	SDL_RenderCopy(renderer, buttonTexture, NULL, &widgetRect);
}

void CButton::HandleEvent(SDL_Event& e) {
	int mx, my;
	SDL_GetMouseState(&mx, &my);
	HandleEvent(e, mx, my);
}

void CButton::HandleEvent(SDL_Event& e, int mx, int my) {
	if (e.type == SDL_MOUSEMOTION) {
		if (doesPointIntersect(widgetRect, mx, my)) {
			mouseOver = true;
			if (!held && mouseoverTexture) buttonTexture = mouseoverTexture;
		}
		else {
			mouseOver = false;
			if (Type == btToggle) {
				if (toggled) buttonTexture = pressedTexture;
				else buttonTexture = unpressedTexture;
			}
			else {
				if (held) buttonTexture = pressedTexture;
				else buttonTexture = unpressedTexture;
			}
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		if (doesPointIntersect(widgetRect, mx, my)) {
			held = true;

			if (Type != btToggle) buttonTexture = pressedTexture;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		if (doesPointIntersect(widgetRect, mx, my)) {
			if (held) {
				pressed = !pressed;
				if (Type == btToggle) toggled = !toggled;
				OnClick(e);
			}

		}
		held = false;

		if (Type == btToggle) {
			if (toggled) buttonTexture = pressedTexture;
			else buttonTexture = unpressedTexture;
		}
		else buttonTexture = unpressedTexture;
	}
}
	
void CButton::OnClick(SDL_Event& e) {
	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);
}

void CButton::CreateButtonTexture() {
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (pressedTexture) SDL_DestroyTexture(pressedTexture);
	if (mouseoverTexture) SDL_DestroyTexture(mouseoverTexture);

	SDL_Color buttonColor = gui->buttonColor;

	SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
	SDL_RenderClear(renderer);

	int ampPos = Text.find('&');
	if (ampPos != std::string::npos) {
		Text.erase(Text.begin() + ampPos);
	}
	RenderText();
	if (fontTexture) {
		SDL_SetRenderTarget(renderer, mainTexture);
		fontRect.x = (Width / 2) - (fontRect.w / 2);
		fontRect.y = (Height / 2) - (fontRect.h / 2);
		SDL_RenderCopy(renderer, fontTexture, NULL, &fontRect);
		
		//draw the _
		if (ampPos != std::string::npos) {
			int advance;
			int underlineOffset = 0;
			for (int i = 0; i < ampPos; i++) {
				TTF_GlyphMetrics(font, Text[i], NULL, NULL, NULL, NULL, &advance);
				underlineOffset += advance;
			}
			TTF_GlyphMetrics(font, Text[ampPos], NULL, NULL, NULL, NULL, &advance);
			int x1 = fontRect.x + underlineOffset;
			int x2 = x1 + advance - 1;
			int y1 = fontRect.y + TTF_FontHeight(font) - 2;
			hlineRGBA(renderer, x1, x2, y1, fontColor.r, fontColor.g, fontColor.b, fontColor.a);
		}
		SDL_SetRenderTarget(renderer, NULL);
	}

	SDL_SetRenderTarget(renderer, NULL);

	SDL_Color bbColor = gui->buttonBorderColor;
	SDL_Rect renderRect = { 0, 0, Width, Height };

	//Unpressed Texture
	if (usingImages) {
		if (UseGUI) {
			unpressedImage = gui->getSkinTexture(renderer, UnPressedImagePath, Anchor::aTopLeft);
			unpressedTexture = unpressedImage->texture;
		}
		else {
			unpressedImage = new Texture(renderer, UnPressedImagePath);
			unpressedTexture = unpressedImage->texture;
		}
	}
	else {
		unpressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetRenderTarget(renderer, unpressedTexture);
		SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
		hlineRGBA(renderer, 0, Width - 1, 0, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		hlineRGBA(renderer, 0, Width - 1, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		vlineRGBA(renderer, 0, 0, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		SDL_SetRenderTarget(renderer, NULL);
	}

	//Pressed Texture
	if (usingImages) {
		if (UseGUI) {
			pressedImage = gui->getSkinTexture(renderer, PressedImagePath, Anchor::aTopLeft);
			pressedTexture = pressedImage->texture;
		}
		else {
			pressedImage = new Texture(renderer, PressedImagePath);
			pressedTexture = pressedImage->texture;
		}
	}
	else {
		pressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetRenderTarget(renderer, pressedTexture);
		SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
		hlineRGBA(renderer, 0, Width - 1, 0, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		vlineRGBA(renderer, 0, 0, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		SDL_SetRenderTarget(renderer, NULL);
	}

	//Mouseover Texture
	if (!usingImages) {
		mouseoverTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
		SDL_SetRenderTarget(renderer, mouseoverTexture);
		SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
		hlineRGBA(renderer, 0, Width - 1, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
		SDL_SetRenderTarget(renderer, NULL);
	}

	SDL_DestroyTexture(mainTexture);
	if (pressed) buttonTexture = pressedTexture;
	else buttonTexture = unpressedTexture;
}

void CButton::SetType(ButtonType type) {
	Type = type;
}

void CButton::SetText(std::string value) {
	CWidget::SetText(value);
	CreateButtonTexture();
}

void CButton::SetPressedImage(std::string imagePath) {
	PressedImagePath = imagePath;
	usingImages = true;
}

void CButton::SetUnPressedImage(std::string imagePath) {
	UnPressedImagePath = imagePath;
	usingImages = true;
}

void CButton::SetUseGUI(bool use) {
	UseGUI = use;
}

bool CButton::GetToggled() {
	return toggled;
}

// Old implementation, to be deleted

Button::Button(std::string buttonText, int toX, int toY, int w, int h) : Widget(toX, toY) {
	imageButton = false;

	x = toX;
	y = toY;
	buttonType = btPress;

	width = w;
	height = h;
	if (!width) width = 70;
	if (!height) height = 22;
	
	text = buttonText;
	createButtonTexture();
}


Button::Button(SDL_Texture* offTexture, SDL_Texture* onTexture, int toX, int toY) : Widget(toX, toY) {
	imageButton = true;

	x = toX;
	y = toY;
	textureType = BtnTextureType::bttONOFF;

	unpressedTexture = offTexture;
	pressedTexture = onTexture;

	SDL_QueryTexture(unpressedTexture, NULL, NULL, &width, &height);
}


void Button::createButtonTexture() {
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (pressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (mouseoverTexture) SDL_DestroyTexture(unpressedTexture);

	SDL_Color buttonColor = gui->buttonColor;

	SDL_Texture* mainTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	SDL_SetRenderTarget(renderer, mainTexture);
	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
	SDL_RenderClear(renderer);

	int ampPos = text.find('&');
	if (ampPos != std::string::npos) {
		text.erase(text.begin() + ampPos);
	}
	Label* buttonLbl = new Label(text, 0, 0);
	buttonLbl->setFontColor(gui->buttonFontColor);
	SDL_Rect fontRect = buttonLbl->fontRect;
	fontRect.x = (width / 2) - (fontRect.w / 2);
	fontRect.y = (height / 2) - (fontRect.h / 2);
	SDL_RenderCopy(renderer, buttonLbl->fontTexture, NULL, &fontRect);

	//draw the _
	if (ampPos != std::string::npos) {
		int advance;
		int underlineOffset = 0;
		for (int i = 0; i < ampPos; i++) {
			TTF_GlyphMetrics(buttonLbl->font, text[i], NULL, NULL, NULL, NULL, &advance);
			underlineOffset += advance;
		}
		TTF_GlyphMetrics(buttonLbl->font, text[ampPos], NULL, NULL, NULL, NULL, &advance);
		int x1 = fontRect.x + underlineOffset;
		int x2 = x1 + advance - 1;
		int y1 = fontRect.y + TTF_FontHeight(buttonLbl->font) - 2;
		hlineRGBA(renderer, x1, x2, y1, gui->buttonFontColor.r, gui->buttonFontColor.g, gui->buttonFontColor.b, gui->buttonFontColor.a);
	}
	SDL_SetRenderTarget(renderer, NULL);

	SDL_Color bbColor = gui->buttonBorderColor;

	unpressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	pressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	mouseoverTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_Rect renderRect = { 0, 0, width, height };
	SDL_SetRenderTarget(renderer, unpressedTexture);
	SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
	hlineRGBA(renderer, 0, width - 1, 0, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	hlineRGBA(renderer, 0, width - 1, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	vlineRGBA(renderer, 0, 0, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	vlineRGBA(renderer, width - 1, 0, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	SDL_SetRenderTarget(renderer, NULL);

	SDL_SetRenderTarget(renderer, pressedTexture);
	SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
	hlineRGBA(renderer, 0, width - 1, 0, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	vlineRGBA(renderer, 0, 0, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	SDL_SetRenderTarget(renderer, NULL);

	SDL_SetRenderTarget(renderer, mouseoverTexture);
	SDL_RenderCopy(renderer, mainTexture, NULL, &renderRect);
	hlineRGBA(renderer, 0, width - 1, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	vlineRGBA(renderer, width - 1, 0, height - 1, bbColor.r, bbColor.g, bbColor.b, bbColor.a);
	SDL_SetRenderTarget(renderer, NULL);

	delete buttonLbl;
	SDL_DestroyTexture(mainTexture);
}


Button::~Button() {
	SDL_DestroyTexture(lTexture); lTexture = NULL;
	SDL_DestroyTexture(iconTexture); iconTexture = NULL;
}


bool Button::handleEvent(SDL_Event* e) {
	if (e->type == SDL_MOUSEMOTION) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(SDL_Rect{ x, y, width, height }, mx, my)) {
			mouseOver = true;
		}
		else mouseOver = false;
	}

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (mx >= x && mx <= (x + width) && my >= y && my <= (y + height)) {
			if (buttonType == btPress) held = true;
		}
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (mx >= x && mx <= (x + width) && my >= y && my <= (y + height)) {
			if (buttonType == btToggle) { 
				pressed = !pressed; 
				return true; 
			}
			if (buttonType == btPress && held) {
				pressed = true;
			}

		}
		held = false;
	}
	
	return false;
}


void Button::render() {
	if (!visible) return;

	SDL_Texture* renderTexture;

	if (buttonType == btPress) {
		if (held) renderTexture = pressedTexture;
		else {
			if (mouseOver && mouseoverTexture != NULL) renderTexture = mouseoverTexture;
			else renderTexture = unpressedTexture;
		}
	}
	else if (buttonType == btToggle) {
		if (pressed) renderTexture = pressedTexture;
		else {
			if (mouseOver && mouseoverTexture != NULL) renderTexture = mouseoverTexture;
			else renderTexture = unpressedTexture;
		}
	}
	else {
		if (mouseOver && mouseoverTexture != NULL) renderTexture = mouseoverTexture;
		else renderTexture = unpressedTexture;
	}

	int w, h;
	SDL_QueryTexture(renderTexture, NULL, NULL, &w, &h);
	SDL_Rect bRect = { x, y, w, h };
	SDL_RenderCopy(renderer, renderTexture, NULL, &bRect);
}


void Button::setVisibility(bool setVisible) {
	visible = setVisible;
}

void Button::setButtonText(std::string txt) {
	text = txt;
	createButtonTexture();
}

bool Button::wasPressed() {
	bool wasPressed = pressed;
	if (buttonType == btPress) pressed = false;
	return wasPressed;
}

bool Button::isToggled() {
	return pressed;
}