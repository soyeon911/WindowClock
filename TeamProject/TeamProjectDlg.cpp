// 주요 개선:
// - 긴 휴식시간 사용자 설정 (Edit Control 사용)
// - 현재 상태 시각적 표시
// - 상태 변경 시 사운드 알림 추가
// - UI 텍스트 다국어 지원 구조화 준비
// - TimerB 입력에 따라 TimerA/C 표시 제어
// - cnt(세션 회수)에 따라 TimerC 파이차트 업데이트
// 다크모드와 관련된 정의 선언

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "pch.h"
#include "framework.h"
#include "TeamProject.h"
#include "TeamProjectDlg.h"
#include "afxdialogex.h"
#include <Windows.h>
#include "CALLOWWAPP.h"

#include <mmsystem.h>
#include "CRankDialog.h"        // 랭크 다이얼로그 헤더파일
#include "PomodoroDialog.h"
#include "CAlarmDialog.h"  
#include "CClockDialog.h"
#include "CStatisticsDialog.h" 

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDT_TIMERA 2001
#define TEST_FACTOR 1   // 실제 사용시 60으로 변경 (분->초 변환 비율)



enum TimerState { STATE_READY, STATE_FOCUS, STATE_REST, STATE_LONGREST };

CTeamProjectDlg::CTeamProjectDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_TEAMPROJECT_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDI_CLOCK);
}

CTeamProjectDlg::~CTeamProjectDlg()
{
}

BOOL CTeamProjectDlg::PreTranslateMessage(MSG* pMsg) {
    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
        return TRUE;

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CTeamProjectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_SETGOAL, m_btnSet);
    DDX_Control(pDX, IDC_BUTTON_START, m_btnPlay);
    DDX_Control(pDX, IDC_BUTTON_RESET, m_btnReset);
    DDX_Control(pDX, IDC_BUTTON_RESUME, m_btnResume);
}

BEGIN_MESSAGE_MAP(CTeamProjectDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_SETGOAL, &CTeamProjectDlg::OnClickedButtonSetgoal)
    ON_BN_CLICKED(IDC_BUTTON_START, &CTeamProjectDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_STOP, &CTeamProjectDlg::OnBnClickedButtonPause)
    ON_BN_CLICKED(IDC_BUTTON_RESET, &CTeamProjectDlg::OnBnClickedButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_BACK, &CTeamProjectDlg::OnClickedButtonBack)
    ON_BN_CLICKED(IDC_BUTTON_TIMELEFT, &CTeamProjectDlg::OnBnClickedToggleTimeleft)

    // 메뉴바
    ON_COMMAND(ID_MENU_FILE_SAVE, &CTeamProjectDlg::OnMenuFileSave)
    ON_COMMAND(ID_MENU_CLOCK_OPEN, &CTeamProjectDlg::OnMenuClockOpen)
    ON_COMMAND(ID_MENU_ALARM_OPEN, &CTeamProjectDlg::OnMenuAlarmOpen)
    //ON_COMMAND(ID_MENU_POMODORO_OPEN, &CTeamProjectDlg::OnMenuPomodoroOpen)
    ON_COMMAND(ID_MENU_TOGGLE_DARKMODE, &CTeamProjectDlg::OnMenuToggleDarkmode)
    ON_COMMAND(ID_MENU_ALLOW_APP_OPEN, &CTeamProjectDlg::OnMenuAllowAppOpen)
    ON_WM_CTLCOLOR()
    ON_COMMAND(ID_MENU_RANK_OPEN, &CTeamProjectDlg::OnMenuRankOpen)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BUTTON_RESUME, &CTeamProjectDlg::OnClickedButtonResume)
    ON_COMMAND(ID_MENU_STATISTICS, &CTeamProjectDlg::OnMenuStatistics)
END_MESSAGE_MAP()


BOOL CTeamProjectDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetWindowText(_T("종합 시계앱"));

    LoadRankFromFile(_T("RankSave.dat"));

    m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR2));

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // 브러시 생성 (다크 배경, 일반 배경)
    m_brushDarkBg.CreateSolidBrush(RGB(30, 30, 30));      // 진한 회색
    m_brushLightBg.CreateSolidBrush(RGB(255, 255, 255));  // 기본 흰 배경
    m_brBackground.CreateSolidBrush(RGB(165, 80, 77)); // 배경색
    m_brCtrlBk.CreateSolidBrush(RGB(165, 80, 77)); // 컨트롤 배경

    // 메뉴 리소스 연결
    CMenu menu;
    menu.LoadMenu(IDR_MENU2); // 메뉴 리소스 ID
    SetMenu(&menu);


    if (!m_dlgAllowApp.Create(IDD_DIALOG_ALLOW_APP, this)) {
        AfxMessageBox(_T("허용앱 다이얼로그 생성 실패!"));
    }
    else {
        m_dlgAllowApp.m_pMainDlg = this;
    }

    // 1초마다 체크 타이머 등록
    SetTimer(1, 1000, NULL);


    // 초기 UI: TimerB만 보이고 A, C는 숨김
    ShowTimerA(FALSE);
    ShowTimerC(FALSE);

    // 콤보박스 초기화
    // 실제 사용 시 해당 코드로 실행
    CComboBox* pFocusCombo = (CComboBox*)GetDlgItem(IDC_COMBO_FOCUS);
    /*for (int i = 25; i <= 90; i += 5) {
        CString str;
        str.Format(_T("%d 분"), i);
        pFocusCombo->AddString(str);
    }
    */
    CComboBox* pRestCombo = (CComboBox*)GetDlgItem(IDC_COMBO_REST);
    /*for (int i = 5; i <= 30; i += 5) {
        CString str;
        str.Format(_T("%d 분"), i);
        pRestCombo->AddString(str);
    }
    */
    //테스트 콤보박스
#if TEST_FACTOR == 1
// 디버깅용 시간 목록 (초 단위 테스트)
    pFocusCombo->AddString(_T("10 분"));  // 10초
    pRestCombo->AddString(_T("5 분"));    // 5초

    pFocusCombo->AddString(_T("30 분"));  // 10초
#else
    for (int i = 25; i <= 90; i += 5) {
        CString str;
        str.Format(_T("%d 분"), i);
        pFocusCombo->AddString(str);
    }

    for (int i = 5; i <= 30; i += 5) {
        CString str;
        str.Format(_T("%d 분"), i);
        pRestCombo->AddString(str);
    }
#endif

    m_btnPlay.ShowWindow(SW_SHOW);
    m_btnReset.ShowWindow(SW_SHOW);

    GetDlgItem(IDC_STATIC_TIMELEFT)->ShowWindow(SW_HIDE);  // 기본은 숨김
    GetDlgItem(IDC_STATIC_RESTTIME)->ModifyStyle(0, SS_OWNERDRAW | SS_NOTIFY);  //TimerA DrawCircle static 부분 투명화

    //SwitchToDialog();
    //ShowOnly(m_pClockDlg);

    m_btnPlay.ModifyStyle(0, BS_OWNERDRAW); // Owner Draw 설정
    m_btnPlay.m_bDarkMode = m_isDarkMode;

    m_btnReset.ModifyStyle(0, BS_OWNERDRAW);
    m_btnReset.m_bDarkMode = m_isDarkMode;

    m_btnReset.m_bDarkMode = m_isDarkMode;
    m_btnReset.Invalidate();

    m_btnSet.m_bDarkMode = m_isDarkMode;
    m_btnSet.Invalidate();

    m_btnResume.m_bDarkMode = m_isDarkMode;
    m_btnResume.Invalidate();

    m_btnSet.ModifyStyle(0, BS_OWNERDRAW);

    m_btnResume.ModifyStyle(0, BS_OWNERDRAW);

    return TRUE;
}

void CTeamProjectDlg::OnClickedButtonSetgoal()
{
    m_bIsGoalSet = true;
    CString strGoal;
    GetDlgItemText(IDC_EDIT_GOAL, strGoal);
    m_goalSessions = _ttoi(strGoal);

    int focusSel = ((CComboBox*)GetDlgItem(IDC_COMBO_FOCUS))->GetCurSel();
    int restSel = ((CComboBox*)GetDlgItem(IDC_COMBO_REST))->GetCurSel();

    CString strFocus, strRest;
    ((CComboBox*)GetDlgItem(IDC_COMBO_FOCUS))->GetLBText(focusSel, strFocus);
    ((CComboBox*)GetDlgItem(IDC_COMBO_REST))->GetLBText(restSel, strRest);

    if (focusSel == CB_ERR || restSel == CB_ERR || strGoal.IsEmpty())
    {
        AfxMessageBox(_T("모든 값을 올바르게 입력해주세요."));
        return;
    }

    m_focusMinutes = _ttoi(strFocus);
    m_restMinutes = _ttoi(strRest);
    m_longRestMinutes = 30; // ✨ 긴 휴식 시간 고정

    m_completedSessions = 0;
    m_cycleCount = 0;
    m_state = STATE_READY;

    // TimerA, TimerC 보이기, TimerB 숨기기
    ShowTimerA(TRUE);
    ShowTimerB(FALSE);
    ShowTimerC(TRUE);

    UpdateTimerText(m_focusMinutes * 60);
    
    DrawCircleClock();
    DrawSessionPieChartC();
    m_bIsGoalSet = true;
}

void CTeamProjectDlg::OnBnClickedButtonStart()
{

    if (!m_bIsGoalSet)
    {
        AfxMessageBox(_T("먼저 목표를 설정해주세요."));
        return;
    }

    if (!m_isRunning)
    {
        // 타이머 시작
        if (m_state == STATE_READY)
        {
            m_state = STATE_FOCUS;
            m_remainingSeconds = m_focusMinutes * TEST_FACTOR;
            UpdateStateText(_T("⏱ 집중 시간"));

            StartFocusTime();
        }
        else if (m_state == STATE_REST)
        {
            if (m_remainingSeconds <= 0)
                m_remainingSeconds = m_restMinutes * TEST_FACTOR;

            UpdateStateText(_T("🍵 짧은 휴식"));
        }
        else if (m_state == STATE_LONGREST)
        {
            if (m_remainingSeconds <= 0)
                m_remainingSeconds = m_longRestMinutes * TEST_FACTOR;

            UpdateStateText(_T("🌴 긴 휴식"));
        }
        else
        {
            UpdateStateText(_T("⏱ 집중 시간"));
        }

        SetTimer(IDT_TIMERA, 1000, NULL);
        m_isRunning = TRUE;
        m_btnPlay.ToggleState();
    }
    else
    {
        // 타이머 멈춤
        KillTimer(IDT_TIMERA);
        m_isRunning = FALSE;
        UpdateStateText(_T("⏸ 일시정지"));
        m_btnPlay.ToggleState();
    }

    DrawSessionPieChartOutline();
    ShowTimerC(TRUE);
}

void CTeamProjectDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == IDT_TIMERA && m_isRunning)
    {
        if (m_remainingSeconds-- <= 0)
        {
            KillTimer(IDT_TIMERA);
            m_isRunning = FALSE;
            PlaySound(_T("SystemAsterisk"), NULL, SND_ALIAS);

            CString today = GetTodayString();
            if (today != m_lastRankDate) {
                // 날짜가 바뀜 → 랭크 초기화
                m_focusTotal = 0;
                m_totalRankCycleCount = 0;
                SaveRankToFile(_T("RankSave.dat"));
                m_lastRankDate = today;
                if (m_pRankDlg && ::IsWindow(m_pRankDlg->GetSafeHwnd())) {
                    m_pRankDlg->SetFocusTotal(m_focusTotal);
                    m_pRankDlg->UpdateRank();
                }
            }

            // ✅ FOCUS 종료 → REST/ LONGREST 진입
            if (m_state == STATE_FOCUS)
            {
                m_cycleCount++;
                m_totalRankCycleCount++;

                m_state = (m_cycleCount % 4 == 0) ? STATE_LONGREST : STATE_REST;
                m_remainingSeconds = (m_state == STATE_LONGREST)
                    ? m_longRestMinutes * TEST_FACTOR
                    : m_restMinutes * TEST_FACTOR;

                UpdateStateText(m_state == STATE_LONGREST ? _T("🌴 긴 휴식") : _T("🍵 짧은 휴식"));

                m_isRunning = TRUE;
                SetTimer(IDT_TIMERA, 1000, NULL);
                return;
            }

            // ✅ REST / LONGREST 종료 → 세션 완료 후 다음 FOCUS or 종료 판단
            else if (m_state == STATE_REST || m_state == STATE_LONGREST)
            {
                m_completedSessions++;
                DrawSessionPieChartC();

                if (m_completedSessions < m_goalSessions)
                {
                    // 다음 FOCUS 타이머 시작
                    m_state = STATE_FOCUS;
                    m_remainingSeconds = m_focusMinutes * TEST_FACTOR;
                    m_isRunning = TRUE;
                    UpdateStateText(_T("⏱ 집중 시간"));
                    SetTimer(IDT_TIMERA, 1000, NULL);
                    StartFocusTime();
                }
                else
                {
                    // 🎯 최종 세션 종료
                    UpdateStateText(_T("✅ 완료!"));
                    AfxMessageBox(_T("🎉 목표 세션 완료!"));

                    m_state = STATE_READY;
                    m_isRunning = FALSE;
                    m_btnPlay.ResetState();

                    // 상태 초기화
                    m_bIsTimerRunning = FALSE;
                    m_bIsFocusing = FALSE;
                    m_bIsGoalSet = FALSE;
                    m_remainingSeconds = 0;
                    m_cycleCount = 0;


                    // UI 복원
                    ShowTimerA(FALSE);
                    ShowTimerC(FALSE);
                    ShowTimerB(TRUE);
                    GetDlgItem(IDC_EDIT_GOAL)->SetWindowText(_T(""));
                    InitComboBoxes();

                    SetDlgItemText(IDC_BUTTON_BACK, _T("<"));

                    // TimerA에 빈 원 강제 그리기
                    DrawCircleClock();
                    m_focusTotal += m_focusMinutes * m_goalSessions; // 누적 집중 시간 증가
                    SaveRankToFile(_T("RankSave.dat"));
                    SaveAsCSVFile_AppendRow(_T("PomodoroStatus.csv"));

                    if (m_pRankDlg && ::IsWindow(m_pRankDlg->GetSafeHwnd())) {
                        m_pRankDlg->SetFocusTotal(m_focusTotal);
                        m_pRankDlg->UpdateRank();
                    }
                }

                return;
            }
        }

        // ⏱️ 진행 중 UI 갱신
        DrawCircleClock();
        UpdateTimerText(m_remainingSeconds);
    }
    if (nIDEvent == 1)
    {
        if (m_bIsFocusing)
            KillBrowserIfNotAllowed();
        return;
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CTeamProjectDlg::UpdateStateText(const CString& state)
{
    SetDlgItemText(IDC_STATIC_NOW_STATE, state);
}

void CTeamProjectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CDialogEx dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}

void CTeamProjectDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();

    }
}


HCURSOR CTeamProjectDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CTeamProjectDlg::OnBnClickedButtonReset()
{
    // 확인/취소 메시지 박스
    int result = AfxMessageBox(
        _T("현제 진행중인 타이머를 초기화하시겠습니까?\n초기화되면 진행 중이던 타이머 정보가 모두 리셋됩니다."),
        MB_OKCANCEL | MB_ICONWARNING
    );

    if (result == IDCANCEL)//취소
        return;

    m_btnPlay.ResetState();
    KillTimer(IDT_TIMERA);
    m_isRunning = FALSE;
    m_state = STATE_READY;
    m_remainingSeconds = m_focusMinutes * 60;
    UpdateTimerText(m_remainingSeconds);
}

void CTeamProjectDlg::OnBnClickedButtonPause()
{
    KillTimer(IDT_TIMERA);
    m_isRunning = FALSE;

    GetDlgItem(IDC_BUTTON_START)->ShowWindow(SW_SHOW);    // 재생 다시 보임
    GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(SW_SHOW);

    DrawSessionPieChartC();
}

void CTeamProjectDlg::UpdateTimerText(int sec)
{
    CString str;
    str.Format(_T("%02d:%02d"), sec / 60, sec % 60);
    SetDlgItemText(IDC_STATIC_TIMELEFT, str);
}

void CTeamProjectDlg::OnBnClickedToggleTimeleft()
{
    CWnd* pTimeLabel = GetDlgItem(IDC_STATIC_TIMELEFT);
    CButton* pToggleBtn = (CButton*)GetDlgItem(IDC_BUTTON_TIMELEFT);

    if (!pTimeLabel || !pToggleBtn) return;

    if (pTimeLabel->IsWindowVisible())
    {
        pTimeLabel->ShowWindow(SW_HIDE);
        pToggleBtn->SetWindowText(_T("표시"));
    }
    else
    {
        pTimeLabel->ShowWindow(SW_SHOW);
        pToggleBtn->SetWindowText(_T("숨기기"));
    }
}

void CTeamProjectDlg::ShowTimerA(BOOL show)
{
    int cmd = show ? SW_SHOW : SW_HIDE;
    GetDlgItem(IDC_STATIC_RESTTIME)->ShowWindow(cmd);
    GetDlgItem(IDC_BUTTON_START)->ShowWindow(cmd);
    //GetDlgItem(IDC_BUTTON_STOP)->ShowWindow(cmd);
    GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(cmd);
}

void CTeamProjectDlg::ShowTimerB(BOOL show)
{
    int cmd = show ? SW_SHOW : SW_HIDE;
    GetDlgItem(IDC_EDIT_GOAL)->ShowWindow(cmd);
    GetDlgItem(IDC_COMBO_FOCUS)->ShowWindow(cmd);
    GetDlgItem(IDC_COMBO_REST)->ShowWindow(cmd);
    GetDlgItem(IDC_BUTTON_SETGOAL)->ShowWindow(cmd);
    GetDlgItem(IDC_GROUP_TIMERB)->ShowWindow(cmd);
    GetDlgItem(IDC_STATIC_GOAL_POMODORO)->ShowWindow(cmd);
    GetDlgItem(IDC_STATIC_REST)->ShowWindow(cmd);
    GetDlgItem(IDC_STATIC_FOCUS)->ShowWindow(cmd);
    GetDlgItem(IDC_BUTTON_RESUME)->ShowWindow(cmd);
    // TimerB에 속한 다른 컨트롤도 추가
}

void CTeamProjectDlg::ShowTimerC(BOOL show)
{
    int cmd = show ? SW_SHOW : SW_HIDE;
    CWnd* pBackBtn = GetDlgItem(IDC_BUTTON_BACK);
    if (pBackBtn)
    {
        
        pBackBtn->ShowWindow(cmd);
        pBackBtn->EnableWindow(TRUE); // 버튼 활성화
    }
    
    GetDlgItem(IDC_STATIC_PIECHART)->ShowWindow(cmd);
    GetDlgItem(IDC_BUTTON_BACK)->ShowWindow(cmd);
    GetDlgItem(IDC_GROUP_TIMERC)->ShowWindow(cmd);
    GetDlgItem(IDC_STATIC_PIECHART)->ShowWindow(cmd);
    //GetDlgItem(IDC_STATIC_DAYONGOING)->ShowWindow(cmd);
    GetDlgItem(IDC_STATIC_PIECHART)->ShowWindow(cmd);
    DrawSessionPieChartC();
}


void CTeamProjectDlg::DrawSessionPieChartOutline()
{
    CClientDC dc(GetDlgItem(IDC_STATIC_PIECHART));
    CRect rect;
    GetDlgItem(IDC_STATIC_PIECHART)->GetClientRect(&rect);
    //dc.FillSolidRect(&rect, RGB(255, 255, 255));

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2;
    int r = min(cx, cy) - 10;

    CPen pen(PS_SOLID, 2, RGB(100, 149, 237));
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);

    // 🌀 원 테두리만 그림
    dc.Ellipse(rect.left, rect.top, rect.right, rect.bottom);

    // 텍스트
    CString str;
    str.Format(_T("0 / %d 세션"), m_goalSessions);
    dc.SetTextColor(RGB(0, 0, 0));
    dc.SetBkMode(TRANSPARENT);
    dc.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    dc.SelectObject(pOldPen);
}



void CTeamProjectDlg::OnClickedButtonBack()
{
    int result = MessageBox(_T("세션 저장을 하시겠습니까?"), _T("뒤로가기"), MB_ICONQUESTION | MB_YESNOCANCEL);
    // MB_ABORT: 전체 초기화, MB_RETRY: 세션 저장, MB_IGNORE: 취소

    if (result == IDCANCEL) // 취소
        return;

    if (result == IDYES) { // 세션 저장 후 종료
        int save = MessageBox(_T("정말 세션을 저장하시겠습니까? \n수행되지 않은 직전 세션은 폐기됩니다."), _T("세션저장"), MB_ICONWARNING | MB_YESNO);
        
        if (save == IDYES) {
            SaveCurrentSessionToFile(_T("SessionSave.dat"));
        }
        else
            return;
    }

    // 🔁 상태 완전 초기화
    KillTimer(IDT_TIMERA);
    m_isRunning = FALSE;
    m_bIsTimerRunning = FALSE;
    m_bIsFocusing = FALSE;
    m_bIsGoalSet = FALSE;
    m_btnPlay.ResetState();

    m_state = STATE_READY;
    m_remainingSeconds = 0;
    m_completedSessions = 0;
    m_cycleCount = 0;
    
    DrawCircleClock();  // 초기화 시 TimerA pieCircle 빈 원으로 보이게
    UpdateTimerText(0); // 00:00으로 표시

    // 랭크 창 종료
    if (m_pRankDlg && ::IsWindow(m_pRankDlg->GetSafeHwnd())) {
        m_pRankDlg->DestroyWindow();
        delete m_pRankDlg;
        m_pRankDlg = nullptr;
    }

    // UI 복원
    ShowTimerC(FALSE);
    ShowTimerB(TRUE);

    // 사용자 입력 초기화
    GetDlgItem(IDC_EDIT_GOAL)->SetWindowText(_T(""));
    InitComboBoxes();  // SetCurSel(-1) 포함

    UpdateData(FALSE);

    // 버튼 텍스트
    SetDlgItemText(IDC_BUTTON_BACK, _T("<"));
}

void CTeamProjectDlg::InitComboBoxes()
{
    // 집중 시간 콤보박스 초기화
    CComboBox* pFocusCombo = (CComboBox*)GetDlgItem(IDC_COMBO_FOCUS);
    if (pFocusCombo) {
        pFocusCombo->ResetContent();
        for (int i = 25; i <= 90; i += 5) {
            CString str;
            str.Format(_T("%d 분"), i);
            pFocusCombo->AddString(str);
        }
        pFocusCombo->SetCurSel(-1); // 선택 해제
    }

    // 휴식 시간 콤보박스 초기화
    CComboBox* pRestCombo = (CComboBox*)GetDlgItem(IDC_COMBO_REST);
    if (pRestCombo) {
        pRestCombo->ResetContent();
        for (int i = 5; i <= 30; i += 5) {
            CString str;
            str.Format(_T("%d 분"), i);
            pRestCombo->AddString(str);
        }
        pRestCombo->SetCurSel(-1); // 선택 해제
    }
}

void CTeamProjectDlg::DrawSessionPieChartC()
{
    CClientDC dc(GetDlgItem(IDC_STATIC_PIECHART));
    CRect rect;
    GetDlgItem(IDC_STATIC_PIECHART)->GetClientRect(&rect);
    //dc.FillSolidRect(&rect, RGB(255, 255, 255));

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2;
    int r = min(cx, cy) - 10;

    double percent = (m_goalSessions > 0) ? (double)m_completedSessions / m_goalSessions : 0.0;
    double angle = 360.0 * percent;

    dc.SelectStockObject(NULL_BRUSH);
    dc.SetArcDirection(AD_CLOCKWISE);

    // 🔁 시작 각도: 270도 (12시 방향)
    double radStart = 270.0 * 3.141592 / 180.0;
    int startX = (int)(cx + r * cos(radStart));
    int startY = (int)(cy + r * sin(radStart));

    // 끝 각도: 시작 + angle
    double radEnd = (270.0 + angle) * 3.141592 / 180.0;
    int endX = (int)(cx + r * cos(radEnd));
    int endY = (int)(cy + r * sin(radEnd));

    // ✅ 파란색 진행 부분
    CBrush brushDone(RGB(165, 80, 70));
    dc.SelectObject(&brushDone);
    dc.Pie(rect.left, rect.top, rect.right, rect.bottom,
        startX, startY,
        endX, endY);

    // ✅ 회색 남은 부분
    if (percent < 1.0)
    {
        CBrush brushRemain(RGB(255, 255, 255));
        dc.SelectObject(&brushRemain);
        dc.Pie(rect.left, rect.top, rect.right, rect.bottom,
            endX, endY,
            startX, startY);
    }

    CString str;
    str.Format(_T("%d / %d 세션"), m_completedSessions, m_goalSessions);
    dc.SetTextColor(RGB(0, 0, 0));
    dc.SetBkMode(TRANSPARENT);
    dc.DrawText(str, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    
}

CString CTeamProjectDlg::GetCurrentRankString()
{
    int totalFocusMinutes = m_focusTotal;
    if (totalFocusMinutes >= 960) return _T("Challenger");
    else if (totalFocusMinutes >= 660) return _T("Master");
    else if (totalFocusMinutes >= 420) return _T("Diamond");
    else if (totalFocusMinutes >= 240) return _T("Platinum");
    else if (totalFocusMinutes >= 120)  return _T("Gold");
    else if (totalFocusMinutes >= 60)  return _T("Silver");
    else return _T("Bronze");
}

// 메뉴바 기능에 대한 함수
//데이터 파일 저장 관련 함수
void CTeamProjectDlg::OnMenuFileSave()
{
    CFileDialog dlg(FALSE, _T("csv"), _T("PomodoroStatus"), OFN_OVERWRITEPROMPT,
        _T("CSV 파일 (*.csv)|*.csv||"));

    if (dlg.DoModal() == IDOK)
    {
        CString destPath = dlg.GetPathName();

        // 기존 저장된 CSV 파일 경로
        CString srcPath = _T("PomodoroStatus.csv");

        CStdioFile srcFile;
        if (!srcFile.Open(srcPath, CFile::modeRead | CFile::typeText))
        {
            AfxMessageBox(_T("원본 CSV 파일을 열 수 없습니다."));
            return;
        }

        CString fileContent, line;
        while (srcFile.ReadString(line))
        {
            fileContent += line + _T("\n");
        }
        srcFile.Close();

        // 새 파일로 저장
        CStdioFile destFile;
        if (destFile.Open(destPath, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
        {
            destFile.WriteString(fileContent);
            destFile.Close();
            AfxMessageBox(_T("CSV 파일이 성공적으로 저장되었습니다!"));
        }
        else
        {
            AfxMessageBox(_T("파일 저장에 실패했습니다."));
        }
    }
}


void CTeamProjectDlg::SaveAsCSVFile_AppendRow(const CString& path)
{
    CString today = CTime::GetCurrentTime().Format(_T("%Y-%m-%d"));
    CString rank = GetCurrentRankString();

    CString newRow;
    newRow.Format(_T("%s,%d,%d,%d,%d,%d,%02d:%02d,%s\r\n"),
        today,
        m_completedSessions,
        m_goalSessions,
        m_focusMinutes,
        m_restMinutes,
        m_longRestMinutes,
        m_remainingSeconds / 60,
        m_remainingSeconds % 60,
        rank);

    CStringArray lines;
    bool hasHeader = false;

    // 1. 기존 파일에서 내용을 읽어온다 (있으면)
    if (PathFileExists(path))
    {
        CStdioFile fileRead;
        if (fileRead.Open(path, CFile::modeRead | CFile::typeText))
        {
            CString line;
            while (fileRead.ReadString(line))
            {
                // 기존 BOM 제거 (첫 줄의 BOM)
                if (lines.GetSize() == 0 && line.GetLength() >= 1 && line[0] == 0xFEFF)
                    line = line.Mid(1);

                lines.Add(line + _T("\r\n"));
                if (line.Find(_T("Date")) != -1)
                    hasHeader = true;
            }
            fileRead.Close();
        }
    }

    // 2. 헤더가 없으면 추가
    if (!hasHeader)
        lines.InsertAt(0, _T("Date,Completed,Goal,Focus,Rest,LongRest,RemainingTime,Rank\r\n"));

    // 3. 새 행 추가
    lines.Add(newRow);

    // 4. 파일 다시 저장
    CFile file;
    if (file.Open(path, CFile::modeCreate | CFile::modeWrite))
    {
        // UTF-8 BOM 삽입
        //static const BYTE utf8BOM[] = { 0xEF, 0xBB, 0xBF };
        //file.Write(utf8BOM, sizeof(utf8BOM));

        for (int i = 0; i < lines.GetSize(); ++i)
        {
            CT2CA utf8Line(lines[i], CP_UTF8);
            file.Write(utf8Line, (UINT)strlen(utf8Line));
        }

        file.Close();
        AfxMessageBox(_T("CSV 파일 저장 완료"));
    }
    else
    {
        AfxMessageBox(_T("CSV 저장 실패"));
    }
}



void CTeamProjectDlg::SaveCSV_ReadonlyCopy(const CString& writablePath)
{
    // 1. 내부용 저장
    SaveAsCSVFile_AppendRow(writablePath);

    // 2. 읽기 전용 복사본 경로 지정
    CString readOnlyPath = writablePath;
    readOnlyPath.Replace(_T(".csv"), _T("_readonly.csv"));

    // 3. 기존 복사본이 있으면 삭제
    if (PathFileExists(readOnlyPath))
        DeleteFile(readOnlyPath);

    // 4. 복사 + 읽기 전용 속성 설정
    if (::CopyFile(writablePath, readOnlyPath, FALSE))
    {
        ::SetFileAttributes(readOnlyPath, FILE_ATTRIBUTE_READONLY);
        AfxMessageBox(_T("CSV 저장 완료 (읽기 전용 복사본 포함)"));
    }
    else
    {
        AfxMessageBox(_T("복사본 생성 실패"));
    }
}


template<typename T>
void CTeamProjectDlg::SwitchToDialog(T*& pDlg)
{
    if (m_pCurrentChildDlg && ::IsWindow(m_pCurrentChildDlg->GetSafeHwnd()))
        m_pCurrentChildDlg->ShowWindow(SW_HIDE);

    if (!pDlg)
        pDlg = new T;

    m_pCurrentChildDlg = pDlg;

    if (!pDlg->GetSafeHwnd())
        pDlg->Create(T::IDD, this);  // ✅ 정적 멤버로 접근

    CWnd* pPlaceholder = GetDlgItem(IDC_CHILD_PLACEHOLDER);
    if (pPlaceholder)
    {
        CRect rect;
        pPlaceholder->GetWindowRect(&rect);
        ScreenToClient(&rect);
        pDlg->MoveWindow(&rect);
    }

    pDlg->ShowWindow(SW_SHOW);
}



////void CTeamProjectDlg::OnMenuClockOpen()
//{
//    SwitchToDialog<CClockDialog>((CClockDialog*&)m_pClockDlg);
//}
void CTeamProjectDlg::OnMenuClockOpen()
{
    if (m_isRunning != FALSE)
    {
        MessageBox(_T("타이머가 진행 중일때 시계를 볼 수 없습니다."));
        return;
    }
    

    if (m_pAlarmDlg != nullptr) {
        m_pAlarmDlg->DestroyWindow();
    }
    if (m_pRankDlg != nullptr) {
        m_pRankDlg->DestroyWindow();
    }
    
    // 1. 현재 다이얼로그 숨기기
    ShowWindow(SW_HIDE);

    // 2. ⭐ 메인 윈도우 임시 설정
    CWnd* pOldMainWnd = AfxGetApp()->m_pMainWnd;

    // 3. 시계 다이얼로그 생성 및 실행
    CClockDialog clockDlg;
    clockDlg.m_isDarkMode = m_isDarkMode;
    AfxGetApp()->m_pMainWnd = &clockDlg;  // ⭐ 메인 윈도우로 설정
    clockDlg.DoModal();

    // 4. 메인 윈도우 복원
    AfxGetApp()->m_pMainWnd = pOldMainWnd;

    // 5. 현재 다이얼로그 종료
    CDialogEx::OnOK();
}

//void CTeamProjectDlg::OnMenuPomodoroOpen()
//{
//    SwitchToDialog<CTeamProjectDlg>((CTeamProjectDlg*&)m_pPomodoroDlg);
//}

void CTeamProjectDlg::OnMenuAlarmOpen()
{
    SwitchToDialog((CAlarmDialog*&)m_pAlarmDlg);
    // 알람 다이얼로그가 생성된 직후 다크모드 상태를 전달
    if (m_pAlarmDlg)
        ((CAlarmDialog*)m_pAlarmDlg)->SetDarkMode(m_isDarkMode);
}

//void CTeamProjectDlg::OnMenuAlarmOpen()
//{
//    // 1. 현재 다이얼로그 숨기기
//    ShowWindow(SW_HIDE);
//
//    // 2. ⭐ 메인 윈도우 임시 설정
//    CWnd* pOldMainWnd = AfxGetApp()->m_pMainWnd;
//
//    // 3. 시계 다이얼로그 생성 및 실행
//    CAlarmDialog m_pAlarmDlg;
//    AfxGetApp()->m_pMainWnd = &m_pAlarmDlg;  // ⭐ 메인 윈도우로 설정
//    m_pAlarmDlg.DoModal();
//
//    // 4. 메인 윈도우 복원
//    AfxGetApp()->m_pMainWnd = pOldMainWnd;
//
//    // 5. 현재 다이얼로그 종료
//    CDialogEx::OnOK();
//}


HBRUSH CTeamProjectDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (pWnd->GetDlgCtrlID() == IDC_STATIC_RESTTIME)        // TimerA에서 그림 위에 static 박스가 가리지 않도록
    {
        pDC->SetBkMode(TRANSPARENT);
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }

    if (m_isDarkMode)
    {
        switch (nCtlColor)
        {
        case CTLCOLOR_DLG:
            return (HBRUSH)m_brushDarkBg;

        case CTLCOLOR_STATIC:
            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextColor(RGB(255, 255, 255)); // 흰색 글자
            return (HBRUSH)m_brushDarkBg;

        case CTLCOLOR_BTN:
            //pDC->SetBkColor(RGB(60, 60, 60)); // 버튼 배경
            pDC->SetTextColor(RGB(255, 255, 255));
            return (HBRUSH)m_brushDarkBg;

        case CTLCOLOR_EDIT:
        case CTLCOLOR_LISTBOX:
            pDC->SetBkColor(RGB(50, 50, 50)); // 에디트/콤보 배경
            pDC->SetTextColor(RGB(255, 255, 255));
            return (HBRUSH)m_brushDarkBg;
        }
    }

    if (nCtlColor == CTLCOLOR_DLG)  // 다이얼로그 배경
    {
        pDC->SetTextColor(RGB(255, 255, 255));      // 글자색 흰색
        pDC->SetBkMode(TRANSPARENT); // 배경 투명
        return m_brBackground;
    }

    if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_BTN)
    {
        pDC->SetBkColor(RGB(165, 80, 77));       // 배경색
        pDC->SetTextColor(RGB(255, 255, 255));   // 글자색 흰색
        pDC->SetBkMode(TRANSPARENT);             // 텍스트 배경 투명
        return m_brCtrlBk;
    }

    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}


bool EnableDarkMode(HWND hWnd, bool enable)
{
    BOOL value = enable ? TRUE : FALSE;
    HRESULT hr = ::DwmSetWindowAttribute(
        hWnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &value,
        sizeof(value)
    );
    return SUCCEEDED(hr);
}

void CTeamProjectDlg::OnMenuToggleDarkmode()
{
    m_isDarkMode = !m_isDarkMode;

    if (EnableDarkMode(m_hWnd, m_isDarkMode)) {
        Invalidate(TRUE); // 전체 다시 그리기
        AfxMessageBox(m_isDarkMode ? _T("다크모드 ON") : _T("다크모드 OFF"));
    }
    else {
        AfxMessageBox(_T("다크모드 적용 실패"));
    }
    m_btnPlay.m_bDarkMode = m_isDarkMode;
    m_btnPlay.Invalidate();

    m_btnReset.m_bDarkMode = m_isDarkMode;
    m_btnReset.Invalidate();

    m_btnSet.m_bDarkMode = m_isDarkMode;
    m_btnSet.Invalidate();

    m_btnResume.m_bDarkMode = m_isDarkMode;
    m_btnResume.Invalidate();
}

void CTeamProjectDlg::DrawTimerPieChartOutline()
{
    CClientDC dc(GetDlgItem(IDC_STATIC_RESTTIME));
    CRect rect;
    GetDlgItem(IDC_STATIC_RESTTIME)->GetClientRect(&rect);
    //dc.FillSolidRect(&rect, RGB(255, 255, 255));

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2;
    int r = min(cx, cy) - 10;

    CPen pen(PS_SOLID, 2, RGB(100, 149, 237));
    CPen* pOldPen = dc.SelectObject(&pen);
    dc.SelectStockObject(NULL_BRUSH);

    // 🌀 원 테두리만 그림
    dc.Ellipse(rect.left, rect.top, rect.right, rect.bottom);

   
    dc.SelectObject(pOldPen);
}

void CTeamProjectDlg::DrawCircleClock()
{
    CClientDC dc(GetDlgItem(IDC_STATIC_RESTTIME));
    CRect rect;
    GetDlgItem(IDC_STATIC_RESTTIME)->GetClientRect(&rect);

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2;
    int r = min(cx, cy) - 10;

    double percent = 0.0;
    int totalSec = 1;
    switch (m_state)
    {
    case STATE_FOCUS:
        totalSec = m_focusMinutes * TEST_FACTOR;
        break;
    case STATE_REST:
        totalSec = m_restMinutes * TEST_FACTOR;
        break;
    case STATE_LONGREST:
        totalSec = m_longRestMinutes * TEST_FACTOR;
        break;
    }

    if (m_remainingSeconds + 1 >= 0)
    {
        percent = (double)(totalSec - m_remainingSeconds) / totalSec;
        percent = max(0.0, min(1.0, percent));
    }
    //else
    //{
      //  percent = 1.0;
    //}

    dc.SelectStockObject(NULL_BRUSH);
    dc.SetArcDirection(AD_CLOCKWISE);

    // 상태별 색상 지정
    COLORREF fillColor = RGB(165, 80, 70); // 기본 집중색
    if (m_state == STATE_REST) fillColor = RGB(120, 220, 120);
    else if (m_state == STATE_LONGREST) fillColor = RGB(180, 120, 255);

    if (m_remainingSeconds <= 0)
    {
        // 💯 원 전체 채우기
        CBrush brushDone(fillColor);
        dc.SelectObject(&brushDone);
        dc.Ellipse(rect.left, rect.top, rect.right, rect.bottom);
        return;
    }

    // 부채꼴 각도 계산
    double angle = 360.0 * percent;
    double radStart = 270.0 * 3.141592 / 180.0;
    double radEnd = (270.0 + angle) * 3.141592 / 180.0;

    int startX = (int)(cx + r * cos(radStart));
    int startY = (int)(cy + r * sin(radStart));
    int endX = (int)(cx + r * cos(radEnd));
    int endY = (int)(cy + r * sin(radEnd));

    // 🔵 진행된 부분 그리기
    CBrush brushDone(fillColor);
    dc.SelectObject(&brushDone);
    dc.Pie(rect.left, rect.top, rect.right, rect.bottom,
        startX, startY,
        endX, endY);

    // ⚪ 남은 부분 그리기
    CBrush brushRemain(RGB(255, 255, 255));
    dc.SelectObject(&brushRemain);
    dc.Pie(rect.left, rect.top, rect.right, rect.bottom,
        endX, endY,
        startX, startY);
}


void CTeamProjectDlg::OnMenuRankOpen()
{
    if (m_pRankDlg)
        ((CRankDialog*)m_pRankDlg)->SetDarkMode(m_isDarkMode);

    LoadRankFromFile(_T("RankSave.dat"));


    if (m_pRankDlg == nullptr || !::IsWindow(m_pRankDlg->GetSafeHwnd())) {
        m_pRankDlg = new CRankDialog(this);
        m_pRankDlg->SetFocusTotal(m_focusTotal);
        m_pRankDlg->Create(IDD_RANK_DIALOG, this);
        m_pRankDlg->ShowWindow(SW_SHOW);
    }
    else {
        m_pRankDlg->SetFocusTotal(m_focusTotal);
        m_pRankDlg->ShowWindow(SW_SHOW);
        m_pRankDlg->SetForegroundWindow();
    }

    // 랭크 갱신
    //if (m_pRankDlg && ::IsWindow(m_pRankDlg->GetSafeHwnd())) {
    //    m_pRankDlg->SetFocusInfo(m_focusMinutes, m_totalRankCycleCount); // ★ 누적값 전달
    //    m_pRankDlg->UpdateRank();
    //}

    if (m_pRankDlg && ::IsWindow(m_pRankDlg->GetSafeHwnd())) {
        m_pRankDlg->SetFocusTotal(m_focusTotal);
        m_pRankDlg->UpdateRank();
    }

}
// 미허용 앱 실행 시 앱 종료
void CTeamProjectDlg::KillBrowserIfNotAllowed()
{
    static bool wasChromeOpen = false;
    bool isChromeOpen = IsWindowExist();

    // 🚀 크롬이 꺼졌다가 다시 켜진 경우 상태 리셋
    if (!isChromeOpen)
    {
        if (wasChromeOpen) // 꺼졌다가 사라진 경우에만 리셋
        {
            m_bBrowserWarned = FALSE;
            m_bBrowserNotified = FALSE;
            OutputDebugString(_T("[허용앱 감시] 크롬 꺼짐 감지, 상태 리셋\n"));
        }
    }

    // 이전 상태 업데이트
    wasChromeOpen = isChromeOpen;

    // 🚩 이전에 경고한 상태면 return
    if (m_bBrowserWarned) return;

    // 윈도우 리스트 순회
    HWND hWnd = ::GetTopWindow(NULL);
    while (hWnd)
    {
        TCHAR title[256] = { 0 };
        ::GetWindowText(hWnd, title, 256);
        CString strTitle(title);

        // Chrome 창인지 확인
        if (strTitle.Find(_T(" - Chrome")) != -1)
        {
            // 새 탭이면 무시
            if (strTitle.Find(_T("새 탭")) != -1)
            {
                hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
                continue;
            }

            // ✅ 다중 허용 키워드 검사 (대소문자, 공백 무시)
            bool bAllowed = false;

            CString strTitleLower = strTitle;
            strTitleLower.MakeLower();
            strTitleLower.Replace(_T(" "), _T("")); // 공백 제거

            for (int i = 0; i < m_arrAllowUrls.GetSize(); ++i)
            {
                CString strAllow = m_arrAllowUrls[i];
                strAllow.MakeLower();
                strAllow.Replace(_T(" "), _T("")); // 공백 제거

                if (strTitleLower.Find(strAllow) != -1)
                {
                    bAllowed = true;
                    break;
                }
            }

            if (bAllowed)
            {
                if (!m_bBrowserNotified)
                {
                    m_bBrowserNotified = TRUE;
                    AfxMessageBox(_T("허용된 사이트에 접속했습니다."));
                }
            }
            else
            {
                // 🚩 허용되지 않은 사이트 → 강제 종료
                m_bBrowserWarned = true;
                AfxMessageBox(_T("허용되지 않은 사이트입니다. 브라우저를 종료합니다."));
                system("taskkill /IM chrome.exe /F");
                 
                // 🚀 바로 상태 리셋 준비 → 강제 재감시 가능하도록
                m_bBrowserWarned = FALSE;
                m_bBrowserNotified = FALSE;

                break;
            }
        }

        hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
    }
}

void CTeamProjectDlg::StartBreakTime()
{
    // TODO: 여기에 구현 코드 추가.
    m_bIsFocusing = FALSE;
    m_bBrowserWarned = FALSE;
    m_bBrowserNotified = FALSE;
    KillTimer(1);
}

void CTeamProjectDlg::StartFocusTime()
{
    // TODO: 여기에 구현 코드 추가.
    m_bIsTimerRunning = TRUE;
    m_bIsFocusing = TRUE;

    m_bBrowserWarned = FALSE;
    m_bBrowserNotified = FALSE;

    OutputDebugString(_T("[StartFocusTime] 타이머 시작됨\n"));

    SetTimer(1, 1000, NULL);
}

void CTeamProjectDlg::StopTimer()
{
    // TODO: 여기에 구현 코드 추가.
    m_bIsTimerRunning = FALSE;
    KillTimer(1);
    m_bIsFocusing = FALSE;
}

bool CTeamProjectDlg::IsWindowExist() // 윈도우 창이 존재하는지
{
    // TODO: 여기에 구현 코드 추가.

    HWND hWnd = ::GetTopWindow(NULL);
    while (hWnd)
    {
        TCHAR title[256] = { 0 };
        ::GetWindowText(hWnd, title, 256);
        CString strTitle(title);

        if (strTitle.Find(_T(" - Chrome")) != -1)
            return true;

        hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
    }
    return false;
}

void CTeamProjectDlg::OnMenuAllowAppOpen()
{
    m_dlgAllowApp.ShowWindow(SW_SHOW);
}

void CTeamProjectDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (m_pRankDlg) {
        if (::IsWindow(m_pRankDlg->GetSafeHwnd()))
            m_pRankDlg->DestroyWindow();
        delete m_pRankDlg;
        m_pRankDlg = nullptr;
    }

    if (m_pAlarmDlg) {
        if (::IsWindow(m_pAlarmDlg->GetSafeHwnd()))
            m_pAlarmDlg->DestroyWindow();
        delete m_pAlarmDlg;
        m_pAlarmDlg = nullptr;
    }
}

void CTeamProjectDlg::SaveCurrentSessionToFile(const CString& path) {
    CFile file;
    if (file.Open(path, CFile::modeCreate | CFile::modeWrite)) {
        file.Write(&m_state, sizeof(m_state));
        file.Write(&m_focusMinutes, sizeof(m_focusMinutes));
        file.Write(&m_restMinutes, sizeof(m_restMinutes));
        file.Write(&m_longRestMinutes, sizeof(m_longRestMinutes));
        file.Write(&m_goalSessions, sizeof(m_goalSessions));
        file.Write(&m_completedSessions, sizeof(m_completedSessions));
        file.Write(&m_cycleCount, sizeof(m_cycleCount));
        file.Write(&m_remainingSeconds, sizeof(m_remainingSeconds));
        file.Write(&m_totalRankCycleCount, sizeof(m_totalRankCycleCount));
        file.Write(&m_isRunning, sizeof(m_isRunning));
        file.Write(&m_bIsGoalSet, sizeof(m_bIsGoalSet));
        file.Write(&m_bIsFocusing, sizeof(m_bIsFocusing));
        file.Write(&m_bIsTimerRunning, sizeof(m_bIsTimerRunning));
        file.Close();
        AfxMessageBox(_T("세션이 저장되었습니다."));
    }
}

bool CTeamProjectDlg::LoadSessionFromFile(const CString& path) {
    CFile file;
    if (file.Open(path, CFile::modeRead)) {
        file.Read(&m_state, sizeof(m_state));
        file.Read(&m_focusMinutes, sizeof(m_focusMinutes));
        file.Read(&m_restMinutes, sizeof(m_restMinutes));
        file.Read(&m_longRestMinutes, sizeof(m_longRestMinutes));
        file.Read(&m_goalSessions, sizeof(m_goalSessions));
        file.Read(&m_completedSessions, sizeof(m_completedSessions));
        file.Read(&m_cycleCount, sizeof(m_cycleCount));
        file.Read(&m_remainingSeconds, sizeof(m_remainingSeconds));
        file.Read(&m_totalRankCycleCount, sizeof(m_totalRankCycleCount));
        file.Read(&m_isRunning, sizeof(m_isRunning));
        file.Read(&m_bIsGoalSet, sizeof(m_bIsGoalSet));
        file.Read(&m_bIsFocusing, sizeof(m_bIsFocusing));
        file.Read(&m_bIsTimerRunning, sizeof(m_bIsTimerRunning));
        file.Close();
        return true;
    }
    return false;
}

void CTeamProjectDlg::OnClickedButtonResume()
{
    // 1. 세션 데이터 로드
    LoadSessionFromFile(_T("SessionSave.dat"));

    // 2. 상태 초기화
    m_isRunning = FALSE;

    // 3. UI 표시/숨김
    ShowTimerA(TRUE);
    ShowTimerB(FALSE);
    ShowTimerC(TRUE);

    // 4. 데이터 갱신
    UpdateData(FALSE);

    // 5. 화면 강제 갱신
    Invalidate();
    UpdateWindow();

    // 6. 추가로 파이차트 다시 그리기 (안전장치)
    DrawSessionPieChartOutline();
    DrawSessionPieChartC();

    // 7. 기타 UI 갱신
    DrawCircleClock();
    UpdateStateText(_T("⏸ 일시정지"));
    UpdateTimerText(m_remainingSeconds);

    // 8. 세션 요약 표시
    CString summary = GetSessionSummary();
    AfxMessageBox(summary, MB_OK | MB_ICONINFORMATION);
}


CString CTeamProjectDlg::GetSessionSummary()
{
    CString stateText;
    switch (m_state) {
    case STATE_READY:    stateText = _T("대기 중"); break;
    case STATE_FOCUS:    stateText = _T("집중 중"); break;
    case STATE_REST:     stateText = _T("짧은 휴식 중"); break;
    case STATE_LONGREST: stateText = _T("긴 휴식 중"); break;
    default:             stateText = _T("알 수 없음"); break;
    }

    CString summary;
    summary.Format(
        _T("▶ [직전 세션 정보]\r\n")
        _T("상태: %s\r\n")
        _T("완료 세션: %d / %d\r\n")
        _T("집중 시간: %d분\r\n")
        _T("휴식 시간: %d분\r\n")
        _T("긴 휴식 시간: %d분\r\n")
        _T("남은 시간: %02d:%02d\r\n")
        _T("현재 랭크: %s"),
        stateText,
        m_completedSessions, m_goalSessions,
        m_focusMinutes, m_restMinutes, m_longRestMinutes,
        m_remainingSeconds / 60, m_remainingSeconds % 60,
        GetCurrentRankString()
    );
    return summary;
}


void CTeamProjectDlg::SaveRankToFile(const CString& path)
{
    // TODO: 여기에 구현 코드 추가.
    CFile file;
    if (file.Open(path, CFile::modeCreate | CFile::modeWrite)) {
        file.Write(&m_focusTotal, sizeof(m_focusTotal)); // 누적 집중 시간 저장
        CString today = GetTodayString();
        file.Write((LPCTSTR)today, (today.GetLength() + 1) * sizeof(TCHAR)); // 날짜 문자열 저장
        file.Close();
    }
}

void CTeamProjectDlg::LoadRankFromFile(const CString& path) {
    CFile file;
    if (file.Open(path, CFile::modeRead)) {
        int temp = 0;
        CString savedDate;
        if (file.Read(&temp, sizeof(temp)) == sizeof(temp)) {
            m_focusTotal = temp;
            TCHAR dateBuf[16] = { 0 };
            file.Read(dateBuf, sizeof(dateBuf));
            savedDate = dateBuf;
        }
        file.Close();
        CString today = GetTodayString();
        if (savedDate != today) {
            // 날짜가 다르면 랭크 초기화
            m_focusTotal = 0;
            SaveRankToFile(path);
        }
        m_lastRankDate = today;
    }
    else {
        m_focusTotal = 0;
        m_lastRankDate = GetTodayString();
    }
}

CString CTeamProjectDlg::GetTodayString() {
    return CTime::GetCurrentTime().Format(_T("%Y-%m-%d"));
}

void CTeamProjectDlg::OnMenuStatistics()
{
    // 파일 선택 대화상자 추가
    CFileDialog fileDlg(TRUE, _T("csv"), NULL,
        OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST,
        _T("CSV Files (*.csv)|*.csv|All Files (*.*)|*.*||"));

    if (fileDlg.DoModal() == IDOK) {
        CString path = fileDlg.GetPathName();

        // 경로를 다이얼로그에 전달
        CStatisticsDialog dlg;
        dlg.m_strCsvPath = path;  // CStatisticsDialog.h 에 CString m_strCsvPath; 추가 필요
        dlg.DoModal();
    }
    // 사용자가 취소하면 아무것도 하지 않음
}
