#include "WinDump.hpp"
#include "Utils.hpp"
#include <string>
#include <shlobj_core.h>

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);



void CreateMiniDumpFile(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
    CCTV_LOG_DEBUG(_T("[WinDump] CreateMiniDumpFile: Start ----->>>>"));
    HMODULE mhLib = ::LoadLibrary(_T("dbghelp.dll"));

    MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress(mhLib, "MiniDumpWriteDump");

    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);

    std::wstring folderPath = std::wstring(path) + std::wstring(L"\\cctv");
	if (CreateDirectory(folderPath.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
		CCTV_LOG_DEBUG(_T("[WinDump] CreateMiniDumpFile: create cctv_dump file..."));
		std::wstring filePath = folderPath + std::wstring(L"\\cctv_dump.dmp");
		HANDLE  hFile = ::CreateFile(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL, NULL);
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = apExceptionInfo;
		ExInfo.ClientPointers = FALSE;

		pDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL);
		::CloseHandle(hFile);
	}

    CCTV_LOG_DEBUG(_T("[WinDump] CreateMiniDumpFile: Completed!"));
}

LONG WINAPI HandleException(struct _EXCEPTION_POINTERS* apExceptionInfo)
{
    CCTV_LOG_DEBUG(_T("[WinDump] HandleException: U Go there! Uhh, the application may be crashed..."));
    CreateMiniDumpFile(apExceptionInfo);
    return EXCEPTION_CONTINUE_SEARCH;
}

