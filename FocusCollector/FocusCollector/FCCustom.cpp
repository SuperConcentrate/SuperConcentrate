#include "FCCustom.h"
#include <Windows.h>
#include <TlHelp32.h>

LPCWSTR GetCurrExeName() {
	HWND handleFocused;
	DWORD dwProcID;
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;

	DWORD pid = NULL;
	HANDLE pHandle = NULL;
	LPCWSTR rVal;

	handleFocused = GetForegroundWindow();
	GetWindowThreadProcessId(handleFocused, &pid);

	pHandle = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

	wchar_t buffer[MAX_PATH] = {};
	DWORD buffer_size = MAX_PATH;

	if (pHandle) {
		if (QueryFullProcessImageNameW(pHandle, 0, buffer, &buffer_size)) {
			rVal = buffer;
			CloseHandle(pHandle);
			return rVal;
		}
	}
	return L"ERROR : NOPROCESS";
}
