#pragma once

#include "Window.h"

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

class pLoginResponse;
class CMessageForm;
class CField;

class CLoginForm : public CWindow {
public:
	CLoginForm();
	~CLoginForm();

	void handleEvent(SDL_Event &e);
	void step();

private: //Login Handling
	void handleLogin();
	void closeMessage();
	void promptError(std::string errorText);
public: void loginSuccess();

private:
	LoginState loginState = lsNone;
	bool loginResponseReceived;
	int loginAccount, loginSeed;
	std::string loginMessage;
	CMessageForm* messageForm;

private: //Event handlers
	void lblNewAccount_Click(SDL_Event& e);
	void fldPassword_Submit(SDL_Event &e);
	void btnOk_Click(SDL_Event& e);
	void btnCancel_Click(SDL_Event& e);

	CField* fldAccount;
	CField* fldPassword;
};