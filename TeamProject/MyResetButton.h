#pragma once
#include <afxwin.h>
class CMyResetButton : public CButton
{
public:
    BOOL m_bDarkMode = FALSE;

protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};
