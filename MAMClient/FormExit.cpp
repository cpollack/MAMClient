#include "stdafx.h"
#include "FormExit.h"


FormExit::FormExit(std::string name, std::string title, int xPos, int yPos) : SubForm(name, title, 353, 234) {
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	btnOkay = new Button("Okay(&O)", x + 100, y + 194, 70, 22);
	btnCancel = new Button("Cancel(&C)", x + 178, y + 194, 70, 22);
	btnHelp = new Button("Help(&H)", x + 254, y + 194, 70, 22);
	addWidget(btnOkay);
	addWidget(btnCancel);
	addWidget(btnHelp);

	message = new Label("Are you sure that you want to leave?", x + 30, y + 160);
	message->setTextWidth(300);
	addWidget(message);

	minlogo = gui->minlogo->texture;
	logoRect.x = 25;
	logoRect.y = 30;
	logoRect.w = gui->minlogo->width;
	logoRect.h = gui->minlogo->height;
}


FormExit::~FormExit() {

}

void FormExit::render() {
	SubForm::render();

	SDL_Rect logoRendRect = logoRect;
	logoRendRect.x += x;
	logoRendRect.y += y;
	SDL_RenderCopy(renderer, minlogo, NULL, &logoRendRect);
}


bool FormExit::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	return true;
}


void FormExit::handleWidgetEvent() {
	if (btnOkay->wasPressed()) {
		submitted = true;
	}

	if (btnCancel->wasPressed()) {
		closeWindow = true;
	}
}

