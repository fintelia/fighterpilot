
const int UP	= VK_UP;
const int DOWN	= VK_DOWN;
const int LEFT	= VK_LEFT;
const int RIGHT	= VK_RIGHT;
const int _5	= 53;
const int _2	= 50;
const int _9	= 57;
const int W		= 119;
const int A		= 100;
const int S		= 115;
const int D		= 97;
const int U		= 117;
const int J		= 106;
const int SPACE = 32;
const int B		= 98;
const int _0    = 48;
const int F1    = VK_F1;
const int F2    = VK_F2;
const int ENTER = 13;

class Input: public functor<float,int>
{
protected:
	struct mouseButtonState
	{
		long x,y;
		bool down;
	};

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
		int x,y;
		mouseClick(): callBack(MOUSE_SCROLL){}
		mouseClick(bool Down, mouseButton b, int X, int Y): callBack(MOUSE_CLICK), down(Down), button(b), x(X), y(Y){}
	};
	struct mouseScroll: public callBack{
		double rotations;
		mouseScroll(): callBack(MOUSE_SCROLL){}
		mouseScroll(double rot): callBack(MOUSE_SCROLL), rotations(rot){}
	};

protected:
	void sendCallbacks(callBack* c);
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void windowsInput(UINT uMsg, WPARAM wParam, LPARAM lParam){}
public:
	int lastKey; 
	int tPresses;
	virtual void down(int k)=0;
	virtual void up(int k)=0;
	virtual void joystick(unsigned int buttonMask,int x,int y,int z){}
	virtual void update(){}
	virtual bool getKey(int key)=0;
	virtual const mouseButtonState& getMouseState(mouseButton m)=0;
	virtual float operator() (int key)=0;
 };

class standard_input: public Input
{
protected:
	bool keys[256];
	mouseButtonState leftMouse, rightMouse, middleMouse;
	HANDLE  inputMutex; 


public:
	standard_input(): Input()
	{
		int i;
		for(i=0;i<256;i++)
			keys[i]=false;
		inputMutex=CreateMutex(NULL,false,NULL);
		tPresses=0;
	}
	~standard_input()
	{
		CloseHandle( inputMutex );
	}
	virtual void down(int k)
	{
		if(k>=256 || k<0) return;
		WaitForSingleObject( inputMutex, INFINITE );
		keys[k]=true;
		tPresses++;
		lastKey=k;
		ReleaseMutex( inputMutex );


	}
	virtual void up(int k)
	{
		if(k>=256 || k<0) return;
		WaitForSingleObject( inputMutex, INFINITE );
		keys[k]=false;
		ReleaseMutex( inputMutex );


	}
	virtual bool getKey(int key)
	{
		if(key>=256 || key<0) false;
		WaitForSingleObject( inputMutex, INFINITE );
		bool b=keys[key];
		ReleaseMutex( inputMutex );
		return b;
	}
	virtual const mouseButtonState& getMouseState(mouseButton m)
	{
		if(m == LEFT_BUTTON)		return leftMouse;
		else if(m == MIDDLE_BUTTON) return middleMouse;
		else						return rightMouse;
	}
	virtual void windowsInput(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uMsg)
		{
			case WM_KEYDOWN:
				if((lParam & 0x40000000)==0)
				{
					down(wParam);
					sendCallbacks(new keyStroke(false, wParam));
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
	virtual void update(){}
	virtual float operator() (int key) {return getKey(key) ? 1.0f : 0.0f;}
};

extern Input *input;