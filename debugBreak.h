
#pragma once

#if defined _MSC_VER
extern void minimizeActiveWindow();

#if defined _WIN32 && defined _DEBUG
#include <xutility>
#endif

#if defined(_ITERATOR_DEBUG_LEVEL) && _ITERATOR_DEBUG_LEVEL == 2	//_ITERATOR_DEBUG_LEVEL == 2 when compiling in DEBUG mode

	extern void errorMsg(const wchar_t *a, const wchar_t *b, unsigned int c);

	#undef _DEBUG_ERROR2				//we are redefining _DEBUG_ERROR2 so that the window is minimized and then the error message is displayed

	#define _DEBUG_ERROR2(mesg, file, line)	\
		_Debug_message(L ## mesg, file, line)

#endif

#ifdef _DEBUG
	#define debugBreak(){				\
		minimizeActiveWindow();			\
		__debugbreak();					\
	}
	#define debugAssert(a){				\
		if(!(a))						\
		{								\
			minimizeActiveWindow();		\
			__debugbreak();				\
		}								\
	}
#else
	#define debugBreak() {}
	#define debugAssert(a) {}
#endif

#else /* _MSC_VER */

	#define debugBreak(){				\
		__asm__("int3");				\
	}
	#define debugAssert(a){				\
		if(!(a))						\
		{								\
			__asm__("int3");			\
		}								\
	}

#endif /* _MSC_VER */
