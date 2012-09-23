
#include "engine.h"

#if defined(WINDOWS)
	#include <Windows.h>
	#ifdef XINPUT
	#include <Xinput.h>
	const int XBOX_GAMEPAD_OFFSET[4]	= {256,276,296,316};
	#endif

	#ifdef DIRECT_INPUT
	#include <dinput.h>
	LPDIRECTINPUT8  directInputPtr;

	BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
										VOID* pContext )
	{
		DI_ENUM_CONTEXT* pEnumContext = ( DI_ENUM_CONTEXT* )pContext;
		HRESULT hr;

		if( g_bFilterOutXinputDevices && IsXInputDevice( &pdidInstance->guidProduct ) )
			return DIENUM_CONTINUE;

		// Skip anything other than the perferred joystick device as defined by the control panel.  
		// Instead you could store all the enumerated joysticks and let the user pick.
		if( pEnumContext->bPreferredJoyCfgValid &&
			!IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
			return DIENUM_CONTINUE;

		// Obtain an interface to the enumerated joystick.
		hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );

		// If it failed, then we can't use this joystick. (Maybe the user unplugged
		// it while we were in the middle of enumerating it.)
		if( FAILED( hr ) )
			return DIENUM_CONTINUE;

		// Stop enumeration. Note: we're just taking the first joystick we get. You
		// could store all the enumerated joysticks and let the user pick.
		return DIENUM_STOP;
	}
	#endif /* DIRECT_INPUT */
#elif defined(LINUX)
	#include <X11/keysym.h>
	#include <X11/Xlib.h> 
#endif /* WINDOWS */

const unsigned char KEYSTATE_CURRENT	= 0x01;
const unsigned char KEYSTATE_LAST	= 0x02; 
const unsigned char KEYSTATE_VIRTUAL	= 0x04;


InputManager::xboxControllerState::xboxControllerState():
#ifdef XINPUT
                    state(new XINPUT_STATE),
#else
                    state(nullptr),
#endif
                    leftPressLength(0.0), rightPressLength(0.0), upPressLength(0.0), downPressLength(0.0), connected(false),deadZone(0.15)
{

#ifdef DIRECT_INPUT
	if(!FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInputPtr, NULL)))
	{
		directInputPtr->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, NULL, DIEDFL_ATTACHEDONLY);
	}
	else
	{
		directInputPtr = nullptr;
	}
#endif

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
	if(c->type == MOUSE_CLICK && static_cast<mouseClick*>(c)->button == LEFT_BUTTON)
	{
		if((leftMouse.down = static_cast<mouseClick*>(c)->down) == true)
		{
			leftMouse.downPos.x = static_cast<mouseClick*>(c)->pos.x;
			leftMouse.downPos.y = static_cast<mouseClick*>(c)->pos.y;		
		}
		else
		{
			leftMouse.upPos.x = static_cast<mouseClick*>(c)->pos.x;
			leftMouse.upPos.y = static_cast<mouseClick*>(c)->pos.y;
		}
	}
	else if(c->type == MOUSE_CLICK && static_cast<mouseClick*>(c)->button == MIDDLE_BUTTON)
	{
		if((middleMouse.down = static_cast<mouseClick*>(c)->down) == true)
		{
			middleMouse.downPos.x = static_cast<mouseClick*>(c)->pos.x;
			middleMouse.downPos.y = static_cast<mouseClick*>(c)->pos.y;		
		}
		else
		{
			middleMouse.upPos.x = static_cast<mouseClick*>(c)->pos.x;
			middleMouse.upPos.y = static_cast<mouseClick*>(c)->pos.y;
		}
	}
	else if(c->type == MOUSE_CLICK && static_cast<mouseClick*>(c)->button == RIGHT_BUTTON)
	{
		if((rightMouse.down = static_cast<mouseClick*>(c)->down) == true)
		{
			rightMouse.downPos.x = static_cast<mouseClick*>(c)->pos.x;
			rightMouse.downPos.y = static_cast<mouseClick*>(c)->pos.y;			
		}
		else
		{
			rightMouse.upPos.x = static_cast<mouseClick*>(c)->pos.x;
			rightMouse.upPos.y = static_cast<mouseClick*>(c)->pos.y;
		}
	}
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
const InputManager::joystickControllerState* InputManager::getJoystick(unsigned int joystickNum)
{
	return joystickNum < joysticks.size() ? &joysticks[joystickNum] : nullptr;
}
void InputManager::update()
{
	double time = GetTime();
	static double lastUpdateTime = time;

	double updateLength = clamp(time - lastUpdateTime, 0.0, 100.0); //make sure update length is between 0 ms and 100 ms
	lastUpdateTime = time;

	static char newKeyStates[256];
	memset(newKeyStates, 0, 256);
#if defined WINDOWS
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	GetKeyboardState((PBYTE)newKeyStates);
#elif defined LINUX
	Vec2i cursorPos;
	Vec2i root_cursorPos;
	Window root;
	Window child;
	unsigned int mask;
	
	XQueryPointer(x11_display, x11_window, &root, &child, &root_cursorPos.x, &root_cursorPos.y, &cursorPos.x, &cursorPos.y, &mask);
	//TODO:process all keys
	char newKeycodes[32];
	XQueryKeymap(x11_display, newKeycodes);
	
	newKeyStates[VK_LEFT] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_Left)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_Left) % 8)) 	? 0x80 : 0;
	newKeyStates[VK_RIGHT] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_Right)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_Right) % 8)) ? 0x80 : 0;
	newKeyStates[VK_UP] = 		newKeycodes[XKeysymToKeycode(x11_display,XK_Up)/8] 		& (1<<(XKeysymToKeycode(x11_display,XK_Up) % 8)) 	? 0x80 : 0;
	newKeyStates[VK_DOWN] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_Down)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_Down) % 8)) 	? 0x80 : 0;
	newKeyStates[VK_SPACE] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_space)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_space) % 8)) ? 0x80 : 0;
	newKeyStates[VK_RETURN] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_Return)/8]	& (1<<(XKeysymToKeycode(x11_display,XK_Return) % 8))? 0x80 : 0;
	newKeyStates[VK_ESCAPE] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_Escape)/8]	& (1<<(XKeysymToKeycode(x11_display,XK_Escape) % 8))? 0x80 : 0;	

	//keypad should also include key syms for when num lock is not on
	newKeyStates[VK_NUMPAD0] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_KP_0)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_KP_0) % 8)) ? 0x80 : 0;
	newKeyStates[VK_NUMPAD2] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_KP_2)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_KP_2) % 8)) ? 0x80 : 0;
	newKeyStates[VK_NUMPAD5] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_KP_5)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_KP_5) % 8)) ? 0x80 : 0;
	newKeyStates[VK_NUMPAD8] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_KP_8)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_KP_8) % 8)) ? 0x80 : 0;
	newKeyStates[VK_NUMPAD9] = 	newKeycodes[XKeysymToKeycode(x11_display,XK_KP_9)/8] 	& (1<<(XKeysymToKeycode(x11_display,XK_KP_9) % 8)) ? 0x80 : 0;

#endif

#ifdef XINPUT
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

#ifdef XINPUT
	for(int i=0;i<4;i++)
	{
		if(xboxControllers[i].connected)
		{
			if(xboxControllers[i].state->Gamepad.sThumbLX < -32768/2)
			{
				xboxControllers[i].leftPressLength += updateLength;
				xboxControllers[i].rightPressLength = 0;
			}
			else if(xboxControllers[i].state->Gamepad.sThumbLX > 32767/2)
			{
				xboxControllers[i].leftPressLength = 0;
				xboxControllers[i].rightPressLength += updateLength;
			}
			else
			{
				xboxControllers[i].leftPressLength = 0;
				xboxControllers[i].rightPressLength = 0;
			}

			if(xboxControllers[i].state->Gamepad.sThumbLY < -32768/2)
			{
				xboxControllers[i].downPressLength += updateLength;
				xboxControllers[i].upPressLength = 0;
			}
			else if(xboxControllers[i].state->Gamepad.sThumbLY > 32767/2)
			{
				xboxControllers[i].downPressLength = 0;
				xboxControllers[i].upPressLength += updateLength;
			}
			else
			{
				xboxControllers[i].downPressLength = 0;
				xboxControllers[i].upPressLength = 0;
			}

			if(xboxControllers[i].leftPressLength >= 5.0)
			{
				sendCallbacks(new menuKeystroke(MENU_LEFT));
				xboxControllers[i].leftPressLength = -395.0;
			}
			if(xboxControllers[i].rightPressLength >= 5.0)
			{
				sendCallbacks(new menuKeystroke(MENU_RIGHT));
				xboxControllers[i].rightPressLength = -395.0;
			}
			if(xboxControllers[i].upPressLength >= 5.0)
			{
				sendCallbacks(new menuKeystroke(MENU_UP));
				xboxControllers[i].upPressLength = -395.0;
			}
			if(xboxControllers[i].downPressLength >= 5.0)
			{
				sendCallbacks(new menuKeystroke(MENU_DOWN));
				xboxControllers[i].downPressLength = -395.0;
			}
		}
	}

	//if(((newKeyStates[VK_LEFT] & 0x80) && !(keys[VK_LEFT] & KEYSTATE_CURRENT)) || // KEYSTATE_CURRENT actually reflects the old state (we are about to update it)
	//	xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_DPAD_LEFT) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_DPAD_LEFT) ||
	//	xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_DPAD_LEFT) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_DPAD_LEFT) ||
	//	xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_DPAD_LEFT) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_DPAD_LEFT) ||
	//	xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_DPAD_LEFT) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_DPAD_LEFT))
	//		sendCallbacks(new menuKeystroke(MENU_LEFT));
	//if(((newKeyStates[VK_RIGHT] & 0x80) && !(keys[VK_RIGHT] & KEYSTATE_CURRENT)) || 
	//	xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_DPAD_RIGHT) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_DPAD_RIGHT) ||
	//	xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_DPAD_RIGHT) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_DPAD_RIGHT) ||
	//	xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_DPAD_RIGHT) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_DPAD_RIGHT) ||
	//	xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_DPAD_RIGHT) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_DPAD_RIGHT))
	//		sendCallbacks(new menuKeystroke(MENU_RIGHT));
	//if(((newKeyStates[VK_UP] & 0x80) && !(keys[VK_UP] & KEYSTATE_CURRENT)) || 
	//	xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_DPAD_UP) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_DPAD_UP) ||
	//	xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_DPAD_UP) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_DPAD_UP) ||
	//	xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_DPAD_UP) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_DPAD_UP) ||
	//	xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_DPAD_UP) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_DPAD_UP))
	//		sendCallbacks(new menuKeystroke(MENU_UP));
	//if(((newKeyStates[VK_DOWN] & 0x80) && !(keys[VK_DOWN] & KEYSTATE_CURRENT)) || 
	//	xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_DPAD_DOWN) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_DPAD_DOWN) ||
	//	xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_DPAD_DOWN) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_DPAD_DOWN) ||
	//	xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_DPAD_DOWN) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_DPAD_DOWN) ||
	//	xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_DPAD_DOWN) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_DPAD_DOWN))
	//		sendCallbacks(new menuKeystroke(MENU_DOWN));

	if(((newKeyStates[VK_SPACE] & 0x80) && !(keys[VK_SPACE] & KEYSTATE_CURRENT)) || 
		((newKeyStates[VK_RETURN] & 0x80) && !(keys[VK_RETURN] & KEYSTATE_CURRENT)) || 
		xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_A) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_A) ||
		xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_A) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_A) ||
		xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_A) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_A) ||
		xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_A) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_A))
			sendCallbacks(new menuKeystroke(MENU_ENTER));
	if(((newKeyStates[VK_ESCAPE] & 0x80) && !(keys[VK_ESCAPE] & KEYSTATE_CURRENT)) || 
		xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_BACK) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_BACK) ||
		xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_BACK) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_BACK) ||
		xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_BACK) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_BACK) ||
		xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_BACK) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_BACK))
			sendCallbacks(new menuKeystroke(MENU_BACK));
	if( xboxControllers[0].connected && !(xboxControllers[0].state->Gamepad.wButtons & XINPUT_START) && (newXboxControllerStates[0].Gamepad.wButtons & XINPUT_START) ||
		xboxControllers[1].connected && !(xboxControllers[1].state->Gamepad.wButtons & XINPUT_START) && (newXboxControllerStates[1].Gamepad.wButtons & XINPUT_START) ||
		xboxControllers[2].connected && !(xboxControllers[2].state->Gamepad.wButtons & XINPUT_START) && (newXboxControllerStates[2].Gamepad.wButtons & XINPUT_START) ||
		xboxControllers[3].connected && !(xboxControllers[3].state->Gamepad.wButtons & XINPUT_START) && (newXboxControllerStates[3].Gamepad.wButtons & XINPUT_START))
			sendCallbacks(new menuKeystroke(MENU_START)); //can't be triggered from keyboard!!!
#else
	if(((newKeyStates[VK_LEFT] & 0x80) && !(keys[VK_LEFT] & KEYSTATE_CURRENT))) // KEYSTATE_CURRENT actually reflects the old state (we are about to update it)
			sendCallbacks(new menuKeystroke(MENU_LEFT));
	if(((newKeyStates[VK_RIGHT] & 0x80) && !(keys[VK_RIGHT] & KEYSTATE_CURRENT)))
			sendCallbacks(new menuKeystroke(MENU_RIGHT));
	if(((newKeyStates[VK_UP] & 0x80) && !(keys[VK_UP] & KEYSTATE_CURRENT)))
			sendCallbacks(new menuKeystroke(MENU_UP));
	if(((newKeyStates[VK_DOWN] & 0x80) && !(keys[VK_DOWN] & KEYSTATE_CURRENT))) 
			sendCallbacks(new menuKeystroke(MENU_DOWN));

	if(((newKeyStates[VK_SPACE] & 0x80) && !(keys[VK_SPACE] & KEYSTATE_CURRENT)) ||	((newKeyStates[VK_RETURN] & 0x80) && !(keys[VK_RETURN] & KEYSTATE_CURRENT))) 
			sendCallbacks(new menuKeystroke(MENU_ENTER));
	if(((newKeyStates[VK_ESCAPE] & 0x80) && !(keys[VK_ESCAPE] & KEYSTATE_CURRENT))) 
			sendCallbacks(new menuKeystroke(MENU_BACK));
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

#ifdef XINPUT
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
#ifdef WINDOWS
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
		//leftMouse.down = true;
		//leftMouse.downPos.x = (float)LOWORD(lParam) / sh;
		//leftMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, LEFT_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_MBUTTONDOWN)
	{
		//middleMouse.down = true;
		//middleMouse.downPos.x = (float)LOWORD(lParam) / sh;
		//middleMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, MIDDLE_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_RBUTTONDOWN)
	{
		//rightMouse.down = true;
		//rightMouse.downPos.x = (float)LOWORD(lParam) / sh;
		//rightMouse.downPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(true, RIGHT_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_LBUTTONUP)
	{
		//leftMouse.down = false;
		//leftMouse.upPos.x = (float)LOWORD(lParam) / sh;
		//leftMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, LEFT_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_MBUTTONUP)
	{
		//middleMouse.down = false;
		//middleMouse.upPos.x = (float)LOWORD(lParam) / sh;
		//middleMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, MIDDLE_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_RBUTTONUP)
	{
		//rightMouse.down = false;
		//rightMouse.upPos.x = (float)LOWORD(lParam) / sh;
		//rightMouse.upPos.y = (float)HIWORD(lParam) / sh;
		sendCallbacks(new mouseClick(false, RIGHT_BUTTON, Vec2f((float)LOWORD(lParam) / sh, (float)HIWORD(lParam) / sh)));
	}
	else if(uMsg == WM_MOUSEWHEEL)
	{
		sendCallbacks(new mouseScroll(double(GET_WHEEL_DELTA_WPARAM(wParam))/120.0));
	}
}
#endif /*WINDOWS*/
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
void InputManager::setVibration(int controllerNum, float amount)
{
#ifdef XINPUT
	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = vibration.wRightMotorSpeed = clamp(static_cast<int>(amount * 65535), 0, 65535);
	XInputSetState(controllerNum, &vibration);
#endif
}
