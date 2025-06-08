#include "pch.h"
#include "MyResetButton.h"

void CMyResetButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 배경색
    COLORREF bgColor = m_bDarkMode ? RGB(30, 30, 30) : RGB(165, 80, 77);
    COLORREF textColor = RGB(255, 255, 255);

    pDC->FillSolidRect(rect, bgColor);
    pDC->SetBkMode(TRANSPARENT);

    // ■ 정지 아이콘 (중앙 정사각형)
    int boxSize = min(rect.Width(), rect.Height()) / 2;
    CRect box(rect.CenterPoint().x - boxSize / 2,
        rect.CenterPoint().y - boxSize / 2,
        rect.CenterPoint().x + boxSize / 2,
        rect.CenterPoint().y + boxSize / 2);

    CBrush brush(textColor);
    pDC->SelectObject(&brush);
    pDC->FillRect(&box, &brush);

    // 테두리
    pDC->DrawEdge(&rect, EDGE_RAISED, BF_RECT);
}