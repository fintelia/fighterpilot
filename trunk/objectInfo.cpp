
#include "game.h"

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