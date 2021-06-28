#include "FCCustom.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <time.h>
#include <stdio.h>
//#include <CommCtrl.h>

//#pragma comment(lib, "comctl32.lib")
#pragma warning(disable:4996)

WCHAR* extractFileName(WCHAR fRoute[]) {
	WCHAR* wTemp, * wBefore, * wtBuf, * wfName;

	//*.clist �����ϱ�
	//wBefore = (WCHAR*)malloc(wcslen(fRoute) * sizeof(WCHAR));
	wBefore = new WCHAR[wcslen(fRoute)];
	wfName = new WCHAR[wcslen(fRoute)];
	wtBuf = new WCHAR[wcslen(fRoute)];
	int numOfParse = 0, currParse = 0;

	wTemp = wcstok(fRoute, L"\\");

	do
	{
		wsprintf(wBefore, wTemp);
		wTemp = wcstok(NULL, L"\\");
	} while (wTemp != NULL);

	//".clist" �����ϱ�

	wcscpy(wtBuf, wBefore);
	wTemp = wcstok(wBefore, L".");

	do {
		numOfParse++;
		wTemp = wcstok(NULL, L".");
	} while (wTemp != NULL);

	//���� ��ƾ ���� (�� �������� .clist ���� ����)
	memset(wfName, 0, sizeof(WCHAR) * wcslen(fRoute));

	wTemp = wcstok(wtBuf, L".");
	wcscat(wfName, wTemp);
	currParse++;
	while (currParse < numOfParse - 1) {
		wcscat(wfName, L".");
		currParse++;
		wTemp = wcstok(NULL, L".");
		wcscat(wfName, wTemp);
	}

	return wfName;
}


//should add myself
WCHAR** makeNewList(HWND hWnd, RECT rt, WCHAR curr[], WCHAR disp[]) {
	swprintf(curr, sizeof(L"NEWFILE\0"), L"NEWFILE\0");
	swprintf(disp, sizeof(L"�� ����Ʈ ������\0"), L"�� ����Ʈ ������\0");

	WCHAR** aFileList = new WCHAR * [MAXFLIST];
	//memset(aFileList, 0, sizeofBuffer);

	for (int i = 0; i < MAXFLIST; ++i) {
		aFileList[i] = new WCHAR[nFileNameMaxLen];
		memset(aFileList[i], 0, sizeof(WCHAR) * nFileNameMaxLen);
	}

	swprintf(aFileList[0], L"%d", 1);
	swprintf(aFileList[1], MYSELF);

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);

	return aFileList;
}

void saveList(HWND hWnd, WCHAR fRoute[], WCHAR** aFileList, RECT rt, WCHAR curr[], WCHAR disp[]) {
	//���� ���� ����
	swprintf(curr, wcslen(fRoute) * sizeof(WCHAR), fRoute);
	//parse fRoute
	WCHAR* wTemp, * wSaveRoute, * wBefore, * dispStr;

	wSaveRoute = (WCHAR*)malloc((wcslen(fRoute) * sizeof(WCHAR)) + (wcslen(L".clist\0") * sizeof(WCHAR)));
	wBefore = (WCHAR*)malloc(wcslen(fRoute) * sizeof(WCHAR));

	wcscpy(wSaveRoute, fRoute);

	wTemp = wcstok(fRoute, L".");

	do
	{
		wsprintf(wBefore, wTemp);
		wTemp = wcstok(NULL, L".");
	} while (wTemp != NULL);

	if (0 != wcscmp(wBefore, L"clist")) { //������ Ȯ���ڰ� clist�� �ƴ� ���
		wcscat(wSaveRoute, L".clist\0");
	}

	//���Ͽ� ������ �Է�

	FILE* fp = _wfopen(wSaveRoute, L"w");
	UINT numOfFiles = (UINT)_wtoi(aFileList[0]);

	WCHAR* outputBuffer = (WCHAR*)malloc(sizeofOutBuffer);
	memset(outputBuffer, 0, sizeofOutBuffer);


	//wsprintf(outputBuffer, L"%d\n", _wtoi(aFileList[0]));
	fwprintf(fp, L"%d\n", _wtoi(aFileList[0]));
	for (int i = 1; i < numOfFiles + 1; i++) {
		fwprintf(fp, L"%s\n", aFileList[i]);
		/*wcscat(outputBuffer,aFileList[i]);
		wcscat(outputBuffer, L"\n");*/
	}

	//fwprintf(fp, outputBuffer);
	fclose(fp);

	//ȭ�� ����
	dispStr = wcscat(extractFileName(wSaveRoute), L" ���Ͽ� �����\0");
	swprintf(disp, wcslen(dispStr) * sizeof(WCHAR), dispStr);

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);
}

WCHAR** loadList(HWND hWnd, WCHAR fRoute[], RECT rt, WCHAR curr[], WCHAR disp[]) {
	WCHAR oneLine[nFileNameMaxLen * sizeof(WCHAR)];
	WCHAR* dispStr;
	//���� ���� �ʱ�ȭ
	WCHAR** aFileList = new WCHAR * [MAXFLIST];
	//memset(aFileList, 0, sizeofBuffer);

	for (int i = 0; i < MAXFLIST; ++i) {
		aFileList[i] = new WCHAR[nFileNameMaxLen];
		memset(aFileList[i], 0, sizeof(WCHAR) * nFileNameMaxLen);
	}

	//���� ���� ����
	swprintf(curr, wcslen(fRoute) * sizeof(WCHAR), fRoute);

	//���� �ε�
	FILE* fp = _wfopen(fRoute, L"r");
	//UINT numOfFiles;

	for (int i = 0; !feof(fp); i++) { //������ ���� �ƴ� ���
		fgetws(oneLine, nFileNameMaxLen * sizeof(WCHAR), fp); //oneLine ���� ũ�⸸ŭ �� �� �б�
		wcstok(oneLine, L"\n"); // ���� ���� ����
		wcscpy(aFileList[i], oneLine);
	}

	//���÷��� ����

	dispStr = wcscat(extractFileName(fRoute), L" �ε��\0");
	swprintf(disp, wcslen(dispStr) * sizeof(WCHAR), dispStr);

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);

	return aFileList;
}

void addToList(HWND hWnd, WCHAR fRoute[], WCHAR** aFileList, RECT rt, WCHAR curr[], WCHAR disp[]) {
	UINT numOfList = _wtoi(aFileList[0]);
	swprintf(aFileList[0], L"%d", numOfList + 1);
	swprintf(aFileList[numOfList + 1], L"%s\n\0", fRoute);

	//ȭ�� ����
	swprintf(disp, sizeof(L"����Ʈ�� �߰���\0"), L"����Ʈ�� �߰���\0");

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);
}

//true : ����Ʈ���� ���� ���α׷�
//false : ����Ʈ���� ������ ���� ���α׷�
bool checkAllowed(WCHAR fRoute[], WCHAR** aFileList) {
	//ĸ�� ���� �ͼ���
	if (fRoute == NULL)
		return true;

	UINT numOfFiles = _wtoi(aFileList[0]);
	for (int i = 1; i < numOfFiles + 1; i++) {
		if (0 == wcscmp(aFileList[i], fRoute)) {
			return true;
		}
	}
	return false;
}

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
	if (dwRC == 0)
		return 0;

	//Window Title ���� ��ƾ
	wchar_t wTitle[100];
	//���� ������ ���ɼ�
	GetWindowTextW(handleFocused, wTitle, sizeof(wTitle) / 2);
	fInfo->windowTitle = wTitle;

	char timestamp[16];
	wchar_t datetime[17];
	GetDateTime(timestamp);
	charToWChar(timestamp, datetime);
	fInfo->datetime = datetime;

	//Stack around the variable 'wTitle' was corrupted. => ���ۿ����� (/2�� �ذ�)
	return 1;
}
