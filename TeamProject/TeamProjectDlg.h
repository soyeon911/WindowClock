#pragma once

#include "afxdialogex.h"
#include "CALLOWWAPP.h"
#include "CMyButon.h"
#include "MyToggleButton.h"
#include "MyResetButton.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

class CRankDialog;		// 랭크 다이얼로그 선언
class CTeamProjectDlg;
class CAlarmDialog;
class CClockDialog;

struct PomodoroSessionData {
	int m_state;
	int m_focusMinutes;
	int m_restMinutes;
	int m_longRestMinutes;
	int m_goalSessions;
	int m_completedSessions;
	int m_cycleCount;
	int m_remainingSeconds;
	int m_totalRankCycleCount;
	BOOL m_isRunning;
	bool m_bIsGoalSet;
	bool m_bIsFocusing;
	bool m_bIsTimerRunning;
};

// CTeamProjectDlg 대화 상자
class CTeamProjectDlg : public CDialogEx
{
public:
	enum { IDD = IDD_TEAMPROJECT_DIALOG };

	CTeamProjectDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CTeamProjectDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	HACCEL m_hAccel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원
	virtual BOOL OnInitDialog();                         // 다이얼로그 초기화

	// 사용자 설정

	// 외부 다이얼로그 (시계/알람)
	CDialogEx* m_pClockDlg = nullptr;
	CDialogEx* m_pAlarmDlg = nullptr;
	CRankDialog* m_pRankDlg = nullptr;
	CDialogEx* m_pPomodoroDlg = nullptr;
	CDialogEx* m_pCurrentChildDlg = nullptr;

	// 기록 저장용
	CString m_lastSavePath;

	// 아이콘 핸들러
	HICON m_hIcon;

	//배경색 
	CBrush m_brBackground;
	CBrush m_brCtrlBk; // 컨트롤

	//목표 설정하기 버튼
	CMyButton m_btnSet;

	//재생버튼 토글
	CMyToggleButton m_btnPlay;

	//초기화 버튼
	CMyResetButton m_btnReset;

	//직전 세션 불러오기
	CMyButton m_btnResume;

	// 타이머 상태 열거형
	enum TimerState { STATE_READY, STATE_FOCUS, STATE_REST, STATE_LONGREST };
	TimerState m_state;

	// 사용자 설정값
	int m_focusMinutes;         // 집중 시간
	int m_restMinutes;          // 짧은 휴식 시간
	int m_longRestMinutes;      // 긴 휴식 시간
	int m_goalSessions;         // 목표 세션 수
	int m_completedSessions;    // 완료된 세션 수
	int m_cycleCount;           // 세션 반복 횟수
	int m_remainingSeconds;     // 남은 시간 (초)
	BOOL m_isRunning;           // 타이머 작동 여부
	bool m_bIsGoalSet = false;

	// 내부 제어 함수
	void ShowTimerA(BOOL show);                    // 타이머 영역 표시
	void ShowTimerB(BOOL show);						// 타이머 영역 표시
	void ShowTimerC(BOOL show);                    // 차트 영역 표시
	void UpdateTimerText(int sec);                 // 남은 시간 텍스트 업데이트 (IDC_STATIC_RESTTIME)
	//void DrawSessionPieChart();                    // Timer A에 표시되는 파이차트
	void UpdateStateText(const CString& state);    // 현재 상태 출력 (IDC_STATIC_STATE)
	void DrawCircleClock();
	void InitComboBoxes();
	void DrawSessionPieChartC();				//Timer C에 표시되는 파이차트

	// 다크모드 배경 브러시
	CBrush m_brushDarkBg;      // 다이얼로그 배경
	CBrush m_brushLightBg;     // 일반 모드 배경 (필요시)

	// 메시지 맵 함수
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickedButtonSetgoal();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnClickedButtonBack();

	afx_msg void OnMenuFileSave();
	void SaveAsCSVFile_AppendRow(const CString& path);	//.csv 덮어쓰기 함수
	void SaveCSV_ReadonlyCopy(const CString& writablePath);
	//void SwitchToDialog(CDialog* pNewDlg);
	void SwitchToDialog(CDialogEx* pNewDlg);
	//void InitChildDialogs();
	//void ShowOnly(CDialog* pDlgToShow);
	afx_msg void OnMenuClockOpen();
	afx_msg void OnMenuAlarmOpen();
	//afx_msg void OnMenuPomodoroOpen();
	afx_msg void OnMenuToggleDarkmode();
	afx_msg void OnMenuAllowAppOpen();
	afx_msg void OnBnClickedToggleTimeleft();
	
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

public:
	
	// 허용앱 
	CALLOWAPP m_dlgAllowApp; // 서브다이얼로그(허용앱)
	//CString m_strAllowUrl; //허용앱 주소
	CStringArray m_arrAllowUrls; // 허용앱 배열 주소
	afx_msg void OnClickedButtonAllow();
	bool m_bIsFocusing = FALSE; // 집중하고 있는지
	bool m_bBrowserWarned = false; // 처음에는 경고 안뜬상태
	void StartBreakTime();
	void KillBrowserIfNotAllowed();
	bool m_bBrowserNotified = false; // 허용 url 한번만 뜨도록
	bool m_bIsTimerRunning = FALSE; // 타이머가 실행중인지 여부
	void StartFocusTime();
	void StopTimer();
	bool IsWindowExist();

	//TimerC 파이차트 틀
	void DrawSessionPieChartOutline();
	void DrawTimerPieChartOutline();

	CString GetCurrentRankString();
	afx_msg void OnMenuRankOpen();
	afx_msg void OnDestroy();
	afx_msg void OnMenuStatistics();

	int m_totalRankCycleCount; // 전체 누적 집중 세션 수

	afx_msg void OnClickedButtonResume();

	PomodoroSessionData m_savedSession; // 멤버 변수로 선언
	void SaveCurrentSessionToFile(const CString& path);
	bool LoadSessionFromFile(const CString& path);

	template<typename T>
	void SwitchToDialog(T*& pDlg);

	CString GetSessionSummary(); // 직전 세션 정보 요약
	void SaveRankToFile(const CString& path); // 랭크 저장
	void LoadRankFromFile(const CString& path); // 랭크 불러오기

	int m_focusTotal; // 누적 집중 시간(분) ← 추가
	CString m_lastRankDate; // 마지막 랭크 갱신 날짜 (YYYY-MM-DD)
	CString GetTodayString(); // 오늘 날짜 문자열 반환 함수

	bool m_isDarkMode = false;
};
