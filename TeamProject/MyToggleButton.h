#pragma once
#include <afxwin.h>
class CMyToggleButton : public CButton
{
public:
    BOOL m_bDarkMode = FALSE;
    BOOL m_bIsPlaying = FALSE;

    void ToggleState() {
        m_bIsPlaying = !m_bIsPlaying;
        Invalidate(); // �ٽ� �׸���
    }

    void ResetState() {
        m_bIsPlaying = FALSE;
        Invalidate(); // ��ư ����
    }

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};


