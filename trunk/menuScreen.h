
namespace gui{

class element
{
public:
	enum elementType{NONE=-1,BUTTON=0,STATIC,SLIDER,LIST,TOGGLE,LABEL,CHECKBOX,TEXTBOX,LISTBOX};
	enum elementState{ACTIVE=0,DISABLED};
	enum elementView{VISABLE=0,HIDDEN};

	element(elementType t): type(t), active(true), view(true), changed(false), focus(false) {}
	element(elementType t, float X, float Y, string Text="", Color c=Color(0,0,0)): type(t), shape(X,Y,0,0), text(Text), color(c), active(true), view(true), changed(false), focus(false) {}
	element(elementType t, float X, float Y, float Width, float Height, string Text="", Color c=Color(0,0,0)): type(t), shape(X,Y,Width,Height), text(Text), color(c), active(true), view(true), changed(false), focus(false) {}

	virtual ~element(){}

	virtual void render()=0;

	virtual void setElementPosition(Vec2f pos){shape.x = pos.x; shape.y = pos.y;}
	virtual void setElementSize(Vec2f size){shape.w = size.x; shape.h = size.y;}
	virtual void setElementShape(Rect newShape){setElementPosition(newShape.origin()); setElementSize(newShape.size());}
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
	virtual bool mouseDownR(float X, float Y){return false;}
	virtual bool mouseDownL(float X, float Y){return false;}
	virtual bool mouseUpR(float X, float Y){return false;}
	virtual bool mouseUpL(float X, float Y){return false;}
	virtual bool keyDown(int vkey){return false;}
	virtual bool keyUp(int vkey){return false;}

	virtual void gainFocus(){focus = true;}
	virtual void looseFocus(){focus = false;}

	virtual bool hasFocus(){return focus;}

	virtual bool inElement(Vec2f v){return shape.inRect(v);}
	const Rect& getShape(){return shape;}

	const elementType type;
protected:
	Rect shape;

	string text;
	Color color;

	bool active;
	bool view;
	bool changed;

	bool focus;
};

class label: public element
{
public:
	label(float X, float Y, string t,Color c=Color(0,0,0)): element(LABEL, X, Y,t,c){}
	virtual ~label(){}

	void render();
};
class button: public element
{
public:

	button(float X, float Y, float Width, float Height, string t, Color c = Color(0,1,0), Color textC = Color(0,0,0)): element(BUTTON,X,Y,Width,Height,t,c),textColor(textC){setElementText(text);}
	virtual ~button(){}

	void render();

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);

	void setElementText(string t);
	void setElementTextColor(Color c);
	void setElementSize(Vec2f size);
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
	checkBox(float X, float Y, string t, bool startChecked=false, Color c = Color(0,1,0)): element(CHECKBOX,X,Y,graphics->getTextSize(t).x + 0.29,graphics->getTextSize(t).y,t,c),checked(startChecked),clicking(false){}
	virtual ~checkBox(){}

	void render();

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);

	bool getChecked(){return checked;}
protected:

	void click();
	void unclick();

	bool checked;
	bool clicking;
};
class textBox: public element
{
public:
	textBox(float X, float Y, float Width, string str, Color textColor): element(TEXTBOX,X,Y,Width,0.029,str,textColor),clicking(false),cursorPos(0){}

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);
	bool keyDown(int vkey);

	void render();

protected:
	virtual void addChar(char c);
	bool clicking;

	int cursorPos;
};
class numericTextBox: public textBox
{
public:
	numericTextBox(float X, float Y, float Width, float val, Color textColor): textBox(X, Y, Width, lexical_cast<string>(val), textColor), useMinMaxStep(false),mAllowDecimal(false){}
	void allowDecimal(bool b){mAllowDecimal = b;}
	void setMinMaxStep(float Min,float Max,float Step){useMinMaxStep=true;minVal=Min;maxVal=Max;stepVal=Step;}
protected:
	void addChar(char c);
	void loseFocus();
	void gainFocus();

	float lastVal;
	float minVal;
	float maxVal;
	float stepVal;
	bool useMinMaxStep;
	bool mAllowDecimal;
};
class listBox: public element
{
protected:
	vector<string> options;

	bool clicking;
	bool choosing;

	int optionNum;

public:
	listBox(float X, float Y, float Width, string str, Color textColor): element(LISTBOX,X,Y,Width,0.029,str,textColor),clicking(false),choosing(false),optionNum(-1){}

	void addOption(string option);
	void setOption(unsigned int oNum){optionNum = clamp(oNum,0,options.size()-1);}
	void clearOptions(){options.clear(); optionNum=-1;}

	int getNumOptions(){return options.size();}

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);

	void gainFocus();
	void loseFocus();

	void render();

	int getOptionNumber(){return optionNum;}
};

class toggle: public element
{
public:
	toggle(vector<button*> b, Color clickedC, Color unclickedC, label* l = NULL, int startValue=0);
	virtual ~toggle(){}

	int addButton(button* b);
	void setLabel(label* l){Label=l;}
	void render();

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);

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
	slider(float X, float Y, float Width, float Height, float uValue, float lValue = 0, Color c = Color(0,1,0)): element(SLIDER,X,Y,Width,Height,"",c),minValue(lValue),maxValue(uValue),value(lValue),clicking(false),mouseOffset(0.0f){}
	virtual ~slider(){}

	void render();

	bool mouseDownL(float X, float Y);
	bool mouseUpL(float X, float Y);

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
class elementContainer
{
protected:
	element* focus;

	map<string,button*> buttons;
	map<string,label*> labels;
	map<string,toggle*> toggles;
	map<string,slider*> sliders;
	map<string,checkBox*> checkBoxes;
	map<string,textBox*> textBoxes;
	map<string,listBox*> listBoxes;

	friend class manager;

	void inputCallback(InputManager::callBack* callback);
	bool issueInputCallback(InputManager::callBack* callback, element* e);

	elementContainer():focus(NULL){}
};

class popup: public elementContainer
{
public:
	popup(): callback(NULL), done(false){}
	virtual ~popup(){}

	virtual void update()=0;
	virtual void render()=0;

	virtual bool mouseL(bool down, float x, float y){return false;}
	virtual bool mouseR(bool down, float x, float y){return false;}
	virtual bool keyDown(int vkey){return false;}
	virtual bool keyUp(int vkey){return false;}
	virtual bool menuKey(int mkey){return false;}
	virtual bool scroll(float rotations){return false;}

	bool isDone(){return done;}
	functor<void,popup*>* callback;
protected:
	bool done;

	friend class manager;
};

class openFile: public popup
{
public:
	openFile(){}
	~openFile(){}

	bool init();
	bool init(string ExtFilter);
	bool init(set<string> ExtFilters);
	void update();
	void render();

	void refreshView();
	bool validFile() {return file != "";}
	string getFile() {return directory+"/"+file;}

	bool keyDown(int vkey);
	bool mouseL(bool down, float x, float y);
protected:
	struct thumbnail
	{
		string name;
		string displayName;
		enum Type{FOLDER,BMP,UNKNOWN_FILE} type;
		thumbnail(){}
		thumbnail(string f, string n, Type t):name(f),displayName(n),type(t){}
	};


	string file;

	string directory;

	set<string> extFilters;
	vector<thumbnail> files;
	vector<thumbnail> folders;

	float scroll;

	vector<thumbnail>::iterator clickingThumbnail;

	virtual void fileSelected();
	bool getThumbnail(Vec2f v, vector<thumbnail>::iterator& i);

	//vector<button*> folderButtons;
	//vector<button*> fileButtons;

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
class messageBox_c: public popup
{
public:
	messageBox_c():value(-1),clicking(-1){}
	~messageBox_c(){}

	bool init(string t);
	bool init(string t, vector<string> buttons);
	void update(){}
	void render();

	bool mouseL(bool down, float x, float y);
	int getValue(){return value;}
protected:
	vector<label*> options;
	//menuLabel* label;
	int value;

	float x, y, width, height;

	int clicking;

};

class screen: public elementContainer, functor<void,popup*>
{
public:
	screen(){}
	virtual ~screen(){}
	virtual bool init()=0;

	virtual void updateFrame(){}
	virtual void updateSimulation(){}

	virtual void render()=0;
	virtual void render3D(unsigned int view){}
	///////////////////////////////////////////
	virtual bool mouse(mouseButton button, bool down){return false;}

	virtual bool keyDown(int vkey){return false;}
	virtual bool keyUp(int vkey){return false;}
	virtual bool menuKey(int mkey){return false;}
	virtual bool scroll(float rotations){return false;}
	///////////////////////////////////////////
	virtual void operator() (popup* p){}
	//bool popupActive(){return popup != NULL;}
protected:

	friend class manager;
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
	bool setPopup(popup* p){if(p != NULL)popups.push_back(shared_ptr<popup>(p));return p!=NULL;}

	void shutdown();

	void updateFrame();
	void updateSimulation();

	void render();
	void render3D(unsigned int view);


	void inputCallback(InputManager::callBack* callback);

	void drawCursor(){mDrawCursor = true;}
private:
	vector<shared_ptr<popup>> popups;
	screen* menu;

	manager():menu(nullptr){}

	bool mDrawCursor;
};
}

extern gui::manager& menuManager;

void messageBox(string text);
void closingMessage(string text,string title="error");
