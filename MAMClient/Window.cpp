#include "stdafx.h"
#include "resource.h"
#include "Window.h"
#include "GUI.h"
#include "Define.h"
#include "CustomEvents.h"

#include "Widget.h"
#include "Label.h"
#include "Button.h"
#include "CheckBox.h"
#include "Panel.h"
#include "Field.h"
#include "ImageBox.h"
#include "Dropdown.h"
#include "TabControl.h"
#include "Gauge.h"
#include "ListBox.h"

#include "SDL_syswm.h"
#include "include/rapidjson/filereadstream.h"

using namespace rapidjson;

void LoadJSONResource(int jsonName, DWORD& size, const char*& data)
{
	HMODULE handle = ::GetModuleHandle(NULL);
	HRSRC rc = ::FindResource(handle, MAKEINTRESOURCE(jsonName),
		MAKEINTRESOURCE(JSONFILE));
	HGLOBAL rcData = ::LoadResource(handle, rc);
	size = ::SizeofResource(handle, rc);
	data = static_cast<const char*>(::LockResource(rcData));
}

CWindow::CWindow() {
	Type = FT_DEFAULT;
}

CWindow::CWindow(int w, int h) {
	Type = FT_DEFAULT;
	Width = w;
	Height = h;
	init();

	initUI();
}

/*
	if (jsonFile.compare("LoginForm.JSON") == 0) {
		DWORD size = 0;
		const char* data = NULL;
		LoadJSONResource(IDR_LOGINFORM, size, data);
		document.Parse(data);
*/


/// <summary>
/// Create a new window from a json file.
/// </summary>
/// <param name="jsonFile">JSON document containing all window and widget properties.</param>
CWindow::CWindow(std::string jsonFile) {
	Type = FT_DEFAULT;
	std::string path = "JSON/" + jsonFile;

	Document document;
	char* readBuffer = new char[65536];

	//Load JSON document to buffer for parsing
	FILE* fp = fopen(path.c_str(), "rb");
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));	
	document.ParseStream<0, UTF8<>, FileReadStream>(is);
	fclose(fp);
	delete[] readBuffer;

	loadDocument(document);
	SetParentFromStack();
}

CWindow::~CWindow() {
	ClearWidgets();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

/// <summary>
/// Create and initialize all widgets specified in the JSON document
/// </summary>
/// <param name="document">A RapidJSON document</param>
void CWindow::loadDocument(Document& document) {
	if (!document.IsObject()) return;
	if (!document.HasMember("Window")) return;

	Value vDocObject = document.GetObject();
	Value vWindow = vDocObject["Window"].GetObject();

	if (vWindow.HasMember("Width")) Width = vWindow["Width"].GetInt();
	if (vWindow.HasMember("Height")) Height = vWindow["Height"].GetInt();

	init();
	initUI();

	if (vWindow.HasMember("Title")) SetTitle(vWindow["Title"].GetString());
	if (vWindow.HasMember("CloseButton")) SetUseClose(vWindow["CloseButton"].GetBool());

	if (vWindow.HasMember("Widgets")) {
		Value vWidgets = vWindow["Widgets"].GetArray();
		if (vWidgets.IsArray()) {
			for (rapidjson::SizeType i = 0; i < vWidgets.Size(); i++) {
				Value vWidget = vWidgets[i].GetObject();
				LoadWidgetByType(vWidget);
			}
		}
	}
}


CWidget* CWindow::LoadWidgetByType(rapidjson::Value& vWidget) {
	if (!vWidget.IsObject()) return nullptr;
	Value widget = vWidget.GetObject();

	int type;
	if (!widget.HasMember("Type")) return nullptr;
	type = widget["Type"].GetInt();
	std::string name = widget["Name"].GetString();

	CWidget *addWidget = nullptr;
	switch (type) {
	case wtLabel:
		addWidget = new CLabel(this, widget);
		break;
	case wtCheckBox:
		addWidget = new CCheckBox(this, widget);
		break;
	case wtButton:
		addWidget = new CButton(this, widget);
		break;
	case wtPanel:
		addWidget = new CPanel(this, widget);
		break;
	case wtField:
		addWidget = new CField(this, widget);
		break;
	case wtDropDown:
		addWidget = new CDropDown(this, widget);
		break;
	case wtImageBox:
		addWidget = new CImageBox(this, widget);
		break;
	case wtTabControl:
		addWidget = new CTabControl(this, widget);
		break;
	case wtGauge:
		addWidget = new CGauge(this, widget);
		break;
	case wtListBox:
		addWidget = new CListBox(this, widget);
		break;
	}
	if (addWidget) {
		addWidget->SetWindow(this);
		AddWidget(addWidget);
	}
	return addWidget;
}

void CWindow::AddWidget(CWidget* widget) {
	if (!widget) return;
	
	widgets[widget->Name] = widget;
	widgetsByDepth.push_back(widget);
	widgetsByDepth.sort([](CWidget* f, CWidget* s) { return f->GetDepth() > s->GetDepth(); });
}

void CWindow::RemoveWidget(std::string widgetName) {
	std::map<std::string,CWidget*>::iterator it = widgets.begin();

	while (it != widgets.end()) {
		if (it->second->Name.compare(widgetName) == 0) {
			delete it->second;
			it = widgets.erase(it);
			break;
		}
		else ++it;
	}

	std::list<CWidget*>::iterator it2 = widgetsByDepth.begin();
	while (it2 != widgetsByDepth.end()) {
		if ((*it2)->Name.compare(widgetName) == 0) {
			it2 = widgetsByDepth.erase(it2);
			break;
		}
		else ++it2;
	}
}

void CWindow::ClearWidgets() {
	for (auto widget : widgets) {
		delete widget.second;
	}
	widgets.clear();
	widgetsItr = widgets.end();
	widgetsByDepth.clear();

	btnClose = nullptr;
	btnMinimize = nullptr;
}

CWidget* CWindow::GetWidget(std::string widgetName) {
	std::map<std::string, CWidget*>::iterator itr;
	itr = widgets.find(widgetName);
	if (itr != widgets.end())
		return itr->second;
	return nullptr;
}

void CWindow::FocusWidget(std::string widgetName) {
	CWidget *widget = GetWidget(widgetName);
	if (widget && CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WIDGET;
		event.window.windowID = windowID;
		event.user.code = WIDGET_FOCUS_GAINED;
		event.user.data1 = widget;
		event.user.data2 = this;
		SDL_PushEvent(&event);
	}
}

void CWindow::FocusWidget(CWidget* widget) {
	if (widget && CUSTOMEVENT_WIDGET != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WIDGET;
		event.window.windowID = windowID;
		event.user.code = WIDGET_FOCUS_GAINED;
		event.user.data1 = widget;
		event.user.data2 = this;
		SDL_PushEvent(&event);
	}
}

bool CWindow::WidgetHasMouse() {
	for (auto widget : widgets) {
		if (widget.second->IsMouseOver()) {
			return true;
		}
	}
	return false;
}

bool CWindow::init() {
	//Create window
	Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR;
	window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, flags);
	if (window != NULL) {
		SDL_SetWindowBordered(window, SDL_FALSE);
		mouseFocus = true;
		keyboardFocus = true;

		SDL_SysWMinfo info;
		SDL_VERSION(&info.version); /* initialize info structure with SDL version info */
		if (SDL_GetWindowWMInfo(window, &info)) {
			hwnd = info.info.win.window; //pull hwnd from sdl version info
			long style = GetWindowLong(hwnd, GWL_EXSTYLE);
			style |= WS_EX_TOOLWINDOW; //change window style to toolwindow - not shown in switcher
			ShowWindow(hwnd, SW_HIDE); // hide the window
			SetWindowLong(hwnd, GWL_EXSTYLE, style); //apply style change
			ShowWindow(hwnd, SW_SHOW); // show the window for the new style to come into effect
		}

		//Create renderer for window
		//renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		if (renderer == NULL) {
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			SDL_DestroyWindow(window);
			window = NULL;
		}
		else {
			//Initialize renderer color
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

			//Grab window identifier
			windowID = SDL_GetWindowID(window);

			//Flag as opened
			shown = true;
		}
	}
	else printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

	return window != NULL && renderer != NULL;
}

void CWindow::initUI() {
	//Deprecate or move?
	if (!gui) gClient.initializeGUI();

#ifdef SIZE_1024
	topCenter_s = gui->getSkinTexture(renderer, "1024/TopCenter_s.bmp", Anchor::aTopLeft);
	bottomCenter = gui->getSkinTexture(renderer, "1024/BottomCenter.bmp", Anchor::aBottomLeft);
	left = gui->getSkinTexture(renderer, "1024/Left.bmp", Anchor::aTopLeft);
	right = gui->getSkinTexture(renderer, "1024/Right.bmp", Anchor::aTopRight);
#else
	topCenter_s = gui->getSkinTexture(renderer, "TopCenter_s.bmp", Anchor::ANCOR_TOPLEFT);
	bottomCenter = gui->getSkinTexture(renderer, "BottomCenter.bmp", Anchor::ANCOR_BOTTOMLEFT);
	left = gui->getSkinTexture(renderer, "Left.bmp", Anchor::ANCOR_TOPLEFT);
	right = gui->getSkinTexture(renderer, "Right.bmp", Anchor::ANCOR_TOPRIGHT);
#endif

	topLeft_s = gui->getSkinTexture(renderer, "TopLeft_s.bmp", Anchor::ANCOR_TOPLEFT);
	topRight_s = gui->getSkinTexture(renderer, "TopRight_s.bmp", Anchor::ANCOR_TOPRIGHT);
	bottomLeft = gui->getSkinTexture(renderer, "BottomLeft.bmp", Anchor::ANCOR_BOTTOMLEFT);
	bottomRight = gui->getSkinTexture(renderer, "BottomRight.bmp", Anchor::ANCOR_BOTTOMLEFT);

	//close = gui->getSkinTexture(renderer, "Close.bmp", Anchor::TOP_LEFT);
	//minimize = gui->getSkinTexture(renderer, "Min.bmp", Anchor::TOP_LEFT);

	focusedWidget = nullptr;
}

void CWindow::ReloadAssets() {
	for (auto widget : widgets) {
		widget.second->ReloadAssets();
	}
}

void CWindow::handleEvent(SDL_Event& e)
{
	if (e.window.windowID != windowID) return;

	if (e.type == SDL_KEYDOWN) {
		//std::cout << "subwindow keypress\n";
	}

	if (e.type == CUSTOMEVENT_WIDGET) {
		if (e.user.code == WIDGET_FOCUS_GAINED) {
			if (focusedWidget && focusedWidget != e.user.data1) focusedWidget->OnFocusLost();
			focusedWidget = (CWidget*)e.user.data1;
			focusedWidget->OnFocus();
		}

		if (e.user.code == WIDGET_FOCUS_LOST) {
			if (focusedWidget && focusedWidget == e.user.data1) {
				focusedWidget->OnFocusLost();
				focusedWidget = nullptr;
			}
		}
	}

	if (Draggable) {
		int x, y;
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			SDL_GetMouseState(&x, &y);
			//Initiate Drag
			SDL_Rect dragRect{ 0, 0, Width, 20 };
			if (doesPointIntersect(dragRect, x, y)) {
				if (!dragging) {
					dragging = true;
					SDL_CaptureMouse(SDL_TRUE);
					int tX, tY;
					SDL_GetWindowPosition(window, &tX, &tY);
					SDL_GetGlobalMouseState(&drag_start_x, &drag_start_y);
					drag_start_x -= tX;
					drag_start_y -= tY;
				}
			}
		}

		if (dragging && e.type == SDL_MOUSEBUTTONUP) {
			dragging = false;
			SDL_CaptureMouse(SDL_FALSE);
		}

		if (dragging && e.type == SDL_MOUSEMOTION) {
			//Dragging above window doesn't register mouse motion as it isn't in window area. Fix?
			SDL_GetGlobalMouseState(&x, &y);

			int newX, newY;
			newX = x - drag_start_x;
			newY = y - drag_start_y;
			SDL_SetWindowPosition(window, newX, newY);
			return;
		}
	}

	//If an event was detected for this window
	if (e.type == SDL_WINDOWEVENT) {
		//Caption update flag
		bool updateCaption = false;

		switch (e.window.event)
		{
			//Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			shown = true;
			//std::cout << "Subwindow shown " << windowID << "\n";
			break;
			//Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			shown = false;
			//std::cout << "Subwindow hidden\n";
			break;

			//Get new dimensions and repaint
		/*case SDL_WINDOWEVENT_SIZE_CHANGED:
			width = e.window.data1;
			height = e.window.data2;
			SDL_RenderPresent(renderer);
			break;*/

			//Repaint on expose
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(renderer);
			//std::cout << "Subwindow exposed " << windowID << "\n";
			break;

			//Mouse enter
		case SDL_WINDOWEVENT_ENTER:
			mouseFocus = true;
			updateCaption = true;
			//std::cout << "Subwindow mouse enter\n";
			break;
			//Mouse exit
		case SDL_WINDOWEVENT_LEAVE:
			mouseFocus = false;
			updateCaption = true;
			//std::cout << "Subwindow mouse leave\n";
			break;

			//Keyboard focus gained
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			keyboardFocus = true;
			updateCaption = true;

			//Restore text input
			if (focusedWidget && focusedWidget->WidgetType == wtField) {
				SDL_StartTextInput();
				//std::cout << "Subwindow Keyboard input restarted." << std::endl;
			}
			//std::cout << "Subwindow keyboard focus gained " << windowID << "\n";

			break;
			//Keyboard focus lost
		case SDL_WINDOWEVENT_FOCUS_LOST:
			keyboardFocus = false;
			updateCaption = true;
			//std::cout << "Subwindow keyboard focus lost " << windowID << "\n";
			break;

		case SDL_WINDOWEVENT_TAKE_FOCUS:
			//std::cout << "Subwindow take focus " << windowID << "\n";
			break;

			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			minimized = true;
			//std::cout << "Subwindow minimized\n";
			break;
			//Window maxized
		/*case SDL_WINDOWEVENT_MAXIMIZED:
			minimized = false;
			break;*/

			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			minimized = false;
			//std::cout << "Subwindow restored\n";
			break;

			//Hide on close
		/*case SDL_WINDOWEVENT_CLOSE:
			SDL_HideWindow(window);
			break;*/
		}
	}

	widgetsItr = widgets.begin();
	while (widgetsItr != widgets.end()) {
		eventWidget = widgetsItr->second;
		bool doEvents = true;
		if (eventWidget->GetParent() && eventWidget->GetParent()->WidgetType == wtTabControl) {
			if (((CTabControl*)eventWidget->GetParent())->GetVisibleTab() != eventWidget->GetTabItem()) doEvents = false;
		}
		if (doEvents) eventWidget->HandleEvent(e);
		if (widgetsItr != widgets.end()) widgetsItr++;
	}
	eventWidget = nullptr;
}

void CWindow::step() {
	if (widgets.size()) {
		for (auto widget : widgets) {
			eventWidget = widget.second;
			widget.second->Step();
		}
	}
	eventWidget = nullptr;
}
	
void CWindow::focus() {
	if (!shown) SDL_ShowWindow(window);
	SDL_RaiseWindow(window);
	HWND res2 = SetFocus(hwnd);
}

void CWindow::raise() {
	if (shown) SDL_RaiseWindow(window);
}

void CWindow::render() {
	if (minimized) return;
	//Clear screen
	SDL_SetRenderDrawColor(renderer, gui->backColor.r, gui->backColor.g, gui->backColor.b, 0xFF);
	SDL_RenderClear(renderer);

	//Draw background and Widgets	
	SDL_RenderCopy(renderer, topCenter_s->texture, NULL, &getDstRect(topCenter_s, 0, 0));

	SDL_RenderCopy(renderer, bottomCenter->texture, NULL, &getDstRect(bottomCenter, 0, Height));
	SDL_RenderCopy(renderer, left->texture, NULL, &getDstRect(left, 0, 0));
	SDL_RenderCopy(renderer, right->texture, NULL, &getDstRect(right, Width, 0));

	SDL_RenderCopy(renderer, topLeft_s->texture, NULL, &getDstRect(topLeft_s, 0, 0));
	SDL_RenderCopy(renderer, topRight_s->texture, NULL, &getDstRect(topRight_s, Width, 0));

	SDL_RenderCopy(renderer, bottomLeft->texture, NULL, &getDstRect(bottomLeft, 0, Height));
	SDL_RenderCopy(renderer, bottomRight->texture, NULL, &getDstRect(bottomRight, Width, Height));

	renderWidgets();
}

void CWindow::renderWidgets() {
	for (auto widget : widgetsByDepth) {
		if (!widget->GetParent() && widget != focusedWidget) widget->Render();
	}

	if (focusedWidget && !focusedWidget->GetParent()) focusedWidget->Render();
}

void CWindow::renderPresent() {
	SDL_RenderPresent(renderer);
}

SDL_Rect CWindow::getDstRect(Texture* texture, int x, int y) {
	if (texture->anchor == Anchor::ANCOR_TOPRIGHT || texture->anchor == Anchor::ANCOR_BOTTOMRIGHT) texture->rect.x = x - texture->width;
	else texture->rect.x = x;

	if (texture->anchor == Anchor::ANCOR_BOTTOMLEFT || texture->anchor == Anchor::ANCOR_BOTTOMLEFT) texture->rect.y = y - texture->height;
	else texture->rect.y = y;

	texture->rect.w = texture->width;
	texture->rect.h = texture->height;
	return texture->rect;
}

SDL_Window* CWindow::GetWindow() {
	return window;
}

int CWindow::GetWindowID() {
	return SDL_GetWindowID(window);
}

void CWindow::PushWindow(CWindow *pWindow) {
	if (pWindow) Windows.push_back(pWindow); 
}

int CWindow::GetType() {
	return Type;
}

int CWindow::GetWidth() {
	return Width;
}

int CWindow::GetHeight() {
	return Height;
}

void CWindow::SetParent(CWindow* wParent) {
	parent = wParent;
	if (!parent) return;

	int parentX, parentY;
	SDL_GetWindowPosition(parent->GetWindow(), &parentX, &parentY);
	int newX = parentX + (parent->GetWidth() / 2) - (Width / 2);
	int newY = parentY + (parent->GetHeight() / 2) - (Height / 2);
	SDL_SetWindowPosition(GetWindow(), newX, newY);
}

void CWindow::SetParentFromStack() {
	CWindow* top = nullptr;
	if (Windows.size()) top = Windows.back();
	else top = mainWindow;
	SetParent(top);
}

void CWindow::SetTitle(std::string title) {
	CLabel *lblTitle = (CLabel*)GetWidget("lblTitle");
	Title = title;
	if (lblTitle) lblTitle->SetText(Title);
	else {
		lblTitle = new CLabel(this, "lblTitle", 0, 8);
		lblTitle->SetWidth(Width);
		lblTitle->SetHeight(16);
		lblTitle->SetAlignment(laCenter);
		lblTitle->SetVAlignment(lvaCenter);
		lblTitle->SetText(Title);
		AddWidget(lblTitle);
	}
}

void CWindow::SetUseClose(bool close) {
	ShowClose = close;
	RemoveWidget("btnClose");
	btnClose = nullptr;

	if (ShowClose) {
		btnClose = new CButton(this, "btnClose", (Width - 25), 10);
		//btnClose->SetRenderer(renderer);
		btnClose->SetWidth(14);
		btnClose->SetHeight(13);
		btnClose->SetUseGUI(true);
		btnClose->SetPressedImage("Close.bmp");
		btnClose->SetUnPressedImage("Close.bmp");
		AddWidget(btnClose);

		registerEvent("btnClose", "Click", std::bind(&CWindow::btnClose_Click, this, std::placeholders::_1));
	}
}

void CWindow::SetUseMinimize(bool min) {
	ShowMinimize = min;

	RemoveWidget("btnMinimize");
	btnMinimize = nullptr;

	if (ShowMinimize) {
		btnMinimize = new CButton(this, "btnMinimize", (Width - 45), 10);
		btnMinimize->SetWidth(14);
		btnMinimize->SetHeight(13);
		btnMinimize->SetUseGUI(true);
		btnMinimize->SetPressedImage("Min.bmp");
		btnMinimize->SetUnPressedImage("Min.bmp");
		AddWidget(btnMinimize);
	}
}

void CWindow::registerEvent(std::string widgetName, std::string eventName, EventFunc evf) {
	auto iter = widgets.find(widgetName);
	if (iter != widgets.end()) {
		iter->second->RegisterEvent(eventName, evf);
	}
}

void CWindow::btnMinimize_Click(SDL_Event& e) {
	SDL_MinimizeWindow(window);
}

void CWindow::btnClose_Click(SDL_Event& e) {
	CloseWindow = true;

	if (CUSTOMEVENT_WINDOW != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WINDOW;
		if (parent) event.window.windowID = parent->GetWindowID();
		event.user.code = WINDOW_CLOSE;
		event.user.data1 = this;
		event.user.data2 = nullptr;
		SDL_PushEvent(&event);
	}
}