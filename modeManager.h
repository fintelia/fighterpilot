
class modeScreen
{
public:
	virtual int update()=0;
	virtual void draw2D()=0;
	virtual void draw3D()=0;
	virtual void drawParticles(){}
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
	void renderParticles();
	modeScreen* getMode(){return mode;}
private:
	modeScreen* mode;

	ModeManager(): mode(NULL){}
	~ModeManager();
	void destroyMode(modeScreen* mode);
};

extern ModeManager& modeManager;

