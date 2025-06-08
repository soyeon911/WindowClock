#include "pch.h"
#include "CMyButon.h"

void CMyButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 배경색과 텍스트 색상
    COLORREF bgColor = m_bDarkMode ? RGB(30, 30, 30) : RGB(165, 80, 77);
    COLORREF textColor = RGB(255, 255, 255);

    pDC->FillSolidRect(rect, bgColor);

    // 텍스트 출력
    CString text;
    GetWindowText(text);
    pDC->SetTextColor(textColor);
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(text, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    // 테두리
    pDC->DrawEdge(&rect, EDGE_RAISED, BF_RECT);
}
