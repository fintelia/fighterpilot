
//////////////////////////////////////////////////////////////////
const int PLANE						= 0x0100;
const int F12						= 0x0101;
const int F16						= 0x0102;
const int F18						= 0x0103;
const int F22						= 0x0104;
const int UAV						= 0x0105;
const int B2						= 0x0106;
const int MIRAGE					= 0x0107;
const int PLAYER_PLANE				= 0x01ff;

const int ANTI_AIRCRAFT_ARTILLARY	= 0x0200;
const int AA_GUN					= 0x0201;
const int SAM_BATTERY				= 0x0202;
const int FLAK_CANNON				= 0x0203;

const int MISSILE					= 0x0400;
const int MISSILE1					= 0x0401;
const int MISSILE2					= 0x0402;
const int MISSILE3					= 0x0403;
const int MISSILE4					= 0x0404;
const int SAM_MISSILE				= 0x0405;
const int FLAK_MISSILE				= 0x0406;
const int R550_MAGIC_MISSILE		= 0x0407;
const int SUPER_530_MISSILE			= 0x0408;

const int BOMB						= 0x0800;
const int BOMB1						= 0x0801;
const int BOMB2						= 0x0802;

const int BULLET_CLOUD				= 0x1000;

//////////////////////////////////////////////////////////////////
typedef int planeType;
typedef int missileType;
typedef int bombType;

//////////////////////////////////////////////////////////////////
extern planeType defaultPlane;

//class ObjectInfo
//{
//public:
//	objectType typeFromString(string s);
//	string typeString(objectType t);
//};