﻿
// ClockDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Clock.h"
#include "ClockDlg.h"
#include "afxdialogex.h"
#include "CAlarmDlg.h"
#include "CTimerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CClockDlg 대화 상자



CClockDlg::CClockDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CLOCK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClockDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()

	// 메뉴바 관련 메시지 맵
	ON_COMMAND(ID_MENU_CLOCK, &CClockDlg::OnMenuClock)
	ON_COMMAND(ID_MENU_TIMER, &CClockDlg::OnMenuTimer)
	ON_COMMAND(ID_MENU_ALARM, &CClockDlg::OnMenuAlarm)
	ON_COMMAND(ID_MENU_SETTINGS, &CClockDlg::OnMenuSettings)
	ON_COMMAND(ID_FILE_EXIT, &CClockDlg::OnMenuExit)
END_MESSAGE_MAP()

// CClockDlg 메시지 처리기

BOOL CClockDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	CMenu menu;
	menu.LoadMenu(IDR_MAIN_MENU);  // 메뉴 리소스 ID
	SetMenu(&menu);                // 이 다이얼로그에 메뉴 붙이기

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_brBackground.CreateSolidBrush(RGB(255, 255, 255));

	m_fontClockPresentTime.CreatePointFont(150, _T("돋움"));
	m_fontClockAmPm.CreatePointFont(200, _T("맑은 고딕"));
	m_fontClockTime.CreatePointFont(450, _T("맑은 고딕"));
	m_fontClockDate.CreatePointFont(200, _T("돋움"));

	GetDlgItem(IDC_STATIC_PRESENTTIME)->SetFont(&m_fontClockPresentTime);
	GetDlgItem(IDC_STATIC_AMPM)->SetFont(&m_fontClockAmPm);
	GetDlgItem(IDC_STATIC_TIME)->SetFont(&m_fontClockTime);
	GetDlgItem(IDC_STATIC_DAY)->SetFont(&m_fontClockDate);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CClockDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


HBRUSH CClockDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  여기서 DC의 특성을 변경합니다.

	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_brBackground;
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

void CClockDlg::OnMenuClock()
{
	// 시계 다이얼로그는 자신이니까 아무것도 안 해도 됨
	AfxMessageBox(_T("현재 시계 화면입니다."));
}

void CClockDlg::OnMenuTimer()
{
	CTimerDlg dlg;
	dlg.DoModal();  // 타이머 창 띄우기
}

void CClockDlg::OnMenuAlarm()
{
	CAlarmDlg dlg;
	dlg.DoModal();  // 알람 창 띄우기
}

void CClockDlg::OnMenuSettings()
{
	// 아직 Settings 다이얼로그 없으면 임시 메세지
	AfxMessageBox(_T("설정 창 준비 중입니다."));
}

void CClockDlg::OnMenuExit()
{
	OnOK();  // 프로그램 종료
}
