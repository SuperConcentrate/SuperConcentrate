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

	//*.clist 추출하기
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

	//".clist" 제거하기

	wcscpy(wtBuf, wBefore);
	wTemp = wcstok(wBefore, L".");

	do {
		numOfParse++;
		wTemp = wcstok(NULL, L".");
	} while (wTemp != NULL);

	//제거 루틴 시작 (맨 마지막의 .clist 만을 제거)
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
	swprintf(disp, sizeof(L"새 리스트 생성됨\0"), L"새 리스트 생성됨\0");

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
	//파일 상태 갱신
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

	if (0 != wcscmp(wBefore, L"clist")) { //지정된 확장자가 clist가 아닌 경우
		wcscat(wSaveRoute, L".clist\0");
	}

	//파일에 데이터 입력

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

	//화면 갱신
	dispStr = wcscat(extractFileName(wSaveRoute), L" 파일에 저장됨\0");
	swprintf(disp, wcslen(dispStr) * sizeof(WCHAR), dispStr);

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);
}

WCHAR** loadList(HWND hWnd, WCHAR fRoute[], RECT rt, WCHAR curr[], WCHAR disp[]) {
	WCHAR oneLine[nFileNameMaxLen * sizeof(WCHAR)];
	WCHAR* dispStr;
	//기존 버퍼 초기화
	WCHAR** aFileList = new WCHAR * [MAXFLIST];
	//memset(aFileList, 0, sizeofBuffer);

	for (int i = 0; i < MAXFLIST; ++i) {
		aFileList[i] = new WCHAR[nFileNameMaxLen];
		memset(aFileList[i], 0, sizeof(WCHAR) * nFileNameMaxLen);
	}

	//파일 상태 갱신
	swprintf(curr, wcslen(fRoute) * sizeof(WCHAR), fRoute);

	//파일 로드
	FILE* fp = _wfopen(fRoute, L"r");
	//UINT numOfFiles;

	for (int i = 0; !feof(fp); i++) { //파일의 끝이 아닌 경우
		fgetws(oneLine, nFileNameMaxLen * sizeof(WCHAR), fp); //oneLine 버퍼 크기만큼 한 줄 읽기
		wcstok(oneLine, L"\n"); // 개행 문자 버림
		wcscpy(aFileList[i], oneLine);
	}

	//디스플레이 갱신

	dispStr = wcscat(extractFileName(fRoute), L" 로드됨\0");
	swprintf(disp, wcslen(dispStr) * sizeof(WCHAR), dispStr);

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);

	return aFileList;
}

void addToList(HWND hWnd, WCHAR fRoute[], WCHAR** aFileList, RECT rt, WCHAR curr[], WCHAR disp[]) {
	UINT numOfList = _wtoi(aFileList[0]);
	swprintf(aFileList[0], L"%d", numOfList + 1);
	swprintf(aFileList[numOfList + 1], L"%s\n\0", fRoute);

	//화면 갱신
	swprintf(disp, sizeof(L"리스트에 추가됨\0"), L"리스트에 추가됨\0");

	InvalidateRect(hWnd, &rt, TRUE);
	UpdateWindow(hWnd);
}

//true : 리스트에서 허용된 프로그램
//false : 리스트에서 허용되지 않은 프로그램
bool checkAllowed(WCHAR fRoute[], WCHAR** aFileList) {
	//캡쳐 실패 익셉션
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
	if (dwRC == 0)
		return 0;

	//Window Title 추출 루틴
	wchar_t wTitle[100];
	//버퍼 오버런 가능성
	GetWindowTextW(handleFocused, wTitle, sizeof(wTitle) / 2);
	fInfo->windowTitle = wTitle;

	char timestamp[16];
	wchar_t datetime[17];
	GetDateTime(timestamp);
	charToWChar(timestamp, datetime);
	fInfo->datetime = datetime;

	//Stack around the variable 'wTitle' was corrupted. => 버퍼오버런 (/2로 해결)
	return 1;
}
