// CAlarmDlg.cpp: 구현 파일
//

#include "pch.h"
#include "Clock.h"
#include "afxdialogex.h"
#include "CAlarmDlg.h"


// CAlarmDlg 대화 상자

IMPLEMENT_DYNAMIC(CAlarmDlg, CDialogEx)

CAlarmDlg::CAlarmDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_ALARM, pParent)
{

}

CAlarmDlg::~CAlarmDlg()
{
}

void CAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CAlarmDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog(); // 기본 동작 유지

    m_fontNewAlarmButton.CreatePointFont(100, _T("맑은 고딕"));
    m_fontDeleteAlarmButton.CreatePointFont(100, _T("맑은 고딕"));

    GetDlgItem(IDC_BUTTON_NEWALARM)->SetFont(&m_fontNewAlarmButton);
    GetDlgItem(IDC_BUTTON_DELETEALARM)->SetFont(&m_fontDeleteAlarmButton);

    return TRUE;
}


BEGIN_MESSAGE_MAP(CAlarmDlg, CDialogEx)
END_MESSAGE_MAP()


// CAlarmDlg 메시지 처리기
