namespace menu
{

class newObject: public popup
{
public:
	newObject(int Type=defaultPlane,int Team=0):type(Type),team(Team){done=true;}
	~newObject(){}
	int update(){return 0;}
	void render(){}
	int getObjectType(){return type;}
	int getObjectTeam(){return team;}
protected:
	int type;
	int team;
};
class objectProperties: public popup
{
protected:
	LevelFile::Object* object;

public:
	objectProperties():object(NULL){}
	~objectProperties(){}

	bool init(LevelFile::Object* obj);

	int update();
	void render();
};
class levelEditor: public screen
{
public:
	enum Tab{NO_TAB, TERRAIN,OBJECTS,REGIONS};
	levelEditor():awaitingMapFile(false),awaitingMapSave(false),awaitingLevelFile(false),awaitingLevelSave(false),awaitingNewObject(false),newObjectType(0),lastTab((Tab)-1), center(0,0,0), level(NULL), maxHeight(0), minHeight(0), scrollVal(0.0), objPlacementAlt(10.0),newRegionRadius(false){}
	~levelEditor(){}
	bool init();
	int update();
	void render();
	void render3D();
	
	bool mouse(mouseButton button, bool down);
	bool scroll(float rotations);
	Tab getTab();
	int getShader();
	void operator() (popup* p);
protected:
	

	float randomDisplacement(float h1, float h2, float d);
	float randomDisplacement(float h1, float h2,float h3, float h4, float d);
	void diamondSquareFill(int x1, int x2, int y1, int y2);
	void diamondSquare(float h, float m, int subdivide);
	void faultLine();
	void smooth(int a);

	void resetView();
	void fromFile(string filename);

	void addObject(int type, int team, int controlType, float x, float y);
	void updateObjectCircles();

	Rect orthoView();

	Tab lastTab;

	bool awaitingMapFile;
	bool awaitingMapSave;
	bool awaitingLevelFile;
	bool awaitingLevelSave;
	bool awaitingNewObject;

	int newObjectType;

	bool newRegionRadius;
	Vec2f newRegionCenter;

	map<int,circle<float>> objectCircles;

	Quat4f rot;
	Vec3f center;
	editLevel* level;
	float maxHeight;
	float minHeight;
	float scrollVal;
	float objPlacementAlt;

	//bool ortho;
	Vec3f orthoCenter;
	float orthoScale;


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
class inGame: public screen
{
public:
	enum choice{RESUME=0,OPTIONS=1,QUIT=2};
	inGame(): activeChoice(RESUME){}
	~inGame(){}
	bool init();
	int update(){return 30;}
	void render();
	bool keyDown(int vkey);
protected:
	choice activeChoice;
};
class loading:public screen
{
public:
	loading():progress(0.0f){}
	~loading(){}
	bool init();
	int update();
	void render();
protected:
	float progress;
};
}