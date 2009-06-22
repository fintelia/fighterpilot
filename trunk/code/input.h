
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

class Input
{
public:
	bool keys[256];
	bool special[21];
	bool joy[14][4];
	int axes[6][4];
	bool *lastKey;
	int *lastAxis;
	int lastAscii; 
	int tPresses;
	Input()
	{
		int i;
		for(i=0;i<256;i++)
			keys[i]=false;
		for(int l=0;l<4;l++)
		{
			for(i=0;i<14;i++)
				joy[i][l]=false;
			for(i=0;i<6;i++)
				axes[i][l]=0;
		}
	}
	virtual void down(int k){}
	virtual void up(int k){}
	virtual void specialDown(int k){}
	virtual void specialUp(int k){}
	virtual void joystick(unsigned int buttonMask,int x,int y,int z){}
	virtual void update(){}
 };

class standard_input: public Input
{
public:
	//long keyPresses;
	standard_input()
	{
		int i;
		for(i=0;i<256;i++)
			keys[i]=false;
		for(int l=0;l<4;l++)
		{
			for(i=0;i<14;i++)
				joy[i][l]=false;
			for(i=0;i<6;i++)
				axes[i][l]=0;
		}
		tPresses=0;
	}
	virtual void down(int k)
	{
		keys[k]=true;
		tPresses++;
		lastKey=&keys[k];
		lastAscii=k;
	}
	virtual void up(int k)
	{
		keys[k]=false;
	}
	virtual void specialDown(int k)
	{
		keys[k+128]=true;
		lastKey=&keys[k+128];
		tPresses++;
		lastAscii=k+128;
	}
	virtual void specialUp(int k)
	{
		keys[k+128]=false;
	}
	virtual void update(){}
};

//Input *input=new standard_input;