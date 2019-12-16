#include "stdafx.h"
#include "FormPrompt.h"


FormPrompt::FormPrompt(std::string name, std::string title, int xPos, int yPos) : SubForm(name, title, 477, 143) {
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	btnOkay = new Button("Okay(&O)", x + 229, y + 109, 70, 22);
	btnCancel = new Button("Cancel(&C)", x + 309, y + 109, 70, 22);
	addWidget(btnOkay);
	addWidget(btnCancel);
}


FormPrompt::~FormPrompt() {

}

void FormPrompt::render() {
	SubForm::render();

	if (message) {
		message->setPosition(x + 16, y + 30);
		message->render();
	}
}


bool FormPrompt::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	return true;
}


void FormPrompt::handleWidgetEvent() {
	if (btnOkay->wasPressed()) {
		submitted = true;
	}

	if (btnCancel->wasPressed()) {
		closeWindow = true;
	}
}


void FormPrompt::setMessage(std::string msg) {
	if (message) delete message;

	message = new Label(msg, x + 16, y + 30);
	message->setTextWidth(440);
}