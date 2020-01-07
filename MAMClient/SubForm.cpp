#include "stdafx.h"
#include "SubForm.h"

#include "GUI.h"
#include "FormMain.h"

SubForm::SubForm(std::string name, std::string title, int w, int h) {
	renderer = gClient.renderer;
	
	this->name = name;
	width = w;
	height = h;
	setPosition(w, h);

	canEscape = true;
	canDrag = true;
	visible = true;

	this->title = title;
	titleLabel = new Label(title, 0, 0);
}


SubForm::~SubForm() {
	//for (auto widget : widgets) {
	for (int i = 0; i < widgets.size(); i++) {
		delete widgets[i];
	}
}


void SubForm::render() {
	if (!visible) return;

	//Draw back first
	SDL_Rect backRec{ x, y, width, height };
	SDL_RenderCopy(renderer, gui->minBack->texture, NULL, &backRec);

	renderGuiTexture(gui->topCenter_s, 0, 0);
	renderGuiTexture(gui->bottomCenter, 0, height);
	renderGuiTexture(gui->left, 0, 0);
	renderGuiTexture(gui->right, width, 0);

	renderGuiTexture(gui->topLeft_s, 0, 0);
	renderGuiTexture(gui->topRight_s, width, 0);

	renderGuiTexture(gui->bottomLeft, 0, height);
	renderGuiTexture(gui->bottomRight, width, height);

	//Conditionally render window buttons
	if (canEscape) renderGuiTexture(gui->close, (width - 25), 10);

	//title
	SDL_Rect titleRect;
	titleRect.x = x + ((width / 2) - (titleLabel->fontRect.w / 2));
	titleRect.y = y + 8;
	titleRect.w = titleLabel->fontRect.w;
	titleRect.h = titleLabel->fontRect.h;
	SDL_RenderCopy(renderer, titleLabel->fontTexture, NULL, &titleRect);

	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->render();
	}
}


void SubForm::renderGuiTexture(Texture* guiTexture, int posX, int posY) {
	SDL_Rect srcRect = getSrcRect(guiTexture, posX, posY);
	SDL_Rect dstRect = getDstRect(guiTexture, posX, posY);
	SDL_RenderCopy(renderer, guiTexture->texture, &srcRect, &dstRect);
}


SDL_Rect SubForm::getSrcRect(Texture* guiTexture, int atX, int atY) {
	SDL_Rect srcRect;
	srcRect.x = 0;
	srcRect.y = 0;

	srcRect.w = guiTexture->width;
	if (srcRect.w > width) srcRect.w = width;

	srcRect.h = guiTexture->height;
	if (srcRect.h > height) srcRect.h = height;
	return srcRect;
}


SDL_Rect SubForm::getDstRect(Texture* guiTexture, int atX, int atY) {
	atX += x;
	atY += y;

	SDL_Rect dstRect{ 0,0,0,0 };

	//if (guiTexture->anchor == Anchor::TOP_RIGHT || guiTexture->anchor == Anchor::BOTTOM_RIGHT) dstRect.x = atX - guiTexture->width;
	//else dstRect.x = atX;

	//if (guiTexture->anchor == Anchor::BOTTOM_LEFT || guiTexture->anchor == Anchor::BOTTOM_RIGHT) dstRect.y = atY - guiTexture->height;
	//else dstRect.y = atY;

	//dstRect.w = guiTexture->width;
	//if (dstRect.x + dstRect.w > dstRect.x + width) dstRect.w = width;

	//dstRect.h = guiTexture->height;
	//if (dstRect.y + dstRect.h > dstRect.y + height) dstRect.h = height;
	return dstRect;
}


bool SubForm::handleEvent(SDL_Event* e) {
	for (auto widget : widgetsByPriority) {
		if (widget->handleEvent(e)) {
			if (widget->widgetType == wtField) {
				for (auto altWidget : widgetsByPriority) {
					if (altWidget != widget && altWidget->widgetType == wtField && ((Field*)altWidget)->isFocused()) ((Field*)altWidget)->clearFocus();
				}
			}
			return true;
		}
	}

	int mx, my;

	if (e->type == SDL_MOUSEMOTION) {
		//Dragging above window doesn't register mouse motion as it isn't in window area. Fix?
		if (isWindowDragging) {
			SDL_GetMouseState(&mx, &my);

			int newX, newY;
			newX = mx - drag_start_x;
			if (newX < 0) newX = 0;
			if (newX + width > gClient.windowWidth) newX = gClient.windowWidth - width;

			newY = my - drag_start_y;
			if (newY < 0) newY = 0;
			if (newY + height > gClient.windowHeight) newY = gClient.windowHeight - height;

			int changeX = newX - x;
			int changeY = newY - y;
			setPosition(newX, newY);

			for (int i = 0; i < widgets.size(); i++) {
				widgets[i]->offsetPosition(SDL_Point{ changeX, changeY });
			}

			return true;
		}
	}

	//If mouse event happened
	if (e->type == SDL_MOUSEBUTTONUP)
	{
		//End Drag
		if (isWindowDragging) {
			isWindowDragging = false;
			return true;
		}

		SDL_GetMouseState(&mx, &my);

		if (isClosePressed) {
			if (doesPointIntersect(closeRect, mx, my)) {
				//global quit flag only applicable on certain subforms. to be moved!
				/*SDL_Event quitEvent;
				quitEvent.type = SDL_QUIT;
				SDL_PushEvent(&quitEvent);*/
				closeWindow = true;
			}
			isClosePressed = false;
			return true;
		}
	}

	if (e->type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&mx, &my);

		if (canEscape && doesPointIntersect(closeRect, mx, my)) {
			isClosePressed = true;
			return true;
		}

		//Initiate Drag
		if (canDrag && doesPointIntersect(topRect, mx, my)) {
			if (!isWindowDragging) {
				isWindowDragging = true;
				drag_start_x = mx - x;
				drag_start_y = my - y;
				return true;
			}
		}
	}

	return false;
}


void SubForm::handleWidgetEvent() {
	//
}


void SubForm::setPosition(int posX, int posY) {
	x = posX;
	y = posY;
	topRect = { x, y, gui->topCenter_s->width, gui->topCenter_s->height };
	closeRect = { x + width - 25, y + 10, gui->close->width, gui->close->height };
}


void SubForm::setVisible(bool isVisible) {
	visible = isVisible;
}


void SubForm::addWidget(Widget* widget) {
	widgets.push_back(widget);
	//LessThanByDepth
	std::sort(widgets.begin(), widgets.end(), LessThanByDepth());

	widgetsByPriority.push_back(widget);
	std::sort(widgetsByPriority.begin(), widgetsByPriority.end());
}


void SubForm::removeWidget(Widget* widget) {
	for (int i = 0; i < widgets.size(); i++) {
		if (widgets[i] == widget) {
			widgets.erase(widgets.begin() + i);
			break;
		}
	}

	for (int i = 0; i < widgetsByPriority.size(); i++) {
		if (widgetsByPriority[i] == widget) {
			widgetsByPriority.erase(widgetsByPriority.begin() + i);
			break;
		}
	}
}


std::string SubForm::getName() {
	return name;
}