/// @file:  TransparentBitmapButton.h : header file

#pragma once
#include "afxext.h"

/// @brief  TransparentBitmapButton button
class CTransparentBitmapButton : public CBitmapButton
{
    DECLARE_DYNAMIC(CTransparentBitmapButton)

    // ctor/dtor
public:
    CTransparentBitmapButton();
    virtual ~CTransparentBitmapButton();

public:
    BOOL LoadBitmapForHover(LPCTSTR lpszBitmapResourceHover);
    BOOL LoadBitmapForMask(LPCTSTR lpszBitmapResourceMask);
    BOOL AutoLoad(UINT nID, CWnd* pParent);
    BOOL ReloadBitmaps(const CString& buttonName);

    // message handlers
protected:
    void OnMouseMove(UINT nFlags, CPoint point);
    LRESULT OnMouseHover(WPARAM wp, LPARAM lp);
    LRESULT OnMouseLeave(WPARAM wp, LPARAM lp);

protected:
    // bitmap must be the same size as normal image
    CBitmap m_bitmapHover; // hover
    CBitmap m_bitmapMask; // mask

    BOOL m_bHover;

    virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

    DECLARE_MESSAGE_MAP()
};

