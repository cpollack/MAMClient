#pragma once

#include "Window.h"

class CLabel;

class CPromptForm : public CWindow {
public:
	CPromptForm();
	CPromptForm(bool allowCancel);

	void SetMessage(std::string message);
	void SetType(int type);

private: //Event handlers
	void btnOk_Click(SDL_Event& e);
	void btnCancel_Click(SDL_Event& e);

private:
	CLabel *lblMessage;
	int promptType;
};