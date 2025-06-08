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

    // 랭크 기준값 초기화 (CRankDialog와 동일) 
    , silver(60) // Ensure this is part of the initializer list  
    , gold(120)
    , platinum(240)
    , diamond(420)
    , master(660)
    , challenger(960)
{

    // 랭크 이름 초기화
    m_strRank[0] = _T("브론즈");
    m_strRank[1] = _T("실버");
    m_strRank[2] = _T("골드");
    m_strRank[3] = _T("플래티넘");
    m_strRank[4] = _T("다이아");
    m_strRank[5] = _T("마스터");
    m_strRank[6] = _T("챌린저");
}

CStatisticsDialog::~CStatisticsDialog()
{
    // 정리 코드
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

    // 기본 CSV 파일 자동 로드 시도
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
        msg.Format(_T("파일을 찾을 수 없습니다: %s"), path);
        AfxMessageBox(msg);
        return;
    }

    CFile file;
    CFileException ex;
    if (!file.Open(path, CFile::modeRead | CFile::typeBinary, &ex)) {
        CString msg;
        msg.Format(_T("CSV 파일을 열 수 없습니다.\n오류 코드: %d\n파일: %s"),
            ex.m_cause, path);
        AfxMessageBox(msg);
        return;
    }

    // 파일 전체 읽기
    ULONGLONG fileLen = file.GetLength();
    if (fileLen == 0) {
        AfxMessageBox(_T("파일이 비어 있습니다."));
        return;
    }
    CArray<BYTE, BYTE> buffer;
    buffer.SetSize((INT_PTR)fileLen);
    file.Read(buffer.GetData(), (UINT)fileLen);
    file.Close();

    // UTF-8 BOM 제거
    INT_PTR start = 0;
    if (fileLen >= 3 && buffer[0] == 0xEF && buffer[1] == 0xBB && buffer[2] == 0xBF) {
        start = 3;
    }

    // UTF-8 → UTF-16 변환
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
        AfxMessageBox(_T("파일 인코딩 변환 실패"));
        return;
    }

    // 한 줄씩 분리
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

    // 헤더 파싱 및 컬럼 인덱스 찾기
    bool first = true;
    int dateIndex = -1;
    int completedIndex = -1;
    int focusIndex = -1;

    // 날짜별 집중시간 합계를 저장할 맵
    std::map<CString, int> dailyFocusMap;

    for (int i = 0; i < lines.GetSize(); ++i) {
        CString line = lines[i];

        if (first) {
            CStringArray headers;
            ParseCSVLine(line, headers);

            // 첫 번째 헤더에서 BOM 제거
            if (headers.GetSize() > 0) {
                CString& firstHeader = headers[0];
                if (!firstHeader.IsEmpty() && (unsigned short)firstHeader[0] == 0xFEFF) {
                    firstHeader = firstHeader.Mid(1);
                }
                // 또는 UTF-8 BOM(??¿) 제거
                if (firstHeader.Left(3) == _T("\xEF\xBB\xBF")) {
                    firstHeader = firstHeader.Mid(3);
                }
            }

            // CSV파일에서 Date, Completed, Focus 컬럼 찾기
            for (int j = 0; j < headers.GetSize(); ++j) {
                CString header = headers[j].Trim();
                header.Remove(_T('"'));
                header.MakeUpper(); // 대소문자 구분 없이

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
                AfxMessageBox(_T("CSV 파일에 Date, Completed, Focus 컬럼이 없습니다."));
                OnCancel();
                return;
            }

            first = false;
            continue;
        }

        CStringArray arr;
        ParseCSVLine(line, arr);

        if (arr.GetSize() <= max(max(dateIndex, completedIndex), focusIndex)) {
            continue; // 필요한만큼 컬럼이 없으면 건너뛰기
        }

        CString date = arr[dateIndex].Trim();
        date.Remove(_T('"'));
        if (date.Find(_T("#")) != -1 || date.IsEmpty()) {
            continue; // 주석이나 빈 날짜는 무시
        }

        CString completedStr = arr[completedIndex].Trim();
        completedStr.Remove(_T('"'));
        int completed = _ttoi(completedStr);

        CString focusStr = arr[focusIndex].Trim();
        focusStr.Remove(_T('"'));
        int focus = _ttoi(focusStr);

        if (completed <= 0 || focus <= 0) {
            continue; // 유효하지 않은 값은 건너뛰기
        }

        // 총집중시간 = completed * Focus
        int totalFocusTime = completed * focus;

        // 같은 날짜의 집중시간을 합산
        if (dailyFocusMap.find(date) != dailyFocusMap.end()) {
            dailyFocusMap[date] += totalFocusTime;
        }
        else {
            dailyFocusMap[date] = totalFocusTime;
        }
    }

    // 맵에서 데이터를 (m_dateList, m_rankList)에 저장하고 랭크 계산
    for (auto& pair : dailyFocusMap) {
        CString date = pair.first;
        int totalFocus = pair.second;

        // 집중시간을 기반으로 랭크 계산
        int rankValue = CalculateRank(totalFocus);

        m_dateList.push_back(date);
        m_rankList.push_back(rankValue);
    }

    //CSV 칼럼에서 데이터를 읽지 못했을 때
    if (m_dateList.empty()) {
        AfxMessageBox(_T("유효한 데이터가 없습니다.\n헤더: Date, Completed, Focus 컬럼이 있는지 확인해주세요."));
        OnCancel();
        return;
    }
    else {
        CString msg;
        msg.Format(_T("%d개의 날짜별 기록을 로드했습니다."), (int)m_dateList.size());
        AfxMessageBox(msg);
    }

    InvalidateRect(NULL);
}

//랭크 계산
int CStatisticsDialog::CalculateRank(int focusMinutes)
{
    if (focusMinutes >= challenger) return 6;      // 챌린저
    else if (focusMinutes >= master) return 5;     // 마스터
    else if (focusMinutes >= diamond) return 4;    // 다이아
    else if (focusMinutes >= platinum) return 3;   // 플래티넘
    else if (focusMinutes >= gold) return 2;       // 골드
    else if (focusMinutes >= silver) return 1;     // 실버
    else return 0;                                 // 브론즈
}
//사용자가 CSV 파일을 선택할 수 있도록 파일 열기 대화상자를 띄우고, 선택된 파일의 경로를 반환
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
//기본 CSV파일 불러오는 경로 지정 (download 폴더)
CString CStatisticsDialog::GetDefaultCSVPath()
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    CString appPath = szPath;
    int pos = appPath.ReverseFind(_T('\\'));
    if (pos != -1) {
        appPath = appPath.Left(pos + 1);
    }

    // 사용자 다운로드 폴더도 확인
    TCHAR downloadsPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, downloadsPath))) {
        CString downloads = downloadsPath;
        downloads += _T("\\Downloads\\PomodoroStatus.csv");
        if (GetFileAttributes(downloads) != INVALID_FILE_ATTRIBUTES) {
            return downloads;
        }
    }

    // 기본값
    return appPath + _T("PomodoroStatus.csv");
}

//한 줄의 CSV 문자열을 받아서, 각 필드를 분리해 CStringArray에 저장함
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

//데이터를 불러와서 그림 그리기, x축은 날짜, y축은 랭크
void CStatisticsDialog::DrawRankGraph(CDC* pDC, CRect rect)
{
    int n = (int)m_dateList.size();
    if (n == 0 || m_rankList.size() != n) {
        CString msg = _T("데이터가 없습니다. CSV 파일을 로드해주세요.");
        pDC->TextOut(rect.left + 50, rect.top + 50, msg);
        return;
    }

    // 배경 지우기
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

    // 제목 그리기 (폰트 크기 증가)
    CFont titleFont;
    titleFont.CreateFont(30, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    CFont* pOldFont = pDC->SelectObject(&titleFont);

    CString title = _T("날짜별 뽀모도로 랭크 변화");
    CSize titleSize = pDC->GetTextExtent(title);
    pDC->TextOut(rect.left + (rect.Width() - titleSize.cx) / 2, rect.top + 15, title);
    pDC->SelectObject(pOldFont);

    // 축 그리기
    pDC->MoveTo(rect.left + marginLeft, rect.bottom - marginBottom);
    pDC->LineTo(rect.right - marginRight, rect.bottom - marginBottom); // X축
    pDC->MoveTo(rect.left + marginLeft, rect.bottom - marginBottom);
    pDC->LineTo(rect.left + marginLeft, rect.top + marginTop);     // Y축

    int graphW = rect.Width() - marginLeft - marginRight;
    int graphH = rect.Height() - marginTop - marginBottom;

    // Y축 격자 및 라벨 (랭크별) - 폰트 크기 증가
    pDC->SelectObject(&gridPen);
    CFont mediumFont;
    mediumFont.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    pDC->SelectObject(&mediumFont);

    for (int i = 0; i <= 6; ++i) {
        int y = rect.bottom - marginBottom - (graphH * i) / 6;

        // 격자선
        pDC->MoveTo(rect.left + marginLeft, y);
        pDC->LineTo(rect.right - marginRight, y);

        // Y축 라벨
        CString rankLabel = m_strRank[i];
        CSize labelSize = pDC->GetTextExtent(rankLabel);
        pDC->SelectObject(&axisPen);
        pDC->TextOut(rect.left + marginLeft - labelSize.cx - 15, y - 8, rankLabel);

        // 눈금 표시
        pDC->MoveTo(rect.left + marginLeft - 5, y);
        pDC->LineTo(rect.left + marginLeft + 5, y);
        pDC->SelectObject(&gridPen);
    }

    // 꺾은선 그리기
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

    // 데이터 포인트 그리기 (크기 증가)
    for (int i = 0; i < n; ++i) {
        CRect pointRect(points[i].x - 6, points[i].y - 6,
            points[i].x + 6, points[i].y + 6);
        pDC->Ellipse(&pointRect);

        // X축 날짜 라벨 (폰트 크기 증가)
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

        // 랭크 라벨 표시 (폰트 크기 증가)
        if (n <= 15 || i % max(1, n / 10) == 0) {
            CString rankStr = m_strRank[m_rankList[i]];
            CSize rankSize = pDC->GetTextExtent(rankStr);
            pDC->TextOut(points[i].x - rankSize.cx / 2, points[i].y - 25, rankStr);
        }
    }

    // 축 라벨 폰트 크기 증가
    CFont axisFont;
    axisFont.CreateFont(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
    pDC->SelectObject(&axisFont);

    CString yLabel = _T("랭크");
    CSize yLabelSize = pDC->GetTextExtent(yLabel);
    pDC->TextOut(rect.left + 5, rect.top + marginTop + graphH / 2 - yLabelSize.cy / 2, yLabel);

    CString xLabel = _T("날짜");
    CSize xLabelSize = pDC->GetTextExtent(xLabel);
    pDC->TextOut(rect.left + marginLeft + graphW / 2 - xLabelSize.cx / 2,
        rect.bottom - 20, xLabel);

    // 통계 정보 (폰트 크기 증가)
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
        statsStr.Format(_T("총 %d일 | 평균 랭크: %s | 최고 랭크: %s"),
            n, m_strRank[(int)avgRank], m_strRank[maxRank]);
        pDC->TextOut(rect.left + marginLeft, rect.bottom - marginBottom + 45, statsStr);
    }

    pDC->SelectObject(pOldFont);
    pDC->SelectObject(pOldBrush);
    pDC->SelectObject(pOldPen);

    delete[] points;
}