
#include "debugBreak.h"

using namespace std;

#ifdef _ITERATOR_DEBUG_LEVEL
#if _ITERATOR_DEBUG_LEVEL == 2
_CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL std::_Debug_message(const wchar_t *,
	const wchar_t *, unsigned int);

void errorMsg(const wchar_t *a, const wchar_t *b, unsigned int c)
{
	minimizeActiveWindow();
	std::_Debug_message(a, b, c);
}
#endif
#endif
