
extern void minimizeWindow();
#include <xutility>
#if _ITERATOR_DEBUG_LEVEL == 2			//_ITERATOR_DEBUG_LEVEL == 2 when compiling in DEBUG mode
	#undef _DEBUG_ERROR2				//we are redefining _DEBUG_ERROR2 so that the window is minimized and then the error message is displayed 
	#define _DEBUG_ERROR2(mesg, file, line){		\
		minimizeWindow();							\
		_Debug_message(L ## mesg, file, line);		\
	}
 #endif /* _ITERATOR_DEBUG_LEVEL == 2 */


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
#pragma warning( disable : 4996)

#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#pragma comment (linker, "/ENTRY:WinMainCRTStartup")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "glu32.lib")

#pragma comment (lib, "zlib.lib")
#pragma comment (lib, "libpng.lib")


#include <windows.h>
#include <Shlobj.h>
#include <process.h>
#include "GL/glee.h"
#include <GL/glu.h>
//#include "zlib/zlib.h"
#include "png/png.h"

extern bool	active;		// Window Active Flag
extern const double PI;
extern const float size;
extern bool lowQuality;
extern bool done;//setting this to true will terminate the program

const int NumPlayers = 2;

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
#include "particleManager.h"
#include "cameraManager.h"
#include "path.h"
#include "level.h"
#include "script.h"
#include "profiler.h"
#include "explosion.h"
#include "imageloader.h"//should be replaced!!!
#include "partical.h"
#include "player.h"
#include "object.h"
#include "bullet.h" 
#include "missile.h"
#include "plane.h"
#include "aaGun.h"
#include "settings.h"
#include "objectList.h"
#include "worldManager.h"


extern humanControl players[NumPlayers];

extern int sh, sw;

extern GraphicsManager::gID fireParticleEffect;
extern GraphicsManager::gID smokeParticleEffect;
extern GraphicsManager::gID exaustParticleEffect;

extern float radarAng;//should be replaced by gameTime()

extern int frame,Time,timebase;
extern float fps;

#include "menuScreen.h"
#include "modeManager.h"