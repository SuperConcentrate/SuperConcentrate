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
//return 1 : �������
//FOCUSINFO : �� ��ȯ ����ü. FCCustom.h�� ����
//������ ����ü ���� 0xCCCCCCCC (N/A) �� �Ѿ �� ����. output ���� ó�� ��ƾ �߰� �� �ʿ�.
int GetCurrExeInfo(FOCUSINFO* fInfo) {
	HWND handleFocused;
	PROCESSENTRY32 pe32; // ������ �𸣰�����, ������� �ʴ� �����ӿ��� ������ �����ϸ� ��´ܿ��� ���� ���� ���� �߻�

	DWORD pid = NULL;
	HANDLE pHandle = NULL;
	HINSTANCE hInstance = NULL;

	//GetForegroundWIndow()�� ���� Ȱ��ȭ �� �������� HWND�� ���� ��, HINSTANCE ��HANDLE���� ��ȯ �� ���� ����
	handleFocused = GetForegroundWindow();
	GetWindowThreadProcessId(handleFocused, &pid);
	hInstance = (HINSTANCE)GetWindowLong(handleFocused, GWL_HINSTANCE);

	pHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

	//fInfo.fpath �����ϴ� ��ƾ
	wchar_t buffer[MAX_PATH] = {};
	DWORD buffer_size = MAX_PATH;

	if (pHandle) {
		if (QueryFullProcessImageNameW(pHandle, 0, buffer, &buffer_size)) {
			fInfo->fPath = buffer;
			CloseHandle(pHandle);
		}
	}
	else { return 0; }

	//fInfo.hIcon & exename(appname) ���� ��ƾ
	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	DWORD dwRC = SHGetFileInfo(buffer, FILE_ATTRIBUTE_READONLY, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_DISPLAYNAME);
	fInfo->hIcon = sfi.hIcon;
	fInfo->appName = sfi.szDisplayName;

	// 0xCCCCCCCC ���� �Ƹ� Ret0 ������ �Ѿ�� �׷���?
	if (dwRC ==0 )
		return 0;

	//Window Title ���� ��ƾ
	wchar_t wTitle[100];
	//���� ������ ���ɼ�
	GetWindowTextW(handleFocused, wTitle, sizeof(wTitle)/2);
	fInfo->windowTitle = wTitle;

	char timestamp[16];
	wchar_t datetime[17];
	GetDateTime(timestamp);
	charToWChar(timestamp, datetime);
	fInfo->datetime = datetime;

	//Stack around the variable 'wTitle' was corrupted. => ���ۿ����� (/2�� �ذ�)
	return 1;
}
