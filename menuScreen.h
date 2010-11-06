
class menuElement
{
public:
	enum elementType{NONE=-1,BUTTON=0,STATIC,SLIDER,LIST,TOGGLE,LABEL};
	enum elementState{ACTIVE=0,DISABLED};
	enum elementView{VISABLE=0,HIDDEN};

	menuElement(elementType t): type(t), x(0), y(0), active(true), view(true) {}
	virtual ~menuElement(){}

	virtual void render()=0;

	virtual void setElementXYWH(int X, int Y, int Width, int Height)	{x=X; y=Y; width=Width; height=Height;}
	virtual void setElementXY(int X, int Y)	{x=X; y=Y;}
	virtual void setElementText(string t) {text=t;}
	virtual void setElementColor(Color c) {color=c;}
	int getType() {return type;}

	//state
	void activateElement()	{active = true;}
	void disableElement()	{active = false;}
	bool getElementState()	{return active;}

	void showElement()		{view = true;}
	void hideElement()		{view = false;}
	bool getElementView()	{return view;}

	string getText()		{return text;}
	//events
	virtual void mouseDownR(int X, int Y){}
	virtual void mouseDownL(int X, int Y){}
	virtual void mouseUpR(int X, int Y){}
	virtual void mouseUpL(int X, int Y){}
	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}
protected:
	elementType type;
	int x;
	int y;
	int width;
	int height;
	string text;
	Color color;

	bool active;
	bool view;
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

	menuButton(): menuElement(BUTTON), pressed(false){}
	virtual ~menuButton(){}

	void init(int X, int Y, int Width, int Height, string t, Color c = Color(0,1,0));
	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	bool getPressed(){return pressed;}
	void reset(){pressed=false;}

	void setElementText(string t);
	void setElementXYWH(int X, int Y, int Width, int Height);
protected:
	string clampedText;

	void click();
	void unclick();

	bool pressed;
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
	unsigned int getSize(){return buttons.size();}
protected:
	vector<menuButton*> buttons;
	menuLabel* label;
	int value;
	Color clicked, unclicked;
};
class menuPopup
{
public:
	menuPopup(): done(false){}
	virtual ~menuPopup(){}

	virtual int update()=0;
	virtual void render()=0;

	virtual void mouseL(bool down, int x, int y){}
	virtual void mouseR(bool down, int x, int y){}
	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}
	virtual void scroll(float rotations){}

	bool isDone(){return done;}
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

};

class menuScreen
{
public:
	menuScreen(): popup(NULL){}
	virtual ~menuScreen(){}
	virtual bool init()=0;
	virtual int update()=0;
	virtual void render()=0;
	///////////////////////////////////////////
	virtual void mouseL(bool down, int x, int y){if(popup!=NULL)popup->mouseL(down,x,y);}
	virtual void mouseR(bool down, int x, int y){if(popup!=NULL)popup->mouseR(down,x,y);}
	virtual void mouseC(bool down, int x, int y){}

	virtual void keyDown(int vkey){if(popup!=NULL)popup->keyDown(vkey);}
	virtual void keyUp(int vkey){if(popup!=NULL)popup->keyUp(vkey);}

	virtual void scroll(float rotations){}
	///////////////////////////////////////////
	bool popupActive(){return popup != NULL;}
protected:
	static bool loadBackground();
	static int backgroundImage;
	menuPopup* popup;

};

class menuLevelEditor: public menuScreen
{
public:
	friend class mapBuilder;
	enum Tab{NO_TAB, TERRAIN,ZONES,SETTINGS};
	menuLevelEditor(){}
	~menuLevelEditor(){}
	bool init();
	int update();
	void render();

	void mouseL(bool down, int x, int y);
	void scroll(float rotations);
	void mouseC(bool down, int x, int y);
	Tab getTab();
	int getShader();//gets  
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

};

class menuInGame: public menuScreen
{
public:
	enum choice{RESUME=0,OPTIONS=1,QUIT=2};
	menuInGame(): activeChoice(RESUME){}
	~menuInGame(){}
	bool init(){activeChoice=RESUME;return true;}
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

	bool init();
	void shutdown();
	int update();
	void render();

	//void mouseUpdate(bool left,bool right, int x, int y);
	//void keyUpdate(bool down, int vkey);
	//void scrollUpdate(float rotations);

	void inputCallback(Input::callBack* callback);

	menuScreen* getMenu(){return menu;}
private:
	typedef menuScreen *(*menuCreateFunc)();
	//static MenuManager* pInstance;
	MenuManager* popupMenu;
	bool popupActive;
	menuScreen* menu;

	//vector<menuScreen*> menuTrail
	//#define REGISTERMENU(a) registerMenu(#a, &CreateObject<a>);

	MenuManager():popupMenu(NULL), popupActive(false), menu(NULL){}
	~MenuManager();
	map<string,menuCreateFunc> menuList;
	void registerMenu(string menuName,menuCreateFunc menuFunc);
	menuScreen* createMenu(string menuName);
	void destroyMenu(menuScreen* menu);
};

extern MenuManager& menuManager;