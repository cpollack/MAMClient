#include "stdafx.h"

#include "Client.h"
#include "UserManager.h"
#include "AssetManager.h"
#include "GUI.h"
#include "Player.h"
#include "GameMap.h"
#include "Battle.h"
#include "MainWindow.h"
#include "Window.h"

#include "Chat.h"
#include "MessageManager.h"

#include "Global.h"
#include "CustomEvents.h"

//Temp
#include "TestForm.h"

#define FRAMES_PER_SEC 60

bool init();
void close();
CWindow* getTopmost();

//Move these to a Global.h
Client gClient;
UserManager userManager;
AssetManager assetManager;
GUI* gui;
Player* player = nullptr;
GameMap *map = nullptr;
Battle *battle = nullptr;
CMainWindow* mainForm;
CChat* chat;
MessageManager messageManager;

CWindow* mainWindow;
std::vector<CWindow*> Windows;
std::vector<CWindow*> QueueWindows;
void *focusedWindow;

int main(int argc, char *args[]) {
	setlocale(LC_ALL, "");

	//Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
		close();
		return -1;
	}	

	FPSmanager fpsManager;
	SDL_initFramerate(&fpsManager);
	SDL_setFramerate(&fpsManager, FRAMES_PER_SEC);

	//chat = new CChat(); initialized in formMain
	mainForm = new CMainWindow();
	mainWindow = mainForm;
	focusedWindow = mainForm;

	//CTestForm* testForm = new CTestForm();
	//Windows.push_back(testForm);

	bool quit = false;
	CWindow *topmost;
	bool lastFocusLost = false;
	int lastFocus = -1;
	Uint32 tickLength = 0;
	Uint32 tenSec = 0;
	std::vector<Uint32> ticks;
	SDL_Event e;

	#define GLOBAL_CATCH
#ifdef GLOBAL_CATCH
	try {
#endif
		while (!quit) {
			Uint32 thisTick = SDL_GetTicks();
			gClient.handlePackets();

			// Check for window close at the top of the stack
			while (Windows.size() > 0) {
				CWindow *nextWindow = Windows.back();
				if (nextWindow->CloseWindow) {
					delete nextWindow;
					Windows.pop_back();
					lastFocusLost = true;
				}
				else break;
			}
			while (QueueWindows.size() > 0) {
				Windows.push_back(QueueWindows.front());
				QueueWindows.erase(QueueWindows.begin());
			}

			topmost = getTopmost();

			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				//User requests quit
				if (e.type == SDL_QUIT)
				{
					if(bExitApplication) quit = true;
#ifdef NEWGUI
					else {
						mainForm->btnClose_Click(e);
					}
#endif
				}

				//Process Window Events for all Forms
				if (e.type == SDL_WINDOWEVENT) {
					if (e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
						//std::cout << "GAINED " << e.window.windowID << std::endl;
						if (lastFocusLost && e.window.windowID != getTopmost()->GetWindowID()) {
							lastFocus = e.window.windowID;

							//raise static window references here
							//ChatForm->raise(); etc

							//Window stack should be raised in order
							for (auto window : Windows) {
								if (window->CloseWindow) continue;
								window->raise();
								lastFocus = window->GetWindowID();
							}
						}
						lastFocusLost = false;
					}

					if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST && e.window.windowID == getTopmost()->GetWindowID()) {
						std::cout << "LOST " << e.window.windowID << std::endl;
						lastFocusLost = true;
					}
					else {
						mainForm->handleEvent(e);
						if (Windows.size()) getTopmost()->handleEvent(e);
						/*for (auto window : Windows) {
							window->handleEvent(e);
						}*/
					}
				}
				//Non-Window events
				else {
					if (e.type == SDL_RENDER_TARGETS_RESET || e.type == SDL_RENDER_DEVICE_RESET) {
						int a = 1;
						mainForm->ReloadAssets();
						for (auto window : Windows) window->ReloadAssets();
					}

					//Process user events for all windows in reverse
					if (e.type == SDL_USEREVENT || e.type >= FIRSTCUSTOMEVENT) {
						for (int i = Windows.size() - 1; i >= 0; i--) {
							Windows[i]->handleEvent(e);
						}
						mainForm->handleEvent(e);
					}
					else {
						//Windows in stack disable all other events for all windows except topmost
						if (Windows.size() > 0) {
							Windows[Windows.size() - 1]->handleEvent(e); //only the last window in the stack gets focus
						}
						else {
							//Nothing is in the stack, so handle static windows as appropriate
							if (focusedWindow == mainForm) mainForm->handleEvent(e);
							else {
								//Process only hotkeys for the main form and process full events for focused form
								((CWindow*)focusedWindow)->handleEvent(e);
							}
						}
					}
				}
			}
			mainForm->step();
			int size = Windows.size();
			for (int i = 0; i < size; i++) {
				Windows[i]->step();
			}

			//Render texture to screen
			mainForm->render();
			mainForm->renderPresent();
			for (auto window : Windows) {
				window->render();
				window->renderPresent();
			}

			SDL_framerateDelay(&fpsManager);

			//Uint32 afterRender = SDL_GetTicks();
			//Uint32 tickElapsed = afterRender - thisTick;
			//double timeNeeded = 1000.0 / FRAMES_PER_SEC;
			//if (timeNeeded > tickElapsed) SDL_Delay(timeNeeded - tickElapsed);
			//std::cout << "Tick Elapsed: " << tickElapsed << std::endl;

			Uint32 now = SDL_GetTicks();
			tickLength = now - thisTick;

			ticks.push_back(tickLength);
			if (ticks.size() > 100) ticks.erase(ticks.begin());
			AverageTickLength = 0;
			for (int i = 0; i < ticks.size(); i++) AverageTickLength += ticks[i];
			AverageTickLength *= 1.0;
			AverageTickLength /= ticks.size();
			/*if (thisTick - tenSec >= 5000) {
				gClient.addToDebugLog("Average Ticks: " + std::to_string(AverageTickLength));
				tenSec = thisTick;
			}*/
		}
#ifdef GLOBAL_CATCH
	}
	catch (std::bad_alloc) {
		showErrorMessage("The application has experience a Bad Alloc Error in an unknown location, and will now shut down.");
	}
	catch (std::out_of_range) {
		showErrorMessage("The application has experience an Out of Range Error in an unknown location, and will now shut down.");
	}
	catch (std::logic_error) {
		showErrorMessage("The application has experience a Logic Error in an unknown location, and will now shut down.");
	}
	catch (std::range_error) {
		showErrorMessage("The application has experience an Invalid Range Access Error in an unknown location, and will now shut down.");
	}
	catch (std::runtime_error) {
		showErrorMessage("The application has experience a Runtime in an unknown location, and will now shut down.");
	}
	catch (...) {
		showErrorMessage("The application has experience an unknown fault, and will now shut down.");
	}
#endif

	//Free resources and close SDL
	gClient.shutdownThread();
	
	for (auto window : Windows) {
		delete window;
	}
	delete mainForm;

	close();

	return 1;
}


bool init() {
	//Initialization flag
	bool success = true;

	SDL_setenv("SDL_AUDIODRIVER", "directsound", true);

	//Initialize SDL
	if (SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER ) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
		//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

		//Initialize PNG loading
		int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(imgFlags) & imgFlags))
		{
			printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
			success = false;
		}

		//Initialize SDL_ttf
		if (TTF_Init() == -1) {
			printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
			success = false;
		}

		//Initialize the Audio Device
		SDL_AudioSpec desired_spec;
		desired_spec.freq = 44100;
		desired_spec.format = AUDIO_S16SYS;
		desired_spec.channels = 2;
		desired_spec.samples = 1024;
		desired_spec.callback = NULL;

		audioDevice = SDL_OpenAudioDevice(NULL, 0, &desired_spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
		if (audioDevice < 0) {
			fprintf(stderr, "SDL_OpenAudioDevice: %s\n", SDL_GetError());
			success = false;
		}
		else SDL_PauseAudioDevice(audioDevice, 0);
	}

	return success;
}

CWindow* getTopmost() {
	CWindow* topmost = mainForm;
	if (Windows.size()) topmost = Windows[Windows.size() - 1];
	return topmost;
}


void close()
{
	//Quit SDL subsystems
	SDL_Quit();
}
