/// @file   UserInfoDlg.h

#pragma once

#include "afxext.h"
#include "afxcmn.h"
#include "LogonSession.h"
#include "TransparentBitmapButton.h"

namespace WTSEClient {
    /// @brief  UserInfoDlg dialog
    class CUserInfoDlg : public CDialog
    {
        // typedef/enum
    public:
        /// @brief  dialog data
        enum { IDD = IDD_USER_INFO };

        // ctor/dtor
    public:
        CUserInfoDlg(CWnd* pParent = NULL);
        virtual ~CUserInfoDlg();

    protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);
//        // for processing Windows messages
//        virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

        // operations
    public:
        void SetLogonSessionInfo(const LogonSession::InfoList& x) { m_logonSessionInfoList = x; }
        void SetServerName(const CString& cs) { m_csServer = cs; }

        // message handlers
    protected:
        virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
        virtual BOOL OnInitDialog();
        void OnPaint();
        void OnBnClickedUserinfoClose();
        void OnBnClickedUserinfoPin();
        void OnRClickList(NMHDR* pNMHDR, LRESULT* pResult);
        void OnTimer(UINT_PTR nIDEvent);
        HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

        void OnLButtonDown(UINT nFlags, CPoint point);
        void OnLButtonUp(UINT nFlags, CPoint point);
        void OnMouseMove(UINT nFlags, CPoint point);

        // implementation
    protected:
        void InitStaticServerName();
        void InitListCtrl();
        void InitListItems();
        void InitWindow();
        void UpdatePinButton(bool bFixed);

        void OnStartTimer();
        void OnStopTimer();

        void SendMessage();

        void MinimizeLogonSessionList();
        CString FormatLogonTime(const LARGE_INTEGER& time) const;

    protected:
        /// @brief  close button with transparent background
        CTransparentBitmapButton m_btnClose;
        /// @brief  pinning button with transparent background
        CTransparentBitmapButton m_btnPin;
        /// @brief  main list
        CListCtrl m_userinfoList;
        /// @brief  image list of status
        CImageList m_statusImageList;
        /// @brief  window size
        CRect m_rectWindow;
        /// @brief  region of handle to move window
        CRect m_rectHandle;
        /// @brief  server name as dialog headline
        CString m_csServer;
        /// @brief  headline font
        CFont m_fontServerName;
        /// @brief  back-ground image
        CBitmap m_bitmapBg;
        /// @brief  timer identifier
        UINT_PTR m_nTimer;

        CPoint m_pointOld;
        bool m_bMoving;

        /// @brief  logon session data to show
        LogonSession::InfoList m_logonSessionInfoList;

        DECLARE_MESSAGE_MAP()
    };
}
