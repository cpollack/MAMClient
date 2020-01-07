#include "stdafx.h"
#include "Window.h"
#include "TabControl.h"

#include "Texture.h"

CTabControl::CTabControl(CWindow* window, std::string name, int x, int y) : CWidget(window) {
	Name = name;
	X = x;
	Y = y;

	VisibleTab = 0;
}

CTabControl::CTabControl(CWindow* window, rapidjson::Value& vWidget) : CWidget(window, vWidget) {
	if (!vWidget.IsObject()) return;

	VisibleTab = 0;

	if (vWidget.HasMember("Style")) Style = (TabStyle)vWidget["Style"].GetInt();
	else Style = tsSimple;

	if (vWidget.HasMember("TabsOnBottom")) TabsOnBottom = vWidget["TabsOnBottom"].GetBool();

	if (vWidget.HasMember("Tabs")) {
		rapidjson::Value vTabs = vWidget["Tabs"].GetArray();
		if (vTabs.IsArray() && vTabs.Size()) {
			for (rapidjson::SizeType i = 0; i < vTabs.Size(); i++) {
				tabs.push_back(vTabs[i].GetString());
			}
		}
	}

	if (vWidget.HasMember("Widgets")) {
		rapidjson::Value vChildWidgets = vWidget["Widgets"].GetArray();
		if (vChildWidgets.IsArray()) {
			for (rapidjson::SizeType i = 0; i < vChildWidgets.Size(); i++) {
				rapidjson::Value vChildWidget = vChildWidgets[i].GetObject();
				CWidget* newWidget = Window->LoadWidgetByType(vChildWidget);

				if (newWidget) {
					AddChild(newWidget);
					newWidget->SetParent(this);
				}
			}
		}
	}
}

CTabControl::~CTabControl() {
	if (tabControlTexture) SDL_DestroyTexture(tabControlTexture);
	for (auto texture : tabTextures) delete texture;
}

void CTabControl::ReloadAssets() {
	if (tabControlTexture) {
		CreateTabControlTexture();
		for (auto widget : Children) {
			widget->ReloadAssets();
		}
	}
}

void CTabControl::Render() {
	if (!Visible) return;
	if (!tabControlTexture) CreateTabControlTexture();
	SDL_RenderCopy(renderer, tabControlTexture, NULL, &widgetRect);

	SDL_Rect priorViewport;
	SDL_RenderGetViewport(renderer, &priorViewport);
	SDL_RenderSetViewport(renderer, &widgetRect);

	//Render tabs
	//if (Style == tsButton) RenderButtonTabs();
	//else RenderTabs();
	for (auto tab : tabTextures) 
		SDL_RenderCopy(renderer, tab->texture, NULL, &tab->rect);

	SDL_RenderSetViewport(renderer, &priorViewport);

	//Render child widgets
	for (auto widget : Children) {
		if (widget->GetTabItem() == VisibleTab && widget != Window->focusedWidget) widget->Render();
	}
	if (Window->focusedWidget && Window->focusedWidget->GetTabItem() == VisibleTab) Window->focusedWidget->Render();
}

void CTabControl::RenderTabs() {
	//Simple and Detail mode are the same
}

//Deprecate?
void CTabControl::RenderButtonTabs() {
	//Button style should try to center the buttons
	int tabWidthTotal = 0;
	for (auto tab : tabTextures) tabWidthTotal += tab->width;

	int remWidth = Width - tabWidthTotal;
	int tabSpacer = (remWidth / tabTextures.size()) / 2;

	int tx = 0;
	for (auto tab : tabTextures) {
		tx += tabSpacer;

		SDL_Rect tabRect = {tx, 0, tab->width, tab->height};
		if (TabsOnBottom) tabRect.y = Height - TAB_BUTTON_HEIGHT - 1;
		SDL_RenderCopy(renderer, tab->texture, NULL, &tabRect);

		tx += tab->width + tabSpacer;
	}
}

void CTabControl::HandleEvent(SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(widgetRect, mx, my)) {
			for (int i = 0; i < tabTextures.size(); i++) {
				Texture* tab = tabTextures[i];
				SDL_Rect tabRect = tab->rect;
				tabRect.x += X;
				tabRect.y += Y;
				if (doesPointIntersect(tabRect, mx, my)) {
					if (VisibleTab != i) {
						int priorTab = VisibleTab;
						VisibleTab = i;
						CreateTabTexture(priorTab);
						CreateTabTexture(VisibleTab);
						LoadTabRects();
						OnTabChange(e);
					}
					break;
				}
			}
		}
	}
}

void CTabControl::CreateTabControlTexture() {
	if (tabControlTexture) SDL_DestroyTexture(tabControlTexture);

	backColor = { 160, 160, 160, 0 }; //Clear

	tabControlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, Width, Height);
	SDL_Texture* priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tabControlTexture);
	SDL_SetTextureBlendMode(tabControlTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, backColor.r, backColor.g, backColor.b, backColor.a);
	SDL_RenderClear(renderer);

	//Draw Borders
	switch (Style) {
	case tsSimple:
		DrawSimpleBorder();
		break;
	case tsDetail:
		DrawDetailBorder();
		break;
	case tsButton:
		DrawButtonBorder();
		break;
	}

	SDL_SetRenderTarget(renderer, priorTarget);

	CreateTabTextures();
	LoadTabRects();
}

void CTabControl::DrawSimpleBorder() {
	//TODO
}

void CTabControl::DrawDetailBorder() {
	//TODO
}

void CTabControl::DrawButtonBorder() {
	//no border
}

void CTabControl::CreateTabTextures() {
	for (int i = 0; i < tabs.size(); i++) CreateTabTexture(i);
}

void CTabControl::CreateTabTexture(int index) {
	bool vTab = (index == VisibleTab) ? true : false;
	switch (Style) {
	case tsSimple:
		DrawSimpleTab(index, vTab);
		break;
	case tsDetail:
		DrawDetailTab(index, vTab);
		break;
	case tsButton:
		DrawButtonTab(index, vTab);
		break;
	}
}

void CTabControl::DrawSimpleTab(int index, bool vTab) {
	//TODO
}

void CTabControl::DrawDetailTab(int index, bool vTab) {
	//TODO
}

void CTabControl::DrawButtonTab(int index, bool vTab) {
	SDL_Color buttonColor = { 00, 56, 55, 255 };
	SDL_Color fontColor = { 0xAA, 0xD5, 0xFF, 255 };
	SDL_Color borderColor = { 0, 0, 0, 255 };

	Texture* tabText = stringToTexture(renderer, tabs[index], gui->font, 0, fontColor, 0);

	int strSize = tabText->width;
	int tabWidth = (strSize + 16 < TAB_MIN_WIDTH) ? TAB_MIN_WIDTH : (strSize + 16);

	Texture* tabTexture = new Texture(renderer);
	tabTexture->texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tabWidth, TAB_BUTTON_HEIGHT);
	tabTexture->width = tabWidth;			tabTexture->rect.w = tabWidth;
	tabTexture->height = TAB_BUTTON_HEIGHT;	tabTexture->rect.h = TAB_BUTTON_HEIGHT;
	SDL_Rect textRect = { (tabWidth / 2) - (tabText->width / 2),  (TAB_BUTTON_HEIGHT / 2) - (tabText->height / 2), tabText->width, tabText->height };

	SDL_Texture * priorTarget = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, tabTexture->texture);
	SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
	if (vTab) {

		SDL_RenderDrawLine(renderer, 0, 0, 0, TAB_BUTTON_HEIGHT - 1);
		if (!TabsOnBottom) SDL_RenderDrawLine(renderer, 0, 0, tabWidth - 1, 0);
		SDL_RenderDrawLine(renderer, tabWidth - 1, 0, tabWidth - 1, TAB_BUTTON_HEIGHT - 1);
		if (TabsOnBottom) SDL_RenderDrawLine(renderer, 0, TAB_BUTTON_HEIGHT - 1, tabWidth - 1, TAB_BUTTON_HEIGHT - 1);

		SDL_RenderCopy(renderer, tabText->texture, NULL, &textRect);
	}
	else {
		SDL_Rect rect = {0, 0, tabWidth - 1, TAB_BUTTON_HEIGHT - 1 };
		SDL_RenderDrawRect(renderer, &rect);

		SDL_RenderCopy(renderer, tabText->texture, NULL, &textRect);
	}

	SDL_SetRenderTarget(renderer, priorTarget);

	if (tabTextures.size() < index + 1) tabTextures.push_back(tabTexture);
	else {
		delete tabTextures[index];
		tabTextures.erase(tabTextures.begin() + index);
		tabTextures.insert(tabTextures.begin() + index, tabTexture);
	}
}

void CTabControl::LoadTabRects() {
	//Temp, add standard handling
	if (Style != tsButton) return;

	//Button style should try to center the buttons
	int tabWidthTotal = 0;
	for (auto tab : tabTextures) tabWidthTotal += tab->width;

	int remWidth = Width - tabWidthTotal;
	int tabSpacer = (remWidth / tabTextures.size()) / 2;

	int tx = 0;
	for (auto tab : tabTextures) {
		tx += tabSpacer;

		tab->rect.x = tx;
		if (TabsOnBottom) tab->rect.y = Height - TAB_BUTTON_HEIGHT - 1;
		else tab->rect.y = 0;

		tx += tab->width + tabSpacer;
	}
}

void CTabControl::OnTabChange(SDL_Event& e) {
	auto iter = eventMap.find("OnTabChange");
	if (iter != eventMap.end()) iter->second(e);
}