

#pragma warning( disable : 4305)
#pragma warning( disable : 4244)
#pragma warning( disable : 4018)
#pragma warning( disable : 4250)
#pragma warning( disable : 4996)
#pragma warning( disable : 4204)

#include "debugBreak.h"

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <set>
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
#include <boost/algorithm/string.hpp>
//--Namespaces
using namespace std;
using namespace boost;
using namespace boost::filesystem;



#pragma comment (linker, "/SUBSYSTEM:WINDOWS")
#pragma comment (linker, "/ENTRY:WinMainCRTStartup")
#pragma comment (lib, "Winmm.lib")
#pragma comment (lib, "OpenGL32.lib")
#pragma comment (lib, "glu32.lib")

#pragma comment (lib, "zlib.lib")
#pragma comment (lib, "libpng15.lib")
#pragma comment (lib, "tinyxml.lib")

#include <windows.h>
#include <Shlobj.h>
#include <process.h>
#include "GL/glee.h"
#include <GL/glu.h>
//#include "zlib/zlib.h"
#include "png/png.h"
#include "xml/tinyxml.h"


extern bool	active;		// Window Active Flag
extern const double PI;
extern bool lowQuality;
extern bool done;//setting this to true will terminate the program

const int NumPlayers = 2;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
namespace menu{class levelEditor;}

#define OPENGL2

#include "enums.h"

#include "time.h"
#include "text.h"
#include "gameMath.h"
#include "random.h"
#include "graphicsManager.h"
#include "dataManager.h"
#include "particleManager.h"
#include "cameraManager.h"
#include "input.h"
#ifdef _DEBUG
#include "XboxContInput.h"
#endif
#include "path.h"
#include "script.h"
#include "imageloader.h"//should be replaced!!!
#include "profiler.h"
#include "player.h"
#include "object.h"
#include "level.h"
#include "objectList.h"
#include "menuScreen.h"
#include "modeManager.h"
#include "worldManager.h"

extern int sh, sw;
extern float sAspect;
extern int frame,Time,timebase;
extern float fps;

class Game
{
public:
	void init();
	void update();
};

extern Game* game;