
//const int CON_CLIMB=0;
//const int CON_DIVE=1;
//const int CON_RIGHT=2;
//const int CON_LEFT=3;
//const int CON_ACCEL=4;
//const int CON_BRAKE=5;
//const int CON_SHOOT=6;
//const int CON_MISSILE=7;
//const int CON_BOMB=8;
//
//class nControl
//{
//private:
//	bool valid;
//	int key;
//	int direction;
//	functor<float,int>* func;
//	float threshold;
//
//public:
//	static const int NEGATIVE=-1;
//	static const int POSITIVE=1;
//	nControl(int Key, int Direction=POSITIVE, float Threshold=0, functor<float,int>* Func=&input): valid(true), key(Key), direction(Direction), func(Func), threshold(Threshold) {}
//	nControl(): valid(false), key(0), direction(0), func(NULL), threshold(0) {}
//	void invalidate()
//	{
//		valid=false;
//	}
//	bool operator== (const nControl c) const
//	{
//		if(!valid==false || !c.valid) return false;
//		return key==c.key && func==c.func && direction==c.direction;//and threshold?
//	}
//	float operator() () const
//	{
//		if(!valid) return 0;
//		float r=(*func)(key)*direction;
//		return (r > threshold) ? r : 0;
//	}
//	int getKey()
//	{
//		return key;
//	}
//};



//class playerControls
//{
//private:
//
//public:
//	nControl c[9];
//
//	playerControls(){}
//	playerControls(int p);
//	nControl getCotrol(int con);
//	void setControl(int con,nControl k);
//	float operator[](int index);
//};

class player
{
protected:
	int		mObjectNum,
			mKills;
	
public:
	const	playerType type;

	int objectNum()				{return mObjectNum;}
	void objectNum(int value)	{mObjectNum=value;}
	int kills()					{return mKills;}
	void addKill()				{mKills++;}

//	const camera& getCamera() const {return firstPersonView ? firstPerson : thirdPerson;}


	object* getObject();
	virtual void update()=0;

	player(playerType t, int oNum):mObjectNum(oNum),mKills(0),type(t){}
	virtual ~player(){}
};

class humanPlayer: public player
{
private:
	int	mPlayerNum;

	struct controlMapping{
		enum Type{NONE, KEY, BUTTON,AXIS,AXIS_POSITIVE,AXIS_NEGATIVE}type;
		int controllerNum;
		int m;

		float operator() ();
		controlMapping():type(NONE),m(0),controllerNum(-1){}
		controlMapping(int M): type(KEY), m(M),controllerNum(-1){}
		controlMapping(int M, Type t, int cNum): type(t), m(M),controllerNum(cNum){}
	};
	map<int,controlMapping> controls;


public:
	bool	firstPersonView;

	camera	firstPerson;
	camera	thirdPerson;

	humanPlayer(int playerNumber, int objectId);
	~humanPlayer();

	int playerNum()	const		{return mPlayerNum;}
	bool active() const			{return mObjectNum!=0;}
	void toggleFirstPerson()	{firstPersonView = !firstPersonView;}

	void update();
	const camera& getCamera(bool forceThirdPerson=false) const;
};

class AIplayer: public player
{
private:
	int	target;
public:
	AIplayer(int oNum): player(PLAYER_COMPUTER,oNum){}
	void update();
};

class PlayerManager
{
private:
	vector<shared_ptr<humanPlayer>> humanPlayers;
	vector<shared_ptr<AIplayer>> aiPlayers;

	PlayerManager(){}

public:
	static PlayerManager& getInstance();

	void addHumanPlayer(int objectId);
	void addAIplayer(int objectId);

	void resetPlayers();

	void update();

	shared_ptr<humanPlayer> operator[] (unsigned int p);

	unsigned int numPlayers(){return humanPlayers.size();}
	unsigned int numAIs(){return aiPlayers.size();}
};
extern PlayerManager& players;