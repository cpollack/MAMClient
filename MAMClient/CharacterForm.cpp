#include "stdafx.h"
#include "CharacterForm.h"

#include "Player.h"

#include "Texture.h"
#include "Sprite.h"
#include "Label.h"
#include "ImageBox.h"

CCharacterForm::CCharacterForm() : CWindow("CharacterForm.JSON") {
	Type = FT_CHARACTER;

	//registerEvent("btnOk", "Click", std::bind(&CLogoutForm::btnOk_Click, this, std::placeholders::_1));

	HookWidgets();
	LoadPortrait();
	LoadSprite();
}

void CCharacterForm::HookWidgets() {
	imgPortrait = (CImageBox*)GetWidget("imgPortrait");
	imgSprite = (CImageBox*)GetWidget("imgSprite");
	lblName = (CLabel*)GetWidget("lblName");
	lblLevelRank = (CLabel*)GetWidget("lblLevelRank");
	lblMainCult = (CLabel*)GetWidget("lblMainCult");
	lblMainLife = (CLabel*)GetWidget("lblMainLife");
	lblMainAttack = (CLabel*)GetWidget("lblMainAttack");
	lblMainDefence = (CLabel*)GetWidget("lblMainDefence");
	lblMainDexterity = (CLabel*)GetWidget("lblMainDexterity");
}

void CCharacterForm::LoadPortrait() {
	int face = player->GetFace();
	std::string path = "GUI\\player_icon\\";
	if (player->GetGender() == MALE) path += "Man0";
	else path += "Woman0";
	path += std::to_string(face) + ".bmp";
	Texture *portrait = new Texture(renderer, path);
	imgPortrait->SetImage(portrait);
}

void CCharacterForm::LoadSprite() {
	std::string role = getRoleFromLook(player->GetLook());
	int animation = StandBy;
	int direction = 0;
	sprite = new Sprite(renderer, getSpriteFramesFromAni(role, animation, direction), stCharacter);
	sprite->start();

	imgSprite->BindSprite(sprite);
}

void CCharacterForm::LoadOverviewTab() {
	lblName->SetText(player->getName());
	std::string sLevRank = formatInt(player->getLevel()) + " " + player->getRankText();
	lblLevelRank;
	lblMainCult;
	lblMainLife;
	lblMainAttack;
	lblMainDefence;
	lblMainDexterity;
}

void CCharacterForm::LoadAttributeTab() {

}

void CCharacterForm::LoadAdditionalTab() {

}

/*void CCharacterForm::btnOk_Click(SDL_Event& e) {
	//Force an application close
	applicationClose();
}*/
