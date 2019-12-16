#ifndef __FORMERROR_H
#define __FORMERROR_H

#include "SubForm.h"

class FormError : public SubForm {
public:
	bool submitted = false;

	FormError(std::string name, std::string title, int xPos, int yPos);
	~FormError();

	void FormError::render();
	bool FormError::handleEvent(SDL_Event* e);
	void FormError::handleWidgetEvent();

	void FormError::setMessage(std::string msg);

private:
	Label* message = nullptr;
	Button *btnOkay;
};

#endif