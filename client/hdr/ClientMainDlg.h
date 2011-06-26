/// @file:  ClientMainDlg.h

#pragma once

#include <list>

namespace WTSEClient {
    /// @brief  ClientMainDlg dialog
    class ClientMainDlg : public CDialog
    {
        // typedef/enum
    public:
        /// @brief  dialog data
        enum { IDD = IDD_WTSECLIENT_DIALOG };

        /// @brief  server data
        struct ServerInfoEntry
        {
            CString m_csServer;
            CString m_csPort;

            bool operator == (const ServerInfoEntry& x) {
                return m_csServer == x.m_csServer && m_csPort == x.m_csPort;
            }
        };

        typedef std::list<ServerInfoEntry> ServerInfo;

        // ctor/dtor
    public:
        /// @brief  standard constructor
        ClientMainDlg(CWnd* pParent = NULL);

    protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);

        // message map
        DECLARE_MESSAGE_MAP()

    protected:
        // for processing Windows messages
        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        // implementation
    protected:
        /// @brief  application icon
        HICON m_hIcon;
        /// @brief  task-tray icon data
        NOTIFYICONDATA m_stNotifyIcon;
        /// @brief  flag to show/hide the preference window
        bool m_bVisible;
        /// @brief  ListCtrl for servers
        CListCtrl m_serverList;

    public:
        /// @brief  server information
        ServerInfo m_serverInfo;

    protected:
        virtual void UpdateItems();
        virtual void UpdateControlStatus();

        void PopupMenu(const POINT& point);

    public:
        virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

    protected:
        // Generated message map functions
        virtual BOOL OnInitDialog();
        void OnPaint();
        HCURSOR OnQueryDragIcon();

        void OnDestroy();
        void OnWindowPosChanging(WINDOWPOS* lpwndpos);

    public:
        void OnPopupPreference();
        void OnPopupAboutbox();
        void OnPopupClose();
        void OnPopupCheckServer(UINT nID);
        void OnBnClickedBtnAddSvr();
        void OnBnClickedBtnEdtSvr();
        void OnBnClickedBtnDelSvr();
        void OnBnClickedBtnHideDlg();
        void OnLvnItemchangedListServer(NMHDR *pNMHDR, LRESULT *pResult);

    };
}
