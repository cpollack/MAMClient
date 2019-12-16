#include "stdafx.h"
#include "FormMessage.h"


FormMessage::FormMessage(std::string name, std::string title, int xPos, int yPos) : SubForm(name, title, 417, 77) {
	int offsetX = xPos - (width / 2);
	int offsetY = yPos - (height / 2);
	setPosition(offsetX, offsetY);
}


FormMessage::~FormMessage() {

}

void FormMessage::render() {
	SubForm::render();

	if (message) {
		message->setPosition(x + 16, y + 30);
		message->render();
	}
}


bool FormMessage::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	return true;
}


void FormMessage::handleWidgetEvent() {
	//
}


void FormMessage::setMessage(std::string msg) {
	if (message) delete message;

	message = new Label(msg, x + 20, y + 35);
	message->setTextWidth(440);
}