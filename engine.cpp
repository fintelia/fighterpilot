
#include "engine.h"
#include <Windows.h>
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														DEFINITIONS																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
bool active=true;					// Window Active Flag																						//	//
																																				//	//
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;										//	//
int sh=1024;																																	//	//
int sw=1280;																																	//	//
float sAspect=((float)sw)/sh;																													//	//
																																				//  //
profiler Profiler;																																//	//
																																				//	//
InputManager& input=InputManager::getInstance();																								//	//
gui::manager& menuManager = gui::manager::getInstance();																						//	//
DataManager& dataManager = DataManager::getInstance();																							//	//
WorldManager& world = WorldManager::getInstance();																								//	//
CollisionChecker& collisionCheck = CollisionChecker::getInstance();																				//	//
GraphicsManager* graphics = OpenGLgraphics::getInstance();																						//	//
FileManager& fileManager = FileManager::getInstance();																							//  //
SceneManager& sceneManager = SceneManager::getInstance();																						//  //
SettingsManager& settings = SettingsManager::getInstance();																						//  //
//ControlManager& controlManager = ControlManager::getInstance();
PlayerManager& players = PlayerManager::getInstance();
bool done=false;//exits program																													//	//
																																				//	//
bool lowQuality;																																//	//
//humanControl players[NumPlayers];																												//	//
																																				//	//
																																				//	//
int frame=0,Time,timebase=0;																													//	//
float fps;																																		//	//
planeType defaultPlane;																															//	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//																																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShowHideTaskBar(bool bHide)
{
	// find taskbar window
	HWND pWnd = FindWindow(L"Shell_TrayWnd",L"");
	if(!pWnd)	return;
	if(bHide)	ShowWindow(pWnd,SW_HIDE);
	else		ShowWindow(pWnd,SW_SHOW);
}
void minimizeWindow()
{
	graphics->minimizeWindow();
}
LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_SHOWWINDOW:
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=true;						// Program Is Active
			}
			else
			{
				active=false;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
					return 0;						// Prevent From Happening
			}
			break;									// Exit
		}
		//case WM_MOVE:
		//case WM_MOVING:
		//case WM_WINDOWPOSCHANGED:
		//case WM_WINDOWPOSCHANGING:
		//	return 0;

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}
		case WM_ACTIVATEAPP:
		{
			//bool wActive = LOWORD(wParam) != 0;
			//bool wMinimized = HIWORD(wParam) != 0;
			world.time.setPaused(!(wParam != 0));
			active = wParam != 0;
			return 0;
		}
		case WM_KEYDOWN:							// Is A Key Being Held Down?
		case WM_KEYUP:								// Has A Key Been Released?
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
			input.windowsInput(uMsg,wParam,lParam);
			return 0;
		case WM_ERASEBKGND:									// Check To See If Windows Is Trying To Erase The Background
			return 0;										// either return 0 or 1, does not seem to be much of a difference...
		case WM_SIZE:										// Resize The OpenGL Window
			if(wParam != SIZE_MINIMIZED)
				graphics->resize(LOWORD(lParam),HIWORD(lParam));
			return 0;										// Jump Back
	//	case WM_APPCOMMAND:
	//	case WM_SYSKEYDOWN:
		case WM_HOTKEY:
		{
			if(active && (wParam == IDHOT_SNAPDESKTOP || wParam == IDHOT_SNAPWINDOW))
			{
				graphics->takeScreenshot();
				return 0;
			}
			break;
		}
		case WM_DEVICECHANGE:
			if(wParam == 0x007) //DBT_DEVNODES_CHANGED
			{
				input.checkNewHardware();
				return 0;
			}
			break;
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
void outOfMemory()
{
	MessageBox(NULL,L"Out of Memory. Fighter-Pilot must now close.",L"Error",MB_ICONEXCLAMATION);
	exit(EXIT_FAILURE);
}

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure

	set_new_handler(outOfMemory);

	if(!game->init())
	{
		return 1;
	}

	auto f = fileManager.loadZipFile("r.zip");
	f->filename = "document2.zip";
	fileManager.writeZipFile(f);

	float nextUpdate=0;
	float swapTime=0.0;
	float time=0.0;
	while(!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				done=true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			game->update();

			if(active)
			{
				graphics->render();

				///timing code
				float waitTime = (nextUpdate - max(swapTime,0)) - GetTime();
				if(waitTime>1.0)	sleep(waitTime-1);
				do{
					time = GetTime();
				}while(time < nextUpdate - max(swapTime,0));		
				nextUpdate = 1000.0/MAX_FPS + GetTime();
				//end timing code

				graphics->swapBuffers();

				//more timing code:  
				swapTime =  time - GetTime();
			}
			else
			{
				sleep(10);
			}
		}
	}
	world.destroy();
	menuManager.shutdown();
	dataManager.shutdown();
	graphics->destroyWindow();
	return 0;
}
