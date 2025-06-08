#pragma once
#include <afxwin.h>

class CMyResumeButton : public CButton
{
    BOOL m_bDarkMode = FALSE;

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

