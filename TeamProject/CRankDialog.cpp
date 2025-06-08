// CRankDialog.cpp: 구현 파일
//

#include "pch.h"
#include "TeamProject.h"
#include "afxdialogex.h"
#include "CRankDialog.h"


// CRankDialog 대화 상자

IMPLEMENT_DYNAMIC(CRankDialog, CDialogEx)

CRankDialog::CRankDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RANK_DIALOG, pParent)

    , silver(60)
    , gold(120)
    , platinum(240)
    , diamond(420)
    , master(660)
    , challenger(960)

    , m_focusMinutesRank(0)
    , m_cycleCountRank(0)
    , m_focusTotal(0)
    , m_strCurrRank(_T(""))
    , m_strNextRank(_T(""))
    , m_nTimerID(0)
    , m_strRankCriteria(_T(""))
{
    m_strRank[0] = _T("브론즈");
    m_strRank[1] = _T("실버");
    m_strRank[2] = _T("골드");
    m_strRank[3] = _T("플래티넘");
    m_strRank[4] = _T("다이아");
    m_strRank[5] = _T("마스터");
    m_strRank[6] = _T("챌린저");
}


CRankDialog::~CRankDialog()
{
}

void CRankDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_CURR_RANK, m_strCurrRank);
    DDX_Text(pDX, IDC_STATIC_NEXT_RANK, m_strNextRank);
    DDX_Text(pDX, IDC_STATIC_RANK_CRITERIA, m_strRankCriteria);
    DDX_Control(pDX, IDC_PIC_CURR_RANK, m_picCurrRank);
    DDX_Control(pDX, IDC_PIC_NEXT_RANK, m_picNextRank);
}


BEGIN_MESSAGE_MAP(CRankDialog, CDialogEx)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRankDialog 메시지 처리기


BOOL CRankDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    //m_hIcon = AfxGetApp()->LoadIcon(IDI_RANK);
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_brushDarkBg.CreateSolidBrush(RGB(30, 30, 30));         // 다크 모드 배경
    m_brushLightBg.CreateSolidBrush(RGB(255, 255, 255));     // 라이트 모드 배경
    m_brBackground.CreateSolidBrush(RGB(102, 122, 154));     // 팀프로젝트/시계와 동일
    m_brCtrlBk.CreateSolidBrush(RGB(102, 122, 154));           // 컨트롤 배경

    GetDlgItem(IDC_PIC_CURR_RANK)->GetWindowRect(m_rectCurrRank);
    ScreenToClient(m_rectCurrRank);

    GetDlgItem(IDC_PIC_NEXT_RANK)->GetWindowRect(m_rectNextRank); // 다음 랭크 이미지
    ScreenToClient(m_rectNextRank);

    m_strRankCriteria.Format(_T("브론즈 \t: 0시간~%d시간\n\n실버 \t: %d시간~%d시간\n\n골드 \t: %d시간~%d시간\n\n플래티넘 : %d시간~%d시간\n\n다이아 \t: %d시간~%d시간\n\n마스터 \t: %d시간~%d시간\n\n챌린저 \t: %d시간 이상"), silver/60, silver/60, gold/60, gold / 60, platinum/60, platinum / 60, diamond/60, diamond / 60, master/60, master / 60, challenger/60, challenger / 60);

    UpdateRank();

    // 1초마다 랭크 갱신 (실시간 변화 반영)
    m_nTimerID = SetTimer(1, 1000, NULL);

    return TRUE;
}

//void CRankDialog::SetFocusInfo(int focusMinutes, int cycleCount)
//{
//    m_focusMinutesRank = focusMinutes;
//    m_cycleCountRank = cycleCount;
//
//}

void CRankDialog::SetFocusTotal(int focusTotal)
{
    m_focusTotal = focusTotal;
}

void CRankDialog::UpdateRank()
{
    // 랭크 기준값은 기존과 동일
    int rankIdx = 0, nextRankIdx = 1;

    if (!m_imageCurrRank.IsNull()) {
        m_imageCurrRank.Destroy();
    }
    if (!m_imageNextRank.IsNull()) {
        m_imageNextRank.Destroy();
    }

    // 누적 집중 시간 기준으로 랭크 결정
    if (m_focusTotal >= challenger) {
        rankIdx = 6; nextRankIdx = 6;
        m_imageCurrRank.Load(L"챌린저.bmp");
        m_imageNextRank.Load(L"챌린저.bmp");
    }
    else if (m_focusTotal >= master) {
        rankIdx = 5; nextRankIdx = 6;
        m_imageCurrRank.Load(L"마스터.bmp");
        m_imageNextRank.Load(L"챌린저.bmp");
    }
    else if (m_focusTotal >= diamond) {
        rankIdx = 4; nextRankIdx = 5;
        m_imageCurrRank.Load(L"다이아.bmp");
        m_imageNextRank.Load(L"마스터.bmp");
    }
    else if (m_focusTotal >= platinum) {
        rankIdx = 3; nextRankIdx = 4;
        m_imageCurrRank.Load(L"플래티넘.bmp");
        m_imageNextRank.Load(L"다이아.bmp");
    }
    else if (m_focusTotal >= gold) {
        rankIdx = 2; nextRankIdx = 3;
        m_imageCurrRank.Load(L"골드.bmp");
        m_imageNextRank.Load(L"플래티넘.bmp");
    }
    else if (m_focusTotal >= silver) {
        rankIdx = 1; nextRankIdx = 2;
        m_imageCurrRank.Load(L"실버.bmp");
        m_imageNextRank.Load(L"골드.bmp");
    }
    else {
        rankIdx = 0; nextRankIdx = 1;
        m_imageCurrRank.Load(L"브론즈.bmp");
        m_imageNextRank.Load(L"실버.bmp");
    }

    InvalidateRect(m_rectCurrRank, FALSE); // 이미지 변경 시 항상 화면 갱신
    InvalidateRect(m_rectNextRank, FALSE);

    m_strCurrRank = m_strRank[rankIdx];
    if (rankIdx < 6) {
        int thresholds[7] = { 0, silver, gold, platinum, diamond, master, challenger };
        int remain = thresholds[nextRankIdx] - m_focusTotal;
        int hour = remain / 60;
        int min = remain % 60;

        if (min < 0)
            min = 0;

        if (hour == 0)
            m_strNextRank.Format(_T("%s까지 %d분 남음"), m_strRank[nextRankIdx], min);
        else if (min == 0)
            m_strNextRank.Format(_T("%s까지 %d시간 남음"), m_strRank[nextRankIdx], hour);
        else
            m_strNextRank.Format(_T("%s까지 %d시간 %d분 남음"), m_strRank[nextRankIdx], hour, min);
    }
    else {
        m_strNextRank = _T("최고 랭크입니다!");
    }
    UpdateData(FALSE);
}


void CRankDialog::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    if (nIDEvent == 1) {
        // 집중 시간(m_focusTotal)이 외부에서 변경될 수 있다면, 여기서 최신값을 다시 계산하거나 전달받아야 함
        // 예시: m_focusTotal = m_focusMinutes * m_completedSessions;

        UpdateRank();
    }

    CDialogEx::OnTimer(nIDEvent);
}



void CRankDialog::OnDestroy()
{
    CDialogEx::OnDestroy();

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    if (m_nTimerID != 0)
    {
        KillTimer(m_nTimerID);
        m_nTimerID = 0;
    }

}


void CRankDialog::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO: 여기에 메시지 처리기 코드를 추가합니다.
    if (IsIconic())
    {
        SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);

        CRect rect;
        GetClientRect(&rect);
        
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else {
        if (!m_imageCurrRank.IsNull()) { // 그림이 로딩되었는지 체크한다.
            // 이미지가 원본 크기와 다르게 출력될때 어떤 방식으로 이미지를
            // 확대하거나 축소할 것인지를 결정한다.
            dc.SetStretchBltMode(COLORONCOLOR);
            // 그림을 Picture Control 크기로 화면에 출력한다.
            m_imageCurrRank.Draw(dc, m_rectCurrRank);
        }
        if (!m_imageNextRank.IsNull()) { // 추가: 다음 랭크 이미지 출력
            dc.SetStretchBltMode(COLORONCOLOR);
            m_imageNextRank.Draw(dc, m_rectNextRank);
        }
    }
    // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
}


HBRUSH CRankDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    if (m_isDarkMode) {
        switch (nCtlColor) {
        case CTLCOLOR_DLG:
            return (HBRUSH)m_brushDarkBg;
        case CTLCOLOR_STATIC:
            pDC->SetBkMode(TRANSPARENT);
            pDC->SetTextColor(RGB(255, 255, 255));
            return (HBRUSH)m_brushDarkBg;
        case CTLCOLOR_BTN:
        case CTLCOLOR_EDIT:
        case CTLCOLOR_LISTBOX:
            pDC->SetBkColor(RGB(50, 50, 50));
            pDC->SetTextColor(RGB(255, 255, 255));
            return (HBRUSH)m_brushDarkBg;
        }
    }
    else {
        if (nCtlColor == CTLCOLOR_DLG) {
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->SetBkMode(TRANSPARENT);
            return m_brBackground;
        }
        if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_BTN) {
            pDC->SetBkColor(RGB(165, 80, 77));
            pDC->SetTextColor(RGB(255, 255, 255));
            pDC->SetBkMode(TRANSPARENT);
            return m_brCtrlBk;
        }
    }
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CRankDialog::SetDarkMode(bool enable)
{
    m_isDarkMode = enable;
    // 타이틀바 다크모드 적용
    BOOL value = enable ? TRUE : FALSE;
    ::DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
    Invalidate(TRUE);
}