//------------------------------------------------------------------------------
/// \file	keys.h
/// \author	Rob Bateman
/// \date	8-3-2005
/// \brief	Right then, glut splits keyboard input into normal keys and special
///			keys. To make life easier, i'm simply going to add 256 to the value of
///			all special keys so that i can treat all keys the same. 
///			
///			Generally speaking, within a game engine we simply ask the question,
///			is this key pressed? ie, we want to poll the state of a given key
///			at a specific time. There is no way to do this directly within glut,
///			so instead you need a little hack. All of the press and release callbacks
///			actually update an array of key states internally. We can then simply 
///			check the keys state at any time by querying the value in the array.
///			
/// 
//------------------------------------------------------------------------------

#ifndef __KEYS__H__
#define __KEYS__H__

	//--------------------------------------------------------------------------
	// glut is not very good for keyboard input, to help,
	// i've added these key defines to treat the special
	// keys as normal keys.
	// 
	#define KEY_F1			(GLUT_KEY_F1+256)
	#define KEY_F2			(GLUT_KEY_F2+256)
	#define KEY_F3			(GLUT_KEY_F3+256)
	#define KEY_F4			(GLUT_KEY_F4+256)
	#define KEY_F5			(GLUT_KEY_F5+256)
	#define KEY_F6			(GLUT_KEY_F6+256)
	#define KEY_F7			(GLUT_KEY_F7+256)
	#define KEY_F8			(GLUT_KEY_F8+256)
	#define KEY_F9			(GLUT_KEY_F9+256)
	#define KEY_F10			(GLUT_KEY_F10+256)
	#define KEY_F11			(GLUT_KEY_F11+256)
	#define KEY_F12			(GLUT_KEY_F12+256)

	#define KEY_LEFT		(GLUT_KEY_LEFT+256)
	#define KEY_UP			(GLUT_KEY_UP+256)
	#define KEY_RIGHT		(GLUT_KEY_RIGHT+256)
	#define KEY_DOWN		(GLUT_KEY_DOWN+256)
	#define KEY_PAGE_UP		(GLUT_KEY_PAGE_UP+256)
	#define KEY_PAGE_DOWN	(GLUT_KEY_PAGE_DOWN+256)
	#define KEY_HOME		(GLUT_KEY_HOME+256)
	#define KEY_END			(GLUT_KEY_END+256)
	#define KEY_INSERT		(GLUT_KEY_INSERT+256)

	#define KEY_CONSOLE		'`'
	#define KEY_TAB			9
	#define KEY_BACKSPACE	8
	#define KEY_DELETE		127
	#define KEY_SPACE		' '
	#define KEY_ENTER		13
	#define KEY_ESCAPE		27

	//--------------------------------------------------------------------------
	/// \brief	returns true if the specified key is pressed
	/// \param	key	-	the key to check
	///
	bool IsKeyPressed(int key);

	//--------------------------------------------------------------------------
	/// \brief	This function sets glut callback functions for normal and special keys.
	///
	void InitKeyboard();

	//--------------------------------------------------------------------------
	/// this typedef defines the type of function you need for 
	/// keyboard up and keyboard down functions. Rather than using 
	/// the glutKeyboardFunc etc, just provide a function that handles
	/// both special and normal keys.
	///
	typedef void (*key_func)(int,int,int);

	//--------------------------------------------------------------------------
	/// \brief	this sets the keyboard press callback
	/// \param	func	-	the function to handle key presses
	///
	void SetKeyboardFunc(key_func);

	//--------------------------------------------------------------------------
	/// \brief	this sets the keyboard release callback
	/// \param	func	-	the function to handle key releases
	///
	void SetKeyboardUpFunc(key_func);


#endif
