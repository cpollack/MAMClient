#include "stdafx.h"
#include "MessageForm.h"

#include "MainWindow.h"
#include "Label.h"

CMessageForm::CMessageForm() : CWindow(400, 100) {
	Type = FT_MESSAGE;
	//Center window over main form
	SDL_Rect pos, size;
	SDL_GetWindowPosition(mainForm->GetWindow(), &pos.x, &pos.y);
	SDL_GetWindowSize(mainForm->GetWindow(), &size.x, &size.y);

	SDL_Rect newPos;
	newPos.x = pos.x + (size.x / 2) - (Width / 2);
	newPos.y = pos.y + (size.y / 2) - (Height / 2);
	SDL_SetWindowPosition(window, newPos.x, newPos.y);

	Draggable = false;

	lblMessage = new CLabel(this, "lblMessage", 20, 35);
	//lblMessage->SetRenderer(renderer);
	lblMessage->SetWidth(350);
	lblMessage->SetWrapLength(350);
	AddWidget(lblMessage);
}

void CMessageForm::SetMessage(std::string message) {
	lblMessage->SetText(message);
	
	//lblMessage->GetTextRect();
	//resize window as needed

	//recenter window
}