

#if defined _MSC_VER

    #define VISUAL_STUDIO

    #pragma warning( disable : 4305)
    #pragma warning( disable : 4244)
    #pragma warning( disable : 4018)
    #pragma warning( disable : 4250)
    #pragma warning( disable : 4996)
    #pragma warning( disable : 4204)

    #pragma comment (linker, "/SUBSYSTEM:WINDOWS")
    #pragma comment (linker, "/ENTRY:WinMainCRTStartup")
    #pragma comment (lib, "Winmm.lib")
    #pragma comment (lib, "OpenGL32.lib")
    #pragma comment (lib, "glu32.lib")

    #pragma comment (lib, "libz.lib")
    #pragma comment (lib, "libpng15.lib")

	#pragma comment (lib,"xinput")

	#define XINPUT
	//#define DIRECT_INPUT //still need to write most of the code for directInput

#elif defined __GNUG__

    #define GCC_COMPILER

    #pragma GCC diagnostic ignored "-Wsign-compare"

#endif

#define NOMINMAX
typedef void* HANDLE;

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
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
//--Namespaces
using namespace std;
using boost::lexical_cast;

#ifndef UNICODE
#define UNICODE
#endif

extern const double PI;

const int NumPlayers = 2;

#define OPENGL
#ifdef _WIN32 
#define WINDOWS
#elif __linux__
#define LINUX
#endif
extern int sh, sw;
extern float sAspect;

#include "definitions.h"

#include "time.h"
#include "gameMath.h"
#include "random.h"
#include "ephemeris.h"
#include "fileManager.h"
#include "controlManager.h"
#include "graphicsManager.h"
#include "dataManager.h"
#include "assetLoader.h"
#include "sceneManager.h"
#include "particleManager.h"
#include "input.h"
#include "script.h"
#include "profiler.h"
#include "object.h"
#include "terrain.h"
#include "objectList.h"
#include "menuScreen.h"
#include "worldManager.h"

class Game
{
public:
	vector<string> commandLineOptions;
	bool active;
	bool done;
	float maxFrameRate;

	bool hasCommandLineOption(string option)
	{
		for(auto i=commandLineOptions.begin(); i!=commandLineOptions.end();i++)
			if(*i == option) return true;
		return false;
	}

	Game():active(true), done(false){}
	virtual bool init();
	virtual void update();
};

extern Game* game;
