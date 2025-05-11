// CTimerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Clock.h"
#include "afxdialogex.h"
#include "CTimerDlg.h"


// CTimerDlg 대화 상자

IMPLEMENT_DYNAMIC(CTimerDlg, CDialogEx)

CTimerDlg::CTimerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_TIMER, pParent)
{

}

CTimerDlg::~CTimerDlg()
{
}

void CTimerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTimerDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CTimerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 여기서 배경색, 폰트 등 초기화 작업
	m_brBackGround.CreateSolidBrush(RGB(255, 255, 255));

	m_fontPresentTime.CreatePointFont(300, _T("맑은 고딕"));
	m_fontData.CreatePointFont(100, _T("맑은 고딕"));

	m_fontPauseButton.CreatePointFont(100, _T("돋움"));
	m_fontReStartButton.CreatePointFont(100, _T("돋움"));
	m_fontStopButton.CreatePointFont(100, _T("돋움"));
	m_fontDeleteButton.CreatePointFont(100, _T("돋움"));
	m_fontBackButton.CreatePointFont(100, _T("돋움"));
	m_fontPomodoroButton.CreatePointFont(100, _T("돋움"));
	m_fontDarkModeButton.CreatePointFont(100, _T("돋움"));
	m_fontPlusButton.CreatePointFont(100, _T("돋움"));

	GetDlgItem(IDC_STATIC_PRESENTTIME)->SetFont(&m_fontPresentTime);
	GetDlgItem(IDC_STATIC_DATA)->SetFont(&m_fontData);

	GetDlgItem(IDC_BUTTON_PAUSE)->SetFont(&m_fontPauseButton);
	GetDlgItem(IDC_BUTTON_RESTART)->SetFont(&m_fontReStartButton);
	GetDlgItem(IDC_BUTTON_STOP)->SetFont(&m_fontStopButton);
	GetDlgItem(IDC_BUTTON_DELETE)->SetFont(&m_fontDeleteButton);
	GetDlgItem(IDC_BUTTON_BACK)->SetFont(&m_fontBackButton);
	GetDlgItem(IDC_BUTTON_POMODORO)->SetFont(&m_fontPomodoroButton);
	GetDlgItem(IDC_BUTTON_DARK)->SetFont(&m_fontDarkModeButton);
	GetDlgItem(IDC_BUTTON_PLUSTIMER)->SetFont(&m_fontPlusButton);

	return TRUE; // 포커스를 다른 컨트롤에 설정하지 않으면 TRUE
}



// CTimerDlg 메시지 처리기

HBRUSH CTimerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_brBackGround;
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		UINT id = pWnd->GetDlgCtrlID();

		pDC->SetBkMode(TRANSPARENT);          // 배경 투명
		pDC->SetTextColor(RGB(0, 0, 0));       // 글씨 검정

		if (id == IDC_STATIC_DATA)
			pWnd->SetFont(&m_fontData);
		else
			pWnd->SetFont(&m_fontPresentTime);         // 폰트 적용
		return m_brBackGround;                 // 배경은 다이얼로그 배경 그대로
	}

	if (nCtlColor == CTLCOLOR_BTN)
	{
		pDC->SetBkMode(TRANSPARENT);          // 버튼 배경 투명하게
		pDC->SetTextColor(RGB(50, 50, 50));    // 버튼 글씨 색
		return m_brBackGround;
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
