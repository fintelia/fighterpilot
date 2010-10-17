
#pragma once
#include <map>


enum menuType{
	MENU			=0x100,
	MAIN_MENU		=0x101,
	CLOSED			=0x102,
	SET_CONTROLS	=0x104,
	START			=0x108,
	CHOOSE_MODE		=0x110
};
enum modeType{
	MODE			=0x200,
	LOADING 		=0x201,
	BLANK_MODE		=0x202,
	TWO_PLAYER_VS	=0x204,
	ONE_PLAYER		=0x208,
	MAP_BUILDER		=0x210
};

enum planeType{
	F12				=0x401,
	F16				=0x402,
	F18				=0x404,
	F22				=0x408,
	UAV				=0x410,
	B2				=0x420
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