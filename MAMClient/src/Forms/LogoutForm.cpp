#include "stdafx.h"
#include "LogoutForm.h"

#include "GameLibrary.h"

CLogoutForm::CLogoutForm() : CWindow("LogoutForm.JSON") {
	Type = FT_LOGOUT;
	registerEvent("btnOk", "Click", std::bind(&CLogoutForm::btnOk_Click, this, std::placeholders::_1));
	registerEvent("btnCancel", "Click", std::bind(&CLogoutForm::btnCancel_Click, this, std::placeholders::_1));
	registerEvent("btnHelp", "Click", std::bind(&CLogoutForm::btnHelp_Click, this, std::placeholders::_1));
}

void CLogoutForm::btnOk_Click(SDL_Event& e) {
	//Force an application close
	applicationClose();
}

void CLogoutForm::btnCancel_Click(SDL_Event& e) {
	CloseWindow = true;
}

void CLogoutForm::btnHelp_Click(SDL_Event& e) {
	//std::cout << "LogoutForm Help Clicked\n";
}
