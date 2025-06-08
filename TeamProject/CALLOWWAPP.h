#pragma once
#include "afxdialogex.h"

class CTeamProjectDlg;

// CALLOWAPP 대화 상자

class CALLOWAPP : public CDialogEx
{
	DECLARE_DYNAMIC(CALLOWAPP)

public:
	CALLOWAPP(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CALLOWAPP();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ALLOW_APP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonInput();
	CEdit m_editUrl;
	CListBox m_listUrl;
	//CString m_strAllowUrl;
	CStringArray m_arrAllowUrls;
	

	CTeamProjectDlg* m_pMainDlg = nullptr;

	afx_msg void OnClickedButtonDelete();
	afx_msg void OnClickedButtonOk();
};
