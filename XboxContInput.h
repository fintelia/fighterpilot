
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
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.
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
	CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
	xinput_input(): standard_input()
	{
		int i;
		for(int l=0;l<4;l++)
		{
			for(i=0;i<14;i++)
				joy[i][l]=false;
			for(i=0;i<6;i++)
				axes[i*l]=0;
		}
		tPresses=0;
		//XInputEnable( true );
		update();
	}

	void update()
	{
		DWORD dwResult;
		int contN=256;
		for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
		{
			// Simply get the state of the controller from XInput.
			dwResult = XInputGetState( i, &g_Controllers[i].state );

			if( dwResult == ERROR_SUCCESS )
				g_Controllers[i].bConnected = true;
			else
				g_Controllers[i].bConnected = false;
			if( g_Controllers[i].bConnected )
			{
				//int curMask=0x00000001;
				//for(int l=0;l<14;l++)
				//{
				//	if(l==9)curMask=0x0100;
				//	if(l==11)curMask=0x1000;
				//	if(joy[l][i]!= ((g_Controllers[i].state.Gamepad.wButtons & curMask)!=0) )
				//	{
				//		//tPresses++;
				//		//lastKey=&joy[l][i];
				//		//lastAscii=-1;
				//		joy[l][i]=((g_Controllers[i].state.Gamepad.wButtons & curMask)!=0);
				//	}
				//	curMask*=2;
				//}//-----------------
				XINPUT_GAMEPAD oldGamePad=gamepads[i];
				gamepads[i]=g_Controllers[i].state.Gamepad;
				int lKey=0;

				if(abs(oldGamePad.bLeftTrigger-gamepads[i].bLeftTrigger)>60)				lKey = LEFT_TRIGGER + GAMEPAD1_OFFSET + i*20;
				if(abs(oldGamePad.bRightTrigger-gamepads[i].bRightTrigger)>60)				lKey = RIGHT_TRIGGER + GAMEPAD1_OFFSET + i*20;
				if(abs(oldGamePad.sThumbLX-gamepads[i].sThumbLX)>5000)						lKey = THUMB_LX + GAMEPAD1_OFFSET + i*20;
				if(abs(oldGamePad.sThumbLY-gamepads[i].sThumbLY)>5000)						lKey = THUMB_LY + GAMEPAD1_OFFSET + i*20;
				if(abs(oldGamePad.sThumbRX-gamepads[i].sThumbRX)>5000)						lKey = THUMB_RX + GAMEPAD1_OFFSET + i*20;
				if(abs(oldGamePad.sThumbRY-gamepads[i].sThumbRY)>5000)						lKey = THUMB_RY + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_DPAD_UP))			lKey = DPAD_UP + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_DPAD_DOWN))		lKey = DPAD_DOWN + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_DPAD_LEFT))		lKey = DPAD_LEFT + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_DPAD_RIGHT))		lKey = DPAD_RIGHT + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_START))			lKey = GAMEPAD_START + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_BACK))				lKey = GAMEPAD_BACK + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_LEFT_THUMB))		lKey = LEFT_THUMB + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_RIGHT_THUMB))		lKey = RIGHT_THUMB + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_LEFT_SHOULDER))	lKey = LEFT_SHOULDER + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_RIGHT_SHOULDER))	lKey = RIGHT_SHOULDER + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_A))				lKey = GAMEPAD_A + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_B))				lKey = GAMEPAD_B + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_X))				lKey = GAMEPAD_X + GAMEPAD1_OFFSET + i*20;
				if(!compareButtons(oldGamePad,gamepads[i],XINPUT_GAMEPAD_Y))				lKey = GAMEPAD_Y + GAMEPAD1_OFFSET + i*20;
				if(lKey!=0)
				{
					tPresses++;
					lastKey=lKey;
				}

				//int oldAxes[6];
				//for(int m=0;m<6;m++)
				//	oldAxes[m]=axes[m*i];
				//axes[0*i]=(float)Gamepad.bLeftTrigger/255*1000;
				//axes[1*i]=(float)Gamepad.bRightTrigger/255*1000;
				//axes[2*i]=(float)Gamepad.sThumbLX/32768*1000;
				//axes[3*i]=(float)Gamepad.sThumbLY/32768*1000;
				//axes[4*i]=(float)Gamepad.sThumbRX/32768*1000;
				//axes[5*i]=(float)Gamepad.sThumbRY/32768*1000;
	
				//for(int l=0;l<6;l++)
				//{
				//	if(abs(axes[l*i]-oldAxes[l])>500)
				//	{
				//		tPresses++;
				//		lastKey=contN;
				//	}
				//	contN++;
				//}
			}
		}
	}
	~xinput_input()
	{
		//XInputEnable( false );
	}
	virtual float operator() (int key) 
	{
		float i=0;
		WaitForSingleObject( inputMutex, INFINITE );

		if(key>=0 && key<256)
			i=keys[key] ? 1 : 0;
		else if(key>=GAMEPAD1_OFFSET && key<GAMEPAD4_OFFSET+20)
		{
			int a=(key-GAMEPAD1_OFFSET)/20;
			key -= a*20 + GAMEPAD1_OFFSET;
			if(key == LEFT_TRIGGER)		i = 1.0/255*gamepads[a].bLeftTrigger;
			if(key == RIGHT_TRIGGER)	i = 1.0/255*gamepads[a].bRightTrigger;
			if(key == THUMB_LX)			i = 1.0/32767*gamepads[a].sThumbLX;
			if(key == THUMB_LY)			i = 1.0/32767*gamepads[a].sThumbLY;
			if(key == THUMB_RX)			i = 1.0/32767*gamepads[a].sThumbRX;
			if(key == THUMB_RY)			i = 1.0/32767*gamepads[a].sThumbRY;
			if(key == DPAD_UP)			i = gamepads[a].wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1 : 0;
			if(key == DPAD_DOWN)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1 : 0;
			if(key == DPAD_LEFT)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1 : 0;
			if(key == DPAD_RIGHT)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0;
			if(key == GAMEPAD_START)	i = gamepads[a].wButtons & XINPUT_GAMEPAD_START ? 1 : 0;
			if(key == GAMEPAD_BACK)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_BACK ? 1 : 0;
			if(key == LEFT_THUMB)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1 : 0;
			if(key == RIGHT_THUMB)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1 : 0;
			if(key == LEFT_SHOULDER)	i = gamepads[a].wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1 : 0;
			if(key == RIGHT_SHOULDER)	i = gamepads[a].wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1 : 0;
			if(key == GAMEPAD_A)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_A ? 1 : 0;
			if(key == GAMEPAD_B)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_B ? 1 : 0;
			if(key == GAMEPAD_X)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_X ? 1 : 0;
			if(key == GAMEPAD_Y)		i = gamepads[a].wButtons & XINPUT_GAMEPAD_Y ? 1 : 0;
		}
		ReleaseMutex( inputMutex );
		return i;
	}
};