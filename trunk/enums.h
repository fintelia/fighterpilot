
#pragma once
#include <map>

const int PLANE				= 0x1000;
const int F12				= 0x1000;
const int F16				= 0x1000;
const int F18				= 0x1000;
const int F22				= 0x1004;
const int UAV				= 0x1005;
const int B2				= 0x1006;

const int INPUT_CALLBACK	= 0x2000;
const int KEY_STROKE		= 0x2001;
const int MOUSE_CLICK		= 0x2002;
const int MOUSE_SCROLL		= 0x2003;

const int MOUSE_BUTTON		= 0x2100;
const int LEFT_BUTTON		= 0x2101;
const int MIDDLE_BUTTON		= 0x2102;
const int RIGHT_BUTTON		= 0x2103;


typedef int planeType;
typedef int inputCallbackType;
typedef int mouseButton;

extern planeType defaultPlane;

struct controlState
{
	float	climb;
	float	dive;
	float	accelerate;
	float	brake;
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

extern char* errorString;
#define glError(){									\
	errorString=(char*)gluErrorString(glGetError());\
	if(string(errorString).compare("no error") != 0)\
		__debugbreak();								\
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