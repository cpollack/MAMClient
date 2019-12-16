#include "stdafx.h"
#include "ListView.h"
#include "Label.h"


ListView::ListView(int tox, int toy, int w, int h) : Widget(tox, toy) {
	width = w;
	rowWidth = width;
	height = h;
	bgColor = gui->backColor;

	renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);

	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
	SDL_RenderClear(renderer);


	SDL_SetRenderTarget(renderer, NULL);
}


ListView::~ListView() {
	for (auto row : rows) {
		delete row->label;
		delete row;
	}

	SDL_DestroyTexture(renderTexture);
	SDL_DestroyTexture(focusTexture);
}


void ListView::render() {
	//remake render texture
	SDL_SetRenderTarget(renderer, renderTexture);
	SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, 255);
	SDL_RenderClear(renderer);

	int curY = 0;
	for (auto row : rows) {
		row->label->setPosition(0, curY);
		row->label->render();
	
		if (row->guid == focusId) {
			SDL_Rect focusRect = { 0, curY, rowWidth, rowHeight };
			SDL_RenderCopy(renderer, focusTexture, NULL, &focusRect);
		}
		curY += rowHeight;
	}
	SDL_SetRenderTarget(renderer, NULL);

	//render it
	renderRect = { x,y,width,height };
	SDL_RenderCopy(renderer, renderTexture, NULL, &renderRect);
}


bool ListView::handleEvent(SDL_Event* e) {
	if (e->type == SDL_MOUSEBUTTONDOWN) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(SDL_Rect{ x,y,width,height }, mx, my)) {
			clicked = true;
		}

		doubleClicked = false;
	}

	if (e->type == SDL_MOUSEBUTTONUP) {
		int mx, my;
		SDL_GetMouseState(&mx, &my);

		if (doesPointIntersect(SDL_Rect{ x,y,width,height }, mx, my)) {
			clicked = false;

			int startY = my - y;
			int clickedRowNum = startY / rowHeight;
			if (clickedRowNum <= rows.size()) {
				lvRow* clickedRow = rows.at(clickedRowNum);
				selectionChange = true;
				selectionId = clickedRow->guid;
				selectionRow = clickedRowNum;
				focusId = clickedRow->guid;
				focusRow = clickedRowNum;
			}

			if (e->button.clicks >= 2) {
				doubleClicked = true;
			}

			return true;
		}

		clicked = false;
	}

	return false;
}


void ListView::setRowWidth(int rw) {
	rowWidth = rw;
}


void ListView::setRowHeight(int rh) {
	rowHeight = rh;

	focusTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, rowWidth, rowHeight);
	SDL_SetTextureBlendMode(focusTexture, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, focusTexture);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 64);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, NULL);
}


void ListView::addRow(int guid, std::string desc) {
	lvRow* row = new lvRow;
	row->guid = guid;
	row->label = new Label(desc, 0, 0);

	rows.push_back(row);
}


bool ListView::didSelectionChange() {
	bool sc = selectionChange;
	selectionChange = false;
	return sc;
}


bool ListView::wasDoubleClicked() {
	bool dc = doubleClicked;
	doubleClicked = false;
	return dc;
}


int ListView::getSelection() {
	return selectionId;
}