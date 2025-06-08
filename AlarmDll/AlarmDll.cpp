// AlarmDll.cpp : DLL의 초기화 루틴을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "AlarmDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 이 DLL이 MFC DLL에 대해 동적으로 링크되어 있는 경우
//		MFC로 호출되는 이 DLL에서 내보내지는 모든 함수의
//		시작 부분에 AFX_MANAGE_STATE 매크로가
//		들어 있어야 합니다.
//
//		예:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 일반적인 함수 본문은 여기에 옵니다.
//		}
//
//		이 매크로는 MFC로 호출하기 전에
//		각 함수에 반드시 들어 있어야 합니다.
//		즉, 매크로는 함수의 첫 번째 문이어야 하며
//		개체 변수의 생성자가 MFC DLL로
//		호출할 수 있으므로 개체 변수가 선언되기 전에
//		나와야 합니다.
//
//		자세한 내용은
//		MFC Technical Note 33 및 58을 참조하십시오.
//

// CAlarmDllApp

BEGIN_MESSAGE_MAP(CAlarmDllApp, CWinApp)
END_MESSAGE_MAP()


// CAlarmDllApp 생성

CAlarmDllApp::CAlarmDllApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CAlarmDllApp 개체입니다.

CAlarmDllApp theApp;


// CAlarmDllApp 초기화

BOOL CAlarmDllApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

extern "C" __declspec(dllexport) int AMPM(CString m_strAMPM, int hour24)
{
	if (m_strAMPM == _T("AM"))
	{
		if (hour24 == 12) hour24 = 0; // 12AM은 0시
	}
	else if (m_strAMPM == _T("PM"))
	{
		if (hour24 != 12) hour24 += 12; // 12PM은 12시, 그 외에는 +12
	}

	return hour24;
}

extern "C" __declspec(dllexport) void Alarming(int today, CTime now, std::vector<AlarmInfo> m_alarms)
{
	for (size_t i = 0; i < m_alarms.size(); ++i)
	{
		if (!m_alarms[i].enabled) // 꺼진 알람은 무시
			continue;

		if (!m_alarms[i].active)
			continue;

		// 오늘 요일에 해당하는지 체크
		// daysOfWeek[0]:월, ..., [6]:일
		int weekdayIdx = (today == 1) ? 6 : today - 2; // 0:월, ..., 6:일
		if (!m_alarms[i].daysOfWeek[weekdayIdx])
			continue;

		// 시간/분이 일치하는지 체크
		if (now.GetHour() == m_alarms[i].time.GetHour() &&
			now.GetMinute() == m_alarms[i].time.GetMinute() &&
			now.GetSecond() == 0)
		{
			int hour = m_alarms[i].time.GetHour();
			CString ampm = (hour < 12) ? _T("AM") : _T("PM");
			int hour12 = hour % 12;
			if (hour12 == 0) hour12 = 12;

			CString msg;
			msg.Format(_T("%02d:%02d %s 알람입니다!"), hour12, m_alarms[i].time.GetMinute(), ampm);
			MessageBox(NULL, msg, _T("알람"), MB_OK | MB_ICONEXCLAMATION);

			// 반복 알람이므로 active를 false로 하지 않음
			// (원하면 1회성 알람만 active=false로 처리)
		}

		return;
	}
}