
#include "main.h"

void Input::sendCallbacks(callBack* c)
{
	menuManager.inputCallback(c);
}
standard_input::standard_input(): Input()
{
	int i;
	for(i=0;i<256;i++)
		keys[i]=false;
	inputMutex=CreateMutex(NULL,false,NULL);
	tPresses=0;
}
standard_input::~standard_input()
{
	CloseHandle( inputMutex );
}
void standard_input::down(int k)
{
	if(k>=256 || k<0) return;
	WaitForSingleObject( inputMutex, INFINITE );
	keys[k]=true;
	tPresses++;
	lastKey=k;
	ReleaseMutex( inputMutex );


}
void standard_input::up(int k)
{
	if(k>=256 || k<0) return;
	WaitForSingleObject( inputMutex, INFINITE );
	keys[k]=false;
	ReleaseMutex( inputMutex );
}
bool standard_input::getKey(int key)
{
	if(key>=256 || key<0) false;
	WaitForSingleObject( inputMutex, INFINITE );
	bool b=keys[key];
	ReleaseMutex( inputMutex );
	return b;
}
void standard_input::update()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mousePos.set(((float)cursorPos.x) / sh, 1.0 - ((float)cursorPos.y) / sh);

}
const Input::mouseButtonState& standard_input::getMouseState(mouseButton m)
{
	if(m == LEFT_BUTTON)		return leftMouse;
	else if(m == MIDDLE_BUTTON) return middleMouse;
	else						return rightMouse;
}
void standard_input::windowsInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_KEYDOWN)
	{
		if((lParam & 0x40000000)==0)//if key was not already down
		{
			if(wParam == VK_SNAPSHOT || wParam == VK_INSERT)
			{
				graphics->takeScreenshot();
			}
			else
			{
				down(wParam);
				sendCallbacks(new keyStroke(false, wParam));
			}
		}
	}
	else if(uMsg == WM_KEYUP)
	{
		up(wParam);
		sendCallbacks(new keyStroke(true, wParam));
	}
	else if(uMsg == WM_LBUTTONDOWN)
	{
		leftMouse.down = true;
		leftMouse.downPos.x = (float)LOWORD(lParam) / sh;
		leftMouse.downPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, LEFT_BUTTON, leftMouse.downPos));
	}
	else if(uMsg == WM_MBUTTONDOWN)
	{
		middleMouse.down = true;
		middleMouse.downPos.x = (float)LOWORD(lParam) / sh;
		middleMouse.downPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, MIDDLE_BUTTON, middleMouse.downPos));
	}
	else if(uMsg == WM_RBUTTONDOWN)
	{
		rightMouse.down = true;
		rightMouse.downPos.x = (float)LOWORD(lParam) / sh;
		rightMouse.downPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, RIGHT_BUTTON, rightMouse.downPos));
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		leftMouse.down = false;
		leftMouse.upPos.x = (float)LOWORD(lParam) / sh;
		leftMouse.upPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, LEFT_BUTTON, leftMouse.upPos));
	}
	else if(uMsg == WM_MBUTTONUP)
	{
		middleMouse.down = false;
		middleMouse.upPos.x = (float)LOWORD(lParam) / sh;
		middleMouse.upPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, MIDDLE_BUTTON, middleMouse.upPos));
	}
	else if(uMsg == WM_RBUTTONUP)
	{
		rightMouse.down = false;
		rightMouse.upPos.x = (float)LOWORD(lParam) / sh;
		rightMouse.upPos.y = 1.0 - (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, RIGHT_BUTTON, rightMouse.upPos));
	}
	else if(uMsg == WM_MOUSEWHEEL)
	{
		sendCallbacks(new mouseScroll(double(GET_WHEEL_DELTA_WPARAM(wParam))/120.0));
	}
}

xinput_input::xinput_input(): standard_input(), deadZone(0.25)
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

void xinput_input::update()
{
	standard_input::update();

	double t = GetTime();
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
	t = GetTime() - t;
	t = GetTime();
}
xinput_input::~xinput_input()
{
	//XInputEnable( false );
}
float xinput_input::operator() (int key) 
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

	if(i >= 0.0)
		return max(0.0,i/(1.0-deadZone) - deadZone);
	else
		return min(0.0,i/(1.0-deadZone) + deadZone);
}