
struct _XINPUT_STATE;
typedef _XINPUT_STATE XINPUT_STATE;
struct IDirectInputDevice8W;

class InputManager
{
protected:
	Vec2f mousePos;
	struct mouseButtonState
	{
		Vec2f downPos;
		Vec2f upPos;
		bool down;
		mouseButtonState(): downPos(0,0), upPos(0,0), down(false){}
	};
	class gamepadState
	{
	public:
		virtual bool getButton(unsigned int button) const=0;
		virtual double getAxis(unsigned int axis) const=0;
		virtual ~gamepadState(){}
	};
#ifdef XINPUT
	class xboxControllerState: public gamepadState
	{
	private:
		XINPUT_STATE* state;
		double leftPressLength;
		double rightPressLength;
		double upPressLength;
		double downPressLength;
		friend class InputManager;
		
	public:
		bool connected;
		double deadZone;

		bool getButton(unsigned int button) const;
		double getAxis(unsigned int axis)const;

		xboxControllerState();
		~xboxControllerState();
	} xboxControllers[4];
#endif
#ifdef DIRECT_INPUT
	class directInputControllerState: public gamepadState
	{
	private:
		friend class InputManager;
		IDirectInputDevice8W* devicePtr;
		string name;

		struct axis{
			long minValue;
			long maxValue;
			long rawValue;
			double value;
		}axisX, axisY, axisZ;

		vector<bool> buttons;
		void acquireController();
		bool updateController();
		void release();

	public:
		double deadZone;

		bool getButton(unsigned int button) const;
		double getAxis(unsigned int axis)const;

		string getName(){return name;}

		directInputControllerState(IDirectInputDevice8W* ptr, string n);
	};
	vector<directInputControllerState> directInputControllers;
#endif

	unsigned char keys[256];
	mouseButtonState leftMouse, rightMouse, middleMouse;
	mutex  inputMutex;

public:
	struct callBack
	{
	protected:
		callBack(inputCallbackType t):type(t){}
	public:
		const inputCallbackType type;
	};
	struct keyStroke: public callBack{
		bool up;
		int vkey;
		keyStroke(): callBack(KEY_STROKE){}
		keyStroke(bool Up, int Vkey): callBack(KEY_STROKE), up(Up), vkey(Vkey){}
	};
	struct mouseClick: public callBack{
		bool down;
		mouseButton button;
		Vec2f pos;
		mouseClick(): callBack(MOUSE_CLICK){}
		mouseClick(bool Down, mouseButton b, Vec2f p): callBack(MOUSE_CLICK), down(Down), button(b), pos(p){}
	};
	struct mouseScroll: public callBack{
		double rotations;
		mouseScroll(): callBack(MOUSE_SCROLL){}
		mouseScroll(double rot): callBack(MOUSE_SCROLL), rotations(rot){}
	};
	struct menuKeystroke: public callBack{
		int mkey;
		menuKeystroke(): callBack(MENU_KEY_STROKE), mkey(0){}
		menuKeystroke(int Mkey): callBack(MENU_KEY_STROKE), mkey(Mkey){}
	};
public:
	static InputManager& getInstance()
	{
		static InputManager* pInstance = new InputManager();
		return *pInstance;
	}
#ifdef WINDOWS
	void windowsInput(unsigned int uMsg, unsigned int wParam, long lParam);
#endif
	void sendCallbacks(callBack* c);
	
	int lastKey;
	int tPresses;
	void down(int k);
	void up(int k);
	void joystick(unsigned int buttonMask,int x,int y,int z){}
	void update();
	bool getKey(int key);
	const mouseButtonState& getMouseState(mouseButton m);
#ifdef XINPUT
	const xboxControllerState& getXboxController(unsigned char controllerNum);
#endif
#ifdef DIRECT_INPUT
	const directInputControllerState* getDirectInputController(unsigned int controllerNum);
	unsigned int getNumDirectInputControllers(){return directInputControllers.size();}
#endif
	void addDirectInputDevice(IDirectInputDevice8W* devicePtr, string name);
	float operator() (int key);
	Vec2f getMousePos(){return mousePos;}
	void checkNewHardware();//scan for new hardware
	void setVibration(int controllerNum, float amount);

	void initialize();
	void shutdown();
 };
extern InputManager& input;

//since the following symbols are #define'd in windows.h, the preprocessor
//check is necessary to prevent compile errors.
#ifndef VK_LBUTTON
const int VK_LBUTTON        = 0x01;
const int VK_RBUTTON        = 0x02;
const int VK_CANCEL         = 0x03;
const int VK_MBUTTON        = 0x04;    /* NOT contiguous with L & RBUTTON */
const int VK_BACK           = 0x08;
const int VK_TAB            = 0x09;
const int VK_CLEAR          = 0x0C;
const int VK_RETURN         = 0x0D;
const int VK_SHIFT          = 0x10;
const int VK_CONTROL        = 0x11;
const int VK_MENU           = 0x12;
const int VK_PAUSE          = 0x13;
const int VK_CAPITAL        = 0x14;
const int VK_ESCAPE         = 0x1B;
const int VK_CONVERT        = 0x1C;
const int VK_NONCONVERT     = 0x1D;
const int VK_ACCEPT         = 0x1E;
const int VK_MODECHANGE     = 0x1F;
const int VK_SPACE          = 0x20;
const int VK_PRIOR          = 0x21;
const int VK_NEXT           = 0x22;
const int VK_END            = 0x23;
const int VK_HOME           = 0x24;
const int VK_LEFT           = 0x25;
const int VK_UP             = 0x26;
const int VK_RIGHT          = 0x27;
const int VK_DOWN           = 0x28;
const int VK_SELECT         = 0x29;
const int VK_PRINT          = 0x2A;
const int VK_EXECUTE        = 0x2B;
const int VK_SNAPSHOT       = 0x2C;
const int VK_INSERT         = 0x2D;
const int VK_DELETE         = 0x2E;
const int VK_HELP           = 0x2F;
const int VK_NUMPAD0		= 0x60;
const int VK_NUMPAD1		= 0x61;
const int VK_NUMPAD2		= 0x62;
const int VK_NUMPAD3		= 0x63;
const int VK_NUMPAD4		= 0x64;
const int VK_NUMPAD5		= 0x65;
const int VK_NUMPAD6		= 0x66;
const int VK_NUMPAD7		= 0x67;
const int VK_NUMPAD8		= 0x68;
const int VK_NUMPAD9		= 0x69;
const int VK_MULTIPLY		= 0x6A;
const int VK_ADD			= 0x6B;
const int VK_SEPARATOR		= 0x6C;
const int VK_SUBTRACT		= 0x6D;
const int VK_DECIMAL		= 0x6E;
const int VK_DIVIDE			= 0x6F;
const int VK_F1				= 0x70;
const int VK_F2				= 0x71;
const int VK_F3				= 0x72;
const int VK_F4				= 0x73;
const int VK_F5				= 0x74;
const int VK_F6				= 0x75;
const int VK_F7				= 0x76;
const int VK_F8				= 0x77;
const int VK_F9				= 0x78;
const int VK_F10			= 0x79;
const int VK_F11			= 0x7A;
const int VK_F12			= 0x7B;
const int VK_F13			= 0x7C;
const int VK_F14			= 0x7D;
const int VK_F15			= 0x7E;
const int VK_F16			= 0x7F;
const int VK_F17			= 0x80;
const int VK_F18			= 0x81;
const int VK_F19			= 0x82;
const int VK_F20			= 0x83;
const int VK_F21			= 0x84;
const int VK_F22			= 0x85;
const int VK_F23			= 0x86;
const int VK_F24			= 0x87;
const int VK_LSHIFT         = 0xA0;
const int VK_RSHIFT         = 0xA1;
const int VK_LCONTROL       = 0xA2;
const int VK_RCONTROL       = 0xA3;
const int VK_LMENU          = 0xA4;
const int VK_RMENU          = 0xA5;
#endif

const int XINPUT_LEFT_TRIGGER	= 0;
const int XINPUT_RIGHT_TRIGGER	= 1;
const int XINPUT_THUMB_LX		= 2;
const int XINPUT_THUMB_LY		= 3;
const int XINPUT_THUMB_RX		= 4;
const int XINPUT_THUMB_RY		= 5;

const int XINPUT_DPAD_UP        = 0x0001;
const int XINPUT_DPAD_DOWN      = 0x0002;
const int XINPUT_DPAD_LEFT      = 0x0004;
const int XINPUT_DPAD_RIGHT     = 0x0008;
const int XINPUT_START          = 0x0010;
const int XINPUT_BACK           = 0x0020;
const int XINPUT_LEFT_THUMB     = 0x0040;
const int XINPUT_RIGHT_THUMB    = 0x0080;
const int XINPUT_LEFT_SHOULDER  = 0x0100;
const int XINPUT_RIGHT_SHOULDER = 0x0200;
const int XINPUT_A              = 0x1000;
const int XINPUT_B              = 0x2000;
const int XINPUT_X              = 0x4000;
const int XINPUT_Y              = 0x8000;

const int MENU_LEFT				= 0x125;
const int MENU_UP				= 0x126;
const int MENU_RIGHT			= 0x127;
const int MENU_DOWN				= 0x128;
const int MENU_ENTER			= 0x129;
const int MENU_BACK				= 0x130;
const int MENU_START			= 0x131;
