#ifndef __LISTVIEW_H
#define __LISTVIEW_H

#include "Widget.h"

class Label;

struct lvRow {
	int guid;
	Label* label = nullptr;
};

class ListView : public Widget {
public:
	ListView(int tox, int toy, int w, int h);
	~ListView();

	void ListView::render();
	bool ListView::handleEvent(SDL_Event* e);

	void ListView::setRowWidth(int rw);
	void ListView::setRowHeight(int rh);
	void ListView::addRow(int guid, std::string desc);

	bool ListView::didSelectionChange();
	bool ListView::wasDoubleClicked();
	int ListView::getSelection();

private:
	SDL_Texture* renderTexture;
	SDL_Rect renderRect;
	SDL_Color bgColor;
	
	SDL_Texture* focusTexture = NULL;
	int focusRow = -1;
	int focusId = -1;
	bool selectionChange = false;
	int selectionRow = -1;
	int selectionId = -1;

	std::vector<lvRow*> rows;
	int rowWidth;
	int rowHeight;

	bool clicked = false;
	bool doubleClicked = false;
};

#endif
