#ifndef __CCTV_LOG_HPP__
#define __CCTV_LOG_HPP__
#include <windows.h>
#include <tchar.h>

#define LOG_NOLOG	0x00
#define LOG_ERROR	0x01
#define LOG_WARN	0x04
#define LOG_INFO	0x08

#define LOG_DEBUG	LOG_ERROR | LOG_WARN



namespace cctv
{
	/*
	* Print log to debug
	*/
	void OutputLog(int logLevel, const wchar_t* function, DWORD linenumber, const wchar_t* fmt, ...);

};

#endif