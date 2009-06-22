
#pragma comment (lib,"xinput")

#include <commdlg.h>
#include <XInput.h> // XInput API
#include <basetsd.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
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

class xinput_input: public standard_input
{
public:
	CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
	xinput_input()
	{
		int i;
		for(i=0;i<256;i++)
			keys[i]=false;
		for(int l=0;l<4;l++)
		{
			for(i=0;i<14;i++)
				joy[i][l]=false;
			for(i=0;i<6;i++)
				axes[i][l]=0;
		}
		tPresses=0;
		XInputEnable( true );
		update();
	}

	void update()
	{
		DWORD dwResult;
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
				int curMask=0x00000001;
				for(int l=0;l<14;l++)
				{
					if(l==9)curMask=0x0100;
					if(l==11)curMask=0x1000;
					if(joy[l][i]!= ((g_Controllers[i].state.Gamepad.wButtons & curMask)!=0) )
					{
						tPresses++;
						lastKey=&joy[l][i];
						lastAscii=-1;
						joy[l][i]=((g_Controllers[i].state.Gamepad.wButtons & curMask)!=0);
					}
					curMask*=2;
				}//-----------------
				XINPUT_GAMEPAD Gamepad=g_Controllers[i].state.Gamepad;
				int oldAxes[6];
				for(int m=0;m<6;m++)
					oldAxes[m]=axes[m][i];
				axes[0][i]=(float)Gamepad.bLeftTrigger/255*1000;
				axes[1][i]=(float)Gamepad.bRightTrigger/255*1000;
				axes[2][i]=(float)Gamepad.sThumbLX/32768*1000;
				axes[3][i]=(float)Gamepad.sThumbLY/32768*1000;
				axes[4][i]=(float)Gamepad.sThumbRX/32768*1000;
				axes[5][i]=(float)Gamepad.sThumbRY/32768*1000;
				for(int l=0;l<6;l++)
				{
					if(abs(axes[l][i]-oldAxes[l])>500)
					{
						tPresses++;
						lastAxis=&axes[l][i];
						lastAscii=-2;
					}
				}
			}
		}
	}
	~xinput_input()
	{
		XInputEnable( false );
	}
};