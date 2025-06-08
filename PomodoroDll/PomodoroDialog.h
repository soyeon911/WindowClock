#pragma once
#include "resource.h"
#include "afxdialogex.h"

class CPomodoroDialog : public CDialogEx
{
public:
    CPomodoroDialog(CWnd* pParent = nullptr);

    enum { IDD = IDD_TEAMPROJECT_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedStart();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    void DrawCircleClock();
    void UpdateTimerText(int sec);

private:
    int m_focusMinutes;
    int m_restMinutes;
    int m_remainingSeconds;
    BOOL m_isRunning;
};
