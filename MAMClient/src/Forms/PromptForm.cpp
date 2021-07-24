#include "stdafx.h"
#include "PromptForm.h"

#include "CustomEvents.h"
#include "MainWindow.h"
#include "Label.h"
#include "Button.h"
CPromptForm::CPromptForm() : CWindow(400, 160) {
	Type = FT_PROMPT;

	parent = mainForm;
	if (Windows.size() > 1) parent = Windows[Windows.size() - 2];

	//Center window over main form
	SDL_Rect pos, size;
	SDL_GetWindowPosition(parent->GetWindow(), &pos.x, &pos.y);
	SDL_GetWindowSize(parent->GetWindow(), &size.x, &size.y);

	SDL_Rect newPos;
	newPos.x = pos.x + (size.x / 2) - (Width / 2);
	newPos.y = pos.y + (size.y / 2) - (Height / 2);
	SDL_SetWindowPosition(window, newPos.x, newPos.y);

	Draggable = false;

	lblMessage = new CLabel(this, "lblMessage", 20, 35);
	lblMessage->SetWidth(350);
	lblMessage->SetHeight(80);
	lblMessage->SetWrapLength(350);
	AddWidget(lblMessage);

	CButton *btnOK = new CButton(this, "btnOK", 164, 120);
	btnOK->SetWidth(72);
	btnOK->SetHeight(24);
	btnOK->SetText("OK(&O)");
	AddWidget(btnOK);

	registerEvent("btnOK", "Click", std::bind(&CPromptForm::btnOk_Click, this, std::placeholders::_1));
}

CPromptForm::CPromptForm(bool allowCancel) : CWindow(400, 160) {
	Type = FT_PROMPT;

	parent = mainForm;
	if (Windows.size() > 1) parent = Windows[Windows.size() - 2];

	//Center window over main form
	SDL_Rect pos, size;
	SDL_GetWindowPosition(parent->GetWindow(), &pos.x, &pos.y);
	SDL_GetWindowSize(parent->GetWindow(), &size.x, &size.y);

	SDL_Rect newPos;
	newPos.x = pos.x + (size.x / 2) - (Width / 2);
	newPos.y = pos.y + (size.y / 2) - (Height / 2);
	SDL_SetWindowPosition(window, newPos.x, newPos.y);

	Draggable = false;

	lblMessage = new CLabel(this, "lblMessage", 20, 35);
	lblMessage->SetWidth(350);
	lblMessage->SetHeight(80);
	lblMessage->SetWrapLength(350);
	AddWidget(lblMessage);

	CButton *btnOK = new CButton(this, "btnOK", 164, 120);
	btnOK->SetWidth(72);
	btnOK->SetHeight(24);
	btnOK->SetText("OK(&O)");
	AddWidget(btnOK);
	
	registerEvent("btnOK", "Click", std::bind(&CPromptForm::btnOk_Click, this, std::placeholders::_1));

	if (allowCancel) {
		CButton *btnCancel = new CButton(this, "btnCancel", 245, 120);
		btnCancel->SetWidth(72);
		btnCancel->SetHeight(24);
		btnCancel->SetText("Cancel(&C)");
		AddWidget(btnCancel);

		registerEvent("btnCancel", "Click", std::bind(&CPromptForm::btnCancel_Click, this, std::placeholders::_1));
	}
}

void CPromptForm::SetMessage(std::string message) {
	lblMessage->SetText(message);

	//lblMessage->GetTextRect();
	//resize window as needed

	//recenter window
}

void CPromptForm::SetMessage(std::wstring message) {
	lblMessage->SetText(message);

	//lblMessage->GetTextRect();
	//resize window as needed

	//recenter window
}

void CPromptForm::SetType(int type) {
	promptType = type;
}

void CPromptForm::btnOk_Click(SDL_Event& e) {
	CloseWindow = true;

	int closeMode = WINDOW_CLOSE_PROMPT_OK;
	if (promptType > 0) closeMode = promptType;

	if (CUSTOMEVENT_WINDOW != ((Uint32)-1)) {
		SDL_Event event;
		SDL_zero(event);
		event.type = CUSTOMEVENT_WINDOW;
		if (parent) event.window.windowID = parent->GetWindowID();
		event.user.code = closeMode;
		event.user.data1 = this;
		event.user.data2 = nullptr;
		SDL_PushEvent(&event);
	}
}

void CPromptForm::btnCancel_Click(SDL_Event& e) {
	CWindow::btnClose_Click(e);
}