#ifndef __FORMEXIT_H
#define __FORMEXIT_H

#include "SubForm.h"

class FormExit : public SubForm {
public:
	bool submitted = false;

	FormExit(std::string name, std::string title, int xPos, int yPos);
	~FormExit();

	void FormExit::render();
	bool FormExit::handleEvent(SDL_Event* e);
	void FormExit::handleWidgetEvent();

private:
	Label* message = nullptr;
	SDL_Texture *minlogo;
	SDL_Rect logoRect;
	Button *btnOkay, *btnCancel, *btnHelp;
};

#endif