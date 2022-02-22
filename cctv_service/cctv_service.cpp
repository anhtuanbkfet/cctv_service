#include "Utils.hpp"
#include "WinDump.hpp"


#define SERVICE_NAME  _T("CCTV_Service")

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);


int _tmain(int argc, TCHAR* argv[])
{
    CCTV_LOG_DEBUG(_T("[CCTV_Service] Main: Entry"));


    // Create a dump file whenever the gateway crashes only on windows
    SetUnhandledExceptionFilter(HandleException);
    //AddVectoredExceptionHandler(0, HandleException);

    SERVICE_TABLE_ENTRY ServiceTable[] =
    {
        {(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
        {NULL, NULL}
    };

    if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] Main: StartServiceCtrlDispatcher returned error"));
        return GetLastError();
    }

    CCTV_LOG_DEBUG(_T("[CCTV_Service] Main: Exit"));

    return 0;
}


VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
    DWORD Status = E_FAIL;

    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Entry"));

    g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

    if (g_StatusHandle == NULL)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: RegisterServiceCtrlHandler returned error"));
        goto EXIT;
    }

    // Tell the service controller we are starting
    ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
    g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwServiceSpecificExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: SetServiceStatus returned error"));
    }

    /*
     * Perform tasks neccesary to start the service here
     */
    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Performing Service Start Operations"));

    // Create stop event to wait on later.
    g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (g_ServiceStopEvent == NULL)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: CreateEvent(g_ServiceStopEvent) returned error"));

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        g_ServiceStatus.dwWin32ExitCode = GetLastError();
        g_ServiceStatus.dwCheckPoint = 1;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
        {
            CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: SetServiceStatus returned error"));
        }
        goto EXIT;
    }

    // Tell the service controller we are started
    g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 0;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: SetServiceStatus returned error"));
    }

    //DebugBreak();

    // Start the thread that will perform the main task of the service
    HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Waiting for Worker Thread to complete"));

    // Wait until our worker thread exits effectively signaling that the service needs to stop
    WaitForSingleObject(hThread, INFINITE);

    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Worker Thread Stop Event signaled"));


    /*
     * Perform any cleanup tasks
     */
    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Performing Cleanup Operations"));

    CloseHandle(g_ServiceStopEvent);

    g_ServiceStatus.dwControlsAccepted = 0;
    g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
    g_ServiceStatus.dwWin32ExitCode = 0;
    g_ServiceStatus.dwCheckPoint = 3;

    if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
    {
        CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceMain: SetServiceStatus returned error"));
    }

EXIT:
    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceMain: Exit"));

    return;
}


VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceCtrlHandler: Entry"));

    switch (CtrlCode)
    {
    case SERVICE_CONTROL_STOP:

        CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceCtrlHandler: SERVICE_CONTROL_STOP Request"));

        if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
            break;

        /*
         * Perform tasks neccesary to stop the service here
         */

        g_ServiceStatus.dwControlsAccepted = 0;
        g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        g_ServiceStatus.dwWin32ExitCode = 0;
        g_ServiceStatus.dwCheckPoint = 4;

        if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
        {
            CCTV_LOG_ERROR(_T("[CCTV_Service] ServiceCtrlHandler: SetServiceStatus returned error"));
        }

        // This will signal the worker thread to start shutting down
        SetEvent(g_ServiceStopEvent);

        break;

    default:
        break;
    }

    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceCtrlHandler: Exit"));
}


int i = 0;
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceWorkerThread: Entry"));

    //  Periodically check if the service has been requested to stop
    while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
    {
        /*
         * Perform main service function here
         */

         //  Simulate some work by sleeping
        CCTV_LOG_DEBUG(_T("[CCTV_Service] Service sleep for 1 sec...."));
        Sleep(1000);

        i++;
        if (i > 5) {
            CCTV_LOG_DEBUG(_T("[CCTV_Service] Service crash here!"));
            RaiseException(0xc0000374, 0, 0, NULL);
        }
    }

    CCTV_LOG_DEBUG(_T("[CCTV_Service] ServiceWorkerThread: Exit"));

    return ERROR_SUCCESS;
}
