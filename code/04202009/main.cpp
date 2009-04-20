

#include <cstdio>
#include <iostream>
//#include <stdlib.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include "GL/glut.h"
#endif



const unsigned long double PI = 3.141592653589793238462643383279502884197169399375105820974944592307816406286208;

const float iheight=1;
const float size=128;
using namespace std;

void cleanup();

#include "input.h"
#include "controls.h"
#include "drawing.h"
#include "vec3f.h"
#include "Terain.h"
#include "imageloader.h"
#include "partical.h"
vector<spark> particles;
#include "entity.h"
#include "bullet.h" 
#include "missile.h"
vector<bullet> bullets;
vector<missile> missiles;
#include "plane.h"

#include "texture2.h"
#include "model.h"

//#define DEBUG

#ifndef DEBUG 
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  
#endif

const int NumPlayers = 2;
const double DegToRad = 3.14159265358979323846264/180;
const double RadToDeg = 180/.314159265358979323846264;

//bool KeyPressed[256]= {false};//double due to special
//int j[3];
//bool buttons[10];

int acplayer;
int sh;
int sw;
vector<planeBase*> planes;
bool firstP[NumPlayers] = {true,true};
vector<int> model;
vector<int> tex;
vector<int> menus;
//int particleTex;
GLuint menuBack;
int disp[1];
float radarAng=0;

Terrain* _terrain;

int frame=0,Time,timebase=0;
float fps;
#include "modes.h"
#include "twoPlayerVs.h"
#include "loading.h"
#include "menu.h"
#include "onePlayer.h"
#include "setControls.h"
modes *mode;

void cleanup() {
	delete _terrain;
}
void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_POINT_SMOOTH);
	glShadeModel(GL_SMOOTH);
//	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}
int oPlayer(int player) {
	if(player==0){return 1;}
	else if(player==1){return 0;}
	else{return -1;}
}
void handleResize(int w, int h) {
	sh = h;
	sw = w;
	//glViewport(0, h/2, w, h);
	//glViewport(0, 0, w, h/2);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}
void draw()
{
	mode->draw();
}
void update(int value)
{
	glutTimerFunc(mode->update(value), update, 0);
	if(mode->newMode)
	{
		switch(mode->newMode)
		{
		case 1:
			mode=new loading;
			break;
		case 2:
			mode=new menu;
			break;
		case 3:
			mode=new twoPlayerVs;
			break;
		case 4:
			mode=new onePlayer;
			break;
		case 7:
			mode=new setControls;
			break;
		}
	}
}
int main(int argc, char** argv) {
	srand ((unsigned int)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);//Glut was the name of the Brother Bob. Bob had eaten chocolate Crab (in british accent).

	//glutCreateWindow("dog fights!" );

	glutGameModeString("800x600:32");
	// enter full screen
	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) 
		glutEnterGameMode();
	else {
		cout << "error";
		glutInitWindowSize(600, 800);
		glutCreateWindow("dog fights!" );
	}


	initRendering();

	glutKeyboardFunc(handleKeypress);
	glutKeyboardUpFunc(KeyUpFunc);
	glutSpecialFunc(SpecialKeyDownFunc);
	glutSpecialUpFunc(SpecialKeyUpFunc);
	glutJoystickFunc(Joystick,25);

	glutDisplayFunc(draw);
	glutTimerFunc(25, update, 0);
	glutReshapeFunc(handleResize);

	glutSetCursor(GLUT_CURSOR_NONE);
	mode=new loading;


	glutMainLoop();
	return 0;
}