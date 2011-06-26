/// @file:  ClientMainDlg.cpp

#include "stdafx.h"

#include <algorithm>

#include "WTSEClient.h"
#include "ClientMainDlg.h"
#include "ServerInfoDlg.h"
#include "UserInfoDlg.h"
#include "PacketReceiver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace WTSEClient;

// anonymous

namespace {
    /// @brief  number of columns
    const int COL_NUM = 2;
    /// @brief  index for specific column
    const int COL_IDX_SERVER = 0;
    const int COL_IDX_PORT = 1;

    /// @brief  definition of columns
    struct ColumnInfo
    {
        wchar_t* m_title;
        int m_width;
    }
    s_columnInfo[COL_NUM] = {
        { L"PC", 90 },
        { L"Port", 50 }
    };
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    enum { IDD = IDD_ABOUTBOX };

protected:
    // DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);

    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// ClientMainDlg dialog

ClientMainDlg::ClientMainDlg(CWnd* pParent /*=NULL*/)
    : CDialog(ClientMainDlg::IDD, pParent)
    , m_bVisible(false) // hidden as default
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClientMainDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SERVER, m_serverList);
}

BEGIN_MESSAGE_MAP(ClientMainDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_WINDOWPOSCHANGING()
    ON_COMMAND(IDM_POPUP_PREFERENCE, &ClientMainDlg::OnPopupPreference)
    ON_COMMAND(IDM_ABOUTBOX, &ClientMainDlg::OnPopupAboutbox)
    ON_COMMAND(IDM_POPUP_CLOSE, &ClientMainDlg::OnPopupClose)
    ON_BN_CLICKED(IDC_BTN_ADD_SVR, &ClientMainDlg::OnBnClickedBtnAddSvr)
    ON_BN_CLICKED(IDC_BTN_EDT_SVR, &ClientMainDlg::OnBnClickedBtnEdtSvr)
    ON_BN_CLICKED(IDC_BTN_DEL_SVR, &ClientMainDlg::OnBnClickedBtnDelSvr)
    ON_BN_CLICKED(IDC_BTN_HIDE, &ClientMainDlg::OnBnClickedBtnHideDlg)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SERVER, &ClientMainDlg::OnLvnItemchangedListServer)
END_MESSAGE_MAP()

LRESULT ClientMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case UWM_USER_POPUP:
        // get cursor location
        POINT point;
        ::GetCursorPos(&point);

        switch(lParam) {
        case WM_RBUTTONDOWN:
            SetForegroundWindow();
            PopupMenu(point);
            PostMessage(WM_NULL);
            break;
        }
        break;
    }

    return CDialog::WindowProc(message, wParam, lParam);
}

void ClientMainDlg::UpdateItems()
{
    m_serverList.DeleteAllItems();
    ASSERT(0 == m_serverList.GetItemCount());

    LV_ITEM item;
    item.mask = LVIF_TEXT;
    item.state = item.stateMask = 0;

    ServerInfo::const_iterator cit = m_serverInfo.begin();
    int pos = 0;
    for (; cit != m_serverInfo.end(); ++cit, ++pos) {
        item.iItem = pos;
        item.iSubItem = 0;
        item.pszText = L"";
        m_serverList.InsertItem(&item);

        m_serverList.SetItemText(pos, COL_IDX_SERVER, (*cit).m_csServer);
        m_serverList.SetItemText(pos, COL_IDX_PORT, (*cit).m_csPort);
    }

    UpdateControlStatus();
}

void ClientMainDlg::UpdateControlStatus()
{
    int iIndex = ListView_GetNextItem(m_serverList.m_hWnd, -1, LVNI_ALL | LVNI_SELECTED);
    if (iIndex < 0) {
        GetDlgItem(IDC_BTN_EDT_SVR)->EnableWindow(FALSE);
        GetDlgItem(IDC_BTN_DEL_SVR)->EnableWindow(FALSE);
    }
    else {
        GetDlgItem(IDC_BTN_EDT_SVR)->EnableWindow(TRUE);
        GetDlgItem(IDC_BTN_DEL_SVR)->EnableWindow(TRUE);
    }
}

void ClientMainDlg::PopupMenu(const POINT& point)
{
    CMenu menu;
    VERIFY(menu.LoadMenu(IDR_MENU_POPUP));
    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup != NULL);

    int pos = 0;
    ServerInfo::const_iterator cit = m_serverInfo.begin();
    for (; cit != m_serverInfo.end(); ++cit, ++pos) {
        pPopup->InsertMenu(pos, MF_STRING | MF_BYPOSITION, IDM_POPUP_DYN_BEGIN + pos, (*cit).m_csServer);
    }
    if (pos > 0) {
        pPopup->InsertMenu(pos, MF_SEPARATOR | MF_BYPOSITION);
    }

    pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
}

// ClientMainDlg message handlers

BOOL ClientMainDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    if (pHandlerInfo == NULL) {
        switch (nCode) {
        case CN_COMMAND:
            if (IDM_POPUP_DYN_BEGIN <= nID && nID < IDM_POPUP_DYN_END) {
                OnPopupCheckServer(nID);
                return TRUE;
            }
            break;
        }
    }
    return CDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL ClientMainDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);  // Set big icon
    SetIcon(m_hIcon, FALSE); // Set small icon

    // TODO: Add extra initialization here
    ModifyStyleEx(WS_EX_APPWINDOW, 0);

    // set up notify icon
    m_stNotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
    m_stNotifyIcon.uID = 0;
    m_stNotifyIcon.hWnd = m_hWnd;
    m_stNotifyIcon.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_stNotifyIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_stNotifyIcon.uCallbackMessage = UWM_USER_POPUP;
    lstrcpy(m_stNotifyIcon.szTip, L"WTSE Client is ready");
    ::Shell_NotifyIcon(NIM_ADD, &m_stNotifyIcon);

    // set up list columns
    LV_COLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.fmt = LVCFMT_LEFT;
    col.cchTextMax = 32;
    for (int i = 0; i < COL_NUM; ++i) {
        col.pszText = s_columnInfo[i].m_title;
        col.cx = s_columnInfo[i].m_width; // width
        col.iSubItem = i; // index of sub-item list
        m_serverList.InsertColumn(i, &col);
    }

    m_serverList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    UpdateItems();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ClientMainDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ClientMainDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void ClientMainDlg::OnDestroy()
{
    CDialog::OnDestroy();

    ::Shell_NotifyIcon(NIM_DELETE, &m_stNotifyIcon);
}

void ClientMainDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
    CDialog::OnWindowPosChanging(lpwndpos);

    if (m_bVisible) {
        lpwndpos->flags |= SWP_SHOWWINDOW;
    }
    else {
        lpwndpos->flags &= ~SWP_SHOWWINDOW;
    }
}

void ClientMainDlg::OnPopupPreference()
{
    // set window position
    CRect rectWindow;
    GetWindowRect(&rectWindow);

    CRect rectWorkArea;
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);

    const int offset = 8;
    int pos_x = rectWorkArea.Width() - rectWindow.Width() - offset;
    int pos_y = rectWorkArea.Height() - rectWindow.Height() - offset;
    SetWindowPos(&wndTop, pos_x, pos_y, 0, 0, SWP_NOSIZE);

    m_bVisible = true;
    ShowWindow(SW_SHOW);
}

void ClientMainDlg::OnPopupAboutbox()
{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
}

void ClientMainDlg::OnPopupClose()
{
    OnOK();
}

void ClientMainDlg::OnPopupCheckServer(UINT nID)
{
    int pos = (nID - IDM_POPUP_DYN_BEGIN);
    ASSERT(0 <= pos && pos < (IDM_POPUP_DYN_END - IDM_POPUP_DYN_BEGIN));

    ServerInfo::const_iterator cit = m_serverInfo.begin();
    ASSERT(cit != m_serverInfo.end());

    for (int i = 0; i < pos; ++i) {
        ++cit;
        ASSERT(cit != m_serverInfo.end());
    }

    WTSEClient::CPacketReceiver receiver;
    receiver.SetServer((*cit).m_csServer);
    receiver.SetPort((*cit).m_csPort);
    if (!receiver.Receive()) {
        // @TODO: show tooltip to notify error
        return;
    }

    CUserInfoDlg dlg;
    dlg.SetServerName((*cit).m_csServer);
    dlg.SetLogonSessionInfo(receiver.GetLogonSessionInfo());

    dlg.DoModal();
    return;
}

void ClientMainDlg::OnBnClickedBtnAddSvr()
{
    CServerInfoDlg dlg;
    dlg.m_csServer = CString(L"Server");
    dlg.m_csPort = CString(L"Port");

    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDCANCEL) {
        return;
    }
    else if (nResponse != IDOK) {
        ASSERT(false);
    }

    ServerInfoEntry info = {dlg.m_csServer, dlg.m_csPort};
    m_serverInfo.push_back(info);

    UpdateItems();
}

void ClientMainDlg::OnBnClickedBtnEdtSvr()
{
    int index = ListView_GetNextItem(m_serverList.m_hWnd, -1, LVNI_ALL | LVNI_SELECTED);
    if (index < 0) {
        ASSERT(false);
        return;
    }
    CString csServer = m_serverList.GetItemText(index, 0);
    CString csPort = m_serverList.GetItemText(index, 1);

    CServerInfoDlg dlg;
    dlg.m_csServer = csServer;
    dlg.m_csPort = csPort;

    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDCANCEL) {
        return;
    }
    else if (nResponse != IDOK) {
        ASSERT(false);
    }

    ServerInfoEntry temp = { csServer, csPort };
    ServerInfo::iterator itDel = std::find(m_serverInfo.begin(), m_serverInfo.end(), temp);
    if (itDel != m_serverInfo.end()) {
        (*itDel).m_csServer = dlg.m_csServer;
        (*itDel).m_csPort = dlg.m_csPort;
    }

    UpdateItems();
}

void ClientMainDlg::OnBnClickedBtnDelSvr()
{
    int index = ListView_GetNextItem(m_serverList.m_hWnd, -1, LVNI_ALL | LVNI_SELECTED);
    if (index < 0) {
        ASSERT(false);
        return;
    }
    CString csServer = m_serverList.GetItemText(index, 0);
    CString csPort = m_serverList.GetItemText(index, 1);

    ServerInfoEntry temp = { csServer, csPort };
    ServerInfo::iterator itDel = std::find(m_serverInfo.begin(), m_serverInfo.end(), temp);
    if (itDel != m_serverInfo.end()) {
        m_serverInfo.erase(itDel);
    }

    UpdateItems();
}

void ClientMainDlg::OnBnClickedBtnHideDlg()
{
    m_bVisible = false;
    ShowWindow(SW_HIDE);

    // unselect item
    int index = m_serverList.GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
    m_serverList.SetItemState(index , 0, LVIS_SELECTED);

    // update enable/disable here,
    // because the odd transition in status of controls can be seen at next time to show
    UpdateControlStatus();
}

void ClientMainDlg::OnLvnItemchangedListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    UpdateControlStatus();
    *pResult = 0;
}
