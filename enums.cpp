
#include "main.h"

const int BULLET_SPEED		= 1000;// 2000 m/s
const int MISSILE_SPEED		= 1180;// 1180 m/s = mach 4 (max speed)
const double MAX_FPS		= 100.0;
const double UPDATE_LENGTH	= 20.0;

const Color white(1.0f,1.0f,1.0f);
const Color black(0.0f,0.0f,0.0f);
const Color red(1.0f,0.0f,0.0f);
const Color green(0.0f,1.0f,0.0f);
const Color blue(0.0f,0.0f,1.0f);
const Color darkBlue(0.11f,0.35f,0.52f);
const Color lightBlue(0.19f,0.58f,0.78f);
const Color darkGreen(0.49f,0.57f,0.06f);
const Color lightGreen(0.68f,0.76f,0.12f);
const Color darkGray(0.3f,0.3f,0.3f);
const Color lightGray(0.6f,0.6f,0.6f);
unsigned __int8 uPowerOfTwo(unsigned __int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return ++i;
}
unsigned __int16 uPowerOfTwo(unsigned __int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return ++i;
}
unsigned __int32 uPowerOfTwo(unsigned __int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return ++i;
}
unsigned __int64 uPowerOfTwo(unsigned __int64 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
	i |= i >> 32;
    return ++i;
}
__int8 uPowerOfTwo(__int8 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    return ++i;
}
__int16 uPowerOfTwo(__int16 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    return ++i;
}
__int32 uPowerOfTwo(__int32 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
    return ++i;
}
__int64 uPowerOfTwo(__int64 i)
{
    i--;
    i |= i >> 1;
    i |= i >> 2;
    i |= i >> 4;
    i |= i >> 8;
    i |= i >> 16;
	i |= i >> 32;
    return ++i;
}

objectType objectTypeFromString(string s)
{
	if(s=="f12")			return F12;
	if(s=="f16")			return F16;
	if(s=="f18")			return F18;
	if(s=="f22")			return F22;
	if(s=="UAV")			return UAV;
	if(s=="B2")				return B2;
	if(s=="missile1")		return MISSILE1;
	if(s=="missile2")		return MISSILE2;
	if(s=="missile3")		return MISSILE3;
	if(s=="missile4")		return MISSILE4;
	if(s=="missile1")		return BOMB1;
	if(s=="missile2")		return BOMB2;
	if(s=="AA gun")			return AA_GUN;
	if(s=="bullet cloud")	return BULLET_CLOUD;
	debugBreak();
	return 0;
}
string objectTypeString(objectType t)
{
	if(t==F12)			return "f12";
	if(t==F16)			return "f16";
	if(t==F18)			return "f18";
	if(t==F22)			return "f22";
	if(t==UAV)			return "UAV";
	if(t==B2)			return "B2";
	if(t==MISSILE1)		return "missile1";
	if(t==MISSILE2)		return "missile2";
	if(t==MISSILE3)		return "missile3";
	if(t==MISSILE4)		return "missile4";
	if(t==BOMB1)		return "missile1";
	if(t==BOMB2)		return "missile2";
	if(t==AA_GUN)		return "AA gun";
	if(t==BULLET_CLOUD)	return "bullet cloud";
	debugBreak();
	return "";
}