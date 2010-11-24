
#include "main.h"

HDC			hDC=NULL;						// Private GDI Device Context
HGLRC		hRC=NULL;						// Permanent Rendering Context
HWND		hWnd=NULL;						// Holds Our Window Handle
HINSTANCE	hInstance;						// Holds The Instance Of The Application
bool		active=true;					// Window Active Flag
bool		fullscreen=true;				// Fullscreen Flag Set To TRUE By Default
bool		arbMultisampleSupported	= false;
int			arbMultisampleFormat	= 0;
bool InitMultisample(HINSTANCE hInstance,HWND hWnd,PIXELFORMATDESCRIPTOR pfd);

//int fontBase;
extern const double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;
extern const float size=128;
char* errorString;

const Color white(1.0f,1.0f,1.0f);
const Color black(0.0f,0.0f,0.0f);
const Color darkBlue(0.11f,0.35f,0.52f);
const Color lightBlue(0.19f,0.58f,0.78f);
const Color darkGreen(0.49f,0.57f,0.06f);
const Color lightGreen(0.68f,0.76f,0.12f);

int sh=1024;
int sw=1280;

vector<bullet> bullets;
vector<missile> missiles;
Smoke newSmoke;
Smoke newExaust;
profiler Profiler;
map<int,planeBase*> planes;
vector<turret*> turrets;
TextManager* textManager;

//menus *Cmenu;
modes *mode;
#if defined USING_XINPUT
	Input *input=new xinput_input;
#else
	Input *input=new standard_input;
#endif
Controls controls(2);
Settings settings;

map<planeType,int> planeModels;
//GLuint menuBack;
int disp[3];//should be combined with model
float radarAng=0;

guiBase* GUI=NULL;
MenuManager& menuManager=MenuManager::getInstance();
DataManager& dataManager=DataManager::getInstance();
WorldManager& worldManager=WorldManager::getInstance();
CollisionChecker& collisionCheck=CollisionChecker::getInstance();
GraphicsManager* graphics=OpenGLgraphics::getInstance();
Terrain* terrain;
bool Redisplay=false;
bool hasContext=true;
bool getContext=false;
bool done=false;//exits program

bool lowQuality;

//objModel m;

player players[NumPlayers];
GameTime gameTime;

GraphicsManager::gID fireParticleEffect;
GraphicsManager::gID smokeParticleEffect;
GraphicsManager::gID exaustParticleEffect;
struct resize_t
{
	resize_t(): mutex(CreateMutex(NULL,false,NULL)), needResize(false), x(0), y(0) {}
	HANDLE mutex;
	bool needResize; 
	int x,y;
	void lock(){WaitForSingleObject(mutex,INFINITE);}
	void unlock(){ReleaseMutex(mutex);}
} resize;

int frame=0,Time,timebase=0;
float fps;
planeType defaultPlane;



bool initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_MULTISAMPLE);

	glActiveTextureARB(GL_TEXTURE4_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE3_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE2_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE1_ARB);	glEnable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);	glEnable(GL_TEXTURE_2D);

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	//glEnable(GL_POLYGON_SMOOTH);
	//glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);

	if (!GLEE_VERSION_2_0)          //is GL1.2 available?
	{
		string s("Your version of opengl must be above 2.0");
		s+="   Opengl version: ";
		s+=(char*)glGetString(GL_VERSION);
		s+="   Renderer: ";
		s+=(char*)glGetString(GL_RENDERER);
		s+="   Vender: ";
		s+=(char*)glGetString(GL_VENDOR);
		MessageBoxA(NULL, s.c_str(),"ERROR",MB_OK);
		return false;
	}

	glEnable(GL_POINT_SPRITE_ARB);
	float quadratic[] =  { 0.0f, 0.0f, 0.000001f };
	glPointParameterfv( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
	glPointParameterf( GL_POINT_SIZE_MIN_ARB, 1 );
	glPointParameterf( GL_POINT_SIZE_MAX_ARB, 8192 );
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);

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
	static double frameTimes[20]={20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};//in milliseconds
	static int curentFrame=0;
	double time=GetTime();
	static double lastTime=time;
	frameTimes[curentFrame++]=time-lastTime;
	lastTime=time;
	if(curentFrame>20)curentFrame=0;

	double spf=0.0;
	for(int i=0;i<20;i++)
		spf+=(frameTimes[i]*0.001)/20;
	fps=1.0/spf;


	Profiler.startElement("mainDraw");
	//frame++;
	//Time=GetTime();
	//if (Time - timebase > 1000) {
	//	fps = frame*1000.0/(Time-timebase);
	//	timebase = Time;
	//	frame = 0;
	//}
	
	mode->draw();glError();
	//Cmenu->draw();
	glViewport(0,0,sw,sh);
	viewOrtho(sw,sh);
	menuManager.render();
	viewPerspective();
	Profiler.endElement("mainDraw");
	graphics->reset();
	glError();
	return true;
}
void viewOrtho(int x, int y)				// Set Up An Ortho View
{
	glMatrixMode(GL_PROJECTION);			// Select Projection
	glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
	glOrtho( 0, x , y , 0, -1, 1 );			// Select Ortho Mode
	glMatrixMode(GL_MODELVIEW);				// Select Modelview Matrix
	//glPushMatrix();							// Push The Matrix
	glLoadIdentity();						// Reset The Matrix
}
void viewPerspective(void)					// Set Up A Perspective View
{
	glMatrixMode( GL_PROJECTION );			// Select Projection
	glPopMatrix();							// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );			// Select Modelview
	//glPopMatrix();							// Pop The Matrix
}
char *textFileRead(char *fn) {


	FILE *fp;
	char *content = NULL;

	int count=0;

	if (fn != NULL) 
	{
		fopen_s(&fp,fn,"rt");

		if (fp != NULL) 
		{
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				memset(content,0,sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
} 
//bool checkExtension(char* extensions, char* checkFor)
//{
//  return 0!=strstr(extensions, checkFor);
//}

int update(float v)
{
	double value=gameTime();
	static double lastValue=value;
	value=value-lastValue;
	lastValue=value;

	input->update();
	menuManager.update();

	int i = 30;//Cmenu->update(v);
	if(!gameTime.isPaused())
		i = mode->update(v);

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
//GLvoid BuildFont()								// Build Our Bitmap Font
//{
//	HFONT	font;										// Windows Font ID
//	HFONT	oldfont;									// Used For Good House Keeping
//
//	fontBase = glGenLists(96);							// Storage For 96 Characters
//
//	font = CreateFont(	-24,							// Height Of Font
//						0,								// Width Of Font
//						0,								// Angle Of Escapement
//						0,								// Orientation Angle
//						FW_BOLD,						// Font Weight
//						FALSE,							// Italic
//						FALSE,							// Underline
//						FALSE,							// Strikeout
//						ANSI_CHARSET,					// Character Set Identifier
//						OUT_TT_ONLY_PRECIS,					// Output Precision
//						CLIP_DEFAULT_PRECIS,			// Clipping Precision
//						ANTIALIASED_QUALITY,			// Output Quality
//						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
//						L"Arial Rounded MT");		// Font Name
//
//	oldfont = (HFONT)SelectObject(hDC, font);           // Selects The Font We Want
//	wglUseFontBitmaps(hDC, 0, 255, fontBase);			// Builds Characters Starting At Character 0
//	SelectObject(hDC, oldfont);							// Selects The Font We Want
//	DeleteObject(font);									// Delete The Font
//}

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
			//MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
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

	if (!UnregisterClass(L"OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}

}
void ShowHideTaskBar(BOOL bHide)
{

	// find taskbar window
	HWND pWnd = FindWindow(L"Shell_TrayWnd",L"");
	if(!pWnd )
		return;
	if( bHide )
		ShowWindow(pWnd,SW_HIDE);
	else
		ShowWindow(pWnd,SW_SHOW);

}



/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/

BOOL CreateGLWindow(char* title, RECT WindowRect, int bits, bool fullscreenflag, bool checkMultisample)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASSEX	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
//	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
//	WindowRect.left=(long)0;			// Set Left Value To 0
//	WindowRect.right=(long)width;		// Set Right Value To Requested Width
//	WindowRect.top=(long)0;				// Set Top Value To 0
//	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag
	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									//- No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= (HICON)LoadImage(NULL,L"media/icon.ico",IMAGE_ICON,256,256,LR_LOADFROMFILE);// Load The Icon
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= L"OpenGL";							// Set The Class Name

	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= WindowRect.right;		// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= WindowRect.bottom;	// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= 32/*bits*/;			// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,L"Program Will Now Close.",L"ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=0;//WS_EX_APPWINDOW;								// Window Extended Style
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
	if (!(hWnd=CreateWindowExA(	dwExStyle,							// Extended Style For The Window
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
		MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}



	static	PIXELFORMATDESCRIPTOR pfd2=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_SUPPORT_COMPOSITION |					// needed for vista
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0,0,0,0,									// Accumulation Bits Ignored
		24,											// 24Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	PIXELFORMATDESCRIPTOR pfd;
	pfd.nSize = sizeof( pfd );
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.cAlphaBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	if(!arbMultisampleSupported)
	{
		if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
		{
			KillGLWindow();								// Reset The Display
			MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE;								// Return FALSE
		}
	}
	else
	{
		PixelFormat = arbMultisampleFormat;
	}
	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}


	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}


	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!arbMultisampleSupported && checkMultisample && !lowQuality)
	{
		if(InitMultisample(hInstance,hWnd,pfd))
		{
			KillGLWindow();
			return CreateGLWindow(title, WindowRect, bits, fullscreenflag, false);
		}
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	handleResize(WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top);	// Set Up Our Perspective GL Screen

	if (!initRendering())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}
	return TRUE;									// Success
}
bool InitMultisample(HINSTANCE hInstance,HWND hWnd,PIXELFORMATDESCRIPTOR pfd)
{  
	if (!wglChoosePixelFormatARB)
	{
		// We Didn't Find Support For Multisampling, Set Our Flag And Exit Out.
		arbMultisampleSupported=false;
		return false;
	}

	int pixelFormat;
	UINT numFormats;
	float fAttributes[] = {0,0};

	// These Attributes Are The Bits We Want To Test For In Our Sample
	// Everything Is Pretty Standard, The Only One We Want To 
	// Really Focus On Is The SAMPLE BUFFERS ARB And WGL SAMPLES
	// These Two Are Going To Do The Main Testing For Whether Or Not
	// We Support Multisampling On This Hardware
	int iAttributes[] = { WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
		WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,24,
		WGL_ALPHA_BITS_ARB,8,
		WGL_DEPTH_BITS_ARB,16,
		WGL_STENCIL_BITS_ARB,0,
		WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
		WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
		WGL_SAMPLES_ARB, 16 ,						// Check For 16x Multisampling
		0,0};

	while(iAttributes[19] > 1)
	{
		if (wglChoosePixelFormatARB(hDC,iAttributes,fAttributes,1,&pixelFormat,&numFormats) && numFormats >= 1)
		{
			arbMultisampleSupported	= true;
			arbMultisampleFormat	= pixelFormat;	
			return arbMultisampleSupported;
		}
		iAttributes[19] >>= 1;
	}
	return  arbMultisampleSupported;//would be false?
}

BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower R
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height
	return CreateGLWindow(title, WindowRect, bits, fullscreenflag,true);
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
		case WM_MBUTTONUP:							//{
		//	// Retrieve mouse screen position
		//	int x=(short)LOWORD(lParam);
		//	int y=(short)HIWORD(lParam);
		//
		//	// Check to see if the left button is held down:
		//	bool leftButtonDown=(wParam & MK_LBUTTON) != 0;
		//
		//	// Check if right button down:
		//	bool rightButtonDown=(wParam & MK_RBUTTON) != 0;
		//	//if(GUI != NULL)
		//	//	GUI->mouseUpdate(leftButtonDown,rightButtonDown,x,y);
		//	menuManager.mouseUpdate(leftButtonDown,rightButtonDown,x,y);
		//	return 0;
		//}
		case WM_MOUSEWHEEL:							//float rotations = float(GET_WHEEL_DELTA_WPARAM(wParam))/120.0;
		//menuManager.scrollUpdate(rotations);
			input->windowsInput(uMsg,wParam,lParam);
			return 0;

		case WM_ERASEBKGND:								// Check To See If Windows Is Trying To Erase The Background
			return 1;								// Return 0 (Prevents Flickering While Resizing A Window)
		case WM_SIZE:								// Resize The OpenGL Window
		{
			resize.lock();
			resize.needResize=true;
			resize.x=LOWORD(lParam);
			resize.y=HIWORD(lParam);
			resize.unlock();
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}
//void eventLoop(void* v)
//{
//	int bRet=1;
//	MSG	msg;
//	while(!done )
//	{
//		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
//		{
//			if(msg.message==WM_QUIT)
//			{
//				done=true;
//			}
//			else											// If Not, Deal With Window Messages
//			{
//				TranslateMessage(&msg);						// Translate The Message
//				DispatchMessage(&msg);						// Dispatch The Message
//			}
//		}
//	}
//	done=true;
//}
void mainLoop(void* v)
{
	static unsigned long frameNumber=0;
	static float nextUpdate=0;
	static float lastUpdate=0;

	unsigned long endFrame=~(0UL);
	if(v!=NULL) endFrame=frameNumber+*((int*)v);
	while(!done && frameNumber!=endFrame)
	{
		if(getContext)
		{
			wglMakeCurrent(hDC,hRC);
			hasContext=true;
			getContext=false;
		}
		
		float waitTime=nextUpdate-GetTime();
		if(waitTime>0)	Sleep(waitTime);
		
		nextUpdate=update(GetTime()-lastUpdate)+GetTime();
		lastUpdate=GetTime();
		
		

		resize.lock();
		if(resize.needResize)
			handleResize(resize.x,resize.y);
		resize.needResize=false;
		resize.unlock();
	Profiler.startElement("Draw Time");
		if(hasContext)
		{
			if(Redisplay && active)
			{
				if ((active && !draw()) || input->getKey(VK_ESCAPE))	// Active?  Was There A Quit Received?
				{
					done=true;							// ESC or DrawGLScene Signalled A Quit
				}
				else									// Not Time To Quit, Update Screen
				{
					Profiler.startElement("buffer swap");
					SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
					Profiler.endElement("buffer swap");
				}
			}
			else if (input->getKey(VK_ESCAPE))			// Was There A Quit Received?
			{
				done=true;								
			}
			Redisplay=false;
		}
	Profiler.endElement("Draw Time");		
		frameNumber++;
	}
}
int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	//bool	done=false;								// Bool Variable To Exit Loop
	
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

	//if(!fullscreen)
	//{
	//	 SystemParametersInfo(SPI_GETWORKAREA,0,(LPVOID)&r,0);
	//	 r.bottom-=15;
	//	 r.right-=5;  //doesn't work???
	//	 r.top+=15;
	//}
	// Create Our OpenGL Window
	if (!CreateGLWindow("Fighter Pilot",r,24,fullscreen,true))
	{
		return 0;									// Quit If Window Was Not Created
	}
	//if(wglSwapIntervalEXT)
	//	wglSwapIntervalEXT(0);//turn on/off vsync (0 = off and 1 = on)
	ShowHideTaskBar(false);
//////
	srand ((unsigned int)time(NULL));
	//Cmenu=new closedMenu;
	mode=new loading;
	textManager = new TextManager();
	menuManager.init();

	fireParticleEffect = graphics->newParticleEffect("explosion fireball",120);
	smokeParticleEffect = graphics->newParticleEffect("explosion smoke",200);
	exaustParticleEffect = graphics->newParticleEffect("missile smoke",20 );
//////

//#define MULTITHREADED
#ifdef MULTITHREADED
	getContext=true;
	hasContext=false;
	wglMakeCurrent(0,0);
	_beginthread( mainLoop, 0, NULL );
	int bRet;
#else
	int f=1;
#endif
	while(!done)									// Loop That Runs While done=FALSE
	{
		
		//if(getContext)
		//{
		//	wglMakeCurrent(hDC,hRC);
		//	hasContext=true;
		//	getContext=false;
		//}
#ifdef MULTITHREADED
		bRet = GetMessage( &msg, NULL, 0, 0 );
		if (bRet==0 || bRet == -1)
		{
		    // handle the error and possibly exit
			done=true;
		}
		else
		{
		    TranslateMessage(&msg); 
		    DispatchMessage(&msg); 
		}
#else
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
		mainLoop(&f);
		
#endif
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

	delete terrain;
	//planes.clear();
	//missiles.clear();
	//bullets.clear();
	KillGLWindow();									// Kill The Window
	//return (msg.wParam);							// Exit The Program
	return 0;
}
