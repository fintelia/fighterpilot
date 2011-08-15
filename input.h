
//const int UP	= VK_UP;
//const int DOWN	= VK_DOWN;
//const int LEFT	= VK_LEFT;
//const int RIGHT	= VK_RIGHT;
//const int _5	= 53;
//const int _2	= 50;
//const int _9	= 57;
//const int _W	= 119;
//const int _A	= 100;
//const int _S	= 115;
//const int _D	= 97;
//const int _U	= 117;
//const int _J	= 106;
//const int SPACE = 32;
//const int _B	= 98;
//const int _0    = 48;m
//const int F1    = VK_F1;
//const int F2    = VK_F2;
//const int ENTER = 13;

class Input: public functor<float,int>
{
protected:
	struct mouseButtonState
	{
		Vec2f downPos;
		Vec2f upPos;
		Vec2f curPos;
		bool down;
		mouseButtonState(): downPos(0,0), upPos(0,0), curPos(0,0), down(false){}
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
		Vec2f pos;
		mouseClick(): callBack(MOUSE_SCROLL){}
		mouseClick(bool Down, mouseButton b, Vec2f p): callBack(MOUSE_CLICK), down(Down), button(b), pos(p){}
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
	standard_input();
	~standard_input();
	virtual void down(int k);
	virtual void up(int k);
	virtual bool getKey(int key);
	virtual const mouseButtonState& getMouseState(mouseButton m);
	virtual void windowsInput(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void update(){}
	virtual float operator() (int key) {return getKey(key) ? 1.0f : 0.0f;}
};

extern Input *input;