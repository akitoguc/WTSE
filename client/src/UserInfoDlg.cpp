/// @file   UserInfoDlg.cpp
/// @brief  Defines the class behaviors for the application.

#include "stdafx.h"

#include <algorithm>
//#include <ntsecapi.h>
#include <wtsapi32.h>

#include "WTSEClient.h"
#include "UserInfoDlg.h"
#include "CommonDef.h"

namespace {
    /// @brief  index for specific column
    const int COL_IDX_ACTIVE = 0;
    const int COL_IDX_USER = 1;
    const int COL_IDX_DOMAIN = 2;
    const int COL_IDX_TIME = 3;
    const int COL_IDX_STATE = 4;
    const int COL_IDX_CLIENT = 5;
    const int COL_IDX_STATION = 6;
    /// @brief  number of columns
    const int COL_NUM = 7;

    /// @brief  definition of columns
    struct ColumnInfo
    {
        wchar_t* m_title;
        int m_width;
    }
    s_columnInfo[COL_NUM] = {
        { L"", 14 },
        { L"User", 48 },
        { L"Domain", 48 },
        { L"Logon Time", 76 },
        { L"State", 44 },
        { L"Client", 44 },
        { L"Win Station", 54 },
    };

    /// @brief  dialog life-time [msec]
    const int DIALOG_DURATION = 4000;
}

using namespace WTSEClient;

CUserInfoDlg::CUserInfoDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CUserInfoDlg::IDD, pParent)
    , m_bInitializing(false)
    , m_bMoving(false)
{
}

CUserInfoDlg::~CUserInfoDlg()
{
}

void CUserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_USERINFO, m_userinfoList);
}

// CUserInfoDlg message handlers

//LRESULT CUserInfoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message) {
//    case WM_NCHITTEST:
//        SetWindowLong(m_hWnd, DWL_MSGRESULT, HTCAPTION);
//        return TRUE;
//    }
//
//    return FALSE;//CDialog::WindowProc(message, wParam, lParam);
//}

BOOL CUserInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_bInitializing = true;

    MinimizeLogonSessionList();

    InitStaticServerName();
    InitListCtrl();
    InitListItems();
    InitWindow();

    m_bInitializing = false;

    OnStartTimer();

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CUserInfoDlg::OnPaint()
{
    CPaintDC dc(this);

    CDC dcImage;
    if (!dcImage.CreateCompatibleDC(&dc))  {
        return;
    }

    CBitmap* pOldBitmap = dcImage.SelectObject(&m_bitmapBg);
    dc.BitBlt(0, 0, m_rectWindow.Width(), m_rectWindow.Height(), &dcImage, 0, 0, SRCCOPY);
    dcImage.SelectObject(pOldBitmap);
}

void CUserInfoDlg::OnBnClickedUserinfoClose()
{
    OnOK();
}

void CUserInfoDlg::OnBnClickedUserinfoPin()
{
    if (m_nTimer) {
        OnStopTimer();
    }
    else {
        OnStartTimer();
    }

    UpdatePinButton(0 == m_nTimer);
}

void CUserInfoDlg::OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult)
{
    *pResult = m_bInitializing ? FALSE : TRUE;
}

void CUserInfoDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        OnOK();
    }
}

HBRUSH CUserInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    CBrush brNULL;
    switch (nCtlColor) {
    case CTLCOLOR_STATIC:
        pDC->SetTextColor(RGB(255,255,255));
        // fall through
    case CTLCOLOR_BTN:
        brNULL.CreateStockObject(NULL_BRUSH);
        pDC->SetBkMode(TRANSPARENT);
        return brNULL;
    }
    return (HBRUSH)NULL;
}

void CUserInfoDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_rectHandle.PtInRect(point)) {
        m_bMoving = TRUE;
        SetCapture();
        m_pointOld = point;
    }

    return CDialog::OnLButtonDown(nFlags, point);
}

void CUserInfoDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bMoving) {
        m_bMoving = FALSE;
        ::ReleaseCapture();
    }

    CDialog::OnLButtonUp(nFlags, point);
}

void CUserInfoDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    if (m_bMoving) {
        CRect rect;
        GetWindowRect(&rect);
        CPoint pos = rect.TopLeft() + (point - m_pointOld);
        SetWindowPos(&wndTopMost, pos.x, pos.y, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
    }
    CDialog::OnMouseMove(nFlags, point);
}

void CUserInfoDlg::InitStaticServerName()
{
    CWnd* pStatic = GetDlgItem(IDC_USERINFO_SVR);
    if (!pStatic) {
        return;
    }

    // set up bold font
    CFont *pFont = pStatic->GetFont();
    LOGFONT lf;
    pFont->GetLogFont(&lf);
    lf.lfWeight = FW_BOLD; // FW_SEMIBOLD;
    m_fontServerName.CreateFontIndirect(&lf);

    pStatic->SetFont(&m_fontServerName);

    pStatic->SetWindowText(m_csServer);
}

void CUserInfoDlg::InitListCtrl()
{
    // init image list
    m_statusImageList.Create(IDB_STATUS_LIST, 10, 1, RGB(255, 255, 255));
    m_userinfoList.SetImageList(&m_statusImageList, LVSIL_STATE);

    // set up list columns
    LV_COLUMN col;
    col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    col.fmt = LVCFMT_LEFT;
    col.cchTextMax = 32;
    for (int i = 0; i < COL_NUM; ++i) {
        col.pszText = s_columnInfo[i].m_title;
        col.cx = s_columnInfo[i].m_width; // width
        col.iSubItem = i; // index of sub-item list
        m_userinfoList.InsertColumn(i, &col);
    }

    m_userinfoList.SetExtendedStyle(LVS_EX_GRIDLINES);
}

void CUserInfoDlg::InitListItems()
{
    m_userinfoList.DeleteAllItems();
    ASSERT(0 == m_userinfoList.GetItemCount());

    LV_ITEM item;
    item.mask = LVIF_TEXT | LVIF_STATE;
    item.stateMask = LVIS_STATEIMAGEMASK;

    LogonSession::InfoList::const_iterator cit = m_logonSessionInfoList.begin();
    int pos = 0;
    for (; cit != m_logonSessionInfoList.end(); ++cit, ++pos) {
        item.iItem = pos;
        item.iSubItem = 0;
        item.pszText = L"";
        item.state = INDEXTOSTATEIMAGEMASK(static_cast<WTS_CONNECTSTATE_CLASS>((*cit).GetConnectionState()) == WTSActive ? 1 : 0);
        m_userinfoList.InsertItem(&item);

        m_userinfoList.SetItemText(pos, COL_IDX_USER, (*cit).GetUserName());
        m_userinfoList.SetItemText(pos, COL_IDX_DOMAIN, (*cit).GetDomain());
        m_userinfoList.SetItemText(pos, COL_IDX_TIME, FormatLogonTime((*cit).GetLogonTime()));
        m_userinfoList.SetItemText(pos, COL_IDX_STATE, WTSECommon::WTS_CONNECTIONSTATE_CLASS_STR[(*cit).GetConnectionState()]);
        m_userinfoList.SetItemText(pos, COL_IDX_CLIENT, (*cit).GetClientName());
        m_userinfoList.SetItemText(pos, COL_IDX_STATION, (*cit).GetWinStationName());
    }
}

void CUserInfoDlg::InitWindow()
{
    GetWindowRect(&m_rectWindow);

    // init handle region
    CRect rectTable;
    m_userinfoList.GetWindowRect(&rectTable);

    m_rectHandle = m_rectWindow;
    m_rectHandle.bottom = rectTable.top;

    // set up close button
    m_btnClose.AutoLoad(IDC_USERINFO_CLOSE, this);

    CRect rectClose;
    m_btnClose.GetWindowRect(&rectClose);

    m_btnClose.MoveWindow(
        m_rectWindow.Width() - rectClose.Width() - 6, // x
        4, //y
        rectClose.Width(), // cy
        rectClose.Height(), // cy
        TRUE);

    // set up pinning button
    m_btnPin.AutoLoad(IDC_USERINFO_PIN, this);
    UpdatePinButton(false);

    // load background image
    m_bitmapBg.LoadBitmap(L"IDB_BG");

    // locate window to initial position
    POINT point;
    ::GetCursorPos(&point);

    CRect rectWorkArea;
    ::SystemParametersInfo(SPI_GETWORKAREA, 0, &rectWorkArea, 0);

    const int offset = 8;

    int pos_x = (point.x > rectWorkArea.Width() - m_rectWindow.Width())
        ? rectWorkArea.Width() - m_rectWindow.Width() - offset
        : pos_x = point.x - (m_rectWindow.Width() / 2);

    int pos_y = (point.y > rectWorkArea.Height() - m_rectWindow.Height())
        ? rectWorkArea.Height() - m_rectWindow.Height() - offset
        : pos_y = point.y - (m_rectWindow.Height() / 2);

    SetWindowPos(&wndTop, pos_x, pos_y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
}

void CUserInfoDlg::UpdatePinButton(bool bFixed)
{
    CString buttonName;
    m_btnPin.GetWindowText(buttonName);
    if (bFixed) {
        buttonName += _T("FIXED_");
    }
    m_btnPin.ReloadBitmaps(buttonName);

    CRect rectPin;
    m_btnPin.GetWindowRect(&rectPin);

    CRect rectClose;
    m_btnClose.GetWindowRect(&rectClose);

    m_btnPin.MoveWindow(
        m_rectWindow.Width() - rectClose.Width() - 6 - rectPin.Width() - 4, // x
        4, //y
        rectPin.Width(), // cy
        rectPin.Height(), // cy
        TRUE);

    Invalidate(TRUE);
}

void CUserInfoDlg::OnStartTimer()
{
    m_nTimer = SetTimer(1, DIALOG_DURATION, NULL);
}

void CUserInfoDlg::OnStopTimer()
{
    KillTimer(m_nTimer);
    m_nTimer = 0;
}

void CUserInfoDlg::MinimizeLogonSessionList()
{
    LogonSession::InfoList::iterator itEnd;

    // remove invalid entry; ex) empty user name
    itEnd = std::remove_if(m_logonSessionInfoList.begin(), m_logonSessionInfoList.end(), &LogonSession::InfoEntry::IsInvalid);
    m_logonSessionInfoList.erase(itEnd, m_logonSessionInfoList.end());

    // remove duplicated entry
    std::sort(m_logonSessionInfoList.begin(), m_logonSessionInfoList.end());
    itEnd = std::unique(m_logonSessionInfoList.begin(), m_logonSessionInfoList.end());
    m_logonSessionInfoList.erase(itEnd, m_logonSessionInfoList.end());
}

CString CUserInfoDlg::FormatLogonTime(const LARGE_INTEGER& time) const
{
    FILETIME logonTime = *reinterpret_cast<const FILETIME*>(&time);
    if (logonTime.dwLowDateTime == 0 && logonTime.dwHighDateTime == 0) {
        return CString(L"unknown");
    }

    FILETIME localTime;
    FileTimeToLocalFileTime(&logonTime, &localTime);

    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&localTime, &systemTime);

    CString cstrLogonTime;
    cstrLogonTime.Format(L"%d/%02d/%02d %d:%d:%d",
            systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

    return cstrLogonTime;
}

BEGIN_MESSAGE_MAP(CUserInfoDlg, CDialog)
    ON_BN_CLICKED(IDC_USERINFO_CLOSE, &CUserInfoDlg::OnBnClickedUserinfoClose)
    ON_BN_CLICKED(IDC_USERINFO_PIN, &CUserInfoDlg::OnBnClickedUserinfoPin)
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
    ON_WM_TIMER()
    ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_USERINFO, &CUserInfoDlg::OnItemchangingList)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()
