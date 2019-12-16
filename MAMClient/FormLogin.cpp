#include "stdafx.h"
#include "FormLogin.h"
#include "FormMain.h"
#include "FormMessage.h"
#include "FormPrompt.h"

#include "Window.h"
#include "LogoutForm.h"

#include "pLoginResponse.h"

FormLogin::FormLogin(std::string title, int xPos, int yPos) : SubForm("Login", title, 430, 210) {
	int offsetX = xPos - (430 / 2);
	int offsetY = yPos - (210 / 2);
	setPosition(offsetX, offsetY);

	lblAccount = new Label("Account:", x + 196, y + 66);
	lblAccount->setAlignment(laRight);
	lblAccount->setPriority(2);
	addWidget(lblAccount);
	lblPassword = new Label("Password:", x + 196, y + 100);
	lblPassword->setAlignment(laRight);
	lblPassword->setPriority(2);
	addWidget(lblPassword);
	lblNewAccount = new Label("New Account", x + 90, y + 120);
	lblNewAccount->setAlignment(laCenter);
	lblNewAccount->setUnderlinded(true);
	addWidget(lblNewAccount);

	std::string defaultLogin = "";
	gClient.configIni->getEntry("DefaultLogin", &defaultLogin);
	login = new Field(defaultLogin, x + 200, y + 62, 150, 24);
	login->setMaxSize(16);

	password = new Field("", x + 200, y + 98, 150, 24);
	password->setMaxSize(16);
	password->setMask(true);
	if (login->getValue() != "") password->setFocused();
	else login->setFocused();

	addWidget(login);
	addWidget(password);

	btnSubmit = new Button("Ok(&O)", x + 142, y + 174, 0, 0);
	btnSubmit->setPriority(1);
	btnCancel = new Button("Cancel(&C)", x + 218, y + 174, 0, 0);
	btnCancel->setPriority(1);
	btnHelp = new Button("Help(&H)", x + 294, y + 174, 0, 0);
	btnHelp->setPriority(1);
	addWidget(btnSubmit);
	addWidget(btnCancel);
	addWidget(btnHelp);

	//cbMemorize = new CheckBox("Memorize Account", x + 200, y + 140);
	//cbMemorize->setPriority(1);
	//if (!defaultLogin.empty()) cbMemorize->setChecked(true);
	//addWidget(cbMemorize);

	lblClearAcct = new Label("Clear Memorized Account", x + 232, y + 155);
	lblClearAcct->setUnderlinded(true);
	lblClearAcct->setPriority(1);
	addWidget(lblClearAcct);

	/*TEMP REMOVE*/
	password->setValue("Mznxbcv123");
	btnSubmit->pressed = true;
}


FormLogin::~FormLogin() {
	gClient.configIni->writeToFile();

	if (detailsForm) {
		formMain->deleteSubform("LoginDetails");
	}
}


void FormLogin::render() {
	if (!visible || detailsForm) {
		//detailsForm->render(); //handled by main
		return;
	}

	SubForm::render();

	SDL_Rect loginPicRect{x + 64, y + 61, gui->login_minpic->width, gui->login_minpic->height};
	SDL_RenderCopy(renderer, gui->login_minpic->texture, NULL, &loginPicRect);
}


bool FormLogin::handleEvent(SDL_Event* e) {
	if (SubForm::handleEvent(e)) return true;

	if (login->handleEvent(e)) return true;
	if (password->handleEvent(e)) return true;
}


void FormLogin::handleWidgetEvent() {
	SubForm* findForm = formMain->getSubform("LoginError");
	if (findForm) {
		if (((FormPrompt*)findForm)->submitted) {
			formMain->deleteSubform("LoginError");
			password->setValue("");
			password->setFocused();
		}
	}

	handleLogin();

	if (btnSubmit->wasPressed() || password->isSubmitted()) {
		loginState = lsStart;
	}

	if (btnCancel->wasPressed()) {
		//Instead of closing, should this go back to server select?
		SDL_Event quitEvent;
		quitEvent.type = SDL_QUIT;
		SDL_PushEvent(&quitEvent);
	}

	if (btnHelp->wasPressed()) {
		//redirect to login help on website
		//ShellExecute(0, 0, "http://www.google.com", 0, 0, SW_SHOW);

		CLogoutForm* test = new CLogoutForm();
		Windows.push_back(test);
	}

	if (lblNewAccount) {
		//link to website registration
		//ShellExecute(0, 0, "http://www.google.com", 0, 0, SW_SHOW);
	}

	if (lblClearAcct) {
		//clear memorized
		//gClient.configIni->setEntry("DefaultLogin", "");
	}
}


void FormLogin::handleLogin() {
	FormMessage* msgForm;
	pLoginResponse* clientResp;

	switch (loginState) {
	case lsNone:
		break;

	case lsStart:
		msgForm = new FormMessage("LoginDetails", "Logging In", formMain->width / 2, formMain->height / 2);
		msgForm->setMessage("Connecting to the Account Server.");
		formMain->addSubform("LoginDetails", msgForm);
		detailsForm = msgForm;
		loginState = lsConnectAccount;
		break;

	case lsConnectAccount:
		if (gClient.connectAccountServer()) {
			loginState = lsLogin;
			detailsForm->setMessage("Logging in...");
		}
		else {
			loginState = lsNone;
			closeMessage();
			promptError("Could not connect to the Account Server. Please try again later.");
		}
		break;

	case lsLogin:
		//server should not be hardcoded
		gClient.login(login->getValue(), password->getValue(), "MythOfOrient");
		//if (cbMemorize->isChecked()) {
			gClient.configIni->setEntry("DefaultLogin", login->getValue());
		//}
		loginState = lsLoginResponse;
		break;

	case lsLoginResponse:
		//wait for response data
		if (loginResponseReceived) {
			if (loginAccount == 0) {
				//std::cout << "Login Error: " << packet->phrase << std::endl;
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
		detailsForm->setMessage("Connecting to the Game Server.");
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
			loginState = lsLoggedIn;
			detailsForm->setMessage("Loading...");

			clientResp = new pLoginResponse(loginAccount, loginSeed);
			gClient.addPacket(clientResp);
		}
		else {
			loginState = lsNone;
			closeMessage();
			promptError("Could not connect to the Game Server. Please try again later.");
			gClient.shutdownSocket();
		}
		break;

	case lsGameResponse:
		break;
	case lsGameFail:
		break;
	case lsGameSuccess:
		break;
	case lsLoggedIn:
		break;

	}
}


void FormLogin::closeMessage() {
	SubForm* findForm = formMain->getSubform("LoginDetails");
	if (findForm) {
		formMain->deleteSubform("LoginDetails");
		detailsForm = nullptr;
	}
}


void FormLogin::promptError(std::string errorText) {
	FormPrompt *prompt = new FormPrompt("LoginError", "Are you sure?", formMain->width / 2, formMain->height / 2);
	prompt->setMessage(errorText);
	formMain->addSubform("LoginError", prompt);
}


void FormLogin::handleLoginResponse(pLoginResponse* packet) {
	loginResponseReceived = true;
	loginAccount = packet->accountID;
	loginSeed = packet->seed;
	loginMessage = (char*)packet->phrase;
}