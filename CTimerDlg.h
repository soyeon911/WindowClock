#pragma once
#include "afxdialogex.h"


// CTimerDlg 대화 상자

class CTimerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTimerDlg)

public:
	CTimerDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTimerDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_TIMER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	CBrush m_brBackGround;

	CFont m_fontPresentTime;
	CFont m_fontData;

	CFont m_fontPauseButton;
	CFont m_fontReStartButton;
	CFont m_fontStopButton;
	CFont m_fontDeleteButton;
	CFont m_fontBackButton;
	CFont m_fontPomodoroButton;
	CFont m_fontDarkModeButton;
	CFont m_fontPlusButton;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
