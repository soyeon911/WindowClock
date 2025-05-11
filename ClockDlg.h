
// ClockDlg.h: 헤더 파일
//

#pragma once
#include "CTimerDlg.h"
#include "CAlarmDlg.h"
#include "CAlarmSetDlg.h"


// CClockDlg 대화 상자
class CClockDlg : public CDialogEx
{
// 생성입니다.
public:
	CClockDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLOCK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CBrush m_brBackground;
	CFont m_fontClockPresentTime;
	CFont m_fontClockAmPm;
	CFont m_fontClockTime;
	CFont m_fontClockDate;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
