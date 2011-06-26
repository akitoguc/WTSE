/// @file:  WTSEClient.h

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h" // main symbols

/// @brief  user window msssages
#define UWM_USER_POPUP (WM_APP + 1)

/// @brief  CWTSEClientApp:
/// @note   See WTSEClient.cpp for the implementation of this class
class CWTSEClientApp : public CWinApp
{
    // ctor/dtor
public:
    CWTSEClientApp();

    // overrides
public:
    virtual BOOL InitInstance();
    virtual int ExitInstance(); // return app exit code

    // implementation
protected:
    virtual BOOL InitAppProfile();

    DECLARE_MESSAGE_MAP()
};

extern CWTSEClientApp theApp;
