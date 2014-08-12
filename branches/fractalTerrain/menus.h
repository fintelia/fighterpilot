namespace gui
{
class inGame: public popup
{
public:
	enum choice{RESUME=0,OPTIONS=1,QUIT=2};
	inGame();
	~inGame(){}
	void updateFrame(){}
	void render();
	bool menuKey(int mkey);
	bool keyDown(int vkey, char ascii);
protected:
	choice activeChoice;
};
class levelEditor: public screen
{
private:
	mutable Vec2i cellFound;
	mutable bool cellFoundValid;
	mutable vector<Vec2i> checkedCells;
	mutable vector<Vec2f> checkLine;
	mutable Vec3f rStart;
	mutable Vec3f rDirection;

	shared_ptr<GraphicsManager::View> view;
	shared_ptr<GraphicsManager::View> objectPreviewView;

	LevelFile levelFile;
	shared_ptr<GraphicsManager::vertexBuffer> terrainVertexBuffer;
	shared_ptr<GraphicsManager::indexBuffer> terrainIndexBuffer;
	shared_ptr<GraphicsManager::vertexBuffer> terrainSkirtVBO;
	shared_ptr<GraphicsManager::texture2D> groundTex;
	unsigned int numTerrainIndices;
	bool terrainValid;

	vector<objectType> typeOptions;

	enum Tab{NO_TAB, TERRAIN,OBJECTS,REGIONS} lastTab;

	bool awaitingMapFile;
	bool awaitingMapSave;
	bool awaitingLevelFile;
	bool awaitingLevelSave;
	//bool awaitingNewObject;

	int selectedObject;
	bool placingNewObject;

	bool newRegionRadius;
	Vec2f newRegionCenter;

	map<int,Circle<float>> objectCircles;

	Vec3f center;
	float fovy;

	float objPlacementAlt;

	Vec3f orthoCenter;
	float orthoScale;
	int LOD;

public:
	
	levelEditor():terrainValid(false), lastTab((Tab)-1), awaitingMapFile(false),awaitingMapSave(false),awaitingLevelFile(false),awaitingLevelSave(false),selectedObject(-1),placingNewObject(false),newRegionRadius(false),center(0,0,0), objPlacementAlt(10.0){}
	~levelEditor(){}
	bool init();
	void update();
	void render();
	void render3D(unsigned int view);

	bool mouse(mouseButton button, bool down);
	bool scroll(float rotations);
	Tab getTab();
	int getShader();
	void operator() (popup* p);

private:
	float getHeight(unsigned int x, unsigned int z) const;
	float getInterpolatedHeight(float x, float z) const;
	float getTrueHeight(float x, float z) const;
	void setHeight(unsigned int x, unsigned int z, float height) const;
	void increaseHeight(unsigned x, unsigned int z, float increase) const;
	Vec3f getNormal(unsigned int x, unsigned int z) const;
	Vec3f getInterpolatedNormal(float x, float z) const;
	Vec3f getTrueNormal(float x, float z) const;
	bool rayHeightmapIntersection(Vec3f rayStart, Vec3f rayDirection, Vec3f& collisionPoint) const;

	void setMinMaxHeights();

	float randomDisplacement(float h1, float h2, float d);
	float randomDisplacement(float h1, float h2,float h3, float h4, float d);
	void diamondSquareFill(int x1, int x2, int y1, int y2);
	void diamondSquare(float h, float m, int subdivide);
	void beautifyCoastline();
	void faultLine();
	void smooth(unsigned int radius);
	void roughen(float a);
	void erode(unsigned int radius);

	void resetView();
	void fromFile(string filename);

	void updateObjectCircles();

	void renderTerrain(bool drawWater, float scale, float seaLevelOffset);

	void renderObjectPreview();
	Rect orthoView();
};
class chooseMode: public screen
{
public:
	enum choice{SINGLE_PLAYER=0,MULTIPLAYER=1,MAP_EDITOR=2};
	chooseMode():activeChoice(SINGLE_PLAYER){}
	~chooseMode(){}
	bool init(){activeChoice=SINGLE_PLAYER;return true;}
	void render();
	bool keyDown(int vkey, char ascii);
	bool menuKey(int mkey);
	void operator() (popup* p);
protected:
	choice activeChoice;
	bool choosingFile;
};
class chooseMap: public screen
{
public:
	chooseMap():currentChoice(0){}
	~chooseMap(){}
	bool init();
	void render();
	bool keyDown(int vkey, char ascii);
protected:
	vector<string> mapChoices;
	int currentChoice;
};
class options: public popup
{
private:
	struct OptionState
	{
		int fullscreenChoice;
		int resolutionChoice;
		int samplesChoice;
		
		unsigned int refreshRate;
		unsigned int rendererVersion;

		float gamma;
		bool vSync;
		bool textureCompression;
	}initialState;

	GraphicsManager::displayMode currentDisplayMode;
	set<GraphicsManager::displayMode> displayModes;

	int lastResolutionChoice;
	vector<Vec2u> resolutionChoices;
	vector<unsigned int> refreshRates;
	shared_ptr<FileManager::iniFile> settingsFile;
public:
	bool init();
	void updateFrame();
	bool keyDown(int vkey, char ascii);
	void render();
private:

};
class help: public popup
{
private:
	string helpText;
public:
	bool init();
	void render();
	void updateFrame();
	bool keyDown(int vkey, char ascii);
};
class credits: public popup
{
public:
	bool init();
	void render();
	void updateFrame();
	bool keyDown(int vkey, char ascii);
};
class loading:public screen
{
public:
	loading();
	bool init();
	void update();
	void render();
protected:
	float progress;
};
class dogFight: public simulationScreen
{
protected:
	shared_ptr<const LevelFile> level;
	vector<LevelFile::Trigger> triggers;

	void healthBar(float x, float y, float width, float height, float health) const;
	void tiltMeter(float x1,float y1,float x2,float y2,float degrees) const;
	void radar(float x, float y, float width, float height,bool firstPerson, shared_ptr<plane> p) const;
	void targeter(float x, float y, float apothem, Angle tilt) const;
	void drawHexCylinder(shared_ptr<GraphicsManager::View> view, Vec3f center, float radius, float height, Color c) const;
	void drawHudIndicator(shared_ptr<GraphicsManager::View> view, shared_ptr<plane> p, shared_ptr<object> targetPtr, Color4 color, Color4 nightColor) const;
	void drawScene(shared_ptr<GraphicsManager::View> view, int acplayer);

public:
	dogFight(shared_ptr<const LevelFile> lvl);
	virtual ~dogFight();

	virtual bool init()=0;
	void updateSimulation();
};
class splitScreen: public dogFight
{
protected:
	enum GameType{COOPERATIVE, PLAYER_VS_PLAYER}gameType;
	float countdown;
	bool restart;
	bool levelup;
	bool victory;
	shared_ptr<GraphicsManager::View> views[2];

	void updateFrame();

public:
	splitScreen(shared_ptr<const LevelFile> lvl);
	bool init();
	bool menuKey(int mkey);
	void render();
	void render3D(unsigned int v);
	void renderTransparency(unsigned int v);
};
class campaign: public dogFight
{
protected:
	float countdown;
	bool restart;
	bool levelup;
	bool victory;

#ifdef _DEBUG
	bool slow;
	bool wireframe;
#endif

	shared_ptr<GraphicsManager::View> view;

	void updateFrame();

public:
	campaign(shared_ptr<const LevelFile> lvl);
	bool init();
	bool menuKey(int mkey);
	void render();
	void render3D(unsigned int v);
	void renderTransparency(unsigned int v);
};
}
