#include "stdafx.h"
#include "Form.h"

Form::Form(int w, int h) {
	width = w;
	height = h;

	window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	gClient.window = window;
	gClient.windowWidth = width;
	gClient.windowHeight = height;

	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		created = false;
		return;
	}

	SDL_SetWindowBordered(window, SDL_FALSE);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		created = false;
		return;
	}

	windowID = SDL_GetWindowID(window);
	//This needs to be set by Skin
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

	if (!gClient.renderer) gClient.renderer = renderer;

	if (!gui) gClient.initializeGUI();
}

Form::~Form() {
}

void Form::render() {
	SDL_RenderClear(renderer);

	if (formStyle == FormStyle::fsCustom) return;

	//All Rendering goes between clear/present

	//render all UI pieces in order
	if (isMainForm) SDL_RenderCopy(renderer, gui->topCenter->texture, NULL, &getDstRect(gui->topCenter, 0, 0));
	else SDL_RenderCopy(renderer, gui->topCenter_s->texture, NULL, &getDstRect(gui->topCenter_s, 0, 0));

	SDL_RenderCopy(renderer, gui->bottomCenter->texture, NULL, &getDstRect(gui->bottomCenter, 0, height));
	SDL_RenderCopy(renderer, gui->left->texture, NULL, &getDstRect(gui->left, 0, 0));
	SDL_RenderCopy(renderer, gui->right->texture, NULL, &getDstRect(gui->right, width, 0));

	if (isMainForm) {
		SDL_RenderCopy(renderer, gui->topLeft->texture, NULL, &getDstRect(gui->topLeft, 0, 0));
		SDL_RenderCopy(renderer, gui->topRight->texture, NULL, &getDstRect(gui->topRight, width, 0));
	}
	else {
		SDL_RenderCopy(renderer, gui->topLeft_s->texture, NULL, &getDstRect(gui->topLeft_s, 0, 0));
		SDL_RenderCopy(renderer, gui->topRight_s->texture, NULL, &getDstRect(gui->topRight_s, width, 0));
	}

	SDL_RenderCopy(renderer, gui->bottomLeft->texture, NULL, &getDstRect(gui->bottomLeft, 0, height));
	SDL_RenderCopy(renderer, gui->bottomRight->texture, NULL, &getDstRect(gui->bottomRight, width, height));

	//Specific to Main Form
	if (isMainForm) {
		SDL_RenderCopy(renderer, gui->mainWindow->texture, NULL, &getDstRect(gui->mainWindow, gui->left->width, gui->topCenter->height));
		SDL_RenderCopy(renderer, gui->surface->texture, NULL, &getDstRect(gui->surface, gui->left->width, (height - gui->bottomCenter->height)));
	}

	//Conditionally render window buttons
	if (canMinimize) SDL_RenderCopy(renderer, gui->min->texture, NULL, &getDstRect(gui->min, (width - 45), 10));
	if (canEscape) SDL_RenderCopy(renderer, gui->close->texture, NULL, &getDstRect(gui->close, (width - 25), 10));

	//SDL_RenderPresent(renderer);
}

void Form::handleEvent(SDL_Event& e) {
	//Todo:
	// - Minimize
	int x, y;

	/*if (e.type == SDL_WINDOWEVENT) {
		switch (e.window.event)
		{
			//Window appeared
		case SDL_WINDOWEVENT_SHOWN:
			std::cout << "Main shown\n";
			break;
			//Window disappeared
		case SDL_WINDOWEVENT_HIDDEN:
			std::cout << "Main hidden\n";
			break;
			//Repaint on expose
		case SDL_WINDOWEVENT_EXPOSED:
			std::cout << "Main exposed\n";
			break;
			//Mouse enter
		case SDL_WINDOWEVENT_ENTER:
			std::cout << "Main mouse enter\n";
			break;
			//Mouse exit
		case SDL_WINDOWEVENT_LEAVE:
			std::cout << "Main mouse leave\n";
			break;
			//Keyboard focus gained
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			std::cout << "Main keyboard focus gained\n";
			break;
			//Keyboard focus lost
		case SDL_WINDOWEVENT_FOCUS_LOST:
			std::cout << "Main keyboard focus lost\n";
			break;
			//Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			std::cout << "Main minimized\n";
			break;
			//Window restored
		case SDL_WINDOWEVENT_RESTORED:
			std::cout << "Main restored\n";
			break;
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			std::cout << "Main take focus\n";
			break;
		}
	}*/

	if (e.type == SDL_WINDOWEVENT) {
		if (e.window.event == SDL_WINDOWEVENT_RESTORED) {
			minimized = false;
		}
		if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			focused = false;
		}
		if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
			focused = true;
		}
		if (e.window.event == SDL_WINDOWEVENT_SHOWN) {
			focused = true;
		}
	}

	if (e.type == SDL_KEYDOWN) {
		//std::cout << "Main keypress\n";
	}


	if (e.type == SDL_MOUSEMOTION) {
		//Dragging above window doesn't register mouse motion as it isn't in window area. Fix?
		if (isWindowDragging) {
			SDL_GetGlobalMouseState(&x, &y);

			int newX, newY;
			newX = x - drag_start_x;
			newY = y - drag_start_y;
			SDL_SetWindowPosition(window, newX, newY);
			return;
		}
	}

	//If mouse event happened
	if (e.type == SDL_MOUSEBUTTONUP)
	{
		//End Drag
		if (isWindowDragging) {
			isWindowDragging = false;
			SDL_CaptureMouse(SDL_FALSE);
			return;
		}

		SDL_GetMouseState(&x, &y);

		if (isClosePressed ) {
			if (doesPointIntersect(gui->close->rect, x, y)) {
				//SDL_Event quitEvent;
				//quitEvent.type = SDL_QUIT;
				//SDL_PushEvent(&quitEvent);
				//FormExit* exitForm = new FormExit("Exit", "Exit", width / 2, height / 2);
				//addSubform("Exit", exitForm);
			}
			isClosePressed = false;
			return;
		}

		if (isMinimizePressed) {
			if (doesPointIntersect(gui->min->rect, x, y)) {
				SDL_MinimizeWindow(window);
				minimized = true;
			}
			isMinimizePressed = false;
			return;
		}
	}

	if (e.type == SDL_MOUSEBUTTONDOWN) {
		SDL_GetMouseState(&x, &y);

		if (canEscape && doesPointIntersect(gui->close->rect, x, y)) {
			isClosePressed = true;
			return;
		}

		if (canMinimize && doesPointIntersect(gui->min->rect, x, y)) {
			isMinimizePressed = true;
			return;
		}

		//Initiate Drag
		if ((isMainForm && doesPointIntersect(gui->topCenter->rect, x, y)) || (!isMainForm && doesPointIntersect(gui->topCenter_s->rect, x, y))) {
			bool intersectSubform = false;
			for (auto subform : subforms) {
				SDL_Rect sfRect = { subform->x, subform->y, subform->width, subform->height };
				if (doesPointIntersect(sfRect, x, y)) {
					intersectSubform = true;
					break;
				}
			}
			if (!isWindowDragging && !intersectSubform) {
				isWindowDragging = true;
				SDL_CaptureMouse(SDL_TRUE);
				int tX, tY;
				SDL_GetWindowPosition(window, &tX, &tY);
				SDL_GetGlobalMouseState(&drag_start_x, &drag_start_y);
				drag_start_x -= tX;
				drag_start_y -= tY;
				return;
			}
		}
	}
}


void Form::addSubform(std::string name, SubForm* subform) {
	subformMap[name] = subform;
	subforms.push_back(subform);
}


std::vector<SubForm*>::iterator Form::deleteSubform(std::string name) {
	SubForm* dSubForm = nullptr;

	std::vector<SubForm*>::iterator vit = subforms.begin();
	std::map<std::string, SubForm*>::iterator mit;
	mit = subformMap.find(name);
	if (mit != subformMap.end()) {
		dSubForm = mit->second;

		for (vit; vit != subforms.end(); vit++) {
			if (*vit == dSubForm) {
				vit = subforms.erase(vit);
				break;
			}
		}

		if (dSubForm) delete dSubForm;
		subformMap.erase(mit);
	}

	return vit;
}


SubForm* Form::getSubform(std::string name) {
	SubForm* dSubForm = nullptr;

	std::map<std::string, SubForm*>::iterator it;
	it = subformMap.find(name);
	if (it != subformMap.end()) {
		dSubForm = it->second;
	}

	return dSubForm;
}


SDL_Rect Form::getDstRect(Texture* aTexture, int x, int y) {
	//if (aTexture->anchor == Anchor::TOP_RIGHT || aTexture->anchor == Anchor::BOTTOM_RIGHT) aTexture->rect.x = x - aTexture->width;
	//else aTexture->rect.x = x;

	//if (aTexture->anchor == Anchor::BOTTOM_LEFT || aTexture->anchor == Anchor::BOTTOM_RIGHT) aTexture->rect.y = y - aTexture->height;
	//else aTexture->rect.y = y;

	aTexture->rect.w = aTexture->width;
	aTexture->rect.h = aTexture->height;
	return aTexture->rect;
}


void Form::setFormType(OldFormType ft) {
	formType = ft;
	if (ft == OldFormType::ftMain) isMainForm = true;
	else isMainForm = false;
}


OldFormType Form::getFormType() {
	return formType;
}

void Form::setFormStyle(FormStyle fs) {
	formStyle = fs;
}


void Form::setCanEscape(bool value) {
	canEscape = value;
}


void Form::setCanMinimize(bool value) {
	canMinimize = value;
}


void Form::createLabel(std::string name, std::string line, int toX, int toY, bool isBold) {
	Label *newLabel = new Label(line, toX, toY, isBold);
	labels[name] = newLabel;
	labelKeys.push_back(name);
}


Label* Form::getLabel(std::string name) {
	Label* findLabel = nullptr;
	if (labels.find(name) != labels.end()) {
		findLabel = labels[name];
	}
	return findLabel;
}