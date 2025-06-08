// CAlarmDialog.cpp: 구현 파일
//
#include "pch.h"
#include "TeamProject.h"
#include "afxdialogex.h"
#include "CAlarmDialog.h"

IMPLEMENT_DYNAMIC(CAlarmDialog, CDialogEx)

extern "C" __declspec(dllexport) int AMPM(CString m_strAMPM, int hour24);
extern "C" __declspec(dllexport) void Alarming(int today, CTime now, std::vector<AlarmInfo> m_alarms);

CAlarmDialog::CAlarmDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ALARM_DIALOG, pParent)
	, m_nHour(1)
	, m_nMin(0)
	, m_strAMPM(_T("AM"))
	, m_strAlarmTime(_T(""))
	, m_strAlarmName(_T(""))
	, m_nTimerID(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_CLOCK);
	for (int i = 0; i < 7; ++i)
		m_bDaysOfWeek[i] = FALSE;
}

CAlarmDialog::~CAlarmDialog()
{
}

void CAlarmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HOUR, m_nHour);
	DDX_Text(pDX, IDC_EDIT_MIN, m_nMin);
	DDX_CBString(pDX, IDC_COMBO_AMPM, m_strAMPM);
	DDX_Text(pDX, IDC_STATIC_ALARMTIME, m_strAlarmTime);
	DDX_Check(pDX, IDC_CHECK_MON, m_bDaysOfWeek[0]);
	DDX_Check(pDX, IDC_CHECK_TUE, m_bDaysOfWeek[1]);
	DDX_Check(pDX, IDC_CHECK_WED, m_bDaysOfWeek[2]);
	DDX_Check(pDX, IDC_CHECK_THU, m_bDaysOfWeek[3]);
	DDX_Check(pDX, IDC_CHECK_FRI, m_bDaysOfWeek[4]);
	DDX_Check(pDX, IDC_CHECK_SAT, m_bDaysOfWeek[5]);
	DDX_Check(pDX, IDC_CHECK_SUN, m_bDaysOfWeek[6]);
	DDX_Text(pDX, IDC_EDIT_ALARM_NAME, m_strAlarmName);
}

BEGIN_MESSAGE_MAP(CAlarmDialog, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

	ON_BN_CLICKED(IDC_BUTTON_SETALARM, &CAlarmDialog::OnBnClickedButtonSetalarm)
	ON_WM_TIMER()

	//	ON_BN_CLICKED(IDC_BUTTON_SETALARMS, &CAlarmAppDlg::OnClickedButtonSetalarms)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_ALARM, &CAlarmDialog::OnClickedButtonDeleteAlarm)
	ON_BN_CLICKED(IDC_BUTTON_TOGGLE_ALARM, &CAlarmDialog::OnClickedButtonToggleAlarm)
	//	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CAlarmAppDlg 메시지 처리기

BOOL CAlarmDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	m_brushDarkBg.CreateSolidBrush(RGB(30, 30, 30));         // 다크 모드 배경
	m_brushLightBg.CreateSolidBrush(RGB(255, 255, 255));     // 라이트 모드 배경
	m_brBackground.CreateSolidBrush(RGB(137, 184, 169));     // 팀프로젝트/시계와 동일
	m_brCtrlBk.CreateSolidBrush(RGB(137, 184, 169));           // 컨트롤 배경

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// ComboBox에 AM/PM 추가
	CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_AMPM);
	pCombo->AddString(_T("AM"));
	pCombo->AddString(_T("PM"));
	pCombo->SetCurSel(0); // 기본 AM

	m_strAlarmTime = _T("알람 시간을 설정하세요.");

	LoadAlarmsFromFile(_T("Alarms.dat")); // 알람 정보 불러오기
	// 리스트 박스에 알람 표시
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_ALARMS);
	pList->ResetContent();
	static const TCHAR* daysKor[7] = { _T("월"),_T("화"),_T("수"),_T("목"),_T("금"),_T("토"),_T("일") };
	for (const auto& alarm : m_alarms) {
		CString dayStr;
		for (int i = 0; i < 7; ++i)
			if (alarm.daysOfWeek[i]) dayStr += daysKor[i];
		int hour = alarm.time.GetHour();
		int min = alarm.time.GetMinute();
		CString ampm = (hour >= 12) ? _T("PM") : _T("AM");
		int hour12 = hour % 12; if (hour12 == 0) hour12 = 12;
		CString str;
		str.Format(_T("[%s] %02d:%02d %s (%s)%s"),
			alarm.name.IsEmpty() ? _T("이름없음") : alarm.name,
			hour12, min, ampm, dayStr, alarm.enabled ? _T("") : _T(" [꺼짐]"));
		pList->AddString(str);
	}
	UpdateData(FALSE);
	return TRUE;
}

void CAlarmDialog::OnBnClickedButtonSetalarm()
{
	UpdateData(TRUE);

	// 입력값 유효성 검사 (1~12시)
	if (m_nHour < 1 || m_nHour > 12 || m_nMin < 0 || m_nMin > 59) {
		MessageBox(_T("시간을 올바르게 입력하세요 (1~12시, 0~59분)"), _T("오류"), MB_OK | MB_ICONERROR);
		return;
	}

	// AM/PM에 따라 24시간제로 변환
	int hour24 = m_nHour;
	hour24 = AMPM(m_strAMPM, hour24);

	// 현재 시간
	CTime now = CTime::GetCurrentTime();
	CTime alarmTime(now.GetYear(), now.GetMonth(), now.GetDay(), hour24, m_nMin, 0);

	if (alarmTime < now)
		alarmTime += CTimeSpan(1, 0, 0, 0);

	bool anyDay = false;
	for (int i = 0; i < 7; ++i)
		if (m_bDaysOfWeek[i]) anyDay = true;
	if (!anyDay)
	{
		MessageBox(_T("적어도 한 개의 요일을 선택하세요!"), _T("오류"), MB_OK | MB_ICONERROR);
		return;
	}

	// 알람 정보 추가
	AlarmInfo info;
	info.time = alarmTime;
	info.active = true;      // 알람 울림 상태 (반복이면 계속 true)
	info.enabled = true;     // 새로 추가하는 알람은 기본 켜짐 상태로 설정
	for (int i = 0; i < 7; ++i)
		info.daysOfWeek[i] = m_bDaysOfWeek[i] ? true : false;
	info.name = m_strAlarmName; // 알람 이름 저장
	m_alarms.push_back(info);

	// 리스트박스에 추가
	CString dayStr;
	static const TCHAR* daysKor[7] = { _T("월"),_T("화"),_T("수"),_T("목"),_T("금"),_T("토"),_T("일") };
	for (int i = 0; i < 7; ++i)
		if (m_bDaysOfWeek[i]) dayStr += daysKor[i];
	CString str;
	str.Format(_T("[%s] %02d:%02d %s (%s)"),
		info.name.IsEmpty() ? _T("이름없음") : info.name,
		m_nHour, m_nMin, m_strAMPM, dayStr);
	((CListBox*)GetDlgItem(IDC_LIST_ALARMS))->AddString(str);

	m_strAlarmTime = _T("알람이 추가되었습니다!");
	SaveAlarmsToFile(_T("Alarms.dat"));
	UpdateData(FALSE);

	if (m_nTimerID == 0)
		m_nTimerID = SetTimer(1, 1000, NULL);
}

void CAlarmDialog::OnTimer(UINT_PTR nIDEvent)
{
	CTime now = CTime::GetCurrentTime();
	int today = now.GetDayOfWeek(); // 1:일, 2:월, ..., 7:토

	Alarming(today, now, m_alarms);

	CDialogEx::OnTimer(nIDEvent);
}


void CAlarmDialog::OnClickedButtonDeleteAlarm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_ALARMS);
	int nIndex = pList->GetCurSel(); // 선택된 항목 인덱스

	if (nIndex == LB_ERR) // 아무것도 선택되지 않음
	{
		MessageBox(_T("삭제할 알람을 선택하세요!"), _T("경고"), MB_OK | MB_ICONWARNING);
		return;
	}

	// 벡터에서 해당 알람 제거
	if (nIndex >= 0 && nIndex < (int)m_alarms.size())
	{
		m_alarms.erase(m_alarms.begin() + nIndex);
	}

	// 리스트 박스에서 항목 제거
	pList->DeleteString(nIndex);

	// 모든 알람이 삭제되면 타이머 종료
	if (m_alarms.empty() && m_nTimerID != 0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID = 0;
		m_strAlarmTime = _T("알람이 없습니다.");
		UpdateData(FALSE);
	}

	SaveAlarmsToFile(_T("Alarms.dat")); // 삭제 후 저장

}


void CAlarmDialog::OnClickedButtonToggleAlarm()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_ALARMS);
	int nIndex = pList->GetCurSel();

	if (nIndex == LB_ERR)
	{
		MessageBox(_T("켜거나 끌 알람을 선택하세요!"), _T("경고"), MB_OK | MB_ICONWARNING);
		return;
	}

	if (nIndex < 0 || nIndex >= (int)m_alarms.size())
		return;

	// 상태 토글
	m_alarms[nIndex].enabled = !m_alarms[nIndex].enabled;

	// 리스트박스 표시 업데이트 (예: "[꺼짐]" 표시 추가/제거)
	CString oldStr;
	pList->GetText(nIndex, oldStr);

	// "[꺼짐]" 텍스트가 있으면 제거, 없으면 추가
	CString offTag = _T(" [꺼짐]");
	int pos = oldStr.Find(offTag);
	if (pos >= 0)
	{
		oldStr.Delete(pos, offTag.GetLength());
	}
	else
	{
		oldStr += offTag;
	}

	pList->DeleteString(nIndex);
	pList->InsertString(nIndex, oldStr);
	pList->SetCurSel(nIndex);

	SaveAlarmsToFile(_T("Alarms.dat")); // 토글 후 저장

}



// 알람 저장
void CAlarmDialog::SaveAlarmsToFile(const CString& path) {
	CFile file;
	if (file.Open(path, CFile::modeCreate | CFile::modeWrite)) {
		int count = (int)m_alarms.size();
		file.Write(&count, sizeof(count));
		for (const auto& alarm : m_alarms) {
			time_t t = alarm.time.GetTime();
			file.Write(&t, sizeof(t));
			file.Write(&alarm.active, sizeof(alarm.active));
			file.Write(&alarm.enabled, sizeof(alarm.enabled));
			file.Write(alarm.daysOfWeek, sizeof(bool) * 7);
			int nameLen = alarm.name.GetLength();
			file.Write(&nameLen, sizeof(nameLen));
			if (nameLen > 0)
				file.Write(alarm.name.GetString(), nameLen * sizeof(TCHAR));
		}
		file.Close();
	}
}

// 알람 불러오기
void CAlarmDialog::LoadAlarmsFromFile(const CString& path) {
	m_alarms.clear();
	CFile file;
	if (file.Open(path, CFile::modeRead)) {
		int count = 0;
		if (file.Read(&count, sizeof(count)) != sizeof(count)) return;
		for (int i = 0; i < count; ++i) {
			AlarmInfo alarm;
			time_t t;
			if (file.Read(&t, sizeof(t)) != sizeof(t)) break;
			alarm.time = CTime(t);
			if (file.Read(&alarm.active, sizeof(alarm.active)) != sizeof(alarm.active)) break;
			if (file.Read(&alarm.enabled, sizeof(alarm.enabled)) != sizeof(alarm.enabled)) break;
			if (file.Read(alarm.daysOfWeek, sizeof(bool) * 7) != sizeof(bool) * 7) break;
			int nameLen = 0;
			if (file.Read(&nameLen, sizeof(nameLen)) != sizeof(nameLen)) break;
			if (nameLen > 0) {
				TCHAR* buf = new TCHAR[nameLen + 1];
				if (file.Read(buf, nameLen * sizeof(TCHAR)) != nameLen * sizeof(TCHAR)) {
					delete[] buf;
					break;
				}
				buf[nameLen] = 0;
				alarm.name = buf;
				delete[] buf;
			}
			else {
				alarm.name = _T("");
			}
			m_alarms.push_back(alarm);
		}
		file.Close();
	}
}

HBRUSH CAlarmDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (m_isDarkMode) {
		switch (nCtlColor) {
		case CTLCOLOR_DLG:
			return (HBRUSH)m_brushDarkBg;
		case CTLCOLOR_STATIC:
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetTextColor(RGB(255, 255, 255));
			return (HBRUSH)m_brushDarkBg;
		case CTLCOLOR_BTN:
		case CTLCOLOR_EDIT:
		case CTLCOLOR_LISTBOX:
			pDC->SetBkColor(RGB(50, 50, 50));
			pDC->SetTextColor(RGB(255, 255, 255));
			return (HBRUSH)m_brushDarkBg;
		}
	}
	else {
		if (nCtlColor == CTLCOLOR_DLG) {
			pDC->SetTextColor(RGB(0, 0, 0));
			pDC->SetBkMode(TRANSPARENT);
			return m_brBackground;
		}
		if (nCtlColor == CTLCOLOR_STATIC || nCtlColor == CTLCOLOR_EDIT || nCtlColor == CTLCOLOR_BTN) {
			pDC->SetBkColor(RGB(165, 80, 77));
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkMode(TRANSPARENT);
			return m_brCtrlBk;
		}
	}
	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CAlarmDialog::SetDarkMode(bool enable)
{
	m_isDarkMode = enable;
	// 타이틀바 다크모드 적용 (선택)
	BOOL value = enable ? TRUE : FALSE;
	::DwmSetWindowAttribute(m_hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));
	Invalidate(TRUE); // 전체 다시 그리기
}