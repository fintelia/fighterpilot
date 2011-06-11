
#pragma once

extern void minimizeWindow();

#ifdef _DEBUG
#include <xutility>
#endif

#if _ITERATOR_DEBUG_LEVEL == 2			//_ITERATOR_DEBUG_LEVEL == 2 when compiling in DEBUG mode

	extern void errorMsg(const wchar_t *a, const wchar_t *b, unsigned int c);

	#undef _DEBUG_ERROR2				//we are redefining _DEBUG_ERROR2 so that the window is minimized and then the error message is displayed 

	#define _DEBUG_ERROR2(mesg, file, line)	\
		errorMsg(L ## mesg, file, line)
	

 #endif /* _ITERATOR_DEBUG_LEVEL == 2 */

#ifdef _DEBUG
	#define debugBreak(){			\
		minimizeWindow();			\
		__debugbreak();				\
	}
#else if
	#define debugBreak() {}
#endif