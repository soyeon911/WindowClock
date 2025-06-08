#pragma once
#include "afxdialogex.h"
#include "TeamProjectDlg.h"
#include <vector>

struct AlarmInfo
{
	CTime time;
	bool active;       // 알람이 울렸는지 여부 (반복 알람이면 계속 true)
	bool enabled;      // 알람 켜짐/꺼짐 상태 (true: 켜짐, false: 꺼짐)
	bool daysOfWeek[7]; // 월~일 (0:월, 1:화, ..., 6:일)

	CString name; // 알람 이름 추가
};

// CAlarmAppDlg 대화 상자
class CAlarmDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAlarmDialog)
	// 생성입니다.
public:
	CAlarmDialog(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	virtual ~CAlarmDialog();

	enum { IDD = IDD_ALARM_DIALOG };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	// 구현입니다.
protected:
	HICON m_hIcon;

	DECLARE_MESSAGE_MAP()
public:
	int m_nHour;
	int m_nMin;

	CString m_strAMPM;
	CString m_strAlarmTime;
	CString m_strAlarmName; // 알람 이름 입력값 추가

	std::vector<AlarmInfo> m_alarms;
	UINT_PTR m_nTimerID;
	BOOL m_bDaysOfWeek[7]; // 체크박스와 연결할 변수

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSetalarm();
	afx_msg void OnClickedButtonDeleteAlarm();
	afx_msg void OnClickedButtonToggleAlarm();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	//추가
	void SaveAlarmsToFile(const CString& path);
	void LoadAlarmsFromFile(const CString& path);

	bool m_isDarkMode = false;        // 다크 모드 상태
	CBrush m_brushDarkBg;      // 다이얼로그 배경
	CBrush m_brushLightBg;     // 일반 모드 배경 (필요시)
	CBrush m_brBackground;
	CBrush m_brCtrlBk; // 컨트롤
	void SetDarkMode(bool enable);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
