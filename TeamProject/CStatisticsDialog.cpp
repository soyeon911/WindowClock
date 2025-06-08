// CStatisticsDialog.cpp

#include "pch.h"
#include "CStatisticsDialog.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <commdlg.h>
#include <map>



IMPLEMENT_DYNAMIC(CStatisticsDialog, CDialogEx)

CStatisticsDialog::CStatisticsDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_STATISTICS_DIALOG, pParent)

    // ��ũ ���ذ� �ʱ�ȭ (CRankDialog�� ����) 
    , silver(60) // Ensure this is part of the initializer list  
    , gold(120)
    , platinum(240)
    , diamond(420)
    , master(660)
    , challenger(960)
{

    // ��ũ �̸� �ʱ�ȭ
    m_strRank[0] = _T("�����");
    m_strRank[1] = _T("�ǹ�");
    m_strRank[2] = _T("���");
    m_strRank[3] = _T("�÷�Ƽ��");
    m_strRank[4] = _T("���̾�");
    m_strRank[5] = _T("������");
    m_strRank[6] = _T("ç����");
}

CStatisticsDialog::~CStatisticsDialog()
{
    // ���� �ڵ�
}

BEGIN_MESSAGE_MAP(CStatisticsDialog, CDialogEx)
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_LOAD_CSV_BUTTON, &CStatisticsDialog::OnLoadCSVButton)
END_MESSAGE_MAP()

void CStatisticsDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BOOL CStatisticsDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // �⺻ CSV ���� �ڵ� �ε� �õ�
    CString defaultPath = GetDefaultCSVPath();
    if (GetFileAttributes(defaultPath) != INVALID_FILE_ATTRIBUTES) {
        LoadCSVAndAnalyze(defaultPath);
    }

    if (!m_strCsvPath.IsEmpty()) {
        LoadCSVAndAnalyze(m_strCsvPath);
    }

    return TRUE;
}

void CStatisticsDialog::OnPaint()
{
    if (!IsWindow(GetSafeHwnd()) || m_dateList.empty() || m_rankList.empty()) {
        return;
    }

    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    DrawRankGraph(&dc, rect);
}


void CStatisticsDialog::OnLoadCSVButton()
{
    CString filePath = SelectCSVFile();
    if (!filePath.IsEmpty()) {
        LoadCSVAndAnalyze(filePath);
    }
}

void CStatisticsDialog::LoadCSVAndAnalyze(const CString& path)
{
    m_dateList.clear();
    m_rankList.clear();

    if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES) {
        CString msg;
        msg.Format(_T("������ ã�� �� �����ϴ�: %s"), path);
        AfxMessageBox(msg);
        return;
    }

    CFile file;
    CFileException ex;
    if (!file.Open(path, CFile::modeRead | CFile::typeBinary, &ex)) {
        CString msg;
        msg.Format(_T("CSV ������ �� �� �����ϴ�.\n���� �ڵ�: %d\n����: %s"),
            ex.m_cause, path);
        AfxMessageBox(msg);
        return;
    }

    // ���� ��ü �б�
    ULONGLONG fileLen = file.GetLength();
    if (fileLen == 0) {
        AfxMessageBox(_T("������ ��� �ֽ��ϴ�."));
        return;
    }
    CArray<BYTE, BYTE> buffer;
    buffer.SetSize((INT_PTR)fileLen);
    file.Read(buffer.GetData(), (UINT)fileLen);
    file.Close();

    // UTF-8 BOM ����
    INT_PTR start = 0;
    if (fileLen >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) {
        start = 3;
    }

    // UTF-8 �� UTF-16 ��ȯ
    CString content;
    int wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(buffer.GetData() + start), (int)(fileLen - start), NULL, 0);
    if (wlen > 0) {
        wchar_t* wbuf = new wchar_t[wlen + 1];
        MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)(buffer.GetData() + start), (int)(fileLen - start), wbuf, wlen);
        wbuf[wlen] = 0;
        content = wbuf;
        delete[] wbuf;
    }
    else {
        AfxMessageBox(_T("���� ���ڵ� ��ȯ ����"));
        return;
    }

    // �� �پ� �и�
    CStringArray lines;
    int cur = 0;
    while (cur < content.GetLength()) {
        int next = content.Find(_T('\n'), cur);
        CString line;
        if (next == -1) {
            line = content.Mid(cur);
            cur = content.GetLength();
        }
        else {
            line = content.Mid(cur, next - cur);
            cur = next + 1;
        }
        if (!line.IsEmpty() && (line.Right(1) == _T("\r") || line.Right(1) == _T("\n")))
            line.TrimRight(_T("\r\n"));

        if (!line.IsEmpty())
            lines.Add(line);
    }

    // ��� �Ľ� �� �÷� �ε��� ã��
    bool first = true;
    int dateIndex = -1;
    int completedIndex = -1;
    int focusIndex = -1;

    // ��¥�� ���߽ð� �հ踦 ������ ��
    std::map<CString, int> dailyFocusMap;

    for (int i = 0; i < lines.GetSize(); ++i) {
        CString line = lines[i];

        if (first) {
            CStringArray headers;
            ParseCSVLine(line, headers);

            // ù ��° ������� BOM ����
            if (headers.GetSize() > 0) {
                CString& firstHeader = headers[0];
                if (!firstHeader.IsEmpty() && (unsigned short)firstHeader[0] == 0xFEFF) {
                    firstHeader = firstHeader.Mid(1);
                }
                // �Ǵ� UTF-8 BOM(??��) ����
                if (firstHeader.Left(3) == _T("\xEF\xBB\xBF")) {
                    firstHeader = firstHeader.Mid(3);
                }
            }

            // CSV���Ͽ��� Date, Completed, Focus �÷� ã��
            for (int j = 0; j < headers.GetSize(); ++j) {
                CString header = headers[j].Trim();
                header.Remove(_T('"'));
                header.MakeUpper(); // ��ҹ��� ���� ����

                if (header == _T("DATE")) {
                    dateIndex = j;
                }
                else if (header == _T("COMPLETED")) {
                    completedIndex = j;
                }
                else if (header == _T("FOCUS")) {
                    focusIndex = j;
                }
            }

            if (dateIndex == -1 || completedIndex == -1 || focusIndex == -1) {
                AfxMessageBox(_T("CSV ���Ͽ� Date, Completed, Focus �÷��� �����ϴ�."));
                OnCancel();
                return;
            }

            first = false;
            continue;
        }

        CStringArray arr;
        ParseCSVLine(line, arr);

        if (arr.GetSize() <= max(max(dateIndex, completedIndex), focusIndex)) {
            continue; // �ʿ��Ѹ�ŭ �÷��� ������ �ǳʶٱ�
        }

        CString date = arr[dateIndex].Trim();
        date.Remove(_T('"'));
        if (date.Find(_T("#")) != -1 || date.IsEmpty()) {
            continue; // �ּ��̳� �� ��¥�� ����
        }

        CString completedStr = arr[completedIndex].Trim();
        completedStr.Remove(_T('"'));
        int completed = _ttoi(completedStr);

        CString focusStr = arr[focusIndex].Trim();
        focusStr.Remove(_T('"'));
        int focus = _ttoi(focusStr);

        if (completed <= 0 || focus <= 0) {
            continue; // ��ȿ���� ���� ���� �ǳʶٱ�
        }

        // �����߽ð� = completed * Focus
        int totalFocusTime = completed * focus;

        // ���� ��¥�� ���߽ð��� �ջ�
        if (dailyFocusMap.find(date) != dailyFocusMap.end()) {
            dailyFocusMap[date] += totalFocusTime;
        }
        else {
            dailyFocusMap[date] = totalFocusTime;
        }
    }

    // �ʿ��� �����͸� (m_dateList, m_rankList)�� �����ϰ� ��ũ ���
    for (auto& pair : dailyFocusMap) {
        CString date = pair.first;
        int totalFocus = pair.second;

        // ���߽ð��� ������� ��ũ ���
        int rankValue = CalculateRank(totalFocus);

        m_dateList.push_back(date);
        m_rankList.push_back(rankValue);
    }

    //CSV Į������ �����͸� ���� ������ ��
    if (m_dateList.empty()) {
        AfxMessageBox(_T("��ȿ�� �����Ͱ� �����ϴ�.\n���: Date, Completed, Focus �÷��� �ִ��� Ȯ�����ּ���."));
        OnCancel();
        return;
    }
    else {
        CString msg;
        msg.Format(_T("%d���� ��¥�� ����� �ε��߽��ϴ�."), (int)m_dateList.size());
        AfxMessageBox(msg);
    }

    InvalidateRect(NULL);
}

//��ũ ���
int CStatisticsDialog::CalculateRank(int focusMinutes)
{
    if (focusMinutes >= challenger) return 6;      // ç����
    else if (focusMinutes >= master) return 5;     // ������
    else if (focusMinutes >= diamond) return 4;    // ���̾�
    else if (focusMinutes >= platinum) return 3;   // �÷�Ƽ��
    else if (focusMinutes >= gold) return 2;       // ���
    else if (focusMinutes >= silver) return 1;     // �ǹ�
    else return 0;                                 // �����
}
//����ڰ� CSV ������ ������ �� �ֵ��� ���� ���� ��ȭ���ڸ� ����, ���õ� ������ ��θ� ��ȯ
CString CStatisticsDialog::SelectCSVFile()
{
    TCHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = this->GetSafeHwnd();
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = _T("CSV Files\0*.csv\0All Files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileName(&ofn)) {
        return CString(szFile);
    }

    return _T("");
}
//�⺻ CSV���� �ҷ����� ��� ���� (download ����)
CString CStatisticsDialog::GetDefaultCSVPath()
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    CString appPath = szPath;
    int pos = appPath.ReverseFind(_T('\\'));
    if (pos != -1) {
        appPath = appPath.Left(pos + 1);
    }

    // ����� �ٿ�ε� ������ Ȯ��
    TCHAR downloadsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, downloadsPath))) {
        CString downloads = downloadsPath;
        downloads += _T("\\Downloads\\PomodoroStatus.csv");
        if (GetFileAttributes(downloads) != INVALID_FILE_ATTRIBUTES) {
            return downloads;
        }
    }

    // �⺻��
    return appPath + _T("PomodoroStatus.csv");
}

//�� ���� CSV ���ڿ��� �޾Ƽ�, �� �ʵ带 �и��� CStringArray�� ������
void CStatisticsDialog::ParseCSVLine(const CString& line, CStringArray& tokens)
{
    tokens.RemoveAll();

    bool inQuotes = false;
    CString currentToken;

    for (int i = 0; i < line.GetLength(); ++i) {
        TCHAR ch = line[i];

        if (ch == _T('"')) {
            inQuotes = !inQuotes;
        }
        else if (ch == _T(',') && !inQuotes) {
            tokens.Add(currentToken.Trim());
            currentToken.Empty();
        }
        else {
            currentToken += ch;
        }
    }

    tokens.Add(currentToken.Trim());
}

//�����͸� �ҷ��ͼ� �׸� �׸���, x���� ��¥, y���� ��ũ
void CStatisticsDialog::DrawRankGraph(CDC* pDC, CRect rect)
{
    int n = (int)m_dateList.size();
    if (n == 0 || m_rankList.size() != n) {
        CString msg = _T("�����Ͱ� �����ϴ�. CSV ������ �ε����ּ���.");
        pDC->TextOut(rect.left + 50, rect.top + 50, msg);
        return;
    }

    // ��� �����
    CBrush bgBrush(RGB(255, 255, 255));
    pDC->FillRect(rect, &bgBrush);

    CPen axisPen(PS_SOLID, 2, RGB(0, 0, 0));
    CPen linePen(PS_SOLID, 3, RGB(0, 128, 255));
    CPen gridPen(PS_DOT, 1, RGB(200, 200, 200));
    CPen* pOldPen = pDC->SelectObject(&axisPen);

    int marginLeft = 100;
    int marginRight = 30;
    int marginTop = 80;
    int marginBottom = 100;

    // ���� �׸��� (��Ʈ ũ�� ����)
    CFont titleFont;
    titleFont.CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    CFont* pOldFont = pDC->SelectObject(&titleFont);

    CString title = _T("��¥�� �Ǹ𵵷� ��ũ ��ȭ");
    CSize titleSize = pDC->GetTextExtent(title);
    pDC->TextOut(rect.left + (rect.Width() - titleSize.cx) / 2, rect.top + 15, title);
    pDC->SelectObject(pOldFont);

    // �� �׸���
    pDC->MoveTo(rect.left + marginLeft, rect.bottom - marginBottom);
    pDC->LineTo(rect.right - marginRight, rect.bottom - marginBottom); // X��
    pDC->MoveTo(rect.left + marginLeft, rect.bottom - marginBottom);
    pDC->LineTo(rect.left + marginLeft, rect.top + marginTop);     // Y��

    int graphW = rect.Width() - marginLeft - marginRight;
    int graphH = rect.Height() - marginTop - marginBottom;

    // Y�� ���� �� �� (��ũ��) - ��Ʈ ũ�� ����
    pDC->SelectObject(&gridPen);
    CFont mediumFont;
    mediumFont.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    pDC->SelectObject(&mediumFont);

    for (int i = 0; i <= 6; ++i) {
        int y = rect.bottom - marginBottom - (graphH * i) / 6;

        // ���ڼ�
        pDC->MoveTo(rect.left + marginLeft, y);
        pDC->LineTo(rect.right - marginRight, y);

        // Y�� ��
        CString rankLabel = m_strRank[i];
        CSize labelSize = pDC->GetTextExtent(rankLabel);
        pDC->SelectObject(&axisPen);
        pDC->TextOut(rect.left + marginLeft - labelSize.cx - 15, y - 8, rankLabel);

        // ���� ǥ��
        pDC->MoveTo(rect.left + marginLeft - 5, y);
        pDC->LineTo(rect.left + marginLeft + 5, y);
        pDC->SelectObject(&gridPen);
    }

    // ������ �׸���
    pDC->SelectObject(&linePen);
    CPoint* points = new CPoint[n];

    for (int i = 0; i < n; ++i) {
        int x = rect.left + marginLeft + (graphW * i) / max(1, n - 1);
        int rankValue = m_rankList[i];
        int y = rect.bottom - marginBottom - (graphH * rankValue) / 6;

        y = max(rect.top + marginTop, min(rect.bottom - marginBottom, y));
        points[i] = CPoint(x, y);
    }

    if (n > 1) {
        pDC->Polyline(points, n);
    }

    CBrush pointBrush(RGB(255, 100, 0));
    CBrush* pOldBrush = pDC->SelectObject(&pointBrush);

    // ������ ����Ʈ �׸��� (ũ�� ����)
    for (int i = 0; i < n; ++i) {
        CRect pointRect(points[i].x - 6, points[i].y - 6,
            points[i].x + 6, points[i].y + 6);
        pDC->Ellipse(&pointRect);

        // X�� ��¥ �� (��Ʈ ũ�� ����)
        int labelInterval = max(1, n / 8);
        if (i % labelInterval == 0 || i == n - 1) {
            CString dateLabel = m_dateList[i];
            if (dateLabel.GetLength() > 10) {
                dateLabel = dateLabel.Left(8) + _T("..");
            }

            CSize textSize = pDC->GetTextExtent(dateLabel);
            pDC->TextOut(points[i].x - textSize.cx / 2,
                rect.bottom - marginBottom + 8, dateLabel);
        }

        // ��ũ �� ǥ�� (��Ʈ ũ�� ����)
        if (n <= 15 || i % max(1, n / 10) == 0) {
            CString rankStr = m_strRank[m_rankList[i]];
            CSize rankSize = pDC->GetTextExtent(rankStr);
            pDC->TextOut(points[i].x - rankSize.cx / 2, points[i].y - 25, rankStr);
        }
    }

    // �� �� ��Ʈ ũ�� ����
    CFont axisFont;
    axisFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    pDC->SelectObject(&axisFont);

    CString yLabel = _T("��ũ");
    CSize yLabelSize = pDC->GetTextExtent(yLabel);
    pDC->TextOut(rect.left + 5, rect.top + marginTop + graphH / 2 - yLabelSize.cy / 2, yLabel);

    CString xLabel = _T("��¥");
    CSize xLabelSize = pDC->GetTextExtent(xLabel);
    pDC->TextOut(rect.left + marginLeft + graphW / 2 - xLabelSize.cx / 2,
        rect.bottom - 20, xLabel);

    // ��� ���� (��Ʈ ũ�� ����)
    if (n > 0) {
        double avgRank = 0;
        for (int i = 0; i < n; ++i) {
            avgRank += m_rankList[i];
        }
        avgRank /= n;

        int maxRank = 0;
        for (int i = 0; i < n; ++i) {
            if (m_rankList[i] > maxRank) {
                maxRank = m_rankList[i];
            }
        }

        CString statsStr;
        statsStr.Format(_T("�� %d�� | ��� ��ũ: %s | �ְ� ��ũ: %s"),
            n, m_strRank[(int)avgRank], m_strRank[maxRank]);
        pDC->TextOut(rect.left + marginLeft, rect.bottom - marginBottom + 45, statsStr);
    }

    pDC->SelectObject(pOldFont);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);

    delete[] points;
}