
class menuElement
{
public:
	enum elementType{NONE=-1,BUTTON=0,STATIC,SLIDER,LIST,TOGGLE,LABEL,CHECKBOX};
	enum elementState{ACTIVE=0,DISABLED};
	enum elementView{VISABLE=0,HIDDEN};

	menuElement(elementType t): type(t), x(0), y(0), active(true), view(true), changed(false) {}
	menuElement(elementType t, float X, float Y, string Text="", Color c=Color(0,0,0)): type(t), x(X), y(Y), text(Text), color(c), active(true), view(true), changed(false) {}
	menuElement(elementType t, float X, float Y, float Width, float Height, string Text="", Color c=Color(0,0,0)): type(t), x(X), y(Y), width(Width), height(Height), text(Text), color(c), active(true), view(true), changed(false) {}

	virtual ~menuElement(){}

	virtual void render()=0;

	virtual void setElementXYWH(int X, int Y, int Width, int Height)	{x=X; y=Y; width=Width; height=Height;}
	virtual void setElementXY(int X, int Y)	{x=X; y=Y;}
	virtual void setElementText(string t) {text=t;}
	virtual void setElementColor(Color c) {color=c;}

	//state
	void activateElement()		{active = true;}
	void disableElement()		{active = false;}
	bool getElementState()		{return active;}

	void showElement()			{view = true;}
	void hideElement()			{view = false;}
	bool getVisibility()		{return view;}
	void setVisibility(bool v)	{view = v;}

	string getText()			{return text;}
	
	//changed
	bool getChanged()			{return changed;}
	bool checkChanged()			{bool b = changed; changed=false; return b;}//returns changed then sets it to false
	void resetChanged()			{changed = false;}

	//events
	virtual void mouseDownR(int X, int Y){}
	virtual void mouseDownL(int X, int Y){}
	virtual void mouseUpR(int X, int Y){}
	virtual void mouseUpL(int X, int Y){}
	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}

	const elementType type;
protected:
	int x;
	int y;
	int width;
	int height;
	string text;
	Color color;

	bool active;
	bool view;
	bool changed;
};

class menuLabel: public menuElement
{
public:
	menuLabel(int X, int Y, string t,Color c=Color(0,0,0)): menuElement(LABEL, X, Y,t,c){}
	virtual ~menuLabel(){}

	void render();
};
class menuButton: public menuElement
{
public:

	menuButton(int X, int Y, int Width, int Height, string t, Color c = Color(0,1,0), Color textC = Color(0,0,0)): menuElement(BUTTON,X,Y,Width,Height,t,c),textColor(textC){setElementText(text);}
	virtual ~menuButton(){}

	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	void setElementText(string t);
	void setElementTextColor(Color c);
	void setElementXYWH(int X, int Y, int Width, int Height);
protected:
	Color textColor;
	string clampedText;

	void click();
	void unclick();

	bool clicking;
};
class menuCheckBox:public menuElement
{
public:
	menuCheckBox(int X, int Y, string t, bool startChecked=false, Color c = Color(0,1,0)): menuElement(CHECKBOX,X,Y,textManager->getTextWidth(t) + 30,textManager->getTextHeight(t),t,c),checked(false),clicking(false){}
	virtual ~menuCheckBox(){}

	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);
protected:


	void click();
	void unclick();

	bool checked;
	bool clicking;
};
class menuToggle: public menuElement
{
public:
	menuToggle(vector<menuButton*> b, Color clickedC, Color unclickedC, menuLabel* l = NULL, int startValue=0);
	virtual ~menuToggle(){}

	int addButton(menuButton* button);
	void setLabel(menuLabel* l){label=l;}
	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	int getValue(){return value;}
	void setValue(int v){value = clamp(v,0,buttons.size()-1);updateColors();}
	unsigned int getSize(){return buttons.size();}
protected:
	void updateColors();

	vector<menuButton*> buttons;
	menuLabel* label;
	int value;
	Color clicked, unclicked;
};
class menuPopup
{
public:
	menuPopup(): done(false),callback(NULL){}
	virtual ~menuPopup(){}

	virtual int update()=0;
	virtual void render()=0;

	virtual void mouseL(bool down, int x, int y){}
	virtual void mouseR(bool down, int x, int y){}
	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}
	virtual void scroll(float rotations){}

	bool isDone(){return done;}
	functor<void,menuPopup*>* callback;
protected:
	bool done;
	map<string,menuButton*> buttons;
	map<string,menuLabel*> labels;
	map<string,menuToggle*> toggles;

	friend class MenuManager;
};

class menuOpenFile: public menuPopup
{
public:
	menuOpenFile(){}
	~menuOpenFile(){}

	bool init();
	bool init(string ExtFilter);
	int update();
	void render();
	
	void refreshView();
	bool validFile() {return file.compare("") != 0;}
	string getFile() {return (directory/file).string();}

	void keyDown(int vkey);
	void mouseL(bool down, int x, int y);
protected:
	virtual void fileSelected(){done=true;}
	string file;

	filesystem::path directory;

	string extFilter;
	vector<string> files;
	vector<string> folders;

	vector<menuButton*> folderButtons;
	vector<menuButton*> fileButtons;

	//menuButton* desktop;
	//menuButton* myDocuments;
	//menuButton* myComputer;
	//menuButton* myNetwork;
};
class menuSaveFile: public menuOpenFile, functor<void,menuPopup*>
{
public:
	void operator() (menuPopup* p);
	menuSaveFile():replaceDialog(false){}
protected:
	bool replaceDialog;
	void fileSelected();
};
class menuMessageBox: public menuPopup
{
public:
	menuMessageBox():value(-1),clicking(-1){}
	~menuMessageBox(){}

	bool init(string t);
	bool init(string t, vector<string> buttons);
	int update(){return 0;}
	void render();

	void mouseL(bool down, int x, int y);
	int getValue(){return value;}
protected:
	vector<menuLabel*> options;
	//menuLabel* label;
	int value;

	int x, y, width, height;

	int clicking;

};
class menuNewObject: public menuPopup
{
public:
	menuNewObject():type(defaultPlane),team(0){done=true;}
	~menuNewObject(){}
	int update(){return 0;}
	void render(){}
	int getObjectType(){return type;}
	int getObjectTeam(){return team;}
protected:
	int type;
	int team;
};
class menuScreen: functor<void,menuPopup*>
{
public:
	menuScreen(){}
	virtual ~menuScreen(){}
	virtual bool init()=0;
	virtual int update()=0;
	virtual void render()=0;
	///////////////////////////////////////////
	virtual void mouseL(bool down, int x, int y){}
	virtual void mouseR(bool down, int x, int y){}
	virtual void mouseC(bool down, int x, int y){}

	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}

	virtual void scroll(float rotations){}
	///////////////////////////////////////////
	virtual void operator() (menuPopup* p){}
	//bool popupActive(){return popup != NULL;}
protected:
	static bool loadBackground();
	static int backgroundImage;

	map<string,menuButton*> buttons;
	map<string,menuLabel*> labels;
	map<string,menuToggle*> toggles;
	friend class MenuManager;
};

class menuLevelEditor: public menuScreen
{
public:
	friend class mapBuilder;
	friend class modeMapBuilder;
	enum Tab{NO_TAB, TERRAIN,OBJECTS,SETTINGS};
	menuLevelEditor():awaitingShaderFile(false),awaitingMapFile(false),awaitingMapSave(false),awaitingLevelFile(false),awaitingLevelSave(false),awaitingNewObject(false),newObjectType(0){}
	~menuLevelEditor(){}
	bool init();
	int update();
	void render();

	void mouseL(bool down, int x, int y);
	void scroll(float rotations);
	void mouseC(bool down, int x, int y);
	Tab getTab();
	int getShader();//gets
	int placingObject(){return newObjectType;}
	void operator() (menuPopup* p);
protected:
	//Tab currentTab;

	void addShader(string filename);
	//menuToggle* bShaders;
	//menuButton* bNewShader;

	//menuButton* bDiamondSquare;
	//menuButton* bFaultLine;
	//menuButton* bFromFile;
	//menuButton* bExportBMP;

	//menuButton* bLoad;
	//menuButton* bSave;
	//menuButton* bExit;

	//menuButton* bAddPlane;

	//menuToggle* bOnHit;
	//menuToggle* bOnAIHit;
	//menuToggle* bGameType;
	//menuToggle* bMapType;
	//menuToggle* bSeaFloorType;

	//menuToggle* bTabs;

	bool awaitingShaderFile;
	bool awaitingMapFile;
	bool awaitingMapSave;
	bool awaitingLevelFile;
	bool awaitingLevelSave;
	bool awaitingNewObject;

	int newObjectType;
};
class menuChooseMode: public menuScreen 
{
public:
	enum choice{SINGLE_PLAYER=0,MULTIPLAYER=1,MAP_EDITOR=2};
	menuChooseMode():activeChoice(MULTIPLAYER){}
	~menuChooseMode(){}
	bool init(){activeChoice=SINGLE_PLAYER;return true;}
	int update(){Redisplay=true;return 30;}
	void render();
	void keyDown(int vkey);
protected:
	choice activeChoice;
};
class menuInGame: public menuScreen
{
public:
	enum choice{RESUME=0,OPTIONS=1,QUIT=2};
	menuInGame(): activeChoice(RESUME){}
	~menuInGame(){}
	bool init();
	int update(){Redisplay=true;return 30;}
	void render();
	void keyDown(int vkey);
protected:
	choice activeChoice;
};
class menuLoading:public menuScreen
{
public:
	menuLoading():progress(0.0f){}
	~menuLoading(){}
	bool init();
	int update();
	void render();
protected:
	float progress;
};
class MenuManager
{
public:
	static MenuManager& getInstance()
	{
		static MenuManager* pInstance = new MenuManager();
		return *pInstance;
	}

	bool setMenu(string menuName);
	bool setPopup(menuPopup* p){if(p != NULL)popups.push_back(p);return p!=NULL;}

	bool init();
	void shutdown();
	int update();
	void render();

	//void mouseUpdate(bool left,bool right, int x, int y);
	//void keyUpdate(bool down, int vkey);
	//void scrollUpdate(float rotations);

	void inputCallback(Input::callBack* callback);

	menuScreen* getMenu(){return menu;}
	menuPopup* getPopup(){return popups.empty() ? NULL : popups.back();}

	void drawCursor(){mDrawCursor = true;}
private:
	typedef menuScreen *(*menuCreateFunc)();
	//static MenuManager* pInstance;
	vector<menuPopup*> popups;
	menuScreen* menu;

	//vector<menuScreen*> menuTrail
	//#define REGISTERMENU(a) registerMenu(#a, &CreateObject<a>);

	MenuManager(): menu(NULL){}
	~MenuManager();
	void registerMenu(string menuName,menuCreateFunc menuFunc);
	menuScreen* createMenu(string menuName);
	void destroyMenu(menuScreen* menu);

	map<string,menuCreateFunc> menuList;

	bool mDrawCursor;
};

extern MenuManager& menuManager;

void messageBox(string text);
void closingMessage(string text,string title="error");