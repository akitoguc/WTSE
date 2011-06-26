/// @file:  WTSEClient.cpp
/// @brief  Defines the class behaviors for the application.

#include "stdafx.h"

#include <shlwapi.h>

#include "WTSEClient.h"
#include "ClientMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace WTSEClient;

// CWTSEClientApp

BEGIN_MESSAGE_MAP(CWTSEClientApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CWTSEClientApp construction

CWTSEClientApp::CWTSEClientApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}


// The one and only CWTSEClientApp object

CWTSEClientApp theApp;

namespace {
    /// @brief  path to application profile
    wchar_t s_profilePath[MAX_PATH];
    /// @brief  company name (used to specify profile dir)
    wchar_t* s_companyName = L"ELYSIUM";

    /// @brief  key and section names for ini file
    const wchar_t* SECTION = L"ServerInfo";
    const wchar_t* KEY_SERVER = L"Server_%d";
    const wchar_t* KEY_PORT = L"Port_%d";

    void RetrieveServerInfo(ClientMainDlg::ServerInfo& serverInfo)
    {
        int nEntry = theApp.GetProfileInt(L"ServerInfo", L"EntryCount", 0);

        for (int i = 0; i < nEntry; ++i) {
            CString csKeyServer, csKeyPort;
            csKeyServer.Format(KEY_SERVER, i);
            csKeyPort.Format(KEY_PORT, i);

            CString csServer = theApp.GetProfileString(SECTION, csKeyServer, L"").Trim();
            CString csPort = theApp.GetProfileString(SECTION, csKeyPort, L"").Trim();
            if (csServer.IsEmpty() && csPort.IsEmpty()) {
                continue;
            }

            ClientMainDlg::ServerInfoEntry entry = { csServer, csPort };
            serverInfo.push_back(entry);
        }
    }

    void StoreServerInfo(const ClientMainDlg::ServerInfo& serverInfo)
    {
        int nEntry = static_cast<int>(serverInfo.size());
        theApp.WriteProfileInt(SECTION, L"EntryCount", nEntry);

        int i = 0;
        ClientMainDlg::ServerInfo::const_iterator cit = serverInfo.begin();
        for (; cit != serverInfo.end(); ++cit, ++i) {
            CString csKeyServer, csKeyPort;
            csKeyServer.Format(KEY_SERVER, i);
            csKeyPort.Format(KEY_PORT, i);

            theApp.WriteProfileString(SECTION, csKeyServer, (*cit).m_csServer);
            theApp.WriteProfileString(SECTION, csKeyPort, (*cit).m_csPort);
        }
    }
}

// CWTSEClientApp initialization

BOOL CWTSEClientApp::InitInstance()
{
    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    if (!AfxSocketInit())
    {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    if (!InitAppProfile()) {
        AfxMessageBox(L"Failed in initializing application profile.", MB_ICONHAND);
        return FALSE;
    }

    ClientMainDlg dlg;
    m_pMainWnd = &dlg;

    RetrieveServerInfo(dlg.m_serverInfo);

    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
    {
        StoreServerInfo(dlg.m_serverInfo);
    }
    else if (nResponse == IDCANCEL)
    {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

int CWTSEClientApp::ExitInstance()
{
    m_pszProfileName = NULL;

	return CWinApp::ExitInstance();
}

BOOL CWTSEClientApp::InitAppProfile()
{
    // use ini file to store application data instead of registry
    m_pszRegistryKey = NULL;

    wchar_t profileDir[MAX_PATH];

    HRESULT hResult = SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, profileDir);
    if (!SUCCEEDED(hResult)) {
        return FALSE;
    }

    wcscat_s(profileDir, MAX_PATH, L"\\");
    wcscat_s(profileDir, MAX_PATH, s_companyName);
    wcscat_s(profileDir, MAX_PATH, L"\\WTSE");

    if (!PathIsDirectory(profileDir)) {
        if (!CreateDirectory(profileDir, NULL)) {
            return FALSE;
        }
    }

    wcscpy_s(s_profilePath, MAX_PATH, profileDir);
    wcscat_s(s_profilePath, MAX_PATH, L"\\");
    wcscat_s(s_profilePath, MAX_PATH, m_pszAppName);
    wcscat_s(s_profilePath, MAX_PATH, L".ini");

    m_pszProfileName = s_profilePath;

    return TRUE;
}
