#pragma once

#include "Window.h"

class CLabel;

class CMessageForm : public CWindow {
public:
	CMessageForm();

	void SetMessage(std::string message);

private:
	CLabel *lblMessage;
};