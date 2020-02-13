#include "stdafx.h"
#include "Window.h"
#include "Panel.h"


CPanel::CPanel(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;
}

CPanel::CPanel(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	if (vWidget.HasMember("Caption")) SetText(vWidget["Caption"].GetString());

	if (vWidget.HasMember("Widgets")) {
		rapidjson::Value vChildWidgets = vWidget["Widgets"].GetArray();
		if (vChildWidgets.IsArray()) {
			for (rapidjson::SizeType i = 0; i < vChildWidgets.Size(); i++) {
				rapidjson::Value vChildWidget = vChildWidgets[i].GetObject();
				CWidget* newWidget = Window->LoadWidgetByType(vChildWidget);

				AddChild(newWidget);
				newWidget->SetParent(this);
			}
		}
	}
}

CPanel::~CPanel() {
	if (panelTexture) SDL_DestroyTexture(panelTexture);
}

void CPanel::ReloadAssets() {
	if (panelTexture) {
		CreatePanelTexture();
		for (auto widget : Children) {
			widget->ReloadAssets();
		}
	}
}

void CPanel::Render() {
	if (!Visible) return;
	if (!panelTexture) CreatePanelTexture();
	SDL_RenderCopy(renderer, panelTexture, NULL, &widgetRect);

	for (auto widget : Children) {
		if (widget != Window->focusedWidget) widget->Render();
	}
	if (Window->focusedWidget) Window->focusedWidget->Render();
}

void CPanel::CreatePanelTexture() {
	if (panelTexture) SDL_DestroyTexture(panelTexture);

	backColor = gui->backColor;
	SDL_Color white{ 255, 255, 255, 255 };
	SDL_Color dark{ 160, 160, 160, 255 };

	panelTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_SetRenderTarget(renderer, panelTexture);
	SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
	SDL_RenderClear(renderer);

	if (Text.length() > 0) {
		//Header, draw complex border
		int captionOffset = 7;

		rectangleRGBA(renderer, 1, captionOffset + 1, Width, Height, white.r, white.g, white.b, white.a);
		rectangleRGBA(renderer, 0, captionOffset, Width - 1, Height - 1, dark.r, dark.g, dark.b, dark.a);

		RenderText();

		fontRect.x = 5;
		fontRect.y = 1;
		boxRGBA(renderer, fontRect.x, fontRect.y, fontRect.w + 4, fontRect.h, backColor.r, backColor.g, backColor.b, backColor.a);
		SDL_RenderCopy(renderer, fontTexture, NULL, &fontRect);
	}
	else {
		//No header, draw simple border
		hlineRGBA(renderer, 0, Width - 1, 0, white.r, white.g, white.b, white.a);
		vlineRGBA(renderer, 0, 0, Height - 1, white.r, white.g, white.b, white.a);

		hlineRGBA(renderer, 0, Width - 1, Height - 1, dark.r, dark.g, dark.b, dark.a);
		vlineRGBA(renderer, Width - 1, 0, Height - 1, dark.r, dark.g, dark.b, dark.a);
	}

	SDL_SetRenderTarget(renderer, NULL);
}
