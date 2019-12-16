#ifndef __FORMSTRUCT_H
#define __FORMSTRUCT_H

#include <SDL.h>
#include "GUI.h"

typedef struct FormStruct {
	SDL_Window* window;
	SDL_Renderer* renderer = NULL;
	int width;
	int height;
	~FormStruct() {
		SDL_DestroyWindow(window);
		window = NULL;
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
}FormStruct; 

#endif
