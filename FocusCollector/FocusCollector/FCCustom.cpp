#include "FCCustom.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <time.h>
#include <stdio.h>
//#include <CommCtrl.h>

//#pragma comment(lib, "comctl32.lib")
#pragma warning(disable:4996)

// char[] to LPCWSTR
void charToWChar(const char* pstrSrc, wchar_t pwstrDest[]) {
	int nLen = (int)strlen(pstrSrc) + 1;
	mbstowcs(pwstrDest, pstrSrc, nLen);
}

// LPCWSTR to char[]
void wCharToChar(const wchar_t* pwstrSrc, char pstrDest[]) {
	int nLen = (int)wcslen(pwstrSrc);
	wcstombs(pstrDest, pwstrSrc, nLen + 1);
}

// LPTSTR = wchar_t* = char*
void lptToChar(const LPTSTR lptstrSrc, char pstrDest[]) {
	wchar_t* tmp = lptstrSrc;
	wCharToChar(tmp, pstrDest);
}

void GetDateTime(char* datetime) {
	struct tm* t;
	time_t tnow;
	tnow = time(NULL);
	t = (struct tm*)localtime(&tnow);
	sprintf(datetime, "%04d%02d%02d%02d%02d%02d", \
		t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}

//return 0 : Have Error Somehow
//return 1 : 정상실행
//FOCUSINFO : 값 반환 구조체. FCCustom.h에 정의
//가끔씩 구조체 내에 0xCCCCCCCC (N/A) 값 넘어갈 때 있음. output 에러 처리 루틴 추가 할 필요.
int GetCurrExeInfo(FOCUSINFO* fInfo) {
	HWND handleFocused;
	PROCESSENTRY32 pe32; // 왜인지 모르겠으나, 사용하지 않는 변수임에도 변수를 삭제하면 출력단에서 글자 깨짐 현상 발생

	DWORD pid = NULL;
	HANDLE pHandle = NULL;
	HINSTANCE hInstance = NULL;

	//GetForegroundWIndow()로 현재 활성화 된 윈도우의 HWND를 얻어온 뒤, HINSTANCE 및HANDLE로의 변환 및 추출 진행
	handleFocused = GetForegroundWindow();
	GetWindowThreadProcessId(handleFocused, &pid);
	hInstance = (HINSTANCE)GetWindowLong(handleFocused, GWL_HINSTANCE);

	pHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

	//fInfo.fpath 추출하는 루틴
	wchar_t buffer[MAX_PATH] = {};
	DWORD buffer_size = MAX_PATH;

	if (pHandle) {
		if (QueryFullProcessImageNameW(pHandle, 0, buffer, &buffer_size)) {
			fInfo->fPath = buffer;
			CloseHandle(pHandle);
		}
	}
	else { return 0; }

	//fInfo.hIcon & exename(appname) 추출 루틴
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	DWORD dwRC = SHGetFileInfo(buffer, FILE_ATTRIBUTE_READONLY, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_DISPLAYNAME);
	fInfo->hIcon = sfi.hIcon;
	fInfo->appName = sfi.szDisplayName;

	// 0xCCCCCCCC 이유 아마 Ret0 잡혀서 넘어가서 그런듯?
	if (dwRC ==0 )
		return 0;

	//Window Title 추출 루틴
	wchar_t wTitle[100];
	//버퍼 오버런 가능성
	GetWindowTextW(handleFocused, wTitle, sizeof(wTitle)/2);
	fInfo->windowTitle = wTitle;

	char timestamp[16];
	wchar_t datetime[17];
	GetDateTime(timestamp);
	charToWChar(timestamp, datetime);
	fInfo->datetime = datetime;

	//Stack around the variable 'wTitle' was corrupted. => 버퍼오버런 (/2로 해결)
	return 1;
}
