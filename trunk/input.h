
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
public:
	int lastKey; 
	int tPresses;
	virtual void down(int k)=0;
	virtual void up(int k)=0;
	virtual void joystick(unsigned int buttonMask,int x,int y,int z){};
	virtual void update(){};
	virtual bool getKey(int key)=0;
	virtual float operator() (int key)=0;
 };

class standard_input: public Input
{
public:
	bool keys[256];
	//long keyPresses;
	HANDLE  inputMutex; 
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
	//virtual void specialDown(int k)
	//{
	//	if(k>=128 || k<0) return;
	//	WaitForSingleObject( inputMutex, INFINITE );
	//	keys[k+128]=true;
	//	lastKey=&keys[k+128];
	//	tPresses++;
	//	lastAscii=k+128;
	//	ReleaseMutex( inputMutex );
	//}
	//virtual void specialUp(int k)
	//{
	//	if(k>=128 || k<0) return;
	//	WaitForSingleObject( inputMutex, INFINITE );
	//	keys[k+128]=false;
	//	ReleaseMutex( inputMutex );
	//}
	virtual bool getKey(int key)
	{
		if(key>=256 || key<0) false;
		WaitForSingleObject( inputMutex, INFINITE );
		bool b=keys[key];
		ReleaseMutex( inputMutex );
		return b;
	}
	virtual void update(){}
	virtual float operator() (int key) {return getKey(key) ? 1.0f : 0.0f;}
};

extern Input *input;