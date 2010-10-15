//#include <cstdlib>
//#include <vector>
//#include <fstream>
//#include <sstream>
//#include <cmath>
//#include "GL/glee.h"
//#include <GL/glu.h>
//using namespace std;
//
//#pragma warning( disable : 4305)
//#pragma warning( disable : 4244)
//#pragma warning( disable : 4018)
//
//
//#include "basic.h"
//#include "input.h"
//#include "controls.h"
//
//extern vector<int> tex;
//extern vector<int> menusTx;
//extern GLuint menuBack;
//extern int sw,sh;
//extern bool Redisplay;
//#include "menu.h"
#include "main.h"

void closedMenu::init()
{
	selected=0;
	TotalRows=0;
	TotalColumns=0;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	paused=false;
	gameTime.unpause();
	type=1;
}
closedMenu::closedMenu()
{
	init();
}
int closedMenu::update(int value)
{
	if(type!=1)
		init();
	if(input->getKey(0x31) && inGame)
	{
		input->up(0x31);
		paused=true;
		gameTime.pause();
		//Cmenu=new inGameMenu(*((inGameMenu*)this));
		menuManager.setMenu("menuInGame");
	}
	return 30;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void mainMenu::init()
{
	selected=1;
	TotalRows=5;
	TotalColumns=1;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	paused=false;
	gameTime.unpause();
	type=2;
	inGame=false;
}
mainMenu::mainMenu()
{
	init();
}
void mainMenu::draw()
{
	float s=(float)sw/800;
	glDisable(GL_DEPTH_TEST);
	//glClearColor(0.4f,0.4f,0.4f,1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0 , sw, sh);
	glLoadIdentity();
	//gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
	viewOrtho(sw,sh);
	//glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping 
	glEnable(GL_BLEND);
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(0,0);
		glTexCoord2f(0,0);	glVertex2f(0,sh);
		glTexCoord2f(1,0);	glVertex2f(sw,sh);		
		glTexCoord2f(1,1);	glVertex2f(sw,0);
	glEnd();
	dataManager.bind("menu main");
	glBegin(GL_QUADS);
	for(int i=1;i<=5;i++)
	{
		if(i==selected)
		{		
			glTexCoord2f(0.5,1.2-0.2*i);	glVertex2f(s*202,s*i*100);
			glTexCoord2f(1  ,1.2-0.2*i);	glVertex2f(s*202+s*395,s*i*100);
			glTexCoord2f(1  ,1.0-0.2*i);	glVertex2f(s*202+s*395,s*i*100+s*52);
			glTexCoord2f(0.5,1.0-0.2*i);	glVertex2f(s*202,s*i*100+s*52);
		}
		else
		{
			glTexCoord2f(0.01  ,1.2-0.2*i);	glVertex2f(s*202,s*i*100);
			glTexCoord2f(0.49,1.2-0.2*i);	glVertex2f(s*202+s*395,s*i*100);
			glTexCoord2f(0.49,1.0-0.2*i);	glVertex2f(s*202+s*395,s*i*100+s*52);
			glTexCoord2f(0.01  ,1.0-0.2*i);	glVertex2f(s*202,s*i*100+s*52);
		}
	}
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	viewPerspective();
}
int mainMenu::update(int value)
{
	if(type!=2)
		init();
	updateMove();
	if(input->getKey(SPACE) || input->getKey(ENTER))
	{
		input->up(SPACE);
		input->up(ENTER);
		switch(selected)
		{
		case 1:
			newMode=TWO_PLAYER_VS;
			Redisplay=true;
			break;
		case 2:
			newMode=ONE_PLAYER;
			Redisplay=true;
			break;
		case 5:
			Cmenu=new setControls(*((setControls*)this));
			Redisplay=true;
			break;
		}
	}
	return 30;
}

//////////////////////////////////////////////////////////////////////////////////////////////

void setControls::init()
{
	selected=1;
	side=1;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	choosing=false;
	TotalRows=9;
	TotalColumns=2;
	type=3;
}
void setControls::draw()
{
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.4f,0.4f,0.4f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0 , sw, sh);
	glLoadIdentity();
	//gluPerspective(80.0, (double)sw / ((double)sh/2),0.999, 10000.0);
	viewOrtho(sw,sh);
	glEnable(GL_BLEND);
	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(0,0);
		glTexCoord2f(0,0);	glVertex2f(0,sh);
		glTexCoord2f(1,0);	glVertex2f(sw,sh);		
		glTexCoord2f(1,1);	glVertex2f(sw,0);
	glEnd();
	dataManager.bind("menu controls");
	glBegin(GL_QUADS);
	for(int i=1;i<=8;i++)
	{
			glTexCoord2f(0,1.077-0.077*i);		glVertex2f(202,i*50+50);
			glTexCoord2f(1,1.077-0.077*i);		glVertex2f(202+395,i*50+50);
			glTexCoord2f(1,1.000-0.077*i);		glVertex2f(202+395,i*50+102);
			glTexCoord2f(0,1.000-0.077*i);		glVertex2f(202,i*50+102);			
	}
	glEnd();
	dataManager.bind("key");
	for(int l=0;l<2;l++)
	{
		for(int i=1;i<=8;i++)
		{
			if(i==selected && l+1==side && !choosing)
				glColor3f(1,1,1);
			else if(i==selected && l+1==side && choosing)
				glColor3f(1,0.3,0.3);
			else
				glColor3f(0.5,0.5,0.5);
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);	glVertex2f(228+280*l,i*50+50);
				glTexCoord2f(1,0);	glVertex2f(228+48+280*l,i*50+50);
				glTexCoord2f(1,1);	glVertex2f(228+48+280*l,i*50+98);
				glTexCoord2f(0,1);	glVertex2f(228+280*l,i*50+98);
			glEnd();
		}
	}
	float texX=0.125;
	float texY=0.875;
	int asciiV;
	dataManager.bind("letters");
	for(int l=0;l<2;l++)
	{
		for(int i=1;i<=8;i++)
		{
			texX=0.875;
			texY=0.125;
			asciiV=controls[l].c[i-1].getKey();
			if(asciiV>=0x41 && asciiV<=0x5a)
			{
				asciiV-=0x41;
				texX=(asciiV%8);
				texY=(asciiV-asciiV%8)/8;
				texX=(texX/8.0);
				texY=1.0-(texY/8.0);
			}
			else if(asciiV>=0x30 && asciiV<=0x39)//numbers
			{
				asciiV-=0x30-28;
				texX=(asciiV%8);
				texY=(asciiV-asciiV%8)/8;
				texX=(texX/8.0);
				texY=1.0-(texY/8.0);
				if(asciiV==28)
				{
					texX=0.75;
					texY=0.5;
				}
			}
			else if(asciiV>=0x60 && asciiV<=0x69)//number pad
			{
				asciiV-=0x60-28;
				texX=(asciiV%8);
				texY=(asciiV-asciiV%8)/8;
				texX=(texX/8.0);
				texY=1.0-(texY/8.0);
				if(asciiV==28)
				{
					texX=0.75;
					texY=0.5;
				}
			}
			else if(asciiV>=0x25 && asciiV<=0x28) //arrows
			{
				switch(asciiV)
				{
				case VK_LEFT:
					asciiV=40;	break;
				case VK_RIGHT:
					asciiV=42;	break;
				case VK_UP:
					asciiV=41;	break;
				case VK_DOWN:
					asciiV=39;	break;
				}
				texX=(asciiV%8);
				texY=(asciiV-asciiV%8)/8;
				texX=(texX/8.0);
				texY=1.0-(texY/8.0);
			}

			glBegin(GL_QUADS);
				glTexCoord2f(texX,texY);				glVertex2f(228+280*l,i*50+50);
				glTexCoord2f(texX+0.125,texY);			glVertex2f(228+48+280*l,i*50+50);
				glTexCoord2f(texX+0.125,texY-0.125);	glVertex2f(228+48+280*l,i*50+98);
				glTexCoord2f(texX,texY-0.125);			glVertex2f(228+280*l,i*50+98);
			glEnd();
		}
	}
	int i=9;
	if(selected==9)
		i=10;
	glColor3f(1,1,1);
	dataManager.bind("menu controls");
	glBegin(GL_QUADS);
			glTexCoord2f(0,1.077-0.077*i);		glVertex2f(202,450+50);
			glTexCoord2f(1,1.077-0.077*i);		glVertex2f(202+395,450+50);
			glTexCoord2f(1,1.000-0.077*i);		glVertex2f(202+395,450+102);
			glTexCoord2f(0,1.000-0.077*i);		glVertex2f(202,450+102);		
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	viewPerspective();
}

int setControls::update(int value)
{
	if(type!=3)
		init();
	if(choosing)
	{
		if(input->tPresses>pressCount)
		{
			//if(input->lastAscii>=-1)
				controls[side-1].c[selected-1]=nControl(input->lastKey);
			//if(input->lastAscii==-2)
			//	controls[side-1].c[selected-1]=nControl(input->lastAxis,*input->lastAxis);
			for(int i=0;i<2;i++)
			{
				for(int l=0;l<8;l++)
				{
					if(controls[i].c[l]==controls[side-1].c[selected-1] && (i!=size-1 && l!=selected-1))
						controls[i].c[l].invalidate();
				}
			}
			choosing=false;
			Redisplay=true;
			input->up(input->lastKey);
		}
	}
	else
	{
		updateMove();
		if(input->getKey(SPACE) || input->getKey(ENTER))
		{
			input->up(SPACE);
			input->up(ENTER);
			if(selected==9)
			{
				Redisplay=true;
				//newMenu=2;
				newMode=BLANK_MODE;
				Cmenu=new m_chooseMode(*((m_chooseMode*)this));
			}
			else
			{
				choosing=true;
				pressCount=input->tPresses;
				Redisplay=true;
			}
		}
		if(input->getKey(VK_BACK))
		{
			//newMenu=2;
			Redisplay=true;
			newMode=BLANK_MODE;
			Cmenu=new m_chooseMode(*((m_chooseMode*)this));
		}
	}
	return 30;
}

setControls::setControls()
{
	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void inGameMenu::init()
{
	selected=1;
	TotalRows=3;
	TotalColumns=1;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	type=4;
	options.clear();
	options.push_back(string("return to game"));
	options.push_back(string("options"));
	options.push_back(string("quit"));
}
inGameMenu::inGameMenu()
{
	init();
}
int inGameMenu::update(int value)
{
	if(type!=4)
		init();
	updateMove();
	if(input->getKey(SPACE) || input->getKey(ENTER))
	{
		switch(selected)
		{
		case 1:
			gameTime.unpause();
			paused=false;
			Cmenu=new closedMenu(*((closedMenu*)this));
			break;
		case 2:
			break;
		case 3:
			newMode=BLANK_MODE;
			Cmenu=new m_chooseMode(*((m_chooseMode*)this));
			break;
		}
		input->up(SPACE);
		input->up(ENTER);
	}
	else if(input->getKey(0x31) && inGame)
	{
		input->up(0x31);
		paused=false;
		gameTime.unpause();
		//Cmenu=new closedMenu(*((closedMenu*)this));
		//newMode=BLANK_MODE;
		Cmenu=new closedMenu(*((closedMenu*)this));
	}
	return 30;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void m_start::init()
{
	selected=1;
	TotalRows=1;
	TotalColumns=1;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	paused=false;
	gameTime.unpause();
	type=2;
}
m_start::m_start()
{
	init();
}
void m_start::draw()
{
	//float s=(float)sw/800;
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0 , sw, sh);
	glLoadIdentity();
	viewOrtho(sw,sh);
	glScalef((float)sw/800,(float)sh/600,1);

	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(0,0);
		glTexCoord2f(0,0);	glVertex2f(0,600);
		glTexCoord2f(1,0);	glVertex2f(800,600);		
		glTexCoord2f(1,1);	glVertex2f(800,0);
	glEnd();

	glEnable(GL_BLEND);
	dataManager.bind("menu start");
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);		glVertex2f(0	,300);
		glTexCoord2f(1  ,1);	glVertex2f(800,300);
		glTexCoord2f(1  ,0);	glVertex2f(800,300+150);//150 was 190
		glTexCoord2f(0,0);		glVertex2f(0	,300+150);
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	viewPerspective();
}
int m_start::update(int value)
{
	if(type!=2)
		init();
	updateMove();
	if(input->getKey(SPACE) || input->getKey(ENTER))
	{
		input->up(SPACE);
		input->up(ENTER);
		newMenu=CHOOSE_MODE;// assign numbers...
		Redisplay=true;
	}
	return 30;
}
///////////////////////////////////////////////////////////////////////////////////////////////
m_chooseMode::m_chooseMode(){init();}
void m_chooseMode::init()
{
	selected=1;
	side=1;
	TotalRows=1;
	TotalColumns=3;
	Redisplay=true;
	newMode=(modeType)0;
	newMenu=(menuType)0;
	paused=false;
	gameTime.unpause();
	inGame=false;
	type=2;
}
void m_chooseMode::draw()
{
	//float s=(float)sw/800;
	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0 , sw, sh);
	glLoadIdentity();
	viewOrtho(sw,sh);
	glScalef((float)sw/800,(float)sh/600,1);

	glColor3f(1,1,1);
	glBindTexture(GL_TEXTURE_2D, menuBack); // We set the active texture
	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2f(0,0);
		glTexCoord2f(0,0);	glVertex2f(0,600);
		glTexCoord2f(1,0);	glVertex2f(800,600);		
		glTexCoord2f(1,1);	glVertex2f(800,0);
	glEnd();

	glEnable(GL_BLEND);
	dataManager.bind("menu slot");
	glBegin(GL_QUADS);
	for(int i=1;i<=5;i++)
	{
		if(i!=side+1)
		{		
			glTexCoord2f(0,1);	glVertex2f(-325+i*210		,300);
			glTexCoord2f(1,1);	glVertex2f(-325+i*210+205	,300);
			glTexCoord2f(1,0);	glVertex2f(-325+i*210+205	,300+150);
			glTexCoord2f(0,0);	glVertex2f(-325+i*210		,300+150);
		}
	}
	glEnd();
	dataManager.bind("menu mode choices");
	glBegin(GL_QUADS);
	for(int i=1;i<=3;i++)
	{
		glTexCoord2f(0,1-0.33*(i-1));	glVertex2f(-325+(i+1)*210		,300);
		glTexCoord2f(1,1-0.33*(i-1));	glVertex2f(-325+(i+1)*210+205	,300);
		glTexCoord2f(1,1-0.33*(i));	glVertex2f(-325+(i+1)*210+205	,300+25);
		glTexCoord2f(0,1-0.33*(i));	glVertex2f(-325+(i+1)*210		,300+25);
	}
	glEnd();
	glDisable(GL_BLEND);
	glBindTexture(GL_TEXTURE_2D, 0);
	viewPerspective();
}
int m_chooseMode::update(int value)
{
	if(type!=2)
		init();
	updateMove();
	if(input->getKey(SPACE) || input->getKey(ENTER))
	{
		input->up(SPACE);
		input->up(ENTER);
		switch(side)
		{
		case 1://two player
			inGame=true;
			newMode=TWO_PLAYER_VS;
			Redisplay=true;
			break;
		case 2://one player
			inGame=true;
			newMode=ONE_PLAYER;
			Redisplay=true;
			break;
		case 3://map builder
			inGame=false;
			newMode=MAP_BUILDER;
			Redisplay=true;	
			Cmenu=new closedMenu(*((closedMenu*)this));
			break;
		}
		input->up(SPACE);
		input->up(ENTER);
		Redisplay=true;
	}
	return 30;
}














//
//void inGameOptions::init()
//{
//	selected=1;
//	TotalRows=3;
//	TotalColumns=1;
//	Redisplay=true;
//	newMode=0;
//	newMenu=0;
//	type=5;
//	options.clear();
//	options.push_back(string("screen resolution"));
//	options.push_back(string("controls"));
//	options.push_back(string("sound"));
//}
//inGameOptions::inGameOptions()
//{
//	init();
//}
//int inGameOptions::update(int value)
//{
//	if(type!=5)
//		init();
//	updateMove();
//	if(input->keys[SPACE] || input->keys[ENTER])
//	{
//		switch(selected)
//		{
//		case 1:
//			paused=false;
//			Cmenu=new closedMenu(*((closedMenu*)this));
//			break;
//		case 2:
//			break;
//		case 3:
//			newMode=2;
//			newMenu=2;
//			break;
//		}
//		input->keys[SPACE]=false;
//		input->keys[ENTER]=false;
//	}
//	else if(input->keys[0x31] && inGame)
//	{
//		input->keys[0x31]=false;
//		paused=false;
//		Cmenu=new closedMenu(*((closedMenu*)this));
//	}
//	return 30;
//}