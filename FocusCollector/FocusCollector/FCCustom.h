#ifndef __FCCUSTOM_H__
#define __FCCUSTOM_H__

#include <Windows.h>

#define MAXFLIST			512
#define nFileNameMaxLen		512
#define MAX_TEXT			256
#define MYSELF L"C:\\Users\\kenei\\source\\repos\\WindowsProject1\\Debug\\WindowsProject1.exe\0"

const UINT sizeofBuffer = ((sizeof(WCHAR) * nFileNameMaxLen) + sizeof(WCHAR*) * MAXFLIST);			//aFileList�� ũ��
const UINT sizeofOutBuffer = ((sizeof(WCHAR) * nFileNameMaxLen) + (MAXFLIST * sizeof(L"\n")))   // ���� ������ ũ�� + ����
+ (sizeof(WCHAR*) * MAXFLIST)		//���� ������ ������ ũ��
+ (sizeof(L"\n\0"));

typedef struct _CURRENT_FOCUSED_EXE_INFORMATION_ {
	LPCWSTR fPath;
	HICON hIcon;
	LPWSTR appName;
	LPWSTR windowTitle;
	wchar_t* datetime;
}FOCUSINFO;

//void renewStatusText(HWND hWnd, RECT rt, );
WCHAR* extractFileName(WCHAR fRoute[]);

WCHAR** makeNewList(HWND hWnd, RECT rt, WCHAR curr[], WCHAR disp[]);
void saveList(HWND hWnd, WCHAR fRoute[], WCHAR** aFileList, RECT rt, WCHAR curr[], WCHAR disp[]);
WCHAR** loadList(HWND hWnd, WCHAR fRoute[], RECT rt, WCHAR curr[], WCHAR disp[]);
void addToList(HWND hWnd, WCHAR fRoute[], WCHAR** aFileList, RECT rt, WCHAR curr[], WCHAR disp[]);

bool checkAllowed(WCHAR fRoute[], WCHAR** aFileList);

void charToWChar(const char* pstrSrc, wchar_t pwstrDest[]);
void wCharToChar(const wchar_t* pwstrSrc, char pstrDest[]);
void lptToChar(const LPTSTR lptstrSrc, char pstrDest[]);
void GetDateTime(char* datetime);

int GetCurrExeInfo(FOCUSINFO* fInfo);

#endif


