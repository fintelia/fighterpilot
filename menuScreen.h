
class menuElement
{
public:
	enum elementType{NONE=-1,BUTTON=0,STATIC,SLIDER,LIST,TOGGLE,LABEL};
	enum elementState{ACTIVE=0,DISABLED};
	enum elementView{VISABLE=0,HIDDEN};

	menuElement(elementType t): type(t), x(0), y(0), active(true), view(true), changed(false) {}
	virtual ~menuElement(){}

	virtual void render()=0;

	virtual void setElementXYWH(int X, int Y, int Width, int Height)	{x=X; y=Y; width=Width; height=Height;}
	virtual void setElementXY(int X, int Y)	{x=X; y=Y;}
	virtual void setElementText(string t) {text=t;}
	virtual void setElementColor(Color c) {color=c;}

	//state
	void activateElement()	{active = true;}
	void disableElement()	{active = false;}
	bool getElementState()	{return active;}

	void showElement()		{view = true;}
	void hideElement()		{view = false;}
	bool getElementView()	{return view;}

	string getText()		{return text;}
	
	//changed
	bool getChanged()		{return changed;}
	void resetChanged()		{changed = false;}

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
	menuLabel(): menuElement(LABEL){}
	virtual ~menuLabel(){}

	void init(int X, int Y, string t){x=X;y=Y;text=t;}
	void render();

protected:
};
class menuButton: public menuElement
{
public:

	menuButton(): menuElement(BUTTON),textColor(Color(0,0,0)){}
	virtual ~menuButton(){}

	void init(int X, int Y, int Width, int Height, string t, Color c = Color(0,1,0), Color textC = Color(0,0,0));
	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	//bool getPressed(){return pressed;}
	//void reset(){pressed=false;}

	void setElementText(string t);
	void setElementTextColor(Color c);
	void setElementXYWH(int X, int Y, int Width, int Height);
protected:
	Color textColor;
	string clampedText;

	void click();
	void unclick();

	//bool pressed;
	bool clicking;
};
class menuToggle: public menuElement
{
public:
	public:

	menuToggle(): menuElement(TOGGLE), value(-1), label(NULL){}
	virtual ~menuToggle(){}

	void init(vector<menuButton*> b, Color clickedC, Color unclickedC, int startValue=0);
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
};

class menuChooseFile: public menuPopup
{
public:
	menuChooseFile(){}
	~menuChooseFile(){}

	bool init();
	bool init(string ExtFilter);
	int update();
	void render();
	
	void refreshView();
	string getFile() {return (directory/file).string();}

	void keyDown(int vkey);
	void mouseL(bool down, int x, int y);
protected:
	string file;

	filesystem::path directory;

	string extFilter;
	vector<string> files;
	vector<string> folders;

	vector<menuButton*> folderButtons;
	vector<menuButton*> fileButtons;

	menuButton* desktop;
	menuButton* myDocuments;
	menuButton* myComputer;
	menuButton* myNetwork;
};
class menuMessageBox: public menuPopup
{
public:
	menuMessageBox():value(-1){}
	~menuMessageBox(){}

	bool init(string t);
	bool init(string t, vector<string> buttons);
	int update(){return 0;}
	void render();

	void mouseL(bool down, int x, int y);
protected:
	vector<menuButton*> buttons;
	menuLabel* label;
	int value;

	int x, y, width, height;
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
	//menuPopup* popup;

};

class menuLevelEditor: public menuScreen
{
public:
	friend class mapBuilder;
	enum Tab{NO_TAB, TERRAIN,ZONES,SETTINGS};
	menuLevelEditor():awaitingShaderFile(false),awaitingMapFile(false){}
	~menuLevelEditor(){}
	bool init();
	int update();
	void render();

	void mouseL(bool down, int x, int y);
	void scroll(float rotations);
	void mouseC(bool down, int x, int y);
	Tab getTab();
	int getShader();//gets
	void operator() (menuPopup* p);
protected:
	//Tab currentTab;

	void addShader(string filename);
	menuToggle* bShaders;
	menuButton* bNewShader;

	menuButton* bDiamondSquare;
	menuButton* bFaultLine;
	menuButton* bFromFile;

	menuButton* bExit;

	menuToggle* bOnHit;
	menuToggle* bOnAIHit;
	menuToggle* bGameType;
	menuToggle* bMapType;
	menuToggle* bSeaFloorType;
	//...

	menuToggle* bTabs;

	bool awaitingShaderFile;
	bool awaitingMapFile;
};
class menuChooseMode: public menuScreen 
{
public:
	enum choice{MULTIPLAYER=0,SINGLE_PLAYER=1,MAP_EDITOR=2};
	menuChooseMode():activeChoice(MULTIPLAYER){}
	~menuChooseMode(){}
	bool init(){activeChoice=MULTIPLAYER;return true;}
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