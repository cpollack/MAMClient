#include "stdafx.h"
#include "CheckBox.h"

CCheckBox::CCheckBox(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
}

CCheckBox::CCheckBox(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("Text")) SetText(vWidget["Text"].GetString());
	fontRect.x = 17;
}

CCheckBox::~CCheckBox() {
	if (cbTexture) SDL_DestroyTexture(cbTexture);
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (pressedTexture) SDL_DestroyTexture(unpressedTexture);
}

void CCheckBox::ReloadAssets() {
	if (cbTexture) {
		CreateCheckBoxTexture();
	}
}

void CCheckBox::Render() {
	if (!Visible) return;
	if (!cbTexture) CreateCheckBoxTexture();

	SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);

	SDL_SetRenderTarget(renderer, cbTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_Texture *cb;
	if (checked) cb = pressedTexture;
	else cb = unpressedTexture;

	SDL_Rect cbRect = { 0, 0, 13, 13 };
	SDL_RenderCopy(renderer, cb, NULL, &cbRect);
	SDL_RenderCopy(renderer, fontTexture, NULL, &fontRect);
	SDL_RenderCopy(renderer, fontTexture, NULL, &fontRect);

	SDL_SetRenderTarget(renderer, oldTarget);
	SDL_RenderCopy(renderer, cbTexture, NULL, &widgetRect);
}

void CCheckBox::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(widgetRect, mx, my)) {
			held = true;
		}
	}

	if (e.type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(widgetRect, mx, my)) {
			if (held) {
				checked = !checked;
				OnClick(e);
			}
		}
		held = false;
	}
}

void CCheckBox::OnClick(SDL_Event& e) {
	auto iter = eventMap.find("Click");
	if (iter != eventMap.end()) iter->second(e);
}

void CCheckBox::CreateCheckBoxTexture() {
	if (cbTexture) SDL_DestroyTexture(cbTexture);
	if (unpressedTexture) SDL_DestroyTexture(unpressedTexture);
	if (pressedTexture) SDL_DestroyTexture(unpressedTexture);

	RenderText();
	fontRect.x = 17;

	cbTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetTextureBlendMode(cbTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, cbTexture);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);

	unpressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 13, 13);
	pressedTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 13, 13);

	//Unpressed Texture
	SDL_Rect cbRect = { 0, 0, 13, 13 };
	SDL_SetRenderTarget(renderer, unpressedTexture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	hlineRGBA(renderer, 0, 13, 0, 0xA0, 0xA0, 0xA0, 0xFF);
	vlineRGBA(renderer, 0, 0, 13, 0xA0, 0xA0, 0xA0, 0xFF);

	hlineRGBA(renderer, 1, 13 - 1, 1, 0x69, 0x69, 0x69, 0xFF);
	vlineRGBA(renderer, 1, 1, 13 - 1, 0x69, 0x69, 0x69, 0xFF);

	hlineRGBA(renderer, 1, 13 - 1, 13 - 2, 0xE3, 0xE3, 0xE3, 0xFF);
	vlineRGBA(renderer, 13 - 2, 1, 13 - 2, 0xE3, 0xE3, 0xE3, 0xFF);

	hlineRGBA(renderer, 0, 13, 13 - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	vlineRGBA(renderer, 13 - 1, 0, 13 - 1, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_SetRenderTarget(renderer, NULL);

	//Pressed Texture
	SDL_SetRenderTarget(renderer, pressedTexture);
	SDL_RenderCopy(renderer, unpressedTexture, NULL, &cbRect);

	vlineRGBA(renderer, 3, 5, 7, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 4, 6, 8, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 5, 7, 9, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 6, 6, 8, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 7, 5, 7, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 8, 4, 6, 0x00, 0x00, 0x00, 0xFF);
	vlineRGBA(renderer, 9, 3, 5, 0x00, 0x00, 0x00, 0xFF);

	SDL_SetRenderTarget(renderer, NULL);
}

bool CCheckBox::isChecked() {
	return checked;
}