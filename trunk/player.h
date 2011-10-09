
class object;//needs to know about the object class to control it

const int CON_CLIMB=0;
const int CON_DIVE=1;
const int CON_RIGHT=2;
const int CON_LEFT=3;
const int CON_ACCEL=4;
const int CON_BRAKE=5;
const int CON_SHOOT=6;
const int CON_MISSILE=7;
const int CON_BOMB=8;

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
	nControl c[9];

	playerControls(){}
	playerControls(int p)//default controls for player p
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
				c[CON_BOMB]=nControl		(LEFT_SHOULDER + GAMEPAD1_OFFSET, nControl::POSITIVE);
			}
			else
		#endif
		if(p==1)
		{
			c[CON_CLIMB]=nControl(VK_DOWN);
			c[CON_DIVE]=nControl(VK_UP);
			c[CON_LEFT]=nControl(VK_LEFT);
			c[CON_RIGHT]=nControl(VK_RIGHT);
			c[CON_ACCEL]=nControl(VK_NUMPAD5);
			c[CON_BRAKE]=nControl(VK_NUMPAD2);
			c[CON_SHOOT]=nControl(VK_NUMPAD0);
			c[CON_MISSILE]=nControl(VK_NUMPAD9);
			c[CON_BOMB]=nControl(VK_NUMPAD8);
		}
		else if(p==2)
		{

			c[CON_CLIMB]=nControl(0x57);	//U
			c[CON_DIVE]=nControl(0x53);		//J
			c[CON_LEFT]=nControl(0x41);		//A
			c[CON_RIGHT]=nControl(0x44);	//D
			c[CON_ACCEL]=nControl(0x55);	//W
			c[CON_BRAKE]=nControl(0x4A);	//S
			c[CON_SHOOT]=nControl(0x20);	//SPACE
			c[CON_MISSILE]=nControl(0x42);	//B
			c[CON_BOMB]=nControl(0x56);	//V
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
	float operator[](int index) {
		return c[index]();
	}
};

class objectController
{
protected:
	int		mObjectNum,
			mKills;

public:
	const controlType type;

	int objectNum()				{return mObjectNum;}
	void objectNum(int value)	{mObjectNum=value;}
	int kills()					{return mKills;}
	void addKill()				{mKills++;}

	object* getObject();

	virtual void update(){}
	virtual controlState getControlState()=0;

	objectController(controlType t, int oNum):mObjectNum(oNum),mKills(0),type(t){}
};

class humanControl: public objectController
{
private:

	int				PlayerNum;
	bool			FirstPerson;
	playerControls	controls;

	static int		TotalPlayers;
public:
	humanControl(int oNum):objectController(CONTROL_HUMAN,oNum), PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}
	humanControl():objectController(CONTROL_HUMAN,0), PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}

	int playerNum()				{return PlayerNum;}

	bool firstPerson()			{return FirstPerson;}
	void toggleFirstPerson()	{FirstPerson=!FirstPerson;}
	void firstPerson(bool value){FirstPerson=value;}

	bool active()				{return mObjectNum!=0;}

	controlState getControlState(){
		controlState c;
		c.accelerate=controls[CON_ACCEL];
		c.brake=controls[CON_BRAKE];
		c.climb=controls[CON_CLIMB];
		c.dive=controls[CON_DIVE];
		c.left=controls[CON_LEFT];
		c.right=controls[CON_RIGHT];
		c.shoot1=controls[CON_SHOOT];
		c.shoot2=controls[CON_MISSILE];
		c.shoot3=controls[CON_BOMB];
		return c;
	}
};

class AIcontrol: public objectController
{
private:
	int	target;
	controlState contState;
public:
	AIcontrol(int oNum): objectController(CONTROL_COMPUTER,oNum)
	{
		contState.accelerate=0.0;
		contState.brake=0.0;
		contState.climb=0.0;
		contState.dive=0.0;
		contState.left=0.0;
		contState.right=0.0;
		contState.shoot1=0.0;
		contState.shoot2=0.0;
		contState.shoot3=0.0;
	}
	controlState getControlState()
	{
		return contState;
	}
	void update();
};
