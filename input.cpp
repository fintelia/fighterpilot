
#include "engine.h"

#if defined(WINDOWS)
	#include <Windows.h>
	#ifdef XINPUT
	#include <Xinput.h>
	const int XBOX_GAMEPAD_OFFSET[4]	= {256,276,296,316};
	#endif

	#ifdef DIRECT_INPUT
	#define DIRECTINPUT_VERSION 0x0800
	#include <dinput.h>
	IDirectInput8*  directInputPtr;

	#ifdef XINPUT //if both DIRECT_INPUT && XINPUT are defined
		#include <wbemidl.h>
		#include <oleauto.h>
		//#include <wmsstd.h>

		#define SAFE_RELEASE(x) if(x) { x->Release(); x = NULL; } 
		//-----------------------------------------------------------------------------
		// Enum each PNP device using WMI and check each device ID to see if it contains 
		// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
		// Unfortunately this information can not be found by just using DirectInput 
		// see: http://msdn.microsoft.com/en-us/library/windows/desktop/ee417014%28v=vs.85%29.aspx
		//-----------------------------------------------------------------------------
		BOOL IsXInputDevice( const GUID* pGuidProductFromDirectInput )
		{
			IWbemLocator*           pIWbemLocator  = NULL;
			IEnumWbemClassObject*   pEnumDevices   = NULL;
			IWbemClassObject*       pDevices[20]   = {0};
			IWbemServices*          pIWbemServices = NULL;
			BSTR                    bstrNamespace  = NULL;
			BSTR                    bstrDeviceID   = NULL;
			BSTR                    bstrClassName  = NULL;
			DWORD                   uReturned      = 0;
			bool                    bIsXinputDevice= false;
			UINT                    iDevice        = 0;
			VARIANT                 var;
			HRESULT                 hr;

			// CoInit if needed
			hr = CoInitialize(NULL);
			bool bCleanupCOM = SUCCEEDED(hr);

			// Create WMI
			hr = CoCreateInstance( __uuidof(WbemLocator),
								   NULL,
								   CLSCTX_INPROC_SERVER,
								   __uuidof(IWbemLocator),
								   (LPVOID*) &pIWbemLocator);
			if( FAILED(hr) || pIWbemLocator == NULL )
				goto LCleanup;

			bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );if( bstrNamespace == NULL ) goto LCleanup;        
			bstrClassName = SysAllocString( L"Win32_PNPEntity" );   if( bstrClassName == NULL ) goto LCleanup;        
			bstrDeviceID  = SysAllocString( L"DeviceID" );          if( bstrDeviceID == NULL )  goto LCleanup;        
    
			// Connect to WMI 
			hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L, 
											   0L, NULL, NULL, &pIWbemServices );
			if( FAILED(hr) || pIWbemServices == NULL )
				goto LCleanup;

			// Switch security level to IMPERSONATE. 
			CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
							   RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE );                    

			hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices ); 
			if( FAILED(hr) || pEnumDevices == NULL )
				goto LCleanup;

			// Loop over all devices
			for( ;; )
			{
				// Get 20 at a time
				hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
				if( FAILED(hr) )
					goto LCleanup;
				if( uReturned == 0 )
					break;

				for( iDevice=0; iDevice<uReturned; iDevice++ )
				{
					// For each device, get its device ID
					hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
					if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
					{
						// Check if the device ID contains "IG_".  If it does, then it's an XInput device
							// This information can not be found from DirectInput 
						if( wcsstr( var.bstrVal, L"IG_" ) )
						{
							// If it does, then get the VID/PID from var.bstrVal
							DWORD dwPid = 0, dwVid = 0;
							WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
							if( strVid && swscanf( strVid, L"VID_%4X", &dwVid ) != 1 )
								dwVid = 0;
							WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
							if( strPid && swscanf( strPid, L"PID_%4X", &dwPid ) != 1 )
								dwPid = 0;

							// Compare the VID/PID to the DInput device
							DWORD dwVidPid = MAKELONG( dwVid, dwPid );
							if( dwVidPid == pGuidProductFromDirectInput->Data1 )
							{
								bIsXinputDevice = true;
								goto LCleanup;
							}
						}
					}   
					SAFE_RELEASE( pDevices[iDevice] );
				}
			}

		LCleanup:
			if(bstrNamespace)
				SysFreeString(bstrNamespace);
			if(bstrDeviceID)
				SysFreeString(bstrDeviceID);
			if(bstrClassName)
				SysFreeString(bstrClassName);
			for( iDevice=0; iDevice<20; iDevice++ )
				SAFE_RELEASE( pDevices[iDevice] );
			SAFE_RELEASE( pEnumDevices );
			SAFE_RELEASE( pIWbemLocator );
			SAFE_RELEASE( pIWbemServices );

			if( bCleanupCOM )
				CoUninitialize();

			return bIsXinputDevice;
		}
	#endif /* XINPUT */
	BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, void* directInputDevicePtr)
	{
		if(pdidoi->dwType & DIDFT_AXIS)
		{
			DIPROPRANGE diprg;
			diprg.diph.dwSize = sizeof(DIPROPRANGE);
			diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			diprg.diph.dwHow = DIPH_BYID;
			diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
			diprg.lMin = -32767;
			diprg.lMax = 32767;

			// Set the range for the axis
			static_cast<IDirectInputDevice8*>(directInputDevicePtr)->SetProperty(DIPROP_RANGE, &diprg.diph);
		}
		return DIENUM_CONTINUE;
	}
	BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, void* inputManagerPtr)	//we need a pointer to input so we can call its 
	{																									//methods, since it is still being constructed
#ifdef XINPUT
		if(IsXInputDevice(&pdidInstance->guidProduct))
			return DIENUM_CONTINUE;
#endif
		IDirectInputDevice8* directInputDevicePtr;
		if(!FAILED(directInputPtr->CreateDevice(pdidInstance->guidInstance,&directInputDevicePtr,NULL)))
		{
			if(FAILED(directInputDevicePtr->SetDataFormat(&c_dfDIJoystick2)))
				return DIENUM_CONTINUE;

			if(FAILED(directInputDevicePtr->SetCooperativeLevel(graphics->getWindowHWND(), DISCL_EXCLUSIVE | DISCL_FOREGROUND)))
				return DIENUM_CONTINUE;

			if(FAILED(directInputDevicePtr->EnumObjects(EnumObjectsCallback, (void*)directInputDevicePtr, DIDFT_ALL)))
				return DIENUM_CONTINUE;

			static_cast<InputManager*>(inputManagerPtr)->addDirectInputDevice(directInputDevicePtr, toString(pdidInstance->tszInstanceName,MAX_PATH));
		}

		return DIENUM_CONTINUE;
	}
	#endif /* DIRECT_INPUT */
#elif defined(LINUX)
	#include <X11/keysym.h>
	#include <X11/Xlib.h> 
#endif /* WINDOWS */

const unsigned char KEYSTATE_CURRENT	= 0x01;
const unsigned char KEYSTATE_LAST		= 0x02; 
const unsigned char KEYSTATE_VIRTUAL	= 0x04;

#ifdef XINPUT
InputManager::xboxControllerState::xboxControllerState(): state(new XINPUT_STATE), leftPressLength(0.0), rightPressLength(0.0), upPressLength(0.0), downPressLength(0.0), connected(false),deadZone(0.15)
{

}
InputManager::xboxControllerState::~xboxControllerState()
{
	delete state;
}

bool InputManager::xboxControllerState::getButton(unsigned int b) const
{
	return connected && ((state->Gamepad.wButtons & b) != 0);
}
double InputManager::xboxControllerState::getAxis(unsigned int a) const
{
	if(!connected)
		return 0.0;

	double d = 0.0;
	if(a == XINPUT_LEFT_TRIGGER)		d = 1.0/255 * state->Gamepad.bLeftTrigger;
	if(a == XINPUT_RIGHT_TRIGGER)		d = 1.0/255 * state->Gamepad.bRightTrigger;
	if(a == XINPUT_THUMB_LX)			d = 1.0/32768 * state->Gamepad.sThumbLX;
	if(a == XINPUT_THUMB_LY)			d = 1.0/32768 * state->Gamepad.sThumbLY;
	if(a == XINPUT_THUMB_RX)			d = 1.0/32768 * state->Gamepad.sThumbRX;
	if(a == XINPUT_THUMB_RY)			d = 1.0/32768 * state->Gamepad.sThumbRY;

	debugAssert(deadZone > -1.0 && deadZone < 1.0);

	if(d < -deadZone)
		return (d + deadZone) / (1.0 - deadZone);
	else if(d < deadZone)
		return 0.0;
	else
		return (d - deadZone) / (1.0 - deadZone);
}
#endif
#ifdef DIRECT_INPUT
InputManager::directInputControllerState::directInputControllerState(IDirectInputDevice8W* ptr, string n): devicePtr(ptr), name(n), deadZone(0.15)
{
	if(devicePtr)
	{
		devicePtr->Acquire();
		axisX.minValue = -32768;
		axisX.maxValue = 32768;
		axisY.minValue = -32768;
		axisY.maxValue = 32768;
		axisZ.minValue = -32768;
		axisZ.maxValue = 32768;

		DIDEVCAPS caps;
		caps.dwSize = sizeof(DIDEVCAPS);
		devicePtr->GetCapabilities(&caps);
		buttons = vector<bool>(caps.dwButtons,false);
	}
}
void InputManager::directInputControllerState::release()
{
	if(devicePtr)
	{
		devicePtr->Unacquire();
		devicePtr->Release();
	}
}
void InputManager::directInputControllerState::acquireController()
{
	if(devicePtr)
	{
		devicePtr->Acquire();
	}
}
bool InputManager::directInputControllerState::updateController()
{
	if(!devicePtr)
		return false;

	//devicePtr->Acquire();
    if(FAILED(devicePtr->Poll()))
    {
		devicePtr->Acquire();
		return false;
    }


	DIJOYSTATE2 joystickState;
    if(FAILED(devicePtr->GetDeviceState(sizeof(DIJOYSTATE2), &joystickState)))
        return false;

	axisX.rawValue = joystickState.lX;
	axisY.rawValue = joystickState.lY;
	axisZ.rawValue = joystickState.lZ;

	axisX.value = static_cast<double>(axisX.rawValue) / 32767.0;
	axisY.value = static_cast<double>(axisY.rawValue) / 32767.0;
	axisZ.value = static_cast<double>(axisZ.rawValue) / 32767.0;

	for(unsigned int i = 0; i < buttons.size() && i < 128; i++)
	{
		buttons[i] = (joystickState.rgbButtons[i] & 0x80) != 0;
	}

	return true;
}
bool InputManager::directInputControllerState::getButton(unsigned int b) const
{
	return b < buttons.size() ? buttons[b] : false;
}
double InputManager::directInputControllerState::getAxis(unsigned int axis)const
{
#ifdef DIRECT_INPUT
	double d = 0.0;
	if(axis == 0)	d = axisX.value;
	if(axis == 1)	d = axisY.value;
	if(axis == 2)	d = axisZ.value;

	debugAssert(deadZone > -1.0 && deadZone < 1.0);

	if(d < -deadZone)
		return (d + deadZone) / (1.0 - deadZone);
	else if(d < deadZone)
		return 0.0;
	else
		return (d - deadZone) / (1.0 - deadZone);
#else
	return 0.0;
#endif
}
#endif
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
void InputManager::initialize()
{
	tPresses = 0;
	for(int i=0; i<256; i++)
	{
		keys[i]=false;
	}

#ifdef XINPUT
	for(int i=0; i<4; i++)
	{
		xboxControllers[i].connected = true;
	}
#endif
#ifdef DIRECT_INPUT
	if(!FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInputPtr, NULL)))
	{
		directInputPtr->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	}
	else
	{
		directInputPtr = nullptr;
	}
#endif
}
void InputManager::shutdown()
{
#ifdef DIRECT_INPUT
	for(auto i = directInputControllers.begin(); i != directInputControllers.end(); i++)
	{
		i->release();
	}

	if(directInputPtr != NULL)
	{
	   static_cast<IDirectInput8*>(directInputPtr)->Release();
	}
#endif
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
#ifdef XINPUT
const InputManager::xboxControllerState& InputManager::getXboxController(unsigned char controllerNum)
{
	return xboxControllers[clamp(controllerNum,0,3)];
}
#endif
#ifdef DIRECT_INPUT
const InputManager::directInputControllerState* InputManager::getDirectInputController(unsigned int joystickNum)
{
	return joystickNum < directInputControllers.size() ? &directInputControllers[joystickNum] : nullptr;
}
void InputManager::addDirectInputDevice(IDirectInputDevice8W* devicePtr, string name)
{
	directInputControllers.push_back(directInputControllerState(devicePtr, name));
}
#endif
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
	
	auto checkKeysym = [newKeycodes, x11_display](int sym)->char
	{
		return newKeycodes[XKeysymToKeycode(x11_display,sym)/8] 	& (1<<(XKeysymToKeycode(x11_display,sym) % 8)) 	? 0x80 : 0;
	};
	
	for(char c='A'; c <= 'Z'; c++)
		newKeyStates[c] = checkKeysym(c);

	for(char c=0; c < 24; c++)
		newKeyStates[VK_F1+c] = checkKeysym(XK_F1+c);
	
	for(char c=0; c <= 9; c++)//keypad should also include key syms for when num lock is not on
		newKeyStates[VK_NUMPAD0+c] = checkKeysym(XK_KP_0+c);
	
	newKeyStates[VK_LSHIFT] = checkKeysym(XK_Shift_L);
	newKeyStates[VK_RSHIFT] = checkKeysym(XK_Shift_R);
	newKeyStates[VK_SHIFT] = checkKeysym(XK_Shift_L) | checkKeysym(XK_Shift_R);

	newKeyStates[VK_LCONTROL] = checkKeysym(XK_Control_L);
	newKeyStates[VK_RCONTROL] = checkKeysym(XK_Control_R);
	newKeyStates[VK_CONTROL] = checkKeysym(XK_Control_L) | checkKeysym(XK_Control_R);
	
	newKeyStates[VK_LEFT] =		checkKeysym(XK_Left);
	newKeyStates[VK_RIGHT] =	checkKeysym(XK_Right);
	newKeyStates[VK_UP] =		checkKeysym(XK_Up);
	newKeyStates[VK_DOWN] =		checkKeysym(XK_Down);
	newKeyStates[VK_SPACE] =	checkKeysym(XK_space);
	newKeyStates[VK_RETURN] =	checkKeysym(XK_Return);
	newKeyStates[VK_ESCAPE] =	checkKeysym(XK_Escape);	
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

	if(((newKeyStates[VK_LEFT] & 0x80) && !(keys[VK_LEFT] & KEYSTATE_CURRENT)))
		sendCallbacks(new menuKeystroke(MENU_LEFT));
	if(((newKeyStates[VK_RIGHT] & 0x80) && !(keys[VK_RIGHT] & KEYSTATE_CURRENT))) 
		sendCallbacks(new menuKeystroke(MENU_RIGHT));
	if(((newKeyStates[VK_UP] & 0x80) && !(keys[VK_UP] & KEYSTATE_CURRENT)))
		sendCallbacks(new menuKeystroke(MENU_UP));
	if(((newKeyStates[VK_DOWN] & 0x80) && !(keys[VK_DOWN] & KEYSTATE_CURRENT))) 
		sendCallbacks(new menuKeystroke(MENU_DOWN));



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

#ifdef DIRECT_INPUT
	for(auto i=directInputControllers.begin(); i != directInputControllers.end(); i++)
	{
		i->updateController();
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
	if(uMsg == WM_ACTIVATEAPP && wParam)
	{
#ifdef DIRECT_INPUT
		for(vector<directInputControllerState>::iterator i=directInputControllers.begin(); i!=directInputControllers.end(); i++)
		{
			i->acquireController();
		}
#endif
	}
	else if(uMsg == WM_KEYDOWN)
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
#ifdef XINPUT
	for(int i=0; i<4; i++)
	{
		xboxControllers[i].connected = true;
	}
#endif
}
void InputManager::setVibration(int controllerNum, float amount)
{
#ifdef XINPUT
	XINPUT_VIBRATION vibration;
	vibration.wLeftMotorSpeed = vibration.wRightMotorSpeed = clamp(static_cast<int>(amount * 65535), 0, 65535);
	XInputSetState(controllerNum, &vibration);
#endif
}
