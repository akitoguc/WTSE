/// @file:  ServerInfoDlg.h

#pragma once

namespace WTSEClient {
    /// @brief  CServerInfoDlg dialog
    class CServerInfoDlg : public CDialog
    {
        // typedef/enum
    public:
        /// @brief  dialog data
        enum { IDD = IDD_SVR_INFO };

        // ctor/dtor
    public:
        CServerInfoDlg(CWnd* pParent = NULL);
        virtual ~CServerInfoDlg();

        // DDX/DDV support
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);

        // attributes
    public:
        CString m_csServer;
        CString m_csPort;

        DECLARE_MESSAGE_MAP()
    };
}
