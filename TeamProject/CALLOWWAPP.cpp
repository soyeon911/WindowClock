// CALLOWAPP.cpp: 구현 파일
//

#include "pch.h"
#include "TeamProject.h"
#include "afxdialogex.h"
#include "CALLOWWAPP.h"
#include "TeamProjectDlg.h"


// CALLOWAPP 대화 상자

IMPLEMENT_DYNAMIC(CALLOWAPP, CDialogEx)

CALLOWAPP::CALLOWAPP(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG_ALLOW_APP, pParent)
{

}

CALLOWAPP::~CALLOWAPP()
{
}

void CALLOWAPP::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
    DDX_Control(pDX, IDC_LIST_URL, m_listUrl);
}


BEGIN_MESSAGE_MAP(CALLOWAPP, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_INPUT, &CALLOWAPP::OnClickedButtonInput)
    ON_BN_CLICKED(IDC_BUTTON_DELETE, &CALLOWAPP::OnClickedButtonDelete)
    ON_BN_CLICKED(IDC_BUTTON_OK, &CALLOWAPP::OnClickedButtonOk)
END_MESSAGE_MAP()


// CALLOWAPP 메시지 처리기

void CALLOWAPP::OnClickedButtonInput()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString strUrl;
    m_editUrl.GetWindowText(strUrl);

    if (strUrl.IsEmpty()) {
        AfxMessageBox(_T("URL을 입력하세요."));
        return;
    }
    // 중복 검사
    for (int i = 0; i < m_arrAllowUrls.GetSize(); ++i)
    {
        if (m_arrAllowUrls[i].Compare(strUrl) == 0)
        {
            AfxMessageBox(_T("이미 등록된 URL입니다."));
            return;
        }
    }

    // 배열에 추가
    m_arrAllowUrls.Add(strUrl);

    // 리스트박스 업데이트
    m_listUrl.AddString(strUrl);
    m_editUrl.SetWindowText(_T("")); // 입력창 비우기
}

void CALLOWAPP::OnClickedButtonDelete()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

    int sel = m_listUrl.GetCurSel();
    if (sel == LB_ERR)
    {
        AfxMessageBox(_T("삭제할 URL을 선택하세요."));
        return;
    }

    CString strToDelete;
    m_listUrl.GetText(sel, strToDelete);

    // 배열에서 삭제
    for (int i = 0; i < m_arrAllowUrls.GetSize(); ++i)
    {
        if (m_arrAllowUrls[i] == strToDelete)
        {
            m_arrAllowUrls.RemoveAt(i);
            break;
        }
    }

    // 리스트박스에서도 삭제
    m_listUrl.DeleteString(sel);

    AfxMessageBox(_T("선택한 URL이 삭제되었습니다."));
}

void CALLOWAPP::OnClickedButtonOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (!m_pMainDlg)
    {
        AfxMessageBox(_T("m_pMainDlg가 NULL입니다!"));
        return;
    }

    // 🚀 메인 다이얼로그의 URL 배열에 복사
    m_pMainDlg->m_arrAllowUrls.RemoveAll();
    for (int i = 0; i < m_arrAllowUrls.GetSize(); ++i)
    {
        m_pMainDlg->m_arrAllowUrls.Add(m_arrAllowUrls[i]);
    }

    AfxMessageBox(_T("허용된 URL 목록이 저장되었습니다."));

    ShowWindow(SW_HIDE);
}
