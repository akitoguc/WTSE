/// @file   ServerInfoDlg.cpp

#include "stdafx.h"

#include "WTSEClient.h"
#include "ServerInfoDlg.h"

using namespace WTSEClient;

/// @brief  CServerInfoDlg dialog

CServerInfoDlg::CServerInfoDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CServerInfoDlg::IDD, pParent)
    , m_csServer(_T(""))
    , m_csPort(_T(""))
{
}

CServerInfoDlg::~CServerInfoDlg()
{
}

void CServerInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_SVR, m_csServer);
    DDX_Text(pDX, IDC_EDIT_PORT, m_csPort);
}

BEGIN_MESSAGE_MAP(CServerInfoDlg, CDialog)
END_MESSAGE_MAP()
