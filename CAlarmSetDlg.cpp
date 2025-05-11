// CAlarmSetDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Clock.h"
#include "afxdialogex.h"
#include "CAlarmSetDlg.h"


// CAlarmSetDlg 대화 상자

IMPLEMENT_DYNAMIC(CAlarmSetDlg, CDialogEx)

CAlarmSetDlg::CAlarmSetDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALARMSET, pParent)
{

}

CAlarmSetDlg::~CAlarmSetDlg()
{
}

void CAlarmSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAlarmSetDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog(); // 기본 동작 유지

	m_brBackground.CreateSolidBrush(RGB(255, 255, 255));

	m_fontHour.CreatePointFont(80, _T("돋움"));
	m_fontMinute.CreatePointFont(80, _T("돋움"));
	m_fontName.CreatePointFont(80, _T("돋움"));

	GetDlgItem(IDC_STATIC_HOUR)->SetFont(&m_fontHour);
	GetDlgItem(IDC_STATIC_MINUTE)->SetFont(&m_fontMinute);
	GetDlgItem(IDC_STATIC_NAME)->SetFont(&m_fontName);


    return TRUE;
}


BEGIN_MESSAGE_MAP(CAlarmSetDlg, CDialogEx)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAlarmSetDlg 메시지 처리기

HBRUSH CAlarmSetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_brBackground; // 다이얼로그 바탕색 변경
	}

	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetBkMode(TRANSPARENT);        // 배경 투명하게
		pDC->SetTextColor(RGB(0, 0, 0));     // 글씨 검정색
		return m_brBackground;              // 배경은 다이얼로그와 같음
	}

	// TODO:  기본값이 적당하지 않으면 다른 브러시를 반환합니다.
	return hbr;
}
