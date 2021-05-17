#ifndef __FCCUSTOM_H__
#define __FCCUSTOM_H__

#include <Windows.h>

typedef struct _CURRENT_FOCUSED_EXE_INFORMATION {
	LPCWSTR fPath;
	HICON hIcon;
	LPWSTR appName;
	LPWSTR windowTitle;
	wchar_t* datetime;
}FOCUSINFO;

void charToWChar(const char* pstrSrc, wchar_t pwstrDest[]);
void wCharToChar(const wchar_t* pwstrSrc, char pstrDest[]);
void lptToChar(const LPTSTR lptstrSrc, char pstrDest[]);
void GetDateTime(char* datetime);

int GetCurrExeInfo(FOCUSINFO* fInfo);

#endif