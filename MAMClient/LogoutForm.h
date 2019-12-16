#pragma once

#include "Window.h"

class CLogoutForm : public CWindow {
public:
	CLogoutForm();

	void btnOk_Click(SDL_Event& e);
	void btnCancel_Click(SDL_Event& e);
	void btnHelp_Click(SDL_Event& e);
};