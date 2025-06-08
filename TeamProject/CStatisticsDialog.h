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

    // ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_STATISTICS_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    CImage m_rankImages[7];  // ����� ~ ç����

    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnPaint();
    afx_msg void OnLoadCSVButton();

    // ������ ���
    std::vector<CString> m_dateList;        // ��¥ ���
    std::vector<int> m_rankList;            // ��ũ ��� (0-6 �ε���)

    // ��ũ �ý��� (CRankDialog�� ����)
    int silver;
    int gold;
    int platinum;
    int diamond;
    int master;
    int challenger;

    CString m_strRank[7];                   // ��ũ �̸� �迭
    CString m_strCsvPath;

    // �Լ���
    void LoadCSVAndAnalyze(const CString& path);
    CString SelectCSVFile();
    CString GetDefaultCSVPath();
    void ParseCSVLine(const CString& line, CStringArray& tokens);
    void DrawRankGraph(CDC* pDC, CRect rect);
    int CalculateRank(int focusMinutes);    // ���߽ð����� ��ũ ���
};