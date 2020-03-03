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
	//if (buttonTexture) SDL_DestroyTexture(buttonTexture); //button texture is always one of the below three

	if (pressedImage) delete pressedImage;
	else if (pressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (unpressedImage) delete unpressedImage;
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	
	if (mouseoverTexture) SDL_DestroyTexture(unpressedTexture);
}

void CButton::Load() {
	if (!buttonTexture) {
		CreateButtonTexture();
	}
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
	CWidget::HandleEvent(e);

	if (e.type == SDL_MOUSEMOTION) {
		if (MouseOver) {
			if (!held && mouseoverTexture) buttonTexture = mouseoverTexture;
		}
		else {
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
		if (MouseOver) {
			held = true;

			if (Type != btToggle) buttonTexture = pressedTexture;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		if (MouseOver) {
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

	if (Type == btToggle) OnToggle(e);
}

void CButton::OnToggle(SDL_Event& e) {
	auto iter = eventMap.find("Toggle");
	if (iter != eventMap.end()) iter->second(e);

	if (toggled) OnToggleOn(e);
	else OnToggleOff(e);
}

void CButton::OnToggleOn(SDL_Event& e) {
	auto iter = eventMap.find("ToggleOn");
	if (iter != eventMap.end()) iter->second(e);
}

void CButton::OnToggleOff(SDL_Event& e) {
	auto iter = eventMap.find("ToggleOff");
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
			if (unpressedImage) delete unpressedImage;
			unpressedImage = gui->getSkinTexture(renderer, UnPressedImagePath, Anchor::aTopLeft);
			unpressedTexture = unpressedImage->texture;
		}
		else {
			if (unpressedImage) delete unpressedImage;
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
			if (pressedImage) delete pressedImage;
			pressedImage = gui->getSkinTexture(renderer, PressedImagePath, Anchor::aTopLeft);
			pressedTexture = pressedImage->texture;
		}
		else {
			if (pressedImage) delete pressedImage;
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

void CButton::Toggle(bool bToggle) {
	toggled = bToggle;
	if (toggled) buttonTexture = pressedTexture;
	else buttonTexture = unpressedTexture;

	SDL_Event e;
	OnToggle(e);
}

