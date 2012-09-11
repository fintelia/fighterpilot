namespace gui
{

//class newObject: public popup
//{
//public:
//	newObject(int Type=defaultPlane,int Team=0):type(Type),team(Team){done=true;}
//	~newObject(){}
//	int update(){return 0;}
//	void render(){}
//	int getObjectType(){return type;}
//	int getObjectTeam(){return team;}
//protected:
//	int type;
//	int team;
//};
//class objectProperties: public popup
//{
//protected:
//	LevelFile::Object* object;
//	vector<objectType> typeOptions;
//public:
//	objectProperties():object(NULL){}
//	~objectProperties(){}
//
//	bool init(LevelFile::Object* obj);
//
//	int update();
//	void render();
//};
class inGame: public popup
{
public:
	enum choice{RESUME=0,OPTIONS=1,QUIT=2};
	inGame();
	~inGame(){}
	int update(){return 30;}
	void render();
	bool menuKey(int mkey);
	bool keyDown(int vkey);
protected:
	choice activeChoice;
};
class levelEditor: public screen
{
private:
	shared_ptr<GraphicsManager::View> view;
	shared_ptr<GraphicsManager::View> objectPreviewView;

	LevelFile levelFile;
	shared_ptr<GraphicsManager::vertexBuffer> terrainVertexBuffer;
	shared_ptr<GraphicsManager::indexBuffer> terrainIndexBuffer;
	shared_ptr<GraphicsManager::vertexBuffer> terrainSkirtVBO;
	shared_ptr<GraphicsManager::texture2D> groundTex;
	unsigned int numTerrainIndices;
	unsigned int numTerrainSkirtVertices;
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
	int update();
	void render();
	void render3D(unsigned int view);

	bool mouse(mouseButton button, bool down);
	bool scroll(float rotations);
	Tab getTab();
	int getShader();
	void operator() (popup* p);

private:
	float getHeight(unsigned int x, unsigned int z) const;
	float getInterpolatedHeight(float x, float y) const;
	void setHeight(unsigned int x, unsigned int z, float height) const;
	void increaseHeight(unsigned x, unsigned int z, float increase) const;
	Vec3f getNormal(unsigned int x, unsigned int z) const;
	Vec3f getInterpolatedNormal(float x, float y) const;

	void setMinMaxHeights();

	float randomDisplacement(float h1, float h2, float d);
	float randomDisplacement(float h1, float h2,float h3, float h4, float d);
	void diamondSquareFill(int x1, int x2, int y1, int y2);
	void diamondSquare(float h, float m, int subdivide);
	void faultLine();
	void smooth(int a);

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
	int update(){return 30;}
	void render();
	bool keyDown(int vkey);
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
	int update(){return 30;}
	void render();
	bool keyDown(int vkey);
protected:
	vector<string> mapChoices;
	int currentChoice;
};
class options: public screen
{
private:
	struct OptionState
	{
		int fullscreenChoice;
		int refreshRateChoice;
		int resolutionChoice;
		int samplesChoice;

		float gamma;
		bool vSync;
		bool textureCompression;
	}initialState;

	GraphicsManager::displayMode currentDisplayMode;
	set<GraphicsManager::displayMode> displayModes;

	int lastResolutionChoice;
	vector<Vec2u> resolutionChoices;
	shared_ptr<FileManager::iniFile> settingsFile;
public:
	bool init();
	int update();
	void render();
private:

};
class loading:public screen
{
public:
	loading();
	bool init();
	int update();
	void render();
protected:
	float progress;
};
class dogFight: public screen
{
protected:
	shared_ptr<LevelFile> level;
	bool firstFrame;

public:
	dogFight(shared_ptr<LevelFile> lvl);
	virtual ~dogFight();

	virtual bool init()=0;

	void healthBar(float x, float y, float width, float height, float health, bool firstPerson);
	void tiltMeter(float x1,float y1,float x2,float y2,float degrees);
	void radar(float x, float y, float width, float height,bool firstPerson, nPlane* p);
	void targeter(float x, float y, float apothem, Angle tilt);
	void planeIdBoxes(nPlane* p, float vX, float vY, float vWidth, float vHeight, shared_ptr<GraphicsManager::View> v);

	void drawHexCylinder(shared_ptr<GraphicsManager::View> view, Vec3f center, float radius, float height, Color c);
	void drawScene(shared_ptr<GraphicsManager::View> view, int acplayer);

	void checkCollisions();
};
class splitScreen: public dogFight
{
protected:
	shared_ptr<GraphicsManager::View> views[2];

public:
	splitScreen(shared_ptr<LevelFile> lvl);
	bool init();
	int update();
	void render();
	void render3D(unsigned int v);
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
#endif

	shared_ptr<GraphicsManager::View> view;
public:
	campaign(shared_ptr<LevelFile> lvl);
	bool init();
	int update();
	void render();
	void render3D(unsigned int v);
};
}
