
const int UP	= 229;
const int DOWN	= 231;
const int LEFT	= 230;
const int RIGHT	= 228;
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
const int F1    = 129;
const int F2    = 130;
const int ENTER = 13;

class Input
{
public:
	bool keys[256];
	bool special[21];
	bool joy[10];
	int axes[3];
	bool *lastKey;
	unsigned char lastAscii; 
	int tPresses;
	Input()
	{
		memset(keys,false,256);
		memset(joy,false,10);
		memset(axes,0,3);
	}
	virtual void down(int k){}
	virtual void up(int k){}
	virtual void specialDown(int k){}
	virtual void specialUp(int k){}
	virtual void joystick(unsigned int buttonMask,int x,int y,int z){}
 };

class GLUT_input: public Input
{
public:
	long keyPresses;
	GLUT_input()
	{
		int i;
		for(i=0;i<256;i++)
			keys[i]=false;
		for(i=0;i<10;i++)
			joy[i]=false;
		for(i=0;i<3;i++)
			axes[i]=0;
		tPresses=0;
	}
	void down(int k)
	{
		keys[k]=true;
		tPresses++;
		lastKey=&keys[k];
		lastAscii=k;
	}
	void up(int k)
	{
		keys[k]=false;
	}
	void specialDown(int k)
	{
		special[0]=special[0]||GLUT_KEY_F1==k;
		special[1]=special[1]||GLUT_KEY_F2==k;
		special[2]=special[2]||GLUT_KEY_F3==k;
		special[3]=special[3]||GLUT_KEY_F4==k;
		special[4]=special[4]||GLUT_KEY_F5==k;
		special[5]=special[5]||GLUT_KEY_F6==k;
		special[6]=special[6]||GLUT_KEY_F7==k;
		special[7]=special[7]||GLUT_KEY_F8==k;
		special[8]=special[8]||GLUT_KEY_F9==k;
		special[9]=special[9]||GLUT_KEY_F10==k;
		special[10]=special[10]||GLUT_KEY_F11==k;
		special[11]=special[11]||GLUT_KEY_F12==k;

		special[12]=special[12]||GLUT_KEY_LEFT==k;
		special[13]=special[13]||GLUT_KEY_UP==k;
		special[14]=special[14]||GLUT_KEY_RIGHT==k;
		special[15]=special[15]||GLUT_KEY_DOWN==k;

		special[16]=special[16]||GLUT_KEY_PAGE_UP==k;
		special[17]=special[17]||GLUT_KEY_PAGE_DOWN==k;
		special[18]=special[18]||GLUT_KEY_HOME==k;
		special[19]=special[19]||GLUT_KEY_END==k;
		special[19]=special[19]||GLUT_KEY_INSERT==k;
		keys[k+128]=true;
		lastKey=&keys[k+128];
		tPresses++;
		lastAscii=k+128;
	}
	void specialUp(int k)
	{
		special[0]=special[0]&&GLUT_KEY_F1!=k;
		special[1]=special[1]&&GLUT_KEY_F2!=k;
		special[2]=special[2]&&GLUT_KEY_F3!=k;
		special[3]=special[3]&&GLUT_KEY_F4!=k;
		special[4]=special[4]&&GLUT_KEY_F5!=k;
		special[5]=special[5]&&GLUT_KEY_F6!=k;
		special[6]=special[6]&&GLUT_KEY_F7!=k;
		special[7]=special[7]&&GLUT_KEY_F8!=k;
		special[8]=special[8]&&GLUT_KEY_F9!=k;
		special[9]=special[9]&&GLUT_KEY_F10!=k;
		special[10]=special[10]&&GLUT_KEY_F11!=k;
		special[11]=special[11]&&GLUT_KEY_F12!=k;

		special[12]=special[12]&&GLUT_KEY_LEFT!=k;
		special[13]=special[13]&&GLUT_KEY_UP!=k;
		special[14]=special[14]&&GLUT_KEY_RIGHT!=k;
		special[15]=special[15]&&GLUT_KEY_DOWN!=k;

		special[16]=special[16]&&GLUT_KEY_PAGE_UP!=k;
		special[17]=special[17]&&GLUT_KEY_PAGE_DOWN!=k;
		special[18]=special[18]&&GLUT_KEY_HOME!=k;
		special[19]=special[19]&&GLUT_KEY_END!=k;
		special[19]=special[19]&&GLUT_KEY_INSERT!=k;
		keys[k+128]=false;
	}
	void joystick(unsigned int buttonMask,int x,int y,int z)
	{	
		axes[0]=x;
		axes[1]=y;
		axes[2]=z;
		for(int i=9;i>=0;i--)
		{
			joy[i]=false;
			if(buttonMask>=pow(2.0f,i))
			{
				joy[i]=true;
				buttonMask-=(unsigned int)pow(2.0f,i);
			}
		}
	}
};

Input *input=new GLUT_input;

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
	}
	input->down(int(key));
}
void KeyUpFunc(unsigned char key,int x,int y) {
	input->up(int(key));
}

void SpecialKeyDownFunc(int key,int x,int y) {
	input->specialDown(key);
}

void SpecialKeyUpFunc(int key,int x,int y) {
	input->specialUp(key);
}


void Joystick(unsigned int buttonMask, int x, int y, int z)
{
	input->joystick(buttonMask,x,y,z);
	//j[0]=x;
	//j[1]=y;
	//j[2]=z;
	//for(int i=9;i>=0;i--)
	//{
	//	buttons[i]=false;
	//	if(buttonMask>=pow(2.0f,i))
	//	{
	//		buttons[i]=true;
	//		buttonMask-=(unsigned int)pow(2.0f,i);
	//	}
	//}
}