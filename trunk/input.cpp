
#include "engine.h"
#include <Windows.h>
#ifdef XINPUT
#include <Xinput.h>
#endif

const unsigned char KEYSTATE_CURRENT	= 0x01;
const unsigned char KEYSTATE_LAST		= 0x02; 
const unsigned char KEYSTATE_VIRTUAL	= 0x04;


InputManager::xboxControllerState::xboxControllerState():
#ifdef XINPUT
                    state(new XINPUT_STATE),
#else
                    state(nullptr),
#endif
                    connected(false),deadZone(0.15)
{

}
InputManager::xboxControllerState::~xboxControllerState()
{
#ifdef XINPUT
	delete state;
#endif
}

bool InputManager::xboxControllerState::getButton(int b) const
{
#ifdef XINPUT
	return connected && ((state->Gamepad.wButtons & b) != 0);
#else
	return false;
#endif
}
float InputManager::xboxControllerState::getAxis(int a) const
{
#ifdef XINPUT
	if(!connected)
		return 0.0f;

	float f = 0.0;
	if(a == XINPUT_LEFT_TRIGGER)		f = 1.0/255 * state->Gamepad.bLeftTrigger;
	if(a == XINPUT_RIGHT_TRIGGER)		f = 1.0/255 * state->Gamepad.bRightTrigger;
	if(a == XINPUT_THUMB_LX)			f = 1.0/32768 * state->Gamepad.sThumbLX;
	if(a == XINPUT_THUMB_LY)			f = 1.0/32768 * state->Gamepad.sThumbLY;
	if(a == XINPUT_THUMB_RX)			f = 1.0/32768 * state->Gamepad.sThumbRX;
	if(a == XINPUT_THUMB_RY)			f = 1.0/32768 * state->Gamepad.sThumbRY;

	debugAssert(deadZone > -1.0 && deadZone < 1.0);

	if(f < -deadZone)
		return (f + deadZone) / (1.0 - deadZone);
	else if(f < deadZone)
		return 0.0;
	else
		return (f - deadZone) / (1.0 - deadZone);
#else
	return 0.0f;
#endif
}
void InputManager::sendCallbacks(callBack* c)
{
	menuManager.inputCallback(c);
}
InputManager::InputManager(): tPresses(0)
{
	for(int i=0; i<256; i++)
	{
		keys[i]=false;
	}

	for(int i=0; i<4; i++)
	{
		xboxControllers[i].connected = true;
	}
	update();
}
InputManager::~InputManager()
{

}
void InputManager::down(int k)
{
	if(k>=256 || k<0) return;
	inputMutex.lock();
	keys[k] |= KEYSTATE_VIRTUAL;
	tPresses++;
	lastKey=k;
	inputMutex.unlock();
}
void InputManager::up(int k)
{
	if(k>=256 || k<0) return;
	inputMutex.lock();
	keys[k] &= ~KEYSTATE_VIRTUAL;
	inputMutex.unlock();
}
bool InputManager::getKey(int key)
{
	if(key>=256 || key<0) return false;
	return (keys[key] & KEYSTATE_VIRTUAL) != 0;
}
const InputManager::xboxControllerState& InputManager::getXboxController(unsigned char controllerNum)
{
	return xboxControllers[clamp(controllerNum,0,3)];
}

void InputManager::update()
{
	POINT cursorPos;
	GetCursorPos(&cursorPos);


	static char newKeyStates[256];
	GetKeyboardState((PBYTE)newKeyStates);


#ifdef USING_XINPUT
	bool xboxControllersConnected[4];
	XINPUT_STATE newXboxControllerStates[4];
	for(int i = 0; i < 4; i++)
	{
		if(xboxControllers[i].connected)
		{
			xboxControllersConnected[i] = XInputGetState(i, &newXboxControllerStates[i]) == ERROR_SUCCESS;
		}
		else
		{
			xboxControllersConnected[i] = false;
		}
	}
#endif
////////////////////////////////////////////////////mutex lock////////////////////////////////////////////////////
	inputMutex.lock();

	mousePos.set(((float)cursorPos.x) / sh, ((float)cursorPos.y) / sh);

	for(int i = 0; i<256; i++)
	{
		setBit(keys[i], KEYSTATE_LAST, keys[i] & KEYSTATE_CURRENT);
		setBit(keys[i], KEYSTATE_CURRENT, (newKeyStates[i] & 0x80) != 0);
		if(!(keys[i] & KEYSTATE_LAST) && (keys[i] & KEYSTATE_CURRENT)) //if key was just pressed
			setBit(keys[i], KEYSTATE_VIRTUAL, true);
		else if((keys[i] & KEYSTATE_LAST) && !(keys[i] & KEYSTATE_CURRENT)) //if key was just released
			setBit(keys[i], KEYSTATE_VIRTUAL, false);
	}

#ifdef USING_XINPUT
	for(int i = 0; i<4; i++)
	{
		xboxControllers[i].connected = xboxControllersConnected[i];
		if(xboxControllers[i].connected)
		{
			*xboxControllers[i].state = newXboxControllerStates[i];
		}
	}
#endif

	inputMutex.unlock();
}/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const InputManager::mouseButtonState& InputManager::getMouseState(mouseButton m)
{
	if(m == LEFT_BUTTON)		return leftMouse;
	else if(m == MIDDLE_BUTTON) return middleMouse;
	else						return rightMouse;
}
void InputManager::windowsInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			//	down(wParam);
				sendCallbacks(new keyStroke(false, wParam));
			}
		}
	}
	else if(uMsg == WM_KEYUP)
	{
	//	up(wParam);
		sendCallbacks(new keyStroke(true, wParam));
	}
	else if(uMsg == WM_LBUTTONDOWN)
	{
		leftMouse.down = true;
		leftMouse.downPos.x = (float)LOWORD(lParam) / sh;
		leftMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, LEFT_BUTTON, leftMouse.downPos));
	}
	else if(uMsg == WM_MBUTTONDOWN)
	{
		middleMouse.down = true;
		middleMouse.downPos.x = (float)LOWORD(lParam) / sh;
		middleMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, MIDDLE_BUTTON, middleMouse.downPos));
	}
	else if(uMsg == WM_RBUTTONDOWN)
	{
		rightMouse.down = true;
		rightMouse.downPos.x = (float)LOWORD(lParam) / sh;
		rightMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, RIGHT_BUTTON, rightMouse.downPos));
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		leftMouse.down = false;
		leftMouse.upPos.x = (float)LOWORD(lParam) / sh;
		leftMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, LEFT_BUTTON, leftMouse.upPos));
	}
	else if(uMsg == WM_MBUTTONUP)
	{
		middleMouse.down = false;
		middleMouse.upPos.x = (float)LOWORD(lParam) / sh;
		middleMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, MIDDLE_BUTTON, middleMouse.upPos));
	}
	else if(uMsg == WM_RBUTTONUP)
	{
		rightMouse.down = false;
		rightMouse.upPos.x = (float)LOWORD(lParam) / sh;
		rightMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, RIGHT_BUTTON, rightMouse.upPos));
	}
	else if(uMsg == WM_MOUSEWHEEL)
	{
		sendCallbacks(new mouseScroll(double(GET_WHEEL_DELTA_WPARAM(wParam))/120.0));
	}
}
float InputManager::operator() (int key)
{
	float i=0;
	inputMutex.lock();


	if(key>=0 && key<256)
		i=keys[key] ? 1 : 0;
#ifdef XINPUT
	else if(key>=XBOX_GAMEPAD_OFFSET[0] && key<XBOX_GAMEPAD_OFFSET[3]+20)
	{
		int a=(key-XBOX_GAMEPAD_OFFSET[0])/20;
		key -= a*20 + XBOX_GAMEPAD_OFFSET[0];
		if(key == XINPUT_LEFT_TRIGGER)		i = 1.0/255*xboxControllers[a].state->Gamepad.bLeftTrigger;
		if(key == XINPUT_RIGHT_TRIGGER)		i = 1.0/255*xboxControllers[a].state->Gamepad.bRightTrigger;
		if(key == XINPUT_THUMB_LX)			i = 1.0/32767*xboxControllers[a].state->Gamepad.sThumbLX;
		if(key == XINPUT_THUMB_LY)			i = 1.0/32767*xboxControllers[a].state->Gamepad.sThumbLY;
		if(key == XINPUT_THUMB_RX)			i = 1.0/32767*xboxControllers[a].state->Gamepad.sThumbRX;
		if(key == XINPUT_THUMB_RY)			i = 1.0/32767*xboxControllers[a].state->Gamepad.sThumbRY;
		if(key == XINPUT_DPAD_UP)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1 : 0;
		if(key == XINPUT_DPAD_DOWN)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1 : 0;
		if(key == XINPUT_DPAD_LEFT)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1 : 0;
		if(key == XINPUT_DPAD_RIGHT)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0;
		if(key == XINPUT_GAMEPAD_START)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1 : 0;
		if(key == XINPUT_GAMEPAD_BACK)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1 : 0;
		if(key == XINPUT_LEFT_THUMB)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1 : 0;
		if(key == XINPUT_RIGHT_THUMB)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1 : 0;
		if(key == XINPUT_LEFT_SHOULDER)		i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1 : 0;
		if(key == XINPUT_RIGHT_SHOULDER)	i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1 : 0;
		if(key == XINPUT_GAMEPAD_A)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1 : 0;
		if(key == XINPUT_GAMEPAD_B)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1 : 0;
		if(key == XINPUT_GAMEPAD_X)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1 : 0;
		if(key == XINPUT_GAMEPAD_Y)			i = xboxControllers[a].state->Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1 : 0;
	}
#endif
	inputMutex.unlock();

	const float deadZone = 0.15;
	if(i >= 0.0)
	{
		return max(0.0,i/(1.0-deadZone) - deadZone);
	}
	else
	{
		return min(0.0,i/(1.0-deadZone) + deadZone);
	}
}
void InputManager::checkNewHardware()
{
	for(int i=0; i<4; i++)
	{
		xboxControllers[i].connected = true;
	}
}
//#ifdef XINPUT
//
//xinput_input::xinput_input(): Input(), deadZone(0.25)
//{
//	int i;
//	for(int l=0;l<4;l++)
//	{
//		for(i=0;i<14;i++)
//			joy[i][l]=false;
//		for(i=0;i<6;i++)
//			axes[i*l]=0;
//
//		controllers[l].connected = true; // controllers are updated only if they are attached
//	}
//	//XInputEnable( true );
//	update();
//
//}
//void xinput_input::checkNewHardware()
//{
//	controllers[0].connected = true;
//	controllers[1].connected = true;
//	controllers[2].connected = true;
//	controllers[3].connected = true;
//}
//bool xinput_input::controllerConnected(int controller)
//{
//	return controller >= 0 && controller < 4 ? controllers[0].connected : false;
//}
//bool xinput_input::compareButtons(const XINPUT_GAMEPAD& g1, const XINPUT_GAMEPAD& g2, int button)
//{
//	return (g1.wButtons & button) == (g2.wButtons & button);
//}
//void xinput_input::update()
//{
//	Input::update();
//
//	DWORD dwResult;
//	int contN=256;
//	for(int i = 0; i < 4; i++)
//	{
//		if(controllers[i].connected)
//		{
//			XINPUT_GAMEPAD oldGamePad=controllers[i].state.Gamepad;
//
//			dwResult = XInputGetState( i, &controllers[i].state );
//
//			if(dwResult == ERROR_SUCCESS)
//			{
//				controllers[i].connected = true;
//
//				int lKey=0;
//
//				if(abs(oldGamePad.bLeftTrigger-controllers[i].state.Gamepad.bLeftTrigger)>60)				lKey = XBOX_LEFT_TRIGGER + XBOX_GAMEPAD_OFFSET[i];
//				if(abs(oldGamePad.bRightTrigger-controllers[i].state.Gamepad.bRightTrigger)>60)				lKey = XBOX_RIGHT_TRIGGER + XBOX_GAMEPAD_OFFSET[i];
//				if(abs(oldGamePad.sThumbLX-controllers[i].state.Gamepad.sThumbLX)>5000)						lKey = XBOX_THUMB_LX + XBOX_GAMEPAD_OFFSET[i];
//				if(abs(oldGamePad.sThumbLY-controllers[i].state.Gamepad.sThumbLY)>5000)						lKey = XBOX_THUMB_LY + XBOX_GAMEPAD_OFFSET[i];
//				if(abs(oldGamePad.sThumbRX-controllers[i].state.Gamepad.sThumbRX)>5000)						lKey = XBOX_THUMB_RX + XBOX_GAMEPAD_OFFSET[i];
//				if(abs(oldGamePad.sThumbRY-controllers[i].state.Gamepad.sThumbRY)>5000)						lKey = XBOX_THUMB_RY + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_DPAD_UP))			lKey = XBOX_DPAD_UP + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_DPAD_DOWN))		lKey = XBOX_DPAD_DOWN + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_DPAD_LEFT))		lKey = XBOX_DPAD_LEFT + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_DPAD_RIGHT))		lKey = XBOX_DPAD_RIGHT + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_START))			lKey = XBOX_GAMEPAD_START + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_BACK))			lKey = XBOX_GAMEPAD_BACK + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_LEFT_THUMB))		lKey = XBOX_LEFT_THUMB + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_RIGHT_THUMB))		lKey = XBOX_RIGHT_THUMB + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_LEFT_SHOULDER))	lKey = XBOX_LEFT_SHOULDER + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_RIGHT_SHOULDER))	lKey = XBOX_RIGHT_SHOULDER + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_A))				lKey = XBOX_GAMEPAD_A + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_B))				lKey = XBOX_GAMEPAD_B + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_X))				lKey = XBOX_GAMEPAD_X + XBOX_GAMEPAD_OFFSET[i];
//				if(!compareButtons(oldGamePad,controllers[i].state.Gamepad,XINPUT_GAMEPAD_Y))				lKey = XBOX_GAMEPAD_Y + XBOX_GAMEPAD_OFFSET[i];
//				if(lKey!=0)
//				{
//					tPresses++;
//					lastKey=lKey;
//				}
//			}
//			else
//			{
//				controllers[i].connected = false;
//
//				for(int l=0;l<14;l++)	joy[l][i] = false;
//				for(int l=0;l<6;l++)	axes[l*i] = 0;
//			}
//		}
//	}
//}
//xinput_input::~xinput_input()
//{
//	//XInputEnable( false );
//}
//float xinput_input::operator() (int key)
//{
//	float i=0;
//	WaitForSingleObject( inputMutex, INFINITE );
//
//	if(key>=0 && key<256)
//		i=keys[key] ? 1 : 0;
//	else if(key>=XBOX_GAMEPAD_OFFSET[0] && key<XBOX_GAMEPAD_OFFSET[3]+20)
//	{
//		int a=(key-XBOX_GAMEPAD_OFFSET[0])/20;
//		key -= a*20 + XBOX_GAMEPAD_OFFSET[0];
//		if(key == XBOX_LEFT_TRIGGER)		i = 1.0/255*controllers[a].state.Gamepad.bLeftTrigger;
//		if(key == XBOX_RIGHT_TRIGGER)	i = 1.0/255*controllers[a].state.Gamepad.bRightTrigger;
//		if(key == XBOX_THUMB_LX)			i = 1.0/32767*controllers[a].state.Gamepad.sThumbLX;
//		if(key == XBOX_THUMB_LY)			i = 1.0/32767*controllers[a].state.Gamepad.sThumbLY;
//		if(key == XBOX_THUMB_RX)			i = 1.0/32767*controllers[a].state.Gamepad.sThumbRX;
//		if(key == XBOX_THUMB_RY)			i = 1.0/32767*controllers[a].state.Gamepad.sThumbRY;
//		if(key == XBOX_DPAD_UP)			i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ? 1 : 0;
//		if(key == XBOX_DPAD_DOWN)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ? 1 : 0;
//		if(key == XBOX_DPAD_LEFT)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ? 1 : 0;
//		if(key == XBOX_DPAD_RIGHT)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ? 1 : 0;
//		if(key == XBOX_GAMEPAD_START)	i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_START ? 1 : 0;
//		if(key == XBOX_GAMEPAD_BACK)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK ? 1 : 0;
//		if(key == XBOX_LEFT_THUMB)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ? 1 : 0;
//		if(key == XBOX_RIGHT_THUMB)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ? 1 : 0;
//		if(key == XBOX_LEFT_SHOULDER)	i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ? 1 : 0;
//		if(key == XBOX_RIGHT_SHOULDER)	i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ? 1 : 0;
//		if(key == XBOX_GAMEPAD_A)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_A ? 1 : 0;
//		if(key == XBOX_GAMEPAD_B)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_B ? 1 : 0;
//		if(key == XBOX_GAMEPAD_X)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_X ? 1 : 0;
//		if(key == XBOX_GAMEPAD_Y)		i = controllers[a].state.Gamepad.wButtons & XINPUT_GAMEPAD_Y ? 1 : 0;
//	}
//	ReleaseMutex( inputMutex );
//
//	if(i >= 0.0)
//		return max(0.0,i/(1.0-deadZone) - deadZone);
//	else
//		return min(0.0,i/(1.0-deadZone) + deadZone);
//}
//#endif
