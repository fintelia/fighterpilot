
class modeScreen
{
public:
	virtual int update()=0;
	virtual void draw2D()=0;
	virtual void draw3D()=0;
	virtual bool init(){return true;}
};

class ModeManager
{
public:
	static ModeManager& getInstance()
	{
		static ModeManager* pInstance = new ModeManager();
		return *pInstance;
	}

	bool setMode(modeScreen* newMode);

	bool init();
	void shutdown();
	int update();

	void render2D();
	void render3D();

	modeScreen* getMode(){return mode;}
private:
	modeScreen* mode;

	ModeManager(): mode(NULL){}
	~ModeManager();
	void destroyMode(modeScreen* mode);
};

extern ModeManager& modeManager;

class modeMapBuilder: public modeScreen
{
	Quat4f rot;
	Vec3f center;
	editLevel* level;
	float maxHeight;
	float minHeight;
	float scroll;
	float objPlacementAlt;

	void zoom(float rotations);
	void trackBallUpdate(int newX, int newY);
	void resetView();
	void diamondSquare(float h);
	void faultLine();
	void fromFile(string filename);
	void addObject(int type, int team, int controlType, int x, int y);
	vector<int> shaderButtons;
	friend class menuLevelEditor;
public:
	virtual int update();
	virtual void draw3D();
	void draw2D();
	modeMapBuilder(): center(0,0,0), level(NULL), maxHeight(0), minHeight(0), scroll(0.0), objPlacementAlt(10.0) {}
	bool init();
};

class modeDogFight: public modeScreen
{
protected:
	FrustumG frustum;
public:
	modeDogFight(Level* lvl);
	virtual ~modeDogFight();

	void healthBar(float x, float y, float width, float height, float health, bool firstPerson);
	void tiltMeter(float x1,float y1,float x2,float y2,float degrees);
	void radar(float x, float y, float width, float height,bool firstPerson);
	void targeter(float x, float y, float apothem, Angle tilt);

	void drawWater(Vec3f eye);
	void drawExaust();
	void drawPlanes(int acplayer,Vec3f e,bool showBehind=false,bool showDead=false);
	void drawBullets();
	void drawScene(int acplayer);
};

class modeSplitScreen: public modeDogFight
{
public:
	modeSplitScreen(Level* lvl);
	modeSplitScreen();
	int update();
	void draw2D();
	void draw3D();
};

class modeCampaign: public modeDogFight
{
protected:
	float countdown;
	bool levelup;
public:
	modeCampaign(Level* lvl): modeDogFight(lvl), levelup(false), countdown(0.0){}
	modeCampaign();
	int update();
	void draw2D();
	void draw3D();
};
