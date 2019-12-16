#include "stdafx.h"
#include "FormError.h"


FormError::FormError(std::string name, std::string title, int xPos, int yPos) : SubForm(name, title, 390, 212) {
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);

	btnOkay = new Button("Okay(&O)", x + 161, y + 167, 70, 22);
	addWidget(btnOkay);
}


FormError::~FormError() {

}

void FormError::render() {
	SubForm::render();

	if (message) {
		message->setPosition(x + 16, y + 30);
		message->render();
	}
}


bool FormError::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	return true;
}


void FormError::handleWidgetEvent() {
	if (btnOkay->wasPressed()) {
		submitted = true;
		SDL_Event quitEvent;
		quitEvent.type = SDL_QUIT;
		SDL_PushEvent(&quitEvent);
	}
}


void FormError::setMessage(std::string msg) {
	if (message) delete message;

	message = new Label(msg, x + 16, y + 30);
	message->setTextWidth(350);
}