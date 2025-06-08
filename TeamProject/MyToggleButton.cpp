#include "pch.h"
#include "MyToggleButton.h"

void CMyToggleButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rect = lpDrawItemStruct->rcItem;

    // 색상 설정
    COLORREF bgColor = m_bDarkMode ? RGB(30, 30, 30) : RGB(165, 80, 77);
    COLORREF textColor = RGB(255, 255, 255);

    pDC->FillSolidRect(rect, bgColor);
    pDC->SetBkMode(TRANSPARENT);
    pDC->SetTextColor(textColor);

    CBrush brush(textColor);
    pDC->SelectObject(&brush);

    if (m_bIsPlaying)
    {
        // Ⅱ 일시정지
        int barWidth = rect.Width() / 8;
        int gap = rect.Width() / 8;

        CRect bar1(rect.CenterPoint().x - gap - barWidth, rect.top + rect.Height() / 4,
            rect.CenterPoint().x - gap, rect.bottom - rect.Height() / 4);

        CRect bar2(rect.CenterPoint().x + gap, rect.top + rect.Height() / 4,
            rect.CenterPoint().x + gap + barWidth, rect.bottom - rect.Height() / 4);

        pDC->FillRect(&bar1, &brush);
        pDC->FillRect(&bar2, &brush);
    }
    else
    {
        // ▶ 재생
        POINT pts[3];
        pts[0].x = rect.left + rect.Width() / 3;
        pts[0].y = rect.top + rect.Height() / 4;

        pts[1].x = rect.left + rect.Width() / 3;
        pts[1].y = rect.bottom - rect.Height() / 4;

        pts[2].x = rect.right - rect.Width() / 4;
        pts[2].y = rect.CenterPoint().y;

        pDC->Polygon(pts, 3);
    }

    pDC->DrawEdge(&rect, EDGE_RAISED, BF_RECT); // 테두리
}