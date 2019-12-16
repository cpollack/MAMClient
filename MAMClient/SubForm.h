#ifndef __SUBFORM_H
#define __SUBFORM_H

#include "Client.h"

#include "Label.h"
#include "Field.h"
#include "Button.h"
#include "CheckBox.h"
#include "Panel.h"
#include "Gauge.h"
#include "ListView.h"

struct LessThanByDepth
{
	bool operator()(const Widget* lhs, const Widget* rhs) const
	{
		return lhs->depth < rhs->depth;
	}
};

class SubForm {
public:
	SubForm(std::string name, std::string title, int w, int h);
	~SubForm();

	SDL_Renderer* renderer;
	int width, height;
	int x, y;
	bool visible;
	bool closeWindow = false;

	virtual void SubForm::render();
	virtual bool SubForm::handleEvent(SDL_Event* e);
	virtual void SubForm::handleWidgetEvent();
	void SubForm::setPosition(int posX, int posY);
	void SubForm::setVisible(bool isVisible);
	void SubForm::addWidget(Widget* widget);
	void SubForm::removeWidget(Widget* widget);

	std::string SubForm::getName();

protected:
	bool isClosePressed = false;

	std::string name;
	std::vector<Widget*> widgetsByPriority;
	std::vector<Widget*> widgets;


private:
	std::string title;
	Label* titleLabel;

	bool canEscape, canDrag;
	bool isWindowDragging = false;
	int drag_start_x, drag_start_y;

	SDL_Rect topRect, closeRect;
	
	void SubForm::renderGuiTexture(Texture* guiTexture, int posX, int posY);
	SDL_Rect SubForm::getSrcRect(Texture* guiTexture, int atX, int atY);
	SDL_Rect SubForm::getDstRect(Texture* guiTexture, int atX, int atY);
};

#endif