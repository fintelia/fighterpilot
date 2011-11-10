
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

class controls
{
public:
	virtual float operator[] (int control) = 0;
};

class keyboardControls: public controls
{
private:
	unsigned char virtualKeys[9];
public:
	keyboardControls(int player);
	float operator[] (int control);
};

#ifdef USING_XINPUT
class xboxControls: public controls
{
	int mControllerNum;
	struct mapping{
		enum Type{BUTTON,AXIS,AXIS_POSITIVE,AXIS_NEGATIVE}type;
		int m;

		float operator() (int controllerNum);
		mapping(int M, Type t): type(t), m(M){}
		mapping(){}
	};
	map<int,mapping> mappings;
public:
	xboxControls(int player);
	int getControllerNum(){return mControllerNum;}
	float operator[] (int control);
};
#endif

class playerControls
{
private:

public:
	nControl c[9];

	playerControls(){}
	playerControls(int p);
	nControl getCotrol(int con);
	void setControl(int con,nControl k);
	float operator[](int index);
};

class objectController
{
protected:
	int		mObjectNum,
			mKills;
	
public:
	struct camera{
		Vec3f eye;
		Vec3f center;
		Vec3f up;
	};

	camera	firstPerson;
	camera	thirdPerson;
	bool	firstPersonView;
	const	controlType type;

	int objectNum()				{return mObjectNum;}
	void objectNum(int value)	{mObjectNum=value;}
	int kills()					{return mKills;}
	void addKill()				{mKills++;}

	const camera& getCamera() const {return firstPersonView ? firstPerson : thirdPerson;}

	void toggleFirstPerson() 	{firstPersonView = !firstPersonView;}

	object* getObject();

	virtual void update(){}
	virtual controlState getControlState()=0;

	objectController(controlType t, int oNum):mObjectNum(oNum),mKills(0),type(t){}
};

class humanControl: public objectController
{
private:
	int				PlayerNum;
	playerControls	controls;

	static int		TotalPlayers;
public:
	humanControl(int oNum):objectController(CONTROL_HUMAN,oNum), PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}
	humanControl():objectController(CONTROL_HUMAN,0), PlayerNum(TotalPlayers), controls(TotalPlayers){TotalPlayers++;}

	int playerNum()				{return PlayerNum;}
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