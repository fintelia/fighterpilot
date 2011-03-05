
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
	switch(uMsg)
	{
		case WM_KEYDOWN:
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
			return;

		case WM_KEYUP:
			up(wParam);
			sendCallbacks(new keyStroke(true, wParam));
			return;

		case WM_LBUTTONDOWN:
			leftMouse.down = true;
			leftMouse.x = LOWORD(lParam);
			leftMouse.y = HIWORD(lParam);
			sendCallbacks(new mouseClick(true, LEFT_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;
		case WM_MBUTTONDOWN:
			middleMouse.down = true;
			middleMouse.x = LOWORD(lParam);
			middleMouse.y = HIWORD(lParam);
			sendCallbacks(new mouseClick(true, MIDDLE_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;
		case WM_RBUTTONDOWN:
			rightMouse.down = true;
			rightMouse.x = LOWORD(lParam);
			rightMouse.y = HIWORD(lParam);
			sendCallbacks(new mouseClick(true, RIGHT_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;	
		case WM_LBUTTONUP:
			leftMouse.down = false;
			sendCallbacks(new mouseClick(false, LEFT_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;
		case WM_MBUTTONUP:
			middleMouse.down = false;
			sendCallbacks(new mouseClick(false, MIDDLE_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;
		case WM_RBUTTONUP:
			rightMouse.down = false;
			sendCallbacks(new mouseClick(false, RIGHT_BUTTON, LOWORD(lParam), HIWORD(lParam)));
			return;

		case WM_MOUSEWHEEL:
			sendCallbacks(new mouseScroll(double(GET_WHEEL_DELTA_WPARAM(wParam))/120.0));
			return;
	}
}