#ifndef __WIN_DUMP_HPP__
#define __WIN_DUMP_HPP__

#include <windows.h>
#include <Dbghelp.h>
#include <tchar.h>


LONG WINAPI HandleException(struct _EXCEPTION_POINTERS* apExceptionInfo);


#endif // WIN32