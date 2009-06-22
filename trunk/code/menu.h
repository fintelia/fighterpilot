

class menus
{
protected:
	int selected;
	int side;//0,1;
	int TotalRows;
	int TotalColumns;
	void ViewOrtho(int x, int y)				// Set Up An Ortho View
	{
		glMatrixMode(GL_PROJECTION);			// Select Projection
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
		glOrtho( 0, x , y , 0, -1, 1 );			// Select Ortho Mode
		glMatrixMode(GL_MODELVIEW);				// Select Modelview Matrix
		glPushMatrix();							// Push The Matrix
		glLoadIdentity();						// Reset The Matrix
	}
	void ViewPerspective()					// Set Up A Perspective View
	{
		glMatrixMode( GL_PROJECTION );			// Select Projection
		glPopMatrix();							// Pop The Matrix
		glMatrixMode( GL_MODELVIEW );			// Select Modelview
		glPopMatrix();							// Pop The Matrix
	}
	void updateMove()
	{
		static int repeat=0;
		static float up=1;
		static float down=1;
		static int sideRepeat=1;
		static float left=1;
		static float right=1;
		int move=0;

		if(input->keys[LEFT]&&!input->keys[RIGHT])
		{
			if(sideRepeat>=0)
			{
				move=1;
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
				move=1;
				left=1;
				if(--sideRepeat<-2)
					sideRepeat=-2;
			}
			right=1;

		}
		else if(!input->keys[LEFT]&&input->keys[RIGHT])
		{
 			if(sideRepeat>=0)
			{
				move=-1;
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
				move=-1;
				right=1;
				if(--sideRepeat<-2)
					sideRepeat=-2;
			}
			left=1;
		}
		else if(!input->keys[LEFT]&&!input->keys[RIGHT])
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

		if(input->keys[UP]&&!input->keys[DOWN])
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
		else if(!input->keys[UP]&&input->keys[DOWN])
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
		else if(!input->keys[UP]&&!input->keys[DOWN])
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
		if(side<0)
			side+=TotalColumns;
	}
public:
	int newMode;
	int newMenu;
	bool paused;
	virtual int update(int value)=0;
	virtual void draw()=0;
};
class closedMenu: public menus
{
public:
	int update(int value)
	{
		return 30;
	}
	void draw(){}
	closedMenu()
	{
		selected=0;
		TotalRows=0;
		TotalColumns=0;
		Redisplay=true;
		newMode=0;
		newMenu=0;
		paused=false;
	}
};


#include "mainMenu.h"
#include "setControls.h"
menus *Cmenu;