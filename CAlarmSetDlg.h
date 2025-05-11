#pragma once
#include "afxdialogex.h"


// CAlarmSetDlg 대화 상자

class CAlarmSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlarmSetDlg)

public:
	CAlarmSetDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAlarmSetDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALARMSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	CBrush m_brBackground;
	CFont m_fontHour;
	CFont m_fontMinute;
	CFont m_fontName;


	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
