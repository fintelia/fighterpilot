
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														DEFINITIONS																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
bool active=true;					// Window Active Flag																						//	//
bool InitMultisample(HINSTANCE hInstance,HWND hWnd,PIXELFORMATDESCRIPTOR pfd);																	//	//
																																				//	//
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;										//	//
extern const float size=128;																													//	//
char* errorString;																																//	//
int sh=1024;																																	//	//
int sw=1280;																																	//	//
																																				//	//
profiler Profiler;																																//	//
TextManager* textManager;																														//	//
																																				//	//
#if defined USING_XINPUT																														//	//
	Input *input=new xinput_input;																												//	//
#else																																			//	//
	Input *input=new standard_input;																											//	//
#endif																																			//	//
ObjectStats settings;																															//	//
float radarAng=0;																																//	//
																																				//	//
MenuManager& menuManager=MenuManager::getInstance();																							//	//
ModeManager& modeManager=ModeManager::getInstance();																							//	//
DataManager& dataManager=DataManager::getInstance();																							//	//
WorldManager& world=WorldManager::getInstance();																								//	//
CollisionChecker& collisionCheck=CollisionChecker::getInstance();																				//	//
GraphicsManager* graphics=OpenGLgraphics::getInstance();																						//	//
CameraManager cameraManager=CameraManager::getInstance();																						//	//
bool hasContext=true;																															//	//
bool getContext=false;																															//	//
bool done=false;//exits program																													//	//
																																				//	//
bool lowQuality;																																//	//
humanControl players[NumPlayers];																												//	//
																																				//	//
GraphicsManager::gID fireParticleEffect;																										//	//
GraphicsManager::gID smokeParticleEffect;																										//	//
GraphicsManager::gID exaustParticleEffect;																										//	//
																																				//	//
int frame=0,Time,timebase=0;																													//	//
float fps;																																		//	//
planeType defaultPlane;																															//	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//																																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void update()
{
	world.time.nextFrame();
	
	input->update();
	menuManager.update();
	modeManager.update();
}
void ShowHideTaskBar(bool bHide)
{
	// find taskbar window
	HWND pWnd = FindWindow(L"Shell_TrayWnd",L"");
	if(!pWnd)	return;
	if(bHide)	ShowWindow(pWnd,SW_HIDE);
	else		ShowWindow(pWnd,SW_SHOW);
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
			input->windowsInput(uMsg,wParam,lParam);
			return 0;
		case WM_ERASEBKGND:									// Check To See If Windows Is Trying To Erase The Background
			return 0;										// either return 0 or 1, does not seem to be much of a difference...
		case WM_SIZE:										// Resize The OpenGL Window
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
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	
	if(!is_directory("media"))
	{
		MessageBox(NULL,L"Media folder not found. Fighter-Pilot will now close.", L"Error",MB_ICONERROR);
		return 0;
	}

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,L"Would you like to run with high graphics settings?", L"Start High Quality?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		lowQuality=true;
	}

	RECT r;
	r.top=0;
	r.left=0;
	r.right=GetSystemMetrics(SM_CXSCREEN);
	r.bottom=GetSystemMetrics(SM_CYSCREEN);
	if(lowQuality)
	{
		r.right = 800;
		r.bottom = 600;
	}

	// Create Our OpenGL Window
	if (!graphics->createWindow("Fighter Pilot",r,true))
	{
		return 0;									// Quit If Window Was Not Created
	}
	//if(wglSwapIntervalEXT)
	//	wglSwapIntervalEXT(0);//turn on/off vsync (0 = off and 1 = on)
	ShowHideTaskBar(false);
//////
	srand ((unsigned int)time(NULL));
	textManager = new TextManager();
	menuManager.init();
	menuManager.setMenu("menuLoading");

	fireParticleEffect = graphics->newParticleEffect("explosion fireball",500.0*r.right/1280);
	smokeParticleEffect = graphics->newParticleEffect("explosion smoke",750.0*r.right/1280);
	exaustParticleEffect = graphics->newParticleEffect("missile smoke",100.0*r.right/1280);

//////
	float nextUpdate=0;
	float lastUpdate=0;

	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else
		{
			float waitTime=nextUpdate-GetTime();
			if(waitTime>0)	Sleep(waitTime);

			update();

			double time = GetTime();		
			nextUpdate=1000.0/MAX_FPS + time;
			lastUpdate = time;

#ifdef _DEBUG
			if(input->getKey(VK_ESCAPE))
			{
				done=true;
			}
			else 
#endif
			if(hasContext && active)
			{
				graphics->render();
				graphics->swapBuffers();
			}
		}
	}

	graphics->destroyWindow();
	return 0;
}
