#ifndef __FORM_H
#define __FORM_H

#include "Client.h"

#include "FormStruct.h"
#include "SubForm.h"
#include "GUI.h"
#include "Widget.h"
#include "Label.h"
#include "Gauge.h"

typedef enum FormStyle {
	fsMain,
	fsSub,
	fsPopup,
	fsMessage,
	fsCustom
}FormStyle;

typedef enum OldFormType {
	ftIntro,
	ftServerSelect,
	ftLogin,
	ftMain
}OldFormType;

class Form {
public:
	SDL_Window* window;
	SDL_Renderer* renderer = NULL;

	int width, height, windowID;
	bool created;
	bool minimized = false;
	bool focused = false;

	Form::Form(int w, int h);
	Form::~Form();
	virtual void Form::render();
	virtual void Form::handleEvent(SDL_Event& e);
	virtual void Form::handleWidgetEvent() { };
	void Form::setFormType(OldFormType ft);
	OldFormType Form::getFormType();
	void Form::setFormStyle(FormStyle fs);

	void Form::setCanEscape(bool value);
	void Form::setCanMinimize(bool value);

	void Form::addSubform(std::string name, SubForm* subform);
	std::vector<SubForm*>::iterator Form::deleteSubform(std::string name);
	SubForm* Form::getSubform(std::string name);

protected:
	std::map<std::string, Label*> labels;
	std::vector<std::string> labelKeys;

	std::vector<SubForm*> subforms;
	std::map<std::string, SubForm*> subformMap;

	void Form::createLabel(std::string name, std::string line, int toX, int toY, bool isBold = false);
	Label* Form::getLabel(std::string name);

private:
	bool isMainForm = false;
	OldFormType formType;
	FormStyle formStyle;

	bool canEscape = true, canMinimize = false;
	bool isClosePressed = false, isMinimizePressed = false;
	bool isWindowDragging = false;
	int drag_start_x, drag_start_y;

	SDL_Rect Form::getDstRect(Texture* aTexture, int x, int y);
};

#endif
