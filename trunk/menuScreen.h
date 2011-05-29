
class modeMapBuilder;
namespace menu{

class element
{
public:
	enum elementType{NONE=-1,BUTTON=0,STATIC,SLIDER,LIST,TOGGLE,LABEL,CHECKBOX};
	enum elementState{ACTIVE=0,DISABLED};
	enum elementView{VISABLE=0,HIDDEN};

	element(elementType t): type(t), x(0), y(0), active(true), view(true), changed(false) {}
	element(elementType t, float X, float Y, string Text="", Color c=Color(0,0,0)): type(t), x(X), y(Y), text(Text), color(c), active(true), view(true), changed(false) {}
	element(elementType t, float X, float Y, float Width, float Height, string Text="", Color c=Color(0,0,0)): type(t), x(X), y(Y), width(Width), height(Height), text(Text), color(c), active(true), view(true), changed(false) {}

	virtual ~element(){}

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

class label: public element
{
public:
	label(int X, int Y, string t,Color c=Color(0,0,0)): element(LABEL, X, Y,t,c){}
	virtual ~label(){}

	void render();
};
class button: public element
{
public:

	button(int X, int Y, int Width, int Height, string t, Color c = Color(0,1,0), Color textC = Color(0,0,0)): element(BUTTON,X,Y,Width,Height,t,c),textColor(textC){setElementText(text);}
	virtual ~button(){}

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
class checkBox:public element
{
public:
	checkBox(int X, int Y, string t, bool startChecked=false, Color c = Color(0,1,0)): element(CHECKBOX,X,Y,textManager->getTextWidth(t) + 30,textManager->getTextHeight(t),t,c),checked(false),clicking(false){}
	virtual ~checkBox(){}

	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);
protected:


	void click();
	void unclick();

	bool checked;
	bool clicking;
};
class toggle: public element
{
public:
	toggle(vector<button*> b, Color clickedC, Color unclickedC, label* l = NULL, int startValue=0);
	virtual ~toggle(){}

	int addButton(button* b);
	void setLabel(label* l){Label=l;}
	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	int getValue(){return value;}
	void setValue(int v){value = clamp(v,0,buttons.size()-1);updateColors();}
	unsigned int getSize(){return buttons.size();}
protected:
	void updateColors();

	vector<button*> buttons;
	label* Label;
	int value;
	Color clicked, unclicked;
};
class slider:public element
{
public:
	slider(int X, int Y, int Width, int Height, float uValue, float lValue = 0, Color c = Color(0,1,0)): element(SLIDER,X,Y,Width,Height,"",c),minValue(lValue),maxValue(uValue),value(lValue),clicking(false),mouseOffset(0.0f){}
	virtual ~slider(){}

	void render();

	void mouseDownL(int X, int Y);
	void mouseUpL(int X, int Y);

	void setMinValue(float m);
	void setMaxValue(float m);
	void setValue(float v){value = clamp(v,minValue,maxValue);}

	float getValue();


protected:
	float minValue;
	float maxValue;
	float value;

	bool clicking;
	float mouseOffset;
};
class popup
{
public:
	popup(): done(false),callback(NULL){}
	virtual ~popup(){}

	virtual int update()=0;
	virtual void render()=0;

	virtual void mouseL(bool down, int x, int y){}
	virtual void mouseR(bool down, int x, int y){}
	virtual void keyDown(int vkey){}
	virtual void keyUp(int vkey){}
	virtual void scroll(float rotations){}

	bool isDone(){return done;}
	functor<void,popup*>* callback;
protected:
	bool done;
	map<string,button*> buttons;
	map<string,label*> labels;
	map<string,toggle*> toggles;
	map<string,slider*> sliders;

	friend class manager;
};

class openFile: public popup
{
public:
	openFile(){}
	~openFile(){}

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

	vector<button*> folderButtons;
	vector<button*> fileButtons;

	//menuButton* desktop;
	//menuButton* myDocuments;
	//menuButton* myComputer;
	//menuButton* myNetwork;
};
class saveFile: public openFile, functor<void,popup*>
{
public:
	void operator() (popup* p);
	saveFile():replaceDialog(false){}
protected:
	bool replaceDialog;
	void fileSelected();
};
class messageBox: public popup
{
public:
	messageBox():value(-1),clicking(-1){}
	~messageBox(){}

	bool init(string t);
	bool init(string t, vector<string> buttons);
	int update(){return 0;}
	void render();

	void mouseL(bool down, int x, int y);
	int getValue(){return value;}
protected:
	vector<label*> options;
	//menuLabel* label;
	int value;

	int x, y, width, height;

	int clicking;

};
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
class screen: functor<void,popup*>
{
public:
	screen(){}
	virtual ~screen(){}
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
	virtual void operator() (popup* p){}
	//bool popupActive(){return popup != NULL;}
protected:
	static bool loadBackground();
	static int backgroundImage;

	map<string,button*> buttons;
	map<string,label*> labels;
	map<string,toggle*> toggles;
	map<string,slider*> sliders;
	friend class manager;
};


class levelEditor: public screen
{
public:
	friend class modeMapBuilder;
	enum Tab{NO_TAB, TERRAIN,OBJECTS,SETTINGS};
	levelEditor():awaitingMapFile(false),awaitingMapSave(false),awaitingLevelFile(false),awaitingLevelSave(false),awaitingNewObject(false),newObjectType(0){}
	~levelEditor(){}
	bool init();
	int update();
	void render();

	void mouseL(bool down, int x, int y);
	void scroll(float rotations);
	void mouseC(bool down, int x, int y);
	Tab getTab();
	int getShader();//gets
	int placingObject(){return newObjectType;}
	void operator() (popup* p);
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

	//bool awaitingShaderFile;
	bool awaitingMapFile;
	bool awaitingMapSave;
	bool awaitingLevelFile;
	bool awaitingLevelSave;
	bool awaitingNewObject;

	int newObjectType;
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
	void keyDown(int vkey);
protected:
	choice activeChoice;
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
	void keyDown(int vkey);
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
class manager
{
public:
	static manager& getInstance()
	{
		static manager* pInstance = new manager();
		return *pInstance;
	}

	void setMenu(screen* m);
	bool setPopup(popup* p){if(p != NULL)popups.push_back(p);return p!=NULL;}

	void shutdown();
	int update();
	void render();

	void inputCallback(Input::callBack* callback);

	screen* getMenu(){return menu;}
	popup* getPopup(){return popups.empty() ? NULL : popups.back();}

	void drawCursor(){mDrawCursor = true;}
private:
	vector<popup*> popups;
	screen* menu;

	manager(): menu(NULL){}
	~manager() {setMenu(NULL);}

	bool mDrawCursor;
};
}

extern menu::manager& menuManager;

void messageBox(string text);
void closingMessage(string text,string title="error");