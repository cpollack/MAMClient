#include "stdafx.h"
#include "LoginForm.h"
#include "Global.h"
#include "CustomEvents.h"

#include "Client.h"
#include "Button.h"
#include "ImageBox.h"
#include "Field.h"

#include "pLoginResponse.h"
#include "MessageForm.h"
#include "PromptForm.h"

CLoginForm::CLoginForm() : CWindow("LoginForm.JSON") {
	Type = FT_LOGIN;
	SetTitle("Login - " + std::string(version) + " " + std::string(versionDate));
	std::string randPic = "login_minpic0" + std::to_string((rand() % 4) + 1) + ".jpg";
	CImageBox *imgFace = (CImageBox *)GetWidget("imgFace");
	if (imgFace) imgFace->SetImageFromSkin(randPic);

	registerEvent("lblNewAccount", "Click", std::bind(&CLoginForm::lblNewAccount_Click, this, std::placeholders::_1));
	registerEvent("fldAccount", "Submit", std::bind(&CLoginForm::fldAccount_Submit, this, std::placeholders::_1));
	registerEvent("fldAccount", "OnTab", std::bind(&CLoginForm::fldAccount_OnTab, this, std::placeholders::_1));
	registerEvent("fldPassword", "Submit", std::bind(&CLoginForm::fldPassword_Submit, this, std::placeholders::_1));
	registerEvent("btnOK", "Click", std::bind(&CLoginForm::btnOk_Click, this, std::placeholders::_1));
	registerEvent("btnCancel", "Click", std::bind(&CLoginForm::btnCancel_Click, this, std::placeholders::_1));

	fldAccount = (CField*)GetWidget("fldAccount");
	fldPassword = (CField*)GetWidget("fldPassword");

	//Load remembered login from config
	std::string defaultLogin = "";
	gClient.configIni->getEntry("DefaultLogin", &defaultLogin);
	if (defaultLogin.length()) {
		CField * fldAccount = (CField *)GetWidget("fldAccount");
		fldAccount->SetText(defaultLogin);
		FocusWidget("fldPassword");
	}
	else FocusWidget("fldAccount");
}

CLoginForm::~CLoginForm() {
	//Update config with changes to remember account settings
	gClient.configIni->writeToFile();
}

void CLoginForm::handleEvent(SDL_Event &e) {
	CWindow::handleEvent(e);

	if (e.window.windowID != windowID) return;
	if (e.type == CUSTOMEVENT_WINDOW) {
		if (e.user.code == WINDOW_CLOSE && e.user.data1 == messageForm) {
			messageForm = nullptr;
			loginState = lsNone;
		}
	}
}

void CLoginForm::step() {
	CWindow::step();

	handleLogin();
}

void CLoginForm::lblNewAccount_Click(SDL_Event& e) {
	//launch browser link to website
	//url should be stored in an ini

	//ShellExecute(0, 0, "http://www.google.com", 0, 0, SW_SHOW);
}


void CLoginForm::fldAccount_Submit(SDL_Event &e) {
	if (fldPassword->GetText().length() == 0) FocusWidget("fldPassword");
	else btnOk_Click(e);
}

void CLoginForm::fldAccount_OnTab(SDL_Event &e) {
	FocusWidget("fldPassword");
}

void CLoginForm::fldPassword_Submit(SDL_Event &e) {
	btnOk_Click(e);
}


void CLoginForm::btnOk_Click(SDL_Event& e) {
	//validate
	if (fldAccount->GetText().length() == 0) {
		doPromptError(this, "Login Error", "Account cannot be empty.");
		return;
	}
	if (fldPassword->GetText().length() == 0) {
		doPromptError(this, "Login Error", "Password cannot be empty.");
		return;
	}

	//start login process
	loginState = lsStart;
}

void CLoginForm::btnCancel_Click(SDL_Event& e) {
	//CloseWindow = true;

	//Force an application close
	applicationClose();
}

void CLoginForm::handleLogin() {
	pLoginResponse* clientResp;

	//Compare to MAM_Client sli project to see if more features were added.

	switch (loginState) {
	case lsNone:
		break;

	case lsStart:
		gClient.clearLoginResponse();

		messageForm = new CMessageForm();
		messageForm->SetTitle("Logging In");
		messageForm->SetMessage("Connecting to the Account Server.");
		messageForm->SetUseClose(true);
		Windows.push_back(messageForm);

		loginState = lsConnectAccount;

		gClient.addToDebugLog("Beginning Login");
		break;

	case lsConnectAccount:
		if (gClient.connectAccountServer()) {
			loginState = lsLogin;
			messageForm->SetMessage("Logging in...");
			gClient.addToDebugLog("Connected to Account Server");
		}
		else {
			loginState = lsNone;
			closeMessage();
			promptError("Could not connect to the Account Server. Please try again later.");
		}
		break;

	case lsLogin:
		//server should not be hardcoded

#ifndef NEWSERVER
#ifndef DEVSERVER
		gClient.login(fldAccount->GetText(), fldPassword->GetText(), "MythOfOrient");
#else
		gClient.login(fldAccount->GetText(), fldPassword->GetText(), "Dev");
#endif // !DEVSERVER
#else
		gClient.login(fldAccount->GetText(), fldPassword->GetText(), "HeavenAscent");
#endif // !NEWSERVER
		

		//if (cbMemorize->isChecked()) {
		gClient.configIni->setEntry("DefaultLogin", fldAccount->GetText());
		//}

		loginState = lsLoginResponse;
		break;

	case lsLoginResponse:
		//wait for response data
		loginResponseReceived = gClient.getLoginResponse(loginAccount, loginSeed, loginMessage);
		if (loginResponseReceived) {
			if (loginAccount == 0) {
				loginState = lsLoginFail;
			}
			else {
				loginState = lsLoginSuccess;
			}
			loginResponseReceived = false;
		}
		break;

	case lsLoginSuccess:
		loginState = lsConnectGame;
		messageForm->SetMessage("Connecting to the Game Server.");
		break;

	case lsLoginFail:
		loginState = lsNone;
		closeMessage();

		if (loginMessage == "帐号名或口令错") loginMessage = "Invalid Username or Password.";
		promptError(loginMessage);

		gClient.shutdownSocket();

		//帐号名或口令错 - Invalid Username or Password
		break;

	case lsConnectGame:
		gClient.shutdownSocket();
		if (gClient.connectGameServer()) {
			loginState = lsGameResponse;
			messageForm->SetMessage("Loading...");

			clientResp = new pLoginResponse(loginAccount, loginSeed);
			gClient.addPacket(clientResp);

			gClient.addToDebugLog("Connected to the Game Server");
		}
		else {
			loginState = lsNone;
			closeMessage();
			promptError("Could not connect to the Game Server. Please try again later.");
			gClient.shutdownSocket();
		}
		break;

	case lsGameResponse:
		//Wait for login process to complete and load
		break;
	case lsGameFail:
		break;
	case lsGameSuccess:
		break;
	case lsLoggedIn:
		closeMessage();
		CloseWindow = true;
		break;

	}
}

void CLoginForm::closeMessage() {
	if (messageForm) {
		messageForm->CloseWindow = true;
	}
}


void CLoginForm::promptError(std::string errorText) {
	CPromptForm *prompt = new CPromptForm();
	prompt->SetTitle("Login Error");
	prompt->SetMessage(errorText);
	prompt->SetParent(this);
	Windows.push_back(prompt);
}

void CLoginForm::loginSuccess() {
	loginState = lsLoggedIn;
	gClient.addToDebugLog("Login Success, loading game");
}