#include "stdafx.h"
#include "CharacterForm.h"

CCharacterForm::CCharacterForm() : CWindow("CharacterForm.JSON") {
	Type = FT_CHARACTER;

	//registerEvent("btnOk", "Click", std::bind(&CLogoutForm::btnOk_Click, this, std::placeholders::_1));
}

/*void CCharacterForm::btnOk_Click(SDL_Event& e) {
	//Force an application close
	applicationClose();
}*/
