// CStatisticsDialog.h
#pragma once
#include "afxdialogex.h"
#include <vector>
#include <map>

class CStatisticsDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CStatisticsDialog)

public:
    CStatisticsDialog(CWnd* pParent = nullptr);
    virtual ~CStatisticsDialog();

    // 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_STATISTICS_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    CImage m_rankImages[7];  // 브론즈 ~ 챌린저

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    afx_msg void OnLoadCSVButton();

    // 데이터 멤버
    std::vector<CString> m_dateList;        // 날짜 목록
    std::vector<int> m_rankList;            // 랭크 목록 (0-6 인덱스)

    // 랭크 시스템 (CRankDialog와 동일)
    int silver;
    int gold;
    int platinum;
    int diamond;
    int master;
    int challenger;

    CString m_strRank[7];                   // 랭크 이름 배열
    CString m_strCsvPath;

    // 함수들
    void LoadCSVAndAnalyze(const CString& path);
    CString SelectCSVFile();
    CString GetDefaultCSVPath();
    void ParseCSVLine(const CString& line, CStringArray& tokens);
    void DrawRankGraph(CDC* pDC, CRect rect);
    int CalculateRank(int focusMinutes);    // 집중시간으로 랭크 계산
};