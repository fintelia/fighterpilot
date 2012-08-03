
#include "engine.h"
#if defined(WINDOWS)
	#include <Windows.h>
#elif defined(LINUX)
	#include <X11/keysym.h>
	#include <X11/Xlib.h>
	Display* x11_display;
	int x11_screen;
	unsigned int x11_window;
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//															DEFINITIONS														    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
																															//	//
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;					//	//
int sh=1024;																												//	//
int sw=1280;																												//	//
float sAspect=((float)sw)/sh;																								//	//
																															//  //
profiler Profiler;																											//	//
																															//	//
Ephemeris& ephemeris=Ephemeris::getInstance();																				//  //
InputManager& input=InputManager::getInstance();																			//	//
gui::manager& menuManager = gui::manager::getInstance();																	//	//
DataManager& dataManager = DataManager::getInstance();																		//	//
AssetLoader& assetLoader = AssetLoader::getInstance();																		//  //
WorldManager& world = WorldManager::getInstance();																			//	//
PhysicsManager& physics = PhysicsManager::getInstance();																	//	//
GraphicsManager* graphics = OpenGLgraphics::getInstance();																	//	//
FileManager& fileManager = FileManager::getInstance();																		//  //
SceneManager& sceneManager = SceneManager::getInstance();																	//  //
																															//  //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//																															    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
#if defined(WINDOWS)
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
				game->active=true;					// Program Is Active
			}
			else
			{
				game->active=false;					// Program Is No Longer Active
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
#elif defined(LINUX)
void linuxEventHandler(XEvent event)
{
	if(event.type == KeyPress || event.type == KeyRelease)
	{
		XKeyEvent* e = &event.xkey;
		bool keyup = e->type == KeyRelease;
		unsigned int keysym = XLookupKeysym(e,0);

		auto trySym = [keysym,keyup](unsigned int sym, unsigned int vk)->bool
		{
			if(keysym == sym)
			{
				input.sendCallbacks(new InputManager::keyStroke(keyup, vk));
				//cout << keysym << endl;
				return true;
			}
			return false;
		};
		auto trySymRange = [keysym,keyup](unsigned int minSym, unsigned int maxSym, unsigned int minVK)
		{
			if(keysym >= (minSym) && keysym <= (maxSym))
			{
				input.sendCallbacks(new InputManager::keyStroke(keyup, keysym-(maxSym)+(minVK)));
				//cout << keysym << endl;
				return true;
			}
			return false;
		};
		
		if(keysym == 0xffe1 || keysym == 0xffe2)// L-SHIFT / R-SHIFT
			input.sendCallbacks(new InputManager::keyStroke(keyup, VK_SHIFT));

		if(keysym == 0xffe3 || keysym == 0xffe4)// L-CONTROL / R-CONTROL
			input.sendCallbacks(new InputManager::keyStroke(keyup, VK_CONTROL));
		
		
		//see input.h and x11/keysymdef.h 
		if(	!trySym(XK_BackSpace, 						VK_BACK) &&
			!trySym(XK_Tab, 							VK_TAB) &&
			!trySym(XK_Return, 							VK_RETURN) &&
			!trySym(XK_Alt_L,							VK_MENU) &&		//L-ALT
			!trySym(XK_Alt_R,							VK_MENU) &&		//R-ALT
			!trySym(XK_Pause,							VK_PAUSE) &&
			!trySym(XK_Caps_Lock,						VK_CAPITAL) &&
			!trySym(XK_Escape,							VK_ESCAPE) &&
			!trySym(XK_space,							VK_SPACE) &&
			!trySymRange(XK_Home, XK_Down,				VK_HOME) && 	// home,up,down,left,right
			!trySymRange(XK_KP_0, XK_KP_9,				VK_NUMPAD0) &&
			!trySymRange(XK_KP_Multiply, XK_KP_Divide,	VK_MULTIPLY) &&	// multiply, add, seperator, subtract, decimal, divide (keypad)
			!trySymRange(XK_F1, XK_F24,					VK_F1) && 		// F1 - F24
			!trySymRange(XK_Shift_L, XK_Control_R,		VK_LSHIFT) &&	// L-SHIFT, R-SHIFT, L-CONTROL, R-CONTROL
			!trySymRange(XK_KP_0, XK_KP_9,				0x30) && 		// 0-9
			!trySymRange(XK_A, XK_Z,					0x41) && 		// A-Z
			!trySymRange(XK_a, XK_z,					0x41)) 			// a-z
		{
			//keysym not found...
		}
	}
	else if(event.type == ButtonPress)
	{
		XButtonEvent* e = &event.xbutton;
		if(e->button == 1)	input.sendCallbacks(new InputManager::mouseClick(true, LEFT_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
		if(e->button == 2)	input.sendCallbacks(new InputManager::mouseClick(true, MIDDLE_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
		if(e->button == 3)	input.sendCallbacks(new InputManager::mouseClick(true, RIGHT_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
		if(e->button == 4)	input.sendCallbacks(new InputManager::mouseScroll(1.0/3.0));
		if(e->button == 5)	input.sendCallbacks(new InputManager::mouseScroll(-1.0/3.0));
	}
	else if(event.type == ButtonRelease)
	{
		XButtonEvent* e = &event.xbutton;
		if(e->button == 1)	input.sendCallbacks(new InputManager::mouseClick(false, LEFT_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
		if(e->button == 2)	input.sendCallbacks(new InputManager::mouseClick(false, MIDDLE_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
		if(e->button == 3)	input.sendCallbacks(new InputManager::mouseClick(false, RIGHT_BUTTON, Vec2f((float)e->x / sh, (float)e->y / sh)));
	}
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

#else
int main(int argc, const char* argv[])
{
#endif

	std::set_new_handler(outOfMemory);

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
	
#if defined(WINDOWS)
    string cmdLineString(lpCmdLine);
	boost::split(game->commandLineOptions, cmdLineString, boost::is_any_of(" "), boost::token_compress_on);	
	MSG	msg; // Windows Message Structure
#elif defined(LINUX)
	for(int i=0; i<argc; i++)
	{
		game->commandLineOptions.push_back(string(argv[i]));
	}
	x11_display = XOpenDisplay(0);
	x11_screen = DefaultScreen(x11_display);
#endif

	if(!game->init())
	{
		return 1;
	}
	
#ifdef LINUX
	XSelectInput(x11_display, x11_window, ButtonPressMask|ButtonReleaseMask|StructureNotifyMask|KeyPressMask|KeyReleaseMask|KeymapStateMask);
	Atom wmDeleteMessage=XInternAtom(x11_display, "WM_DELETE_WINDOW", true);
	XSetWMProtocols(x11_display, x11_window, &wmDeleteMessage, 1);
	XEvent event;
#endif

	float nextUpdate=0;
	float swapTime=0.0;
	float time=0.0;
	while(!game->done)
	{
#if defined(WINDOWS)
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
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
#elif defined(LINUX)
		if(XPending(x11_display))
		{
			XNextEvent(x11_display, &event);
			if((event.type == ClientMessage && event.xclient.data.l[0] == wmDeleteMessage)/* || (event.type == KeyPress && event.xkey.keycode == 9)*/) 
			{
				XDestroyWindow(x11_display,event.xclient.window);
				break;
			}
			else
			{
				linuxEventHandler(event);
			}
		}
		//else
#endif
		{
			game->update();
			
			if(game->active)
			{
				graphics->render();

				///timing code
				float waitTime = (nextUpdate - swapTime) - GetTime();
				if(waitTime>1.0)	threadSleep(waitTime-1);
				do{
					time = GetTime();
				}while(time < nextUpdate - swapTime);
				nextUpdate = 1000.0/game->maxFrameRate + GetTime();
				time = GetTime();
				//end timing code

				graphics->swapBuffers();

				//more timing code:
				swapTime = max(time - GetTime(), 0.0f);
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
	fileManager.shutdown(); //will wait for all files to be written
	return 0;
}
