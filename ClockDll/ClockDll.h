// ClockDll.h : ClockDll DLL의 주 헤더 파일
//

#pragma once

#ifdef CLOCKDLL_EXPORTS
#define CLOCKDLL_API __declspec(dllexport)
#else
#define CLOCKDLL_API __declspec(dllimport)
#endif


#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.

// Ensure consistent declaration of GetCurrentTimeFormatted across translation units  
#ifdef __cplusplus  
extern "C" {  
#endif  

CLOCKDLL_API void GetCurrentTimeFormatted(bool is24Hour, TCHAR* buffer, int bufferSize);  

#ifdef __cplusplus  
}  
#endif
extern "C" CLOCKDLL_API void GetCurrentTimeFormatted(bool is24Hour, TCHAR* buffer, int bufferSize);

// CClockDllApp
// 이 클래스 구현에 대해서는 ClockDll.cpp를 참조하세요.
//

class CClockDllApp : public CWinApp
{
public:
	CClockDllApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
