#ifndef __FORMPROMPT_H
#define __FORMPROMPT_H

#include "SubForm.h"

class FormPrompt : public SubForm {
public:
	bool submitted = false;

	FormPrompt(std::string name, std::string title, int xPos, int yPos);
	~FormPrompt();

	void FormPrompt::render();
	bool FormPrompt::handleEvent(SDL_Event* e);
	void FormPrompt::handleWidgetEvent();

	void FormPrompt::setMessage(std::string msg);

private:
	Label* message = nullptr;
	Button *btnOkay, *btnCancel;
};

#endif