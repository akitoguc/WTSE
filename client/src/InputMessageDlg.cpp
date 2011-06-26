/// @file:  InputMessageDlg.cpp

#include "stdafx.h"

#include "WTSEClient.h"
#include "InputMessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace WTSEClient;

CInputMessageDlg::CInputMessageDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CInputMessageDlg::IDD, pParent)
    , m_csMessage(_T(""))
{

}

void CInputMessageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_MESSAGE, m_csMessage);
}

BEGIN_MESSAGE_MAP(CInputMessageDlg, CDialog)
END_MESSAGE_MAP()

