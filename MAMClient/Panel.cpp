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
	//
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

		rectangleRGBA(renderer, 1, captionOffset + 1, Width - 2, Height - captionOffset - 2, white.r, white.g, white.b, white.a);
		rectangleRGBA(renderer, 0, captionOffset, Width - 3, Height - captionOffset - 3, dark.r, dark.g, dark.b, dark.a);

		if (!fontTexture) RenderText();

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


/* Old - to be deprecated */

#include "Label.h"

Panel::Panel(int tox, int toy, int w, int h, std::string header) : Widget(tox, toy) {
	width = w;
	height = h;
	panelHeader = header;

	SDL_Color bgColor = gui->backColor;
	int lblOffset = 0;
	if (panelHeader != "") {
		//Header, draw complex border
		lblHeader = new Label(panelHeader, 9, 0);
		lblOffset = (lblHeader->height / 2);
		height += lblOffset;
		y -= lblOffset;
	}
	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
	SDL_RenderClear(renderer);

	//Border
	if (panelHeader != "") {
		//Header, draw complex border

		rectangleRGBA(renderer, 1, lblOffset + 1, width - 1, height - 1, 0xFF, 0xFF, 0xFF, 0xFF);
		rectangleRGBA(renderer, 0, lblOffset, width - 2, height - 2, 0xA0, 0xA0, 0xA0, 0xFF);
		
		boxRGBA(renderer, 9, 0, 9 + lblHeader->textRect.w, lblHeader->textRect.h, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		lblHeader->render();
	}
	else {
		//No header, draw simple border
		hlineRGBA(renderer, 0, width-1, 0, 0xFF, 0xFF, 0xFF, 0xFF);
		vlineRGBA(renderer, 0, 0, height-1, 0xFF, 0xFF, 0xFF, 0xFF);

		hlineRGBA(renderer, 0, width-1, height-1, 0xA0, 0xA0, 0xA0, 0xFF);
		vlineRGBA(renderer, width-1, 0, height-1, 0xA0, 0xA0, 0xA0, 0xFF);		
	}
	SDL_SetRenderTarget(renderer, NULL);
}


Panel::~Panel() {

}


void Panel::render() {
	renderRect = { x,y,width,height };
	SDL_RenderCopy(renderer, renderTexture, NULL, &renderRect);
}