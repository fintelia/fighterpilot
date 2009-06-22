
#include <cstdio>
//#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <cassert>
using namespace std;

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4018)

#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#pragma comment (linker, "/ENTRY:wWinMainCRTStartup")
#pragma comment (lib, "Winmm.lib")

#include <windows.h>
#include <process.h>
#include <GL/glee.h>
#include <GL/glu.h>
HDC			hDC=NULL;					// Private GDI Device Context
HGLRC		hRC=NULL;					// Permanent Rendering Context
HWND		hWnd=NULL;					// Holds Our Window Handle
HINSTANCE	hInstance;					// Holds The Instance Of The Application
bool		active=TRUE;					// Window Active Flag
bool		fullscreen=TRUE;				// Fullscreen Flag Set To TRUE By Default
LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc


const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;
extern const float size=128;


int fontBase;
void cleanup();
void BitmapCharacter(int i);
int GetTime();
void watchStart();
int watchStop();
void PostRedisplay();

#include "input.h"
//#include "xinput.h"
#include "controls.h"
#include "basic.h"
#include "gameMath.h"
#include "Terain.h"
#include "imageloader.h"
#include "settings.h"
Settings settings;
#include "partical.h"
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"
#include "texture2.h"
#include "model.h"
#include "obj.h"

//#define DEBUG


const int NumPlayers = 2;
const double DegToRad = 3.14159265358979323846264/180;
const double RadToDeg = 180/.314159265358979323846264;

int sh;
int sw;
vector<bullet> bullets;
vector<missile> missiles;
vector<spark> fire;
vector<spark> smoke;
map<int,planeBase*> planes;

bool firstP[NumPlayers] = {true,true};
vector<int> model;
vector<int> tex;
vector<int> menusTx;
vector<int> shaders;
GLuint menuBack;
int disp[1];//should be combined with model
float radarAng=0;

Terrain* terrain;
bool Redisplay=false;
bool hasContext=true;
bool getContext=false;

int frame=0,Time,timebase=0;
float fps;


#include "menu.h"
#include "modes.h"

bool initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_POINT_SMOOTH);
	glShadeModel(GL_SMOOTH);
//	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	if (!GLEE_VERSION_2_0)          //is GL1.2 available?
	{
		string s("Your version of opengl must be above 2.0");
		s+="   Opengl version: ";
		s+=(char*)glGetString(GL_VERSION);
		s+="   Renderer: ";
		s+=(char*)glGetString(GL_RENDERER);
		s+="   Vender: ";
		s+=(char*)glGetString(GL_VENDOR);
		MessageBox(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}

	return true;
}
void handleResize(int w, int h) {
	sh = h;
	sw = w;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}
bool draw()
{
	frame++;
	Time=GetTime();
	if (Time - timebase > 1000) {
		fps = frame*1000.0/(Time-timebase);
 		timebase = Time;
		frame = 0;
	}
	mode->draw();
	Cmenu->draw();
	return true;
}
//void handleKeypress(unsigned char key, int x, int y) {
//	switch (key) {
//		case 27: //Escape key
//			cleanup();
//			exit(0);
//	}
//	input->down(int(key));
//}
//void KeyUpFunc(unsigned char key,int x,int y) {
//	input->up(int(key));
//}
//void SpecialKeyDownFunc(int key,int x,int y) {
//	input->specialDown(key);
//}
//void SpecialKeyUpFunc(int key,int x,int y) {
//	input->specialUp(key);
//}
//void Joystick(unsigned int buttonMask, int x, int y, int z)
//{
//	input->joystick(buttonMask,x,y,z);
//}
int GetTime()
{
	return GetTickCount();
}

int update(int value)
{
	input->update();
	int i = mode->update(value);
	int l = Cmenu->update(value);
	if(mode->newMode)
	{
		switch(mode->newMode)
		{
		case 1:
			mode=new loading;break;
		case 3:
			mode=new twoPlayerVs;break;
		case 4:
			mode=new onePlayer;break;
		}
		//menu=new closedMenu;
	}
	if(Cmenu->newMode)
	{
		switch(Cmenu->newMode)
		{
		case 1:
			mode=new loading;break;
		case 2:
			mode=new blankMode;break;
		case 3:
			mode=new twoPlayerVs;break;
		case 4:
			mode=new onePlayer;break;
		}
	}
	if(Cmenu->newMenu)
	{
		switch(Cmenu->newMenu)
		{
		case 1:
			Cmenu=new closedMenu;break;
		case 2:
			Cmenu=new mainMenu;break;
		case 3:
			Cmenu=new setControls;break;
		}
	}
	return i;
}
GLvoid BuildFont()								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID
	HFONT	oldfont;									// Used For Good House Keeping

	fontBase = glGenLists(96);							// Storage For 96 Characters

	font = CreateFont(	-24,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
						FW_BOLD,						// Font Weight
						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Arial Rounded MT Bold");		// Font Name

	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
	wglUseFontBitmaps(hDC, 0, 255, fontBase);			// Builds Characters Starting At Character 0
	SelectObject(hDC, oldfont);							// Selects The Font We Want
	DeleteObject(font);									// Delete The Font
}

void KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			//MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		//MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		//MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}
/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= (HICON)LoadImage(NULL,"media2/icon.ico",IMAGE_ICON,16,16,LR_LOADFROMFILE);// Load The Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;	// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		1,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		1,											// No Accumulation Buffer
		8,8,8,8,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	handleResize(width, height);					// Set Up Our Perspective GL Screen

	if (!initRendering())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		//MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
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
		{
			if((lParam & 0x40000000)==0)
				input->down(wParam);				// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			input->up(wParam);						// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			handleResize(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
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
	BOOL	done=FALSE;								// Bool Variable To Exit Loop
	
	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}

	// Create Our OpenGL Window
	if (!CreateGLWindow("Fighter Pilot",800,600,32,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}
//////
	srand ((unsigned int)time(NULL));
	Cmenu=new closedMenu;
	mode=new loading;
	BuildFont();
	PlaySound(TEXT("media2/07_Creedence_Clearwater_Revival_Fortunate_Son.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
//////


	int nextUpdate=0;
	int lastUpdate=0;
	while(!done)									// Loop That Runs While done=FALSE
	{
		if(getContext)
		{
			wglMakeCurrent(hDC,hRC);
			hasContext=true;
			getContext=false;
		}
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
		else							// If There Are No Messages and we have the opengl context
		{
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			int waitTime=nextUpdate-GetTime();
			if(waitTime>0)
				Sleep(waitTime);
			nextUpdate=update(GetTime()-lastUpdate)+GetTime();
			lastUpdate=GetTime();
			if(hasContext)
			{
				if(Redisplay)
				{
					if ((active && !draw()) || input->keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
					{
						done=TRUE;							// ESC or DrawGLScene Signalled A Quit
					}
					else									// Not Time To Quit, Update Screen
					{
						SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
					}
				}
				Redisplay=false;
			}
			//if (input->keys[VK_F1])						// Is F1 Being Pressed?
			//{
			//	input->keys[VK_F1]=FALSE;					// If So Make Key FALSE
			//	KillGLWindow();						// Kill Our Current Window
			//	fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
			//	// Recreate Our OpenGL Window
			//	if (!CreateGLWindow("NeHe's Rotation Tutorial",640,480,16,fullscreen))
			//	{
			//		return 0;						// Quit If Window Was Not Created
			//	}
			//}
		}
	}

	// Shutdown
	
	delete terrain;
	//planes.clear();
	//missiles.clear();
	//bullets.clear();
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}
