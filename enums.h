
#pragma once
#include <map>


enum menuType{
	MENU			=0x0100,
	MAIN_MENU		=0x0101,
	CLOSED			=0x0102,
	SET_CONTROLS	=0x0104,
	START			=0x0108,
	CHOOSE_MODE		=0x0110
};
enum modeType{
	MODE			=0x0200,
	LOADING 		=0x0201,
	BLANK_MODE		=0x0202,
	TWO_PLAYER_VS	=0x0204,
	ONE_PLAYER		=0x0208,
	MAP_BUILDER		=0x0210
};

enum planeType{
	F12				=0x0401,
	F16				=0x0402,
	F18				=0x0404,
	F22				=0x0408,
	UAV				=0x0410,
	B2				=0x0420
};

enum inputCallbackType{
	KEY_STROKE		=0x0801,
	MOUSE_CLICK		=0x0802,
	MOUSE_SCROLL	=0x0804
};
enum mouseButton{
	LEFT_BUTTON		=0x1001,
	MIDDLE_BUTTON	=0x1002,
	RIGHT_BUTTON	=0x1004
};

extern planeType defaultPlane;
extern map<planeType,int> planeModels;

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
	float r,g,b;
	Color(float R, float G, float B): r(R), g(G), b(B){}
	Color(): r(0), g(0), b(0){}
};

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