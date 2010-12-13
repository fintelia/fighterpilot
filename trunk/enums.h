
#pragma once
#include <map>


enum planeType{
	PLANE			=0x1000,
	F12				=0x1001,
	F16				=0x1002,
	F18				=0x1003,
	F22				=0x1004,
	UAV				=0x1005,
	B2				=0x1006
};

enum inputCallbackType{
	INPUT_CALLBACK	=0x2000,
	KEY_STROKE		=0x2001,
	MOUSE_CLICK		=0x2002,
	MOUSE_SCROLL	=0x2003
};
enum mouseButton{
	MOUSE_BUTTON	=0x2000,
	LEFT_BUTTON		=0x2001,
	MIDDLE_BUTTON	=0x2002,
	RIGHT_BUTTON	=0x2003
};

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
//struct polyType
//{
//	union{
//		int i;
//		long l;
//		signed char c;
//		unsigned int ui;
//		unsigned long ul;
//		unsigned char uc;
//		float f;
//		double d;
//		bool b;
//		string s;
//		Vec2f f2;
//		Vec3f f3;
//		Quat4f f4;
//		Vec2d d2;
//		Vec3d d3;
//		Quat4d d4;
//		Angle a;
//		void* vPtr;
//	} value;
//	enum{
//		NONE		=0x00000,
//		CHAR		=0x00001,
//		INT			=0x00002,
//		LONG		=0x00004,
//		UCHAR		=0x00008,
//		UINT		=0x00010,
//		ULONG		=0x00020,
//		FLOAT		=0x00040,
//		DOUBLE		=0x00080,
//		BOOLEAN		=0x00100,
//		STRING		=0x00200,
//		VEC2F		=0x00400,
//		VEC3F		=0x00800,
//		QUATF		=0x01000,
//		VEC2D		=0x02000,
//		VEC3D		=0x04000,
//		QUAT4D		=0x08000,
//		ANGLE		=0x10000,
//		VOID_PTR	=0x20000
//	} type;
//}