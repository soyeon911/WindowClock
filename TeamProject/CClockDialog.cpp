// CClockDialog.cpp: 구현 파일
//

#include "pch.h"
#include "TeamProject.h"
#include "afxdialogex.h"

#include "CClockDialog.h"
#include "CRankDialog.h"        // 랭크 다이얼로그 : 추가
#include "PomodoroDialog.h"
#include "CAlarmDialog.h"  



// CClockDialog 대화 상자

IMPLEMENT_DYNAMIC(CClockDialog, CDialogEx)

CClockDialog::CClockDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CLOCK_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDI_CLOCK);
}

CClockDialog::~CClockDialog()
{
}

BOOL CClockDialog::PreTranslateMessage(MSG* pMsg) {
    if (m_hAccel && ::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
        return TRUE;

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CClockDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClockDialog, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_STN_CLICKED(IDC_STATIC_TIME, &CClockDialog::OnStnClickedStaticTime)
    //    ON_COMMAND(ID_MENU_FILE_SAVE, &CClockDialog::OnMenuFileSave)
    ON_COMMAND(ID_MENU_ALARM_OPEN, &CClockDialog::OnMenuAlarmOpen)
    ON_COMMAND(ID_MENU_POMODORO_OPEN, &CClockDialog::OnMenuPomodoroOpen)
    ON_WM_CTLCOLOR()
    ON_COMMAND(ID_MENU_TOGGLE_DARKMODE, &CClockDialog::OnMenuToggleDarkmode)
END_MESSAGE_MAP()


// 초기화
BOOL CClockDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_hAccel = ::LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR2));

    // 아이콘 설정
    SetIcon(m_hIcon, TRUE);   // 큰 아이콘
    SetIcon(m_hIcon, FALSE);  // 작은 아이콘

    m_brushDarkBg.CreateSolidBrush(RGB(30, 30, 30));      // 진한 회색
    m_brushLightBg.CreateSolidBrush(RGB(255, 255, 255));  // 기본 흰 배경
    m_brBackground.CreateSolidBrush(RGB(164, 120, 100)); // 배경색
    m_brCtrlBk.CreateSolidBrush(RGB(164, 120, 100)); // 컨트롤 배경

    // 시간 표시용 큰 폰트 생성 (시:분:초)
    m_timeFont.CreateFont(
        100,                        // 높이
        0,                         // 너비 (0이면 높이에 비례)
        0,                         // 회전각도
        0,                         // 기울기
        FW_BOLD,                   // 굵기
        FALSE,                     // 이탤릭
        FALSE,                     // 밑줄
        FALSE,                     // 취소선
        DEFAULT_CHARSET,           // 문자셋
        OUT_TT_PRECIS,            // 출력 정밀도
        CLIP_DEFAULT_PRECIS,       // 클리핑 정밀도
        ANTIALIASED_QUALITY,       // 출력 품질
        DEFAULT_PITCH | FF_MODERN, // 피치와 패밀리
        _T("Arial")            // 폰트명 (고정폭 폰트)
    );

    // AM/PM 표시용 폰트 생성
    m_ampmFont.CreateFont(
        40,                        // 높이
        0,                         // 너비
        0,                         // 회전각도
        0,                         // 기울기
        FW_NORMAL,                 // 굵기
        FALSE,                     // 이탤릭
        FALSE,                     // 밑줄
        FALSE,                     // 취소선
        DEFAULT_CHARSET,           // 문자셋
        OUT_TT_PRECIS,            // 출력 정밀도
        CLIP_DEFAULT_PRECIS,       // 클리핑 정밀도
        ANTIALIASED_QUALITY,       // 출력 품질
        DEFAULT_PITCH | FF_SWISS,  // 피치와 패밀리
        _T("Arial")               // 폰트명
    );

    // 날짜 표시용 폰트 생성
    m_dateFont.CreateFont(
        30,                        // 높이
        0,                         // 너비
        0,                         // 회전각도
        0,                         // 기울기
        FW_NORMAL,                 // 굵기
        FALSE,                     // 이탤릭
        FALSE,                     // 밑줄
        FALSE,                     // 취소선
        DEFAULT_CHARSET,           // 문자셋
        OUT_TT_PRECIS,            // 출력 정밀도
        CLIP_DEFAULT_PRECIS,       // 클리핑 정밀도
        ANTIALIASED_QUALITY,       // 출력 품질
        DEFAULT_PITCH | FF_SWISS,  // 피치와 패밀리
        _T("Arial")               // 폰트명
    );

    // 컨트롤에 폰트 적용
    if (GetDlgItem(IDC_STATIC_TIME))
        GetDlgItem(IDC_STATIC_TIME)->SetFont(&m_timeFont);

    if (GetDlgItem(IDC_STATIC_AMPM))
        GetDlgItem(IDC_STATIC_AMPM)->SetFont(&m_ampmFont);

    if (GetDlgItem(IDC_STATIC_DATE))
        GetDlgItem(IDC_STATIC_DATE)->SetFont(&m_dateFont);

    // 타이머 시작 (1초마다 호출)
    SetTimer(1, 1000, nullptr);

    // 초기 시간 표시
    UpdateClock();

    return TRUE;
}

// 페인트 이벤트
void CClockDialog::OnPaint()
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
        // 나머지 기본 그리기
        CDialogEx::OnPaint();
    }
}


// 드래그 아이콘
HCURSOR CClockDialog::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

// 타이머 이벤트
void CClockDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        UpdateClock();
    }
    CDialogEx::OnTimer(nIDEvent);
}

// 소멸 시 타이머 정리
void CClockDialog::OnDestroy()
{
    KillTimer(1);
    CDialogEx::OnDestroy();
}

// 시계 업데이트 함수
void CClockDialog::UpdateClock()
{
    // 현재 시간 획득
    CTime currentTime = CTime::GetCurrentTime();

    // 시간 구성요소 추출
    int hour = currentTime.GetHour();
    int minute = currentTime.GetMinute();
    int second = currentTime.GetSecond();

    // 12시간 형식으로 변환
    CString ampm;
    int displayHour = hour;

    if (hour == 0)
    {
        ampm = _T("오전");
        displayHour = 12;
    }
    else if (hour < 12)
    {
        ampm = _T("오전");
    }
    else if (hour == 12)
    {
        ampm = _T("오후");
    }
    else
    {
        ampm = _T("오후");
        displayHour = hour - 12;
    }

    // 시간 문자열 생성 (HH:MM:SS 형식)
    CString timeStr;
    timeStr.Format(_T("%02d:%02d:%02d"), displayHour, minute, second);

    // 날짜 정보 추출
    int year = currentTime.GetYear();
    int month = currentTime.GetMonth();
    int day = currentTime.GetDay();
    int dayOfWeek = currentTime.GetDayOfWeek(); // 1=일요일, 2=월요일, ..., 7=토요일

    // 날짜 문자열 생성
    CString koreanDayOfWeek = GetKoreanDayOfWeek(dayOfWeek);
    CString dateStr;
    dateStr.Format(_T("%d년 %d월 %d일 %s"), year, month, day, koreanDayOfWeek);

    // UI 컨트롤 업데이트
    SetDlgItemText(IDC_STATIC_AMPM, ampm);
    SetDlgItemText(IDC_STATIC_TIME, timeStr);
    SetDlgItemText(IDC_STATIC_DATE, dateStr);
}

// 한국어 요일 반환 함수
CString CClockDialog::GetKoreanDayOfWeek(int dayOfWeek)
{
    switch (dayOfWeek)
    {
    case 1: return _T("일요일");
    case 2: return _T("월요일");
    case 3: return _T("화요일");
    case 4: return _T("수요일");
    case 5: return _T("목요일");
    case 6: return _T("금요일");
    case 7: return _T("토요일");
    default: return _T("알 수 없음");
    }
}
void CClockDialog::OnStnClickedStaticTime()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

template<typename T>
void CClockDialog::SwitchToDialog(T*& pDlg)
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

void CClockDialog::OnMenuAlarmOpen()
{
    SwitchToDialog((CAlarmDialog*&)m_pAlarmDlg);
    // 알람 다이얼로그가 생성된 직후 다크모드 상태를 전달
    if (m_pAlarmDlg)
        ((CAlarmDialog*)m_pAlarmDlg)->SetDarkMode(m_isDarkMode);
}

void CClockDialog::OnMenuPomodoroOpen()
{
    // 1. 뽀모도로 다이얼로그 생성 (모달)
    if (m_pAlarmDlg != nullptr) {
        m_pAlarmDlg->DestroyWindow();
    }
    
    ShowWindow(SW_HIDE);
    CTeamProjectDlg m_pPomodoroDlg;
    m_pPomodoroDlg.m_isDarkMode = m_isDarkMode;
    m_pPomodoroDlg.DoModal(); // 새로운 모달 다이얼로그 실행

    CDialogEx::OnOK(); // 모달 다이얼로그 종료
}

HBRUSH CClockDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


void CClockDialog::OnMenuToggleDarkmode()
{
    m_isDarkMode = !m_isDarkMode;

    if (EnableDarkMode(m_hWnd, m_isDarkMode)) {
        Invalidate(TRUE); // 전체 다시 그리기
        AfxMessageBox(m_isDarkMode ? _T("다크모드 ON") : _T("다크모드 OFF"));
    }
    else {
        AfxMessageBox(_T("다크모드 적용 실패"));
    }
}


bool CClockDialog::EnableDarkMode(HWND hWnd, bool enable)
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
