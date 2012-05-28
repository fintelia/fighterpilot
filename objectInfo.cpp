
#include "game.h"

objectType objectTypeFromString(string s)
{
	if(s=="f12")				return F12;
	if(s=="f16")				return F16;
	if(s=="f18")				return F18;
	if(s=="f22")				return F22;
	if(s=="UAV")				return UAV;
	if(s=="b2")					return B2;
	if(s=="mirage")				return MIRAGE;
	if(s=="j37")				return J37;
	if(s=="missile1")			return BOMB1;
	if(s=="bomb1")				return BOMB1;
	if(s=="missile2")			return BOMB2;
	if(s=="bomb2")				return BOMB2;
	if(s=="missile3")			return MISSILE3;
	if(s=="missile4")			return MISSILE4;
	if(s=="r550 magic missile")	return R550_MAGIC_MISSILE;
	if(s=="super530 missile")	return SUPER_530_MISSILE;
	if(s=="j37 missile")		return J37_MISSILE;
	if(s=="SAM missile")		return SAM_MISSILE;
	if(s=="flak missile")		return FLAK_MISSILE;
	if(s=="AA gun")				return AA_GUN;
	if(s=="SAM battery")		return SAM_BATTERY;
	if(s=="flak cannon")		return FLAK_CANNON;
	if(s=="bullet cloud")		return BULLET_CLOUD;
	if(s=="PLAYER_PLANE")		return PLAYER_PLANE;
	debugBreak();
	return 0;
}
string objectTypeString(objectType t)
{
	if(t==F12)					return "f12";
	if(t==F16)					return "f16";
	if(t==F18)					return "f18";
	if(t==F22)					return "f22";
	if(t==UAV)					return "UAV";
	if(t==B2)					return "b2";
	if(t==MIRAGE)				return "mirage";
	if(t==J37)					return "j37";
	if(t==MISSILE1)				return "missile1";
	if(t==MISSILE2)				return "missile2";
	if(t==MISSILE3)				return "missile3";
	if(t==MISSILE4)				return "missile4";
	if(t==BOMB1)				return "missile1";
	if(t==BOMB2)				return "missile2";
	if(t==R550_MAGIC_MISSILE)	return "r550 magic missile";
	if(t==SUPER_530_MISSILE)	return "super530 missile";
	if(t==J37_MISSILE)			return "j37 missile";
	if(t==SAM_MISSILE)			return "SAM missile";
	if(t==FLAK_MISSILE)			return "flak missile";
	if(t==AA_GUN)				return "AA gun";
	if(t==SAM_BATTERY)			return "SAM battery";
	if(t==FLAK_CANNON)			return "flak cannon";
	if(t==BULLET_CLOUD)			return "bullet cloud";
	if(t==PLAYER_PLANE)			return "PLAYER_PLANE";
	debugBreak();
	return "";
}