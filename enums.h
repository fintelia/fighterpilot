
#pragma once
#include <map>

//nst int TYPE				= 0xXXxx;
const int PLANE				= 0x0100;
const int F12				= 0x0100;
const int F16				= 0x0100;
const int F18				= 0x0100;
const int F22				= 0x0104;
const int UAV				= 0x0105;
const int B2				= 0x0106;

const int AA_GUN			= 0x0200;

const int MISSILE			= 0x0400;


const int NEUTRAL			= 0x00;
const int TEAM0				= 0x01;
const int TEAM1				= 0x02;
const int TEAM2				= 0x04;
const int TEAM3				= 0x08;
const int TEAM4				= 0x10;
const int TEAM5				= 0x20;
const int TEAM6				= 0x40;
const int TEAM7				= 0x80;

const int INPUT_CALLBACK	= 0x100;
const int KEY_STROKE		= 0x101;
const int MOUSE_CLICK		= 0x102;
const int MOUSE_SCROLL		= 0x103;

const int MOUSE_BUTTON		= 0x200;
const int LEFT_BUTTON		= 0x201;
const int MIDDLE_BUTTON		= 0x202;
const int RIGHT_BUTTON		= 0x203;

const int CONTROL_NONE		= 0x001;
const int CONTROL_HUMAN		= 0x002;
const int CONTROL_COMPUTER	= 0x004;

typedef int objectType;
typedef int planeType;
typedef int missileType;
typedef int controlType;

typedef int teamNum;
typedef int inputCallbackType;
typedef int mouseButton;

extern planeType defaultPlane;

extern const int BULLET_SPEED;
extern const int MISSILE_SPEED;

const double MAX_FPS		= 60.0;

extern planeType planeTypeFromString(string s);

struct controlState
{
	union{
		float climb;
		float up;
	};
	union{
		float dive;
		float down;
	};
	union{
		float accelerate;
		float forward;
	};
	union{
		float brake;
		float reverse;
	};
	float	right;
	float	left;
	float	shoot1;
	float	shoot2;
	controlState(): climb(0), dive(0), accelerate(0), brake(0), right(0), left(0), shoot1(0), shoot2(0) {}
};

struct Color
{
	float r,g,b,a;
	Color(float R, float G, float B, float A=1.0): r(R), g(G), b(B), a(A){}
	Color(): r(0), g(0), b(0),a(0){}
};

extern const Color white;
extern const Color black;
extern const Color darkBlue;
extern const Color lightBlue;
extern const Color darkGreen;
extern const Color lightGreen;

template<class A, class B>
class functor
{
public:
	virtual A operator() (B b)=0;
};

template<class A>
class functor<A,void>
{
public:
	virtual A operator() ()=0;
};

template<class B>
class functor<void,B>
{
public:
	virtual void operator() (B b)=0;
};

extern void minimizeWindow();
#ifdef _DEBUG
	#define debugBreak(){			\
		minimizeWindow();			\
		__debugbreak();				\
	}
#else if
	#define debugBreak() {}
#endif

extern char* errorString;
#define glError(){										\
	errorString=(char*)gluErrorString(glGetError());	\
	if(string(errorString).compare("no error") != 0){	\
		debugBreak()									\
	}													\
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

extern unsigned __int8 uPowerOfTwo(unsigned __int8 i);
extern unsigned __int16 uPowerOfTwo(unsigned __int16 i);
extern unsigned __int32 uPowerOfTwo(unsigned __int32 i);
extern unsigned __int64 uPowerOfTwo(unsigned __int64 i);
extern __int8 uPowerOfTwo(__int8 i);
extern __int16 uPowerOfTwo(__int16 i);
extern __int32 uPowerOfTwo(__int32 i);
extern __int64 uPowerOfTwo(__int64 i);

