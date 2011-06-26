/// @file:  WTSEServerSvc.cpp

#include "stdafx.h"
#include "resource.h"
#include "WTSEServerSvc.h"
#include "WorkerThreadService.h"

#include <stdio.h>

class CWTSEServerSvcModule : public CAtlServiceModuleT< CWTSEServerSvcModule, IDS_SERVICENAME >
{
public :
    DECLARE_LIBID(LIBID_WTSEServerSvcLib)
    DECLARE_REGISTRY_APPID_RESOURCEID(IDR_WTSESERVERSVC, "{07AE92EB-840F-4453-A9F9-081EADF1FAFD}")
    HRESULT InitializeSecurity() throw()
    {
        // TODO : CoInitializeSecurity を呼び出し、サービスに適切なセキュリティ設定を 
        // 指定します。
        // 推奨 - PKT レベルの認証、 
        // RPC_C_IMP_LEVEL_IDENTIFY の偽装レベル、 
        // および適切な NULL 以外のセキュリティ記述子。

        return S_OK;
    }

    HRESULT PreMessageLoop(int nShowCmd) throw()
    {
        HRESULT hr = __super::PreMessageLoop(nShowCmd);
#if _ATL_VER >= 0x0700
        if (SUCCEEDED(hr) && !m_bDelayShutdown) {
          hr = CoResumeClassObjects();
        }
#endif
        if (SUCCEEDED(hr)) {
          // 起動時の処理を書く
            m_workerThreadService = new WTSEServer::CWorkerThreadService;
            m_workerThreadService->OnStart(3333);
        }
        return hr;
    }

    HRESULT PostMessageLoop() throw()
    {
        return __super::PostMessageLoop();
    }

    // ctor
    // @TODO: write ctor and initialize members
    CWTSEServerSvcModule() throw()
        : m_workerThreadService(NULL)
        , m_portNumber(-1)
    {
        // update the initial service status 
//        m_hServiceStatus = NULL;
//        m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
//        m_status.dwCurrentState = SERVICE_STOPPED;
//        m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
//        m_status.dwWin32ExitCode = 0;
//        m_status.dwServiceSpecificExitCode = 0;
//        m_status.dwCheckPoint = 0;
//        m_status.dwWaitHint = 0;
    }

    // overrides
    // Parses the command line and obtain the port number on which service starts
    bool ParseCommandLine(LPCTSTR lpCmdLine, HRESULT* pnRetCode) throw()
    {
        // get port number
        TCHAR szTokens[] = _T("-/");
        *pnRetCode = S_OK;

        LPCTSTR lpszToken = FindOneOf(lpCmdLine, szTokens);
        while (lpszToken != NULL)
        {
            if (WordCmpI(lpszToken, _T("Port"))==0)
            {
                TCHAR szSpaces[] = _T(" \t");
                lpszToken = FindOneOf(lpszToken, szSpaces);
                m_portNumber = _tstoi(lpszToken);
                if (m_portNumber <= 0 || errno == ERANGE) {
                    *pnRetCode = E_INVALIDARG;
                    return false;
                }
            }
            lpszToken = FindOneOf(lpszToken, szTokens);
        }

        return __super::ParseCommandLine(lpCmdLine, pnRetCode);
    }

    void OnStop() throw()
    {
        if (m_workerThreadService) {
            m_workerThreadService->OnEnd();
            delete m_workerThreadService;
        }

        __super::OnStop();
    }

    void OnPause() throw()
    {
        __super::OnPause();
    }

    void OnContinue() throw()
    {
        __super::OnContinue();
    }

    void OnInterrogate() throw()
    {
        __super::OnInterrogate();
    }

    void OnShutdown() throw()
    {
        __super::OnShutdown();
    }

private:
    /// @brief  thread for server service
    WTSEServer::CWorkerThreadService* m_workerThreadService;
    /// @brief  port number of the service
    int m_portNumber;
};

CWTSEServerSvcModule _AtlModule;

extern "C" int WINAPI _tWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, 
                                LPTSTR /*lpCmdLine*/, int nShowCmd)
{
    return _AtlModule.WinMain(nShowCmd);
}

