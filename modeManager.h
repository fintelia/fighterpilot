
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

class modeDogFight: public modeScreen
{
protected:
	FrustumG frustum;

	GLuint radarTexture;
	GLuint radarFBO;
	GLuint radarRBO;
public:
	modeDogFight(Level* lvl);
	virtual ~modeDogFight();

	void healthBar(float x, float y, float width, float height, float health, bool firstPerson);
	void tiltMeter(float x1,float y1,float x2,float y2,float degrees);
	void radar(float x, float y, float width, float height,bool firstPerson, nPlane* p);
	void targeter(float x, float y, float apothem, Angle tilt);
	void planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight);

	void drawHexCylinder(Vec3f center, float radius, float height, Color c);
	void drawPlanes(int acplayer,bool showBehind=false,bool showDead=false);
	void drawBullets();
	void drawScene(int acplayer);
};

class modeSplitScreen: public modeDogFight
{
public:
	modeSplitScreen(Level* lvl): modeDogFight(lvl){}
	modeSplitScreen(string lvlFile): modeDogFight(new Level(LevelFile(lvlFile))){}
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
	modeCampaign(string lvlFile): modeDogFight(new Level(LevelFile(lvlFile))), levelup(false), countdown(0.0){}
	int update();
	void draw2D();
	void draw3D();
};