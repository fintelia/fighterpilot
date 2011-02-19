
const int CON_CLIMB=0;
const int CON_DIVE=1;
const int CON_RIGHT=2;
const int CON_LEFT=3;
const int CON_ACCEL=4;
const int CON_BRAKE=5;
const int CON_SHOOT=6;
const int CON_MISSILE=7;

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
};

class objectController
{
public:
	virtual controlState getControlState()=0;
};

class player: public objectController
{
private:
	int				PlaneNum,
					PlayerNum,
					Kills;
	bool			Active,
					FirstPerson;
	playerControls	controls;

	static int		TotalPlayers;
public:
	player(): PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}

	int planeNum()				{return PlaneNum;}
	void planeNum(int value)	{PlaneNum=value;}

	int playerNum()				{return PlayerNum;}

	int kills()					{return Kills;}
	void addKill()				{Kills++;}

	bool firstPerson()			{return FirstPerson;}
	void toggleFirstPerson()	{FirstPerson=!FirstPerson;}
	void firstPerson(bool value){FirstPerson=value;}

	void active(bool value)		{Active=value;}
	bool active()				{return Active;}

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
		return c;
	}

};

class AIplayer: public objectController
{
private:
	int target;
public:
	controlState getControlState(){
		controlState c;
		c.accelerate=0.0;
		c.brake=0.0;
		c.climb=0.0;
		c.dive=0.0;
		c.left=0.0;
		c.right=0.0;
		c.shoot1=0.0;
		c.shoot2=0.0;
		return c;
	}

};