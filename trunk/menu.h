
extern int sh,sw;

class menus
{
protected:
	int selected;
	int side;//0,1;
	int TotalRows;
	int TotalColumns;
	int type;
	vector<string> options;
	void updateMove()
	{
		static int repeat=0;
		static float up=1;
		static float down=1;
		static int sideRepeat=1;
		static float left=1;
		static float right=1;
		int move=0;

		if(input->getKey(LEFT)&&!input->getKey(RIGHT))
		{
			if(sideRepeat>=0)
			{
				move=-1;
				left=1;
				sideRepeat=-1;
			}
			else if(sideRepeat==-1)
			{
				left-=0.05;
			}
			else if(sideRepeat==-2)
			{
				left-=0.1;
			}
			if(left<=0)
			{
				move=-1;
				left=1;
				if(--sideRepeat<-2)
					sideRepeat=-2;
			}
			right=1;

		}
		else if(!input->getKey(LEFT)&&input->getKey(RIGHT))
		{
 			if(sideRepeat>=0)
			{
				move=1;
				right=1;
				sideRepeat=-1;
			}
			else if(sideRepeat==-1)
			{
				right-=0.05;
			}
			else if(sideRepeat==-2)
			{
				right-=0.1;
			}
			if(right<=0)
			{
				move=1;
				right=1;
				if(--sideRepeat<-2)
					sideRepeat=-2;
			}
			left=1;
		}
		else if(!input->getKey(LEFT)&&!input->getKey(RIGHT))
		{
			right=1;
			left=1;
			sideRepeat=0;
		}
	
		if(move!=0)
		{
			side+=move;
			//Redisplay=true;
		}

		move=0;

		if(input->getKey(UP)&&!input->getKey(DOWN))
		{
			if(repeat>=0)
			{
				move=-1;
				up=1;
				repeat=-1;
			}
			else if(repeat==-1)
			{
				up-=0.05;
			}
			else if(repeat==-2)
			{
				up-=0.1;
			}
			if(up<=0)
			{
				move=-1;
				up=1;
				if(--repeat<-2)
					repeat=-2;
			}
			down=1;
		}
		else if(!input->getKey(UP)&&input->getKey(DOWN))
		{
			if(repeat>=0)
			{
				move=1;
				down=1;
				repeat=-1;
			}
			else if(repeat==-1)
			{
				down-=0.05;
			}
			else if(repeat==-2)
			{
				down-=0.1;
			}
			if(down<=0)
			{
				move=1;
				down=1;
				if(--repeat<-2)
					repeat=-2;
			}
			up=1;
		}
		else if(!input->getKey(UP)&&!input->getKey(DOWN))
		{
			down=1;
			up=1;
			repeat=0;
		}
	
		if(move!=0)
		{
			selected+=move;

		}
		Redisplay=true;

		if(selected>TotalRows)
			selected-=TotalRows;
		if(selected<1)
			selected+=TotalRows;
		if(side>=TotalColumns)
			side-=TotalColumns;
		if(side<1)
			side+=TotalColumns;
	}
	virtual void init()=0;
public:
	modeType newMode;
	menuType newMenu;
	bool inGame;
	bool paused;
	virtual int update(int value)=0;
	virtual void draw()
	{
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0 , sw, sh);
		glLoadIdentity();
		viewOrtho(sw,sh);
		glColor3f(1,1,1);
		int spacing=50;
		int firstHeight=(sh-(options.size()-1)*spacing)/2;
		for(int i=0;i<options.size();i++)
		{
			if(selected==i+1)
				glColor3f(1,1,0);
			else
				glColor3f(1,1,1);
			textManager->renderText(options[i],sw-textManager->getTextWidth(options[i])/2,firstHeight+spacing*i);
		}
		viewPerspective();
	}
};
class closedMenu: public menus
{
private:
	void init();
public:
	int update(int value);
	void draw(){}
	closedMenu();
};
class mainMenu:public menus
{
private:
	void init();
public:
	mainMenu();
	int update(int value);
	void draw();
};

class setControls: public menus
{
private:
	void init();
public:
	bool choosing;
	int pressCount;
	setControls();
	int update(int value);
	void draw();
};

class inGameMenu: public menus
{
private:
	void init();
public:
	inGameMenu();
	int update(int value);
	//void draw();
};
class inGameOptions: public menus
{
private:
	void init();
public:
	inGameOptions();
	int update(int value);
	//void draw();
};
class m_start:public menus
{
private:
	void init();
public:
	m_start();
	int update(int value);
	void draw();
};
class m_chooseMode:public menus
{
private:
	void init();
public:
	m_chooseMode();
	int update(int value);
	void draw();
};
//
//class m_multiPlayer:public menus
//{
//private:
//	void init();
//public:
//	m_multiPlayer();
//	int update(int value);
//	void draw();
//};
//
//class m_singlePlayer:public menus
//{
//private:
//	void init();
//public:
//	m_singlePlayer();
//	int update(int value);
//	void draw();
//};

extern menus *Cmenu;