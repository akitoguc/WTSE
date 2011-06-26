/// @file:  TransparentBitmapButton.cpp
/// @brief  Defines the class behaviors for the extended CBitmapButton.

#include "stdafx.h"
#include "TransparentBitmapButton.h"

CTransparentBitmapButton::CTransparentBitmapButton()
    : m_bHover(FALSE)
{
}

CTransparentBitmapButton::~CTransparentBitmapButton()
{
}

BOOL CTransparentBitmapButton::LoadBitmapForHover(LPCTSTR lpszBitmapResourceHover)
{
    m_bitmapHover.DeleteObject();

    if (!m_bitmapHover.LoadBitmap(lpszBitmapResourceHover)) {
        TRACE(traceAppMsg, 0, "Failed to load bitmap for normal image.\n");
        return FALSE;   // need this one image
    }

    return TRUE;
}

BOOL CTransparentBitmapButton::LoadBitmapForMask(LPCTSTR lpszBitmapResourceMask)
{
    m_bitmapMask.DeleteObject();

    if (!m_bitmapMask.LoadBitmap(lpszBitmapResourceMask)) {
        TRACE(traceAppMsg, 0, "Failed to load bitmap for normal image.\n");
        return FALSE;   // need this one image
    }

    return TRUE;
}

BOOL CTransparentBitmapButton::AutoLoad(UINT nID, CWnd* pParent)
{
    BOOL bAllLoaded = CBitmapButton::AutoLoad(nID, pParent);

    CString buttonName;
    GetWindowText(buttonName);
    ASSERT(!buttonName.IsEmpty());

    LoadBitmapForHover(buttonName + _T("H"));
    LoadBitmapForMask(buttonName + _T("M"));

    return bAllLoaded;
}

BOOL CTransparentBitmapButton::ReloadBitmaps(const CString& buttonName)
{
    ASSERT(!buttonName.IsEmpty());      // must provide a title

    LoadBitmaps(buttonName + _T("U"), buttonName + _T("D"),
        buttonName + _T("F"), buttonName + _T("X"));

    LoadBitmapForHover(buttonName + _T("H"));
    LoadBitmapForMask(buttonName + _T("M"));

    // we need at least the primary
    if (m_bitmap.m_hObject == NULL)
        return FALSE;

    // size to content
    SizeToContent();
    return TRUE;
}

void CTransparentBitmapButton::OnMouseMove(UINT nFlags, CPoint point)
{
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_HOVER | TME_LEAVE;
    tme.hwndTrack = this->GetSafeHwnd();
    tme.dwHoverTime = 1;
    ::TrackMouseEvent(&tme);
    CBitmapButton::OnMouseMove(nFlags, point);
}

LRESULT CTransparentBitmapButton::OnMouseHover(WPARAM wp, LPARAM lp)
{
    m_bHover = TRUE;
    Invalidate(FALSE);
    return FALSE;
}

LRESULT CTransparentBitmapButton::OnMouseLeave(WPARAM wp, LPARAM lp)
{
    m_bHover = FALSE;
    Invalidate(FALSE);
    return FALSE;
}

void CTransparentBitmapButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
    ASSERT(lpDIS != NULL);
    // must have at least the first bitmap loaded before calling DrawItem
    ASSERT(m_bitmap.m_hObject != NULL);     // required

    // use the main bitmap for up, the selected bitmap for down
    CBitmap* pBitmap = &m_bitmap;
    UINT state = lpDIS->itemState;
    if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL) {
        pBitmap = &m_bitmapSel;
    }
    else if (!(state & ODS_SELECTED) && m_bHover && m_bitmapHover.m_hObject != NULL) {
        pBitmap = &m_bitmapHover;
    }
    else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL) {
        pBitmap = &m_bitmapFocus;   // third image for focused
    }
    else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL) {
        pBitmap = &m_bitmapDisabled;   // last image for disabled
    }

    CBitmap* pBitmapMask = &m_bitmapMask;

    // draw the whole button
    CDC* pDC = CDC::FromHandle(lpDIS->hDC);

    CDC memDC, memDCMask, memDCImage;
    memDC.CreateCompatibleDC(pDC);
    if (pBitmapMask->m_hObject != NULL) {
        memDCMask.CreateCompatibleDC(pDC);
        memDCImage.CreateCompatibleDC(pDC);
    }

    CBitmap* pOld = memDC.SelectObject(pBitmap);
    if (pOld == NULL) {
        return;     // destructors will clean up
    }
    CBitmap *pOldMask, *pOldImage;
    if (pBitmapMask->m_hObject != NULL) {
        pOldMask = memDCMask.SelectObject(pBitmapMask);

        BITMAP bm;
        m_bitmap.GetBitmap(&bm);

        CBitmap bmpImage;
        bmpImage.CreateCompatibleBitmap(pDC, bm.bmWidth, bm.bmHeight);

        pOldImage = memDCImage.SelectObject(&bmpImage);
    }

    CRect rect;
    rect.CopyRect(&lpDIS->rcItem);

    if (pBitmapMask->m_hObject == NULL) {
        // copy the normal bitmap directly to the target DC
        pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    }
    else {
        // copy image pattern from the target device(pDC) to memory bitmap(memDCImage)
        //memDCImage.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, rect.left, rect.top, SRCCOPY);
        memDCImage.BitBlt(0, 0, rect.Width(), rect.Height(), pDC, rect.left, rect.top, SRCCOPY);

        // copy normal bitmap and mask bitmap to this memory bitmap three times,
        // using different operation modes
        memDCImage.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCINVERT);
        memDCImage.BitBlt(0, 0, rect.Width(), rect.Height(), &memDCMask, 0, 0, SRCAND);
        memDCImage.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCINVERT);

        // copy the final result to the target DC
        pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDCImage, 0, 0, SRCCOPY);
    }

    // select the bitmaps out of DCs
    memDC.SelectObject(pOld);
    if (pBitmapMask->m_hObject != NULL) {
        memDCMask.SelectObject(pOldMask);
        memDCImage.SelectObject(pOldImage);
    }
}

IMPLEMENT_DYNAMIC(CTransparentBitmapButton, CBitmapButton)

BEGIN_MESSAGE_MAP(CTransparentBitmapButton, CBitmapButton)
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()
