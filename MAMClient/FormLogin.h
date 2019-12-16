#ifndef __FORMLOGIN_H
#define __FORMLOGIN_H

#include "SubForm.h"

enum LoginState {
	lsNone,
	lsStart,
	lsConnectAccount,
	lsLogin,
	lsLoginResponse,
	lsLoginFail,
	lsLoginSuccess,
	lsConnectGame,
	lsGameResponse,
	lsGameFail,
	lsGameSuccess,
	lsLoggedIn
};

class FormMessage;

class FormLogin : public SubForm {
private:
	FormMessage* detailsForm = nullptr;

	bool loginResponseReceived = false;
	bool firstLogin = true;
	int loginAccount, loginSeed;
	std::string loginMessage;

	void FormLogin::handleLogin();
	void FormLogin::closeMessage();
	void FormLogin::promptError(std::string errorText);

public:
	Label *lblAccount = nullptr, *lblPassword = nullptr, *lblNewAccount = nullptr, *lblClearAcct = nullptr;
	Field *login = nullptr, *password = nullptr;
	Button *btnSubmit = nullptr, *btnCancel = nullptr, *btnHelp = nullptr;
	//CheckBox *cbMemorize = nullptr;

	int loginState = lsNone;

	FormLogin(std::string title, int xPos, int yPos);
	~FormLogin();

	void FormLogin::render();
	bool FormLogin::handleEvent(SDL_Event* e);
	void FormLogin::handleWidgetEvent();

	void FormLogin::handleLoginResponse(pLoginResponse* packet);
};

#endif