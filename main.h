
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
#include <cstdarg>
#include <iomanip>
#include <cassert>
//--Boost C++ Library
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
//#include <boost\python.hpp>
//--Namespaces
using namespace std;
using namespace boost;
using namespace boost::filesystem;

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4018)
#pragma warning( disable : 4250)

#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#pragma comment (linker, "/ENTRY:WinMainCRTStartup")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "glu32.lib")

#include <windows.h>
#include <Shlobj.h>
#include <process.h>
#include "GL/glee.h"
#include <GL/glu.h>

extern bool	active;		// Window Active Flag
extern const double PI;
extern const float size;
extern bool lowQuality;
extern bool done;//setting this to true will terminate the program

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

#include "enums.h"
#include "time.h"
#include "input.h"
#ifdef _DEBUG
#include "XboxContInput.h"
#endif
#include "text.h"
#include "gameMath.h"
#include "random.h"
#include "shader.h"
#include "graphicsManager.h"
#include "dataManager.h"
#include "cameraManager.h"
#include "path.h"
#include "level.h"
#include "script.h"
#include "profiler.h"
#include "explosion.h"
#include "imageloader.h"//should be replaced!!!
#include "partical.h"
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"
#include "turret.h"
#include "settings.h"
#include "player.h"
#include "objectList.h"
#include "worldManager.h"

const int NumPlayers = 2;
extern player players[NumPlayers];

extern int sh, sw;

extern GraphicsManager::gID fireParticleEffect;
extern GraphicsManager::gID smokeParticleEffect;
extern GraphicsManager::gID exaustParticleEffect;

extern float radarAng;//should be replaced by gameTime()

extern bool Redisplay;

extern int frame,Time,timebase;
extern float fps;

#include "menuScreen.h"
#include "modeManager.h"