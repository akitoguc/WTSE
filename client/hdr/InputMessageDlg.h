/// @file:  InputMessageDlg.h

#pragma once

namespace WTSEClient {

    /// @brief  InputMessageDlg dialog
    class CInputMessageDlg : public CDialog
    {
        // typedef/enum
    public:
        /// @brief  dialog data
        enum { IDD = IDD_INPUT_MESSAGE };

    public:
        // ctor/dtor
        CInputMessageDlg(CWnd* pParent = NULL);

    protected:
        // DDX/DDV support
        virtual void DoDataExchange(CDataExchange* pDX);

    public:
        CString m_csMessage;

        // message map
        DECLARE_MESSAGE_MAP()
    };
}
