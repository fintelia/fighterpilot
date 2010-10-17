
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include "debug.h"
#include <cassert>
//--Boost C++ Library
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
//--Namespaces
using namespace std;
using namespace boost;

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4018)

#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#pragma comment (linker, "/ENTRY:WinMainCRTStartup")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "glu32.lib")

#include <windows.h>
#include <process.h>
#include "GL/glee.h"
#include <GL/glu.h>
extern HDC			hDC;						// Private GDI Device Context
extern HGLRC		hRC;						// Permanent Rendering Context
extern HWND			hWnd;						// Holds Our Window Handle
extern HINSTANCE	hInstance;					// Holds The Instance Of The Application
extern bool			active;						// Window Active Flag
extern bool			fullscreen;					// Fullscreen Flag Set To TRUE By Default
//extern int fontBase;

extern const float PI;
extern const float size;

extern bool lowQuality;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
void cleanup();
char *textFileRead(char *fn);
bool checkExtension(char* extensions, char* checkFor);
void viewOrtho(int x, int y);
void viewPerspective();

#include "enums.h"
//#include "debug.h"
#include "time.h"
#include "input.h"
#ifdef _DEBUG
#include "XboxContInput.h"
#endif
#include "controls.h"
#include "text.h"
#include "gameMath.h"
#include "random.h"


#include "dataManager.h"
#include "load_save.h"
#include "level.h"
#include "script.h"
#include "profiler.h"
#include "explosion.h"
#include "Terain.h"
#include "gridFloat.h"
#include "imageloader.h"
#include "partical.h"
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"
#include "turret.h"
#include "texture2.h"
#include "settings.h"
#include "player.h"
#include "gui.h"
#include "worldManager.h"

const int NumPlayers = 2;
const double DegToRad = 3.14159265358979323846264/180;
const double RadToDeg = 180.0/3.14159265358979323846264;

extern player players[NumPlayers];

extern int sh;
extern int sw;
extern vector<bullet> bullets;
extern vector<missile> missiles;
extern Smoke newSmoke;
extern Smoke newExaust;

extern map<int,planeBase*> planes;
extern vector<turret*> turrets;

//extern bool firstP[NumPlayers];
//extern vector<int> model;

//extern vector<int> tex;
//extern vector<int> menusTx;
//extern vector<int> shaders;
extern GLuint menuBack;
extern int disp[3];//should be combined with model
extern float radarAng;

extern Terrain* terrain;
//extern Terrain* sea;
extern bool Redisplay;
extern bool hasContext;
extern bool getContext;

extern int frame,Time,timebase;
extern float fps;


//extern objModel m;

#include "menuScreen.h"
#include "menu.h"
#include "modes.h"