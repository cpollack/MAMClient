#include "stdafx.h"
#include "TestForm.h"
#include "Global.h"

#include <SDL_audio.h>

#include "Button.h"
#include "VideoFrame.h"

#include <functional>

CTestForm::CTestForm() : CWindow("TestForm.JSON") {
	//registerEvent("btnOk", "Click", std::bind(&CLogoutForm::btnOk_Click, this, std::placeholders::_1));

	std::string aviPath = "GUI\\select\\avi\\man01\\2.avi";

	CVideoFrame *vf = new CVideoFrame(this, "VideoFrame", 30, 30);
	//vf->SetRenderer(renderer);
	vf->SetWidth(500);
	vf->SetHeight(500);
	vf->SetVideo(aviPath);
	vf->SetRepeat(true);
	AddWidget(vf);
}

CTestForm::~CTestForm() {
  //
}

void CTestForm::render() {
	CWindow::render();
}

/*void CTestForm::btnOk_Click(SDL_Event& e) {
	//std::cout << "LogoutForm Ok Clicked\n";

	//Force an application close
	applicationClose()
}*/
