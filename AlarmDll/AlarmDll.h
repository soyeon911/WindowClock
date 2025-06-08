// AlarmDll.h : AlarmDll DLL의 주 헤더 파일
//

#pragma once

#ifndef __AFXWIN_H__
#error "pch.h를 포함하세요."
#endif

#include "resource.h"		// 주 기호입니다.
#include <vector>
#include <afx.h>

struct AlarmInfo
{
	CTime time;
	bool active;       // 알람이 울렸는지 여부 (반복 알람이면 계속 true)
	bool enabled;      // 알람 켜짐/꺼짐 상태 (true: 켜짐, false: 꺼짐)
	bool daysOfWeek[7]; // 월~일 (0:월, 1:화, ..., 6:일)
};

// CAlarmDllApp
// 이 클래스 구현에 대해서는 AlarmDll.cpp를 참조하세요.
//

class CAlarmDllApp : public CWinApp
{
public:
	CAlarmDllApp();

	// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};