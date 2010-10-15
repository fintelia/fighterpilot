
class guiBase
{
public:
	typedef unsigned int handle;
	enum elementType{BUTTON,MSGBOX,LABEL,TEXTBOX};
	struct callBack
	{
		handle generator;
		elementType type;
		int result;
	};
private:
	struct color
	{
		float r,g,b;
		color(float R, float G, float B): r(R), g(G), b(B) {}
	};
	struct element
	{
		handle id;
		string text;
		elementType type;
		float x,y,width,height;
	};
	struct label: public element{};
	struct textBox: public element{};
	struct button: public element
	{
		functor<void,callBack>* callback;
		void issueCallBack()
		{
			callBack b;
			b.generator = id;
			b.result = 0;
			b.type = BUTTON;
			(*callback)(b);
		}
	};
	struct msgBox: public element
	{
		vector<button> buttons;
	};

	map<handle,element*> elements;
	handle currentId;
	handle currentFocus;
	bool draggingMsg;
	int dragX;
	int dragY;

public:

	guiBase(): currentId(0), currentFocus(0), draggingMsg(false) {}
	void draw();
	handle newMsgBox(string t);
	handle newLabel(string t, float x, float y);
	handle newButton(string t, float x, float y, float width, float height, functor<void,callBack>* callback);

	void editElement(handle id, string t);
	void editElement(handle id, float x, float y);
	void editElement(handle id, string t, float x, float y);
	void editElement(handle id, float x, float y, float width, float height);
	void editElement(handle id, string t, float x, float y, float width, float height);

	//handle newTextBox(string t, float x, float y, float width, float height, functor<void,callBack>* callback);

	void mouseUpdate(bool left,bool right, int x, int y);
};

extern guiBase* GUI;