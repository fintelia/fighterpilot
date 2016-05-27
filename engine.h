

#if defined _MSC_VER

    #define VISUAL_STUDIO

    #pragma warning (disable : 4305)
    #pragma warning (disable : 4244)
    #pragma warning (disable : 4018)
    #pragma warning (disable : 4250)
    #pragma warning (disable : 4996)
    #pragma warning (disable : 4204)

    #pragma comment (linker, "/SUBSYSTEM:WINDOWS")
    #pragma comment (linker, "/ENTRY:WinMainCRTStartup")
    #pragma comment (lib, "Winmm.lib")
    #pragma comment (lib, "OpenGL32.lib")
    #pragma comment (lib, "glu32.lib")

    #pragma comment (lib, "libz.lib")
    #pragma comment (lib, "libpng15.lib")

//	#define XINPUT
//	#pragma comment (lib,"xinput")

	#define DIRECT_INPUT	//can cause performance issues?
	#pragma comment (lib, "dinput8.lib")
	#pragma comment (lib, "dxguid.lib")

	#pragma comment (lib, "dsound.lib")

#elif defined __GNUG__
    #define GCC_COMPILER
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
#include <deque>
#include <set>
#include <array>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include <cstdarg>
#include <iomanip>
#include <cassert>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
//--Boost C++ Library
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
//--Namespaces
using std::string;
using std::vector;
using std::map;
using std::set;
using std::queue;
using std::deque;
using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;
using std::dynamic_pointer_cast;
using std::static_pointer_cast;
using std::cout;
using std::endl;
using std::swap;
using boost::lexical_cast;
using namespace std::literals::string_literals;

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
	struct _XDisplay;
	typedef _XDisplay Display;
	extern Display* x11_display;
	extern int x11_screen;
	extern unsigned int x11_window;
#endif

extern int sh, sw;
extern float sAspect;

#include "definitions.h"

#include "time.h"
#include "gameMath.h"
#include "collide.h"
#include "random.h"
#include "settings.h"
#include "ephemeris.h"
#include "controlManager.h"
#include "graphicsManager.h"
#include "soundManager.h"
#include "fileManager.h"
#include "sceneManager.h"
#include "dataManager.h"
#include "assetLoader.h"
#include "particleManager.h"
#include "input.h"
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
	bool needsRedraw;
	bool done;

	bool hasCommandLineOption(string option)
	{
		for(auto i=commandLineOptions.begin(); i!=commandLineOptions.end();i++)
			if(*i == option) return true;
		return false;
	}
	bool isWideScreen()
	{
		return abs(sAspect-16.0/9.0) < abs(sAspect-4.0/3.0);//return whether the aspect ration is closer to 4:3 or 16:9
	}
	Game():active(true), needsRedraw(false), done(false){}
	virtual bool init();
	virtual void update();
};

extern unique_ptr<Game> game;
