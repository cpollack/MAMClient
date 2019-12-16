#ifndef __FORMMESSAGE_H
#define __FORMMESSAGE_H

#include "SubForm.h"

class FormMessage : public SubForm {
public:
	bool submitted = false;

	FormMessage(std::string name, std::string title, int xPos, int yPos);
	~FormMessage();

	void FormMessage::render();
	bool FormMessage::handleEvent(SDL_Event* e);
	void FormMessage::handleWidgetEvent();

	void FormMessage::setMessage(std::string msg);

private:
	Label* message = nullptr;
};

#endif