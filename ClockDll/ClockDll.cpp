//#include "pch.h"
//#include "ClockDll.h"
//#include <afx.h>
//
//extern "C" CLOCKDLL_API void GetCurrentTimeFormatted(bool is24Hour, TCHAR* buffer, int bufferSize)
//{
//    AFX_MANAGE_STATE(AfxGetStaticModuleState());
//
//    CTime now = CTime::GetCurrentTime();
//    int h = now.GetHour();
//    int m = now.GetMinute();
//    int s = now.GetSecond();
//
//    if (is24Hour)
//    {
//        _stprintf_s(buffer, bufferSize, _T("%02d:%02d:%02d"), h, m, s);
//    }
//    else
//    {
//        CString ampm = (h < 12) ? _T("AM") : _T("PM");
//        int hour12 = h % 12;
//        if (hour12 == 0) hour12 = 12;
//
//        _stprintf_s(buffer, bufferSize, _T("%02d:%02d:%02d %s"), hour12, m, s, ampm);
//    }
//}

#include "pch.h"
#include "ClockDll.h"
#include <afx.h>

// 'dllimport'를 제거하고 함수 정의를 유지합니다.
extern "C" CLOCKDLL_API void GetCurrentTimeFormatted(bool is24Hour, TCHAR* buffer, int bufferSize)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CTime now = CTime::GetCurrentTime();
    int h = now.GetHour();
    int m = now.GetMinute();
    int s = now.GetSecond();

    if (is24Hour)
    {
        _stprintf_s(buffer, bufferSize, _T("%02d:%02d:%02d"), h, m, s);
    }
    else
    {
        CString ampm = (h < 12) ? _T("AM") : _T("PM");
        int hour12 = h % 12;
        if (hour12 == 0) hour12 = 12;

        _stprintf_s(buffer, bufferSize, _T("%02d:%02d:%02d %s"), hour12, m, s, ampm);
    }
}
