#pragma once
#include "afxdialogex.h"


// CAlarmDlg 대화 상자

class CAlarmDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAlarmDlg)

public:
	CAlarmDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CAlarmDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALARM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	CFont m_fontNewAlarmButton;
	CFont m_fontDeleteAlarmButton;

	DECLARE_MESSAGE_MAP()
};
