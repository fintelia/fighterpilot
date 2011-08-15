
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