#include "Log.hpp"
#include <atlstr.h>

int		gl_logLevel = LOG_DEBUG;

void SetLogLevel(int logLevel)
{
	gl_logLevel = logLevel;
}

bool CheckLogEnable(int logLevel)
{
	return logLevel & gl_logLevel;
}


void cctv::OutputLog(int logLevel, const wchar_t* function, DWORD linenumber, const wchar_t* fmt, ...)
{
	if (!CheckLogEnable(logLevel))
		return;

	CString strLog;
	wchar_t buf[2048];
	int len;

	try
	{
		va_list args;
		va_start(args, fmt);
		len = wsprintf(buf, fmt, args);
		va_end(args);

		if (len > 0)
		{
			strLog.Format(L"[cctv] [%s-(%d)] %s", function, linenumber, buf);
			if (strLog.Right(1) != '\n')
				strLog.Append(L"\r\n");
			OutputDebugStringW(strLog);
		}
	}
	catch (...)
	{
		strLog.Format(L"[cctv] %s %s %s Error\r\n", _T(__FILE__), _T(__FUNCTION__), fmt);
		OutputDebugStringW(strLog);
	}
}