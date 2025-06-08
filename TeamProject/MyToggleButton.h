#pragma once
#include <afxwin.h>
class CMyToggleButton : public CButton
{
public:
    BOOL m_bDarkMode = FALSE;
    BOOL m_bIsPlaying = FALSE;

    void ToggleState() {
        m_bIsPlaying = !m_bIsPlaying;
        Invalidate(); // 다시 그리기
    }

    void ResetState() {
        m_bIsPlaying = FALSE;
        Invalidate(); // 버튼 리셋
    }

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


