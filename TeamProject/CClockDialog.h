#pragma once
#include "afxdialogex.h"

class CRankDialog;		// 랭크 다이얼로그 선언 : 추가
class CTeamProjectDlg;
class CAlarmDialog;
class CClockDialog;

// CClockDialog 대화 상자

class CClockDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CClockDialog)
public:
	enum { IDD = IDD_CLOCK_DIALOG };

	CClockDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CClockDialog();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	HACCEL m_hAccel;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
private:
	HICON m_hIcon;          // 애플리케이션 아이콘
	CFont m_timeFont;       // 시간 표시용 폰트
	CFont m_dateFont;       // 날짜 표시용 폰트
	CFont m_ampmFont;       // AM/PM 표시용 폰트

	// 내부 함수들
	void UpdateClock();                     // 시계 업데이트
	CString GetKoreanDayOfWeek(int dayOfWeek);  // 한국어 요일 반환
public:
	CStatic m_staticAMPM;
	CStatic m_staticDate;
	CStatic m_staticTime;
	afx_msg void OnStnClickedStaticTime();
	afx_msg void OnMenuAlarmOpen();
	afx_msg void OnMenuPomodoroOpen();


	template<typename T>
	void SwitchToDialog(T*& pDlg);
	CDialogEx* m_pAlarmDlg = nullptr;
	CRankDialog* m_pRankDlg = nullptr;
	CDialogEx* m_pPomodoroDlg = nullptr;
	CDialogEx* m_pCurrentChildDlg = nullptr;


	bool m_isDarkMode = false;        // 다크 모드 상태
	CBrush m_brushDarkBg;      // 다이얼로그 배경
	CBrush m_brushLightBg;     // 일반 모드 배경 (필요시)
	CBrush m_brBackground;
	CBrush m_brCtrlBk; // 컨트롤
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMenuToggleDarkmode();
	bool EnableDarkMode(HWND hWnd, bool enable);
};
