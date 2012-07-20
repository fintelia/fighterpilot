
#include "engine.h"
#ifdef WINDOWS
#include <Windows.h>
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														DEFINITIONS																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//bool active=true;					// Window Active Flag																						//	//
																																				//	//
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;										//	//
int sh=1024;																																	//	//
int sw=1280;																																	//	//
float sAspect=((float)sw)/sh;																													//	//
																																				//  //
profiler Profiler;																																//	//
																																				//	//
Ephemeris& ephemeris=Ephemeris::getInstance();																									//  //
InputManager& input=InputManager::getInstance();																								//	//
gui::manager& menuManager = gui::manager::getInstance();																						//	//
DataManager& dataManager = DataManager::getInstance();																							//	//
AssetLoader& assetLoader = AssetLoader::getInstance();																							//  //
WorldManager& world = WorldManager::getInstance();																								//	//
CollisionChecker& collisionCheck = CollisionChecker::getInstance();																				//	//
GraphicsManager* graphics = OpenGLgraphics::getInstance();																						//	//
FileManager& fileManager = FileManager::getInstance();																							//  //
SceneManager& sceneManager = SceneManager::getInstance();																						//  //

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//																																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ShowHideTaskBar(bool bHide)
{
#ifdef WINDOWS
	// find taskbar window
	HWND pWnd = FindWindow(L"Shell_TrayWnd",L"");
	if(!pWnd)	return;
	if(bHide)	ShowWindow(pWnd,SW_HIDE);
	else		ShowWindow(pWnd,SW_SHOW);
#endif
}
void minimizeActiveWindow()
{
	graphics->minimizeWindow();
}
#ifdef WINDOWS
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
				game->active=true;						// Program Is Active
			}
			else
			{
				game->active=false;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			if(wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)// Screensaver Trying To Start or Monitor Trying To Enter Powersave?
				return 0;											//Prevent From Happening
			break;
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
			game->active = wParam != 0;
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
			if(game->active && (wParam == IDHOT_SNAPDESKTOP || wParam == IDHOT_SNAPWINDOW))
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
#endif
const char* emergencyMemory = new char[5 * 1024];
void outOfMemory()
{
	delete[] emergencyMemory; //free up enough memory to ensure that we can successfully display an error message
	emergencyMemory = nullptr;
#ifdef WINDOWS
	MessageBox(NULL,L"Out of Memory. Fighter-Pilot must now close.",L"Error",MB_ICONEXCLAMATION|MB_SYSTEMMODAL);
#endif
	exit(EXIT_FAILURE);
}
//#pragma comment (lib, "Urlmon.lib")
#ifdef WINDOWS
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG	msg; // Windows Message Structure
#else
int main(int argc, const char* argv[])
{
#endif

	set_new_handler(outOfMemory);

	srand((unsigned int)time(nullptr));
	randomGen.seed(time(nullptr));

	//URLDownloadToFileA(nullptr,
	//	"http://services.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/export/0/0/0.png?bbox=20037507%2C20037507%2C10037507%2C10037507&bboxSR=&layers=&layerdefs=&size=800%2C800&imageSR=&format=png24&transparent=false&dpi=&time=&layerTimeOptions=&f=image",
	//	"imagery.png", 0, nullptr);


	//char path[256];
	//_fullpath(path,nullptr, 256); //even though we get a pointer, windows handles the memory for us
	//string cmdLineStr(cmdLine);
	//if(!cmdLineStr.empty())
	//{
	//	cmdLineStr.erase(cmdLineStr.begin());//remove the first character of the string (a quote)
	//	cmdLineStr = cmdLineStr.substr(0,cmdLineStr.find_first_of("\""));
	//	cmdLineStr = cmdLineStr.substr(0,cmdLineStr.find_last_of("\\/"));
	//	SetCurrentDirectoryA(cmdLineStr.c_str());
	//}
	
#ifdef WINDOWS
    	string cmdLineString(lpCmdLine);
	boost::split(game->commandLineOptions, cmdLineString, boost::is_any_of(" "), boost::token_compress_on);
#else
	//TODO: parse argv
#endif
	if(!game->init())
	{
		return 1;
	}

	float nextUpdate=0;
	float swapTime=0.0;
	float time=0.0;
	while(!game->done)
	{
#ifdef WINDOWS
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
#else
	//TODO: handle linux input
#endif
		{
			game->update();

			if(game->active)
			{
				graphics->render();

				///timing code
				float waitTime = (nextUpdate - max(swapTime,0)) - GetTime();
				if(waitTime>1.0)	threadSleep(waitTime-1);
				do{
					time = GetTime();
				}while(time < nextUpdate - max(swapTime,0));
				nextUpdate = 1000.0/game->maxFrameRate + GetTime();
				//end timing code

				graphics->swapBuffers();

				//more timing code:
				swapTime =  time - GetTime();
			}
			else
			{
				threadSleep(10);
			}
		}
	}
	world.destroy();
	menuManager.shutdown();
	dataManager.shutdown();
	graphics->destroyWindow();
	return 0;
}
