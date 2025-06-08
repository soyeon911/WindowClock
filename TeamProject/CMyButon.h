#pragma once
#include <afxwin.h>

class CMyButton : public CButton
{
public:
    BOOL m_bDarkMode = FALSE;

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};

