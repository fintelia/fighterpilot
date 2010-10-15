
extern Input *input;


const int CON_CLIMB=0;
const int CON_DIVE=1;
const int CON_RIGHT=2;
const int CON_LEFT=3;
const int CON_ACCEL=4;
const int CON_BRAKE=5;
const int CON_SHOOT=6;
const int CON_MISSILE=7;

class control
{
private:
	bool valid;
	bool integer;
	int dir;
	int *axis;
	int key;
public:
	unsigned char my_char;
	control(int *a,bool positive): integer(true), axis(a), valid(true)
	{
		if(positive)
			dir=1;
		else
			dir=-1;
	}
	control(int k,unsigned char c): integer(false), key(k), valid(true), my_char(c)
	{

	}
	control(): integer(false), dir(0), valid(false)
	{

	}
	int value()
	{
		if(!valid)
			return 0;
		if(integer)
		{
			if(*axis*dir>0)
				return *axis*dir;
			else 
				return 0;
		}
		else
		{
			if(input->getKey(key))
				return 1000;
			else 
				return 0;
		}
	}
	void newControl(int *a,bool positive)
	{
		valid=true;
		integer=true;
		my_char=0;
		axis=a;
		if(positive)
			dir=1;
		else
			dir=-1;
	}
	void newControl(int k,unsigned char c)
	{
		valid=true;
		integer=false;
		key=k;
		my_char=c;
	}
	void newKey(unsigned char k)
	{
		valid=true;
		integer=false;
		key=k;
		my_char=k;
	}
	void invalidate()
	{
		valid=false;
		my_char=-1;
	}
	bool operator== (const control c) const
	{
		if(integer==c.integer)//if both are integers or keys
		{
			if(integer)
			{
				if(axis==c.axis && dir==c.dir)
					return true;
			}
			else
			{
				if(key==c.key)
					return true;
			}
		}
		return false;
	}

};
class nControl
{
private:
	bool valid;
	int key;
	int direction;
	functor<float,int>* func;
	float threshold;

public:
	static const int NEGATIVE=-1;
	static const int POSITIVE=1;
	nControl(int Key, int Direction=POSITIVE, float Threshold=0, functor<float,int>* Func=input): valid(true), key(Key), direction(Direction), func(Func), threshold(Threshold) {}
	nControl(): valid(false), key(0), direction(0), func(NULL), threshold(0) {}
	void invalidate()
	{
		valid=false;
	}
	bool operator== (const nControl c) const
	{
		if(!valid==false || !c.valid) return false;
		return key==c.key && func==c.func && direction==c.direction;//and threshold?
	}
	float operator() () const
	{
		if(!valid) return 0;
		float r=(*func)(key)*direction;
		return (r > threshold) ? r : 0;
	}
	int getKey()
	{
		return key;
	}
};


class playerControls
{
private:

public:
	nControl c[8];
	bool load_controls(char *filename)
	{
		ifstream myfile (filename);
		if (myfile.is_open())
		{
			for(int i=0;i<8;i++)
			{
				myfile.read((char *)(&c[i]),sizeof(c[i]));
			}
			myfile.close();
		}
		else
		{
			return false;
		}
	}
	bool save_controls(char *filename)
	{
		ofstream myfile (filename);
		if (myfile.is_open())
		{
			for(int i=0;i<8;i++)
			{
				myfile.write((char *)(&c[i]),sizeof(c[i]));
			}
			myfile.close();
		}
		else
		{
			return false;
		}
	}

	playerControls(){}
	playerControls(int p)//default controls for player p
	{
		if(p==1)
		{
			#ifdef USING_XINPUT
				xinput_input* x=(xinput_input*)input;
				if(x->g_Controllers[0].bConnected)
				{
					c[CON_CLIMB]=nControl		(THUMB_LY +	GAMEPAD1_OFFSET, nControl::NEGATIVE);
					c[CON_DIVE]=nControl		(THUMB_LY +	GAMEPAD1_OFFSET, nControl::POSITIVE);
					c[CON_LEFT]=nControl		(THUMB_LX +	GAMEPAD1_OFFSET, nControl::NEGATIVE);
					c[CON_RIGHT]=nControl		(THUMB_LX +	GAMEPAD1_OFFSET, nControl::POSITIVE);
					c[CON_ACCEL]=nControl		(THUMB_RY +	GAMEPAD1_OFFSET, nControl::POSITIVE);
					c[CON_BRAKE]=nControl		(THUMB_RY +	GAMEPAD1_OFFSET, nControl::NEGATIVE);
					c[CON_SHOOT]=nControl		(RIGHT_TRIGGER + GAMEPAD1_OFFSET, nControl::POSITIVE);
					c[CON_MISSILE]=nControl		(LEFT_TRIGGER +	GAMEPAD1_OFFSET, nControl::POSITIVE);
				}
				else
			#endif
				{
					c[CON_CLIMB]=nControl(VK_NUMPAD5);
					c[CON_DIVE]=nControl(VK_NUMPAD2);
					c[CON_LEFT]=nControl(VK_LEFT);
					c[CON_RIGHT]=nControl(VK_RIGHT);
					c[CON_ACCEL]=nControl(VK_UP);
					c[CON_BRAKE]=nControl(VK_DOWN);
					c[CON_SHOOT]=nControl(VK_NUMPAD0);
					c[CON_MISSILE]=nControl(VK_NUMPAD9);
				}
			//save_controls("saved controls/P1 default.ctrl"); //uncomment to update defaults
		}
		else if(p==2)
		{
			#ifdef USING_XINPUT
				xinput_input* x=(xinput_input*)input;
				if(x->g_Controllers[1].bConnected)
				{
					c[CON_CLIMB]=nControl		(THUMB_LY +	GAMEPAD2_OFFSET, nControl::NEGATIVE);
					c[CON_DIVE]=nControl		(THUMB_LY +	GAMEPAD2_OFFSET, nControl::POSITIVE);
					c[CON_LEFT]=nControl		(THUMB_LX +	GAMEPAD2_OFFSET, nControl::NEGATIVE);
					c[CON_RIGHT]=nControl		(THUMB_LX +	GAMEPAD2_OFFSET, nControl::POSITIVE);
					c[CON_ACCEL]=nControl		(THUMB_RY +	GAMEPAD2_OFFSET, nControl::POSITIVE);
					c[CON_BRAKE]=nControl		(THUMB_RY +	GAMEPAD2_OFFSET, nControl::NEGATIVE);
					c[CON_SHOOT]=nControl		(RIGHT_TRIGGER + GAMEPAD2_OFFSET, nControl::POSITIVE);
					c[CON_MISSILE]=nControl		(LEFT_TRIGGER +	GAMEPAD2_OFFSET, nControl::POSITIVE);
				}
				else
			#endif
				{
					c[CON_CLIMB]=nControl(0x55);
					c[CON_DIVE]=nControl(0x4A);
					c[CON_LEFT]=nControl(0x41);
					c[CON_RIGHT]=nControl(0x44);
					c[CON_ACCEL]=nControl(0x57);
					c[CON_BRAKE]=nControl(0x53);
					c[CON_SHOOT]=nControl(0x20);
					c[CON_MISSILE]=nControl(0x42);
				}
			//save_controls("saved controls/P2 default.ctrl"); //uncomment to update defaults
		}
	}
	nControl getCotrol(int con)
	{
		
		if(con>=0 && con<8)
			return c[con];
		else
		{
			return nControl();
		}
	}
	void setControl(int con,nControl k)
	{
		if(con>=0 && con<8)
			c[con]=k;
	}
	float playerControls::operator[](int index) {
		return c[index]();
	}

	//int playerControls::operator[](int index) const {
	//	return getCotrol(index).value();
	//}
};

class Controls
{
private:
	vector<playerControls> pControls;
public:
	Controls(int numPlayers)
	{
		for(int i=0;i<numPlayers;i++)
		{
			playerControls p(i+1);
			pControls.push_back(p);
		}
	}
	playerControls &Controls::operator[](int index) {
		return pControls[index];
	}

	//playerControls Controls::operator[](int index) const {
	//	return pControls[index];
	//}
};

extern Controls controls;