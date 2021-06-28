#include "FCModals.h"
#include <Windows.h>

//MODALS
INT_PTR CALLBACK MakeOrLoad(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    HWND startBtn;
    RECT rt = { 40, 40, 40, 40 };
    PAINTSTRUCT ps{ 0 };
    HDC hdc = BeginPaint(hDlg, &ps);

    switch (iMsg) {
    case WM_INITDIALOG:
        //CheckRadioButton(hDlg, 0, 0, 0);
        startBtn = CreateWindow(L"Button", L"Start", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 10, 10, 50, 50, hDlg, (HMENU)ID_MOL_B_OK, (HINSTANCE)hDlg, NULL);
        break;

    case WM_PAINT:

        DrawText(hdc, L"MAKEORLOAD", sizeof(L"MAKEORLOAD"), &rt, DT_LEFT | DT_SINGLELINE);
        //TextOutW(hdc, 10, 10, szCurrFileName, sizeof(szCurrFileName));

        ReleaseDC(hDlg, hdc);
        EndPaint(hDlg, &ps);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
            EndDialog(hDlg, IDOK);
        if (LOWORD(wParam) == IDCANCEL)
            EndDialog(hDlg, IDCANCEL);


    }
    return (INT_PTR)FALSE;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}