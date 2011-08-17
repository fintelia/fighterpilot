
#pragma comment (lib,"xinput")

#include <commdlg.h>
#include <Xinput.h> // XInput API
#include <basetsd.h>
//#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 )

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
//#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
#define USING_XINPUT

struct CONTROLER_STATE
{
    XINPUT_STATE state;
    bool bConnected;
};

const int LEFT_TRIGGER		= 0;
const int RIGHT_TRIGGER		= 1;
const int THUMB_LX			= 2;
const int THUMB_LY			= 3;
const int THUMB_RX			= 4;
const int THUMB_RY			= 5;
const int DPAD_UP			= 6;
const int DPAD_DOWN			= 7;
const int DPAD_LEFT			= 8;
const int DPAD_RIGHT		= 9;
const int GAMEPAD_START		= 10;
const int GAMEPAD_BACK		= 11;
const int LEFT_THUMB		= 12;
const int RIGHT_THUMB		= 13;
const int LEFT_SHOULDER		= 14;
const int RIGHT_SHOULDER	= 15;
const int GAMEPAD_A			= 16;
const int GAMEPAD_B			= 17;
const int GAMEPAD_X			= 18;
const int GAMEPAD_Y			= 19;

const int GAMEPAD1_OFFSET	= 256;
const int GAMEPAD2_OFFSET	= 276;
const int GAMEPAD3_OFFSET	= 296;
const int GAMEPAD4_OFFSET	= 316;

class xinput_input: public standard_input
{
protected:
	bool joy[14][4];
	int axes[6 * 4];
	XINPUT_GAMEPAD gamepads[MAX_CONTROLLERS];
	bool compareButtons(XINPUT_GAMEPAD g1, XINPUT_GAMEPAD g2, int button)
	{
		return (g1.wButtons & button) == (g2.wButtons & button);
	}
public:
	float deadZone;

	CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
	xinput_input();

	void update();
	~xinput_input();
	virtual float operator() (int key);
};