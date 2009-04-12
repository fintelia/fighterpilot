//------------------------------------------------------------------------------
/// \file	keys.cpp
/// \author	Rob Bateman
/// \date	8-3-2005
/// \brief	
/// 
//------------------------------------------------------------------------------

#include "keys.h"
#include <stdlib.h>
#include <GL/glut.h>

// callback functions
key_func g_upfunc=0;
key_func g_downfunc=0;

//--------------------------------------------------------------------------
/// \brief	this sets the keyboard press callback
/// \param	func	-	the function to handle key presses
///
void SetKeyboardFunc(key_func func) {
	g_downfunc = func;
}

//--------------------------------------------------------------------------
/// \brief	this sets the keyboard release callback
/// \param	func	-	the function to handle key releases
///
void SetKeyboardUpFunc(key_func func) {
	g_upfunc = func;
}


// the state of all keys. We need to store this so that we can find out if more
// than one key is pressed at any given time. 
//
bool g_keystates[512]={false};

//--------------------------------------------------------------------------	IsKeyPressed
//
bool IsKeyPressed(int key) {

	// make sure valid key requested
	if (key>=0&&key<512) {
		// return the state of the requested key
		return g_keystates[key];
	}
	return false;
}


//--------------------------------------------------------------------------	KeyUpFunc
//
void KeyUpFunc(unsigned char key,int x,int y) {

	// set state of key
	g_keystates[key]=false;

	// if callback function set, call it
	if (g_upfunc) {
		g_upfunc(key,x,y);
	}
}
//--------------------------------------------------------------------------	KeyDownFunc
//
void KeyDownFunc(unsigned char key,int x,int y) {

	// set state of key
	g_keystates[key]=true;

	// if callback function set, call it
	if (g_upfunc) {
		g_upfunc(key,x,y);
	}
}
//--------------------------------------------------------------------------	SpecialKeyUpFunc
//
void SpecialKeyUpFunc(int key,int x,int y) {

	// set state of key
	g_keystates[key+256]=false;

	// if callback function set, call it
	if (g_upfunc) {
		g_upfunc(key+256,x,y);
	}
}
//--------------------------------------------------------------------------	SpecialKeyDownFunc
//
void SpecialKeyDownFunc(int key,int x,int y) {

	// set state of key
	g_keystates[key+256]=true;

	// if callback function set, call it
	if (g_upfunc) {
		g_upfunc(key+256,x,y);
	}
}
//--------------------------------------------------------------------------	InitKeyboard
//
void InitKeyboard() {

	// set the glutcallback functions
	glutKeyboardFunc(KeyDownFunc);
	glutKeyboardUpFunc(KeyUpFunc);
	glutSpecialFunc(SpecialKeyDownFunc);
	glutSpecialUpFunc(SpecialKeyUpFunc);
}