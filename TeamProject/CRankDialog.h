#pragma once
#include "afxdialogex.h"
#include "TeamProjectDlg.h"

// CRankDialog 대화 상자

class CRankDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CRankDialog)

public:
	CRankDialog(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CRankDialog();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RANK_DIALOG};
#endif

	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();
	

	DECLARE_MESSAGE_MAP()
public:
	int m_focusMinutesRank;		// 한 세션 집중 시간(분)
	int m_cycleCountRank;		// 완료된 집중 세션 수
	int m_focusTotal;           // 총 포커스 시간(분)

	int silver;
	int gold;
	int platinum;
	int diamond;
	int master;
	int challenger;

	CString m_strRank[7];       // {"브론즈", "실버", "골드", "플래티넘" "다이아", "마스터", "챌린저"}
	CString m_strCurrRank;      // 현재 랭크 
	CString m_strNextRank;      // 다음 랭크
	CString m_strRankCriteria;	// 랭크 기준 설명 static text
	UINT_PTR m_nTimerID;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	
	void UpdateRank();
	//void SetFocusInfo(int focusMinutes, int cycleCount);
	void SetFocusTotal(int focusTotal);

	HICON m_hIcon;

	CStatic m_picCurrRank;
	CStatic m_picNextRank;  // 추가: 다음 랭크 이미지 컨트롤

	CRect m_rectCurrRank;   // 현재 랭크 Picture Control 위치
	CRect m_rectNextRank;   // 추가: 다음 랭크 Picture Control 위치
	
	CImage m_imageCurrRank; // 현재 랭크 이미지
	CImage m_imageNextRank; // 추가: 다음 랭크 이미지
	
	afx_msg void OnPaint();

	bool m_isDarkMode = false;        // 다크 모드 상태
	CBrush m_brushDarkBg;      // 다이얼로그 배경
	CBrush m_brushLightBg;     // 일반 모드 배경 (필요시)
	CBrush m_brBackground;
	CBrush m_brCtrlBk; // 컨트롤
	void SetDarkMode(bool enable);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
