
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//														DEFINITIONS																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//HDC			hDC=NULL;						// Private GDI Device Context																	//	//
//HGLRC		hRC=NULL;						// Permanent Rendering Context																		//	//
//HWND		hWnd=NULL;						// Holds Our Window Handle																			//	//
//HINSTANCE	hInstance;						// Holds The Instance Of The Application															//	//
bool		active=true;					// Window Active Flag																				//	//
bool InitMultisample(HINSTANCE hInstance,HWND hWnd,PIXELFORMATDESCRIPTOR pfd);																	//	//
																																				//	//
//int fontBase;																																	//	//
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;										//	//
extern const float size=128;																													//	//
char* errorString;																																//	//
																																				//	//
																																				//	//
int sh=1024;																																	//	//
int sw=1280;																																	//	//
																																				//	//
//vector<bullet> bullets;																														//	//
//vector<missile> missiles;																														//	//
//Smoke newSmoke;																																//	//
//Smoke newExaust;																																//	//
profiler Profiler;																															//	//
//map<int,planeBase*> planes;																													//	//
//vector<turret*> turrets;																														//	//
TextManager* textManager;																														//	//
																																				//	//
#if defined USING_XINPUT																														//	//
	Input *input=new xinput_input;																												//	//
#else																																			//	//
	Input *input=new standard_input;																											//	//
#endif																																			//	//
Controls controls(2);																															//	//
Settings settings;																																//	//
																																				//	//
int disp[3];//should be combined with model																										//	//
float radarAng=0;																																//	//
																																				//	//
guiBase* GUI=NULL;																																//	//
MenuManager& menuManager=MenuManager::getInstance();																							//	//
ModeManager& modeManager=ModeManager::getInstance();																							//	//
DataManager& dataManager=DataManager::getInstance();																							//	//
WorldManager& world=WorldManager::getInstance();																							//	//
CollisionChecker& collisionCheck=CollisionChecker::getInstance();																				//	//
GraphicsManager* graphics=OpenGLgraphics::getInstance();																						//	//
CameraManager cameraManager=CameraManager::getInstance();																						//	//
bool Redisplay=false;																															//	//
bool hasContext=true;																															//	//
bool getContext=false;																															//	//
bool done=false;//exits program																													//	//
																																				//	//
bool lowQuality;																																//	//
																																				//	//
//objModel m;																																	//	//
																																				//	//
player players[NumPlayers];																														//	//
																																			//	//
GraphicsManager::gID fireParticleEffect;																										//	//
GraphicsManager::gID smokeParticleEffect;																										//	//
GraphicsManager::gID exaustParticleEffect;																										//	//
struct resize_t																																	//	//
{																																				//	//
	resize_t(): mutex(CreateMutex(NULL,false,NULL)), needResize(false), x(0), y(0) {}															//	//
	HANDLE mutex;																																//	//
	bool needResize; 																															//	//
	int x,y;																																	//	//
	void lock(){WaitForSingleObject(mutex,INFINITE);}																							//	//
	void unlock(){ReleaseMutex(mutex);}																											//	//
} resize;																																		//	//
																																				//	//
int frame=0,Time,timebase=0;																													//	//
float fps;																																		//	//
planeType defaultPlane;																															//	//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  //
//																																				    //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int update(float v)
{
	double time=world.time();
	static double lastTime=time;
	double ms=time-lastTime;
	lastTime=time;

	input->update();
	menuManager.update();

	int i = 30;//Cmenu->update(v);
	if(!world.time.isPaused())
		modeManager.update(ms);

	//modeType nMode=(mode->newMode != (modeType)0) ? mode->newMode : Cmenu->newMode;
	//menuType nMenu=Cmenu->newMenu;
	//if(nMode != (modeType)0)
	//{
	//	delete mode;
	//	if(nMode==LOADING)				mode=new loading;
	//	else if(nMode==TWO_PLAYER_VS)	mode=new twoPlayerVs;
	//	else if(nMode==ONE_PLAYER)		mode=new onePlayer;
	//	else if(nMode==BLANK_MODE)		mode=new blankMode;
	//	else if(nMode==MAP_BUILDER)		mode=new mapBuilder;
	//	mode->init();
	//}
	//else if(nMenu != (menuType)0)
	//{
	//	delete Cmenu;
	//	if(nMenu==MAIN_MENU)			Cmenu=new closedMenu;
	//	else if(nMenu==CLOSED)			Cmenu=new mainMenu;
	//	else if(nMenu==SET_CONTROLS)	Cmenu=new setControls;
	//	else if(nMenu==START)			Cmenu=new m_start;
	//	else if(nMenu==CHOOSE_MODE)		Cmenu=new m_chooseMode;
	//}

	return i;
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
//		case WM_MOUSEACTIVATE:
//		{
//			active=true;
//			return 0;
//		}
//		case WM_ACTIVATE:							// Watch For Window Activate Message
		case WM_SHOWWINDOW:
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
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
		case WM_MOVE:
		case WM_MOVING:
		case WM_WINDOWPOSCHANGED:
		case WM_WINDOWPOSCHANGING:
		{
			Redisplay=true;
			return 0;
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		//{
		//	
		//	if((lParam & 0x40000000)==0)
		//	{
		//		menuManager.keyUpdate(true,wParam);
		//		input->down(wParam);				// If So, Mark It As TRUE
		//	}
		//	return 0;								// Jump Back
		//}
		case WM_KEYUP:								// Has A Key Been Released?
		//{
		//	menuManager.keyUpdate(false,wParam);
		//	input->up(wParam);						// If So, Mark It As FALSE
		//	return 0;								// Jump Back
		//}
		//case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
			input->windowsInput(uMsg,wParam,lParam);
			return 0;

		case WM_ERASEBKGND:								// Check To See If Windows Is Trying To Erase The Background
			return 0;								// Return 0 (Prevents Flickering While Resizing A Window)
		case WM_SIZE:								// Resize The OpenGL Window
			resize.lock();
			resize.needResize=true;
			resize.x=LOWORD(lParam);
			resize.y=HIWORD(lParam);
			resize.unlock();
			return 0;								// Jump Back
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
void mainLoop()
{
	//static unsigned long frameNumber=0;
	static float nextUpdate=0;
	static float lastUpdate=0;

	if(getContext)
	{
		//wglMakeCurrent(hDC,hRC);
		//hasContext=true;//WE DON'T HAVE A CONTEXT: THIS IS BAD!!!
		//getContext=false;
		done=true;
	}
		
	float waitTime=nextUpdate-GetTime();
	if(waitTime>0)	Sleep(waitTime);
		
	nextUpdate=update(GetTime()-lastUpdate)+GetTime();
	lastUpdate=GetTime();
	
	resize.lock();
	if(resize.needResize)
		graphics->resize(resize.x,resize.y);
	resize.needResize=false;
	resize.unlock();

	Profiler.startElement("Draw Time");
	if(input->getKey(VK_ESCAPE))
	{
		done=true;
	}
	else if(hasContext)
	{
		if(Redisplay && active)
		{
			graphics->render();
			Profiler.startElement("buffer swap");
			graphics->swapBuffers();
			Profiler.endElement("buffer swap");
		}
		Redisplay=false;
	}
	Profiler.endElement("Draw Time");
}
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	
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

	fireParticleEffect = graphics->newParticleEffect("explosion fireball",120);
	smokeParticleEffect = graphics->newParticleEffect("explosion smoke",200);
	exaustParticleEffect = graphics->newParticleEffect("missile smoke",35);

//////

	while(!done)									// Loop That Runs While done=FALSE
	{
		
		//if(getContext)
		//{
		//	wglMakeCurrent(hDC,hRC);
		//	hasContext=true;
		//	getContext=false;
		//}
		Profiler.startElement("loop");
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
		Profiler.endElement("loop");
		mainLoop();

		////else							// If There Are No Messages and we have the opengl context
		//{
		//	Profiler.startElement("xLoop Time");
		//	// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
		//	Profiler.startElement("xUpdate Time");
		//	float waitTime=nextUpdate-GetTime();
		//	if(waitTime>0)
		//		Sleep(waitTime);
		//	nextUpdate=update(GetTime()-lastUpdate)+GetTime();
		//	lastUpdate=GetTime();
		//	Profiler.endElement("xUpdate Time");
		//	Profiler.startElement("xDraw Time");

		//	resize.lock();
		//	if(resize.needResize)
		//		handleResize(resize.x,resize.y);
		//	resize.needResize=false;
		//	resize.unlock();
		//	if(hasContext)
		//	{
		//		if(Redisplay && active)
		//		{
		//			if ((active && !draw()) || input->keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
		//			{
		//				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
		//			}
		//			else									// Not Time To Quit, Update Screen
		//			{
		//				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
		//			}
		//		}
		//		else if (input->keys[VK_ESCAPE])			// Was There A Quit Received?
		//		{
		//			done=TRUE;								
		//		}
		//		Redisplay=false;
		//	}
		//	Profiler.endElement("xDraw Time");
		//	//if (input->keys[VK_F1])						// Is F1 Being Pressed?
		//	//{
		//	//	input->keys[VK_F1]=FALSE;					// If So Make Key FALSE
		//	//	KillGLWindow();						// Kill Our Current Window
		//	//	fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
		//	//	// Recreate Our OpenGL Window
		//	//	if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
		//	//	{
		//	//		return 0;						// Quit If Window Was Not Created
		//	//	}
		//	//}
		//	Profiler.endElement("xLoop Time");
		//}
		
	}

	//planes.clear();
	//missiles.clear();
	//bullets.clear();
	graphics->destroyWindow();
	return 0;
}
