// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "FocusCollector.h"
#include "FCCustom.h" //backend
#include "FCModals.h"

#include <ObjIdl.h>
#include <windef.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <ShellAPI.h>
#include <commdlg.h> //openfilename
#include <time.h>

using namespace Gdiplus;

#pragma warning(disable:4996)

#pragma comment(lib, "Gdiplus.lib")
#pragma comment(lib, "Dwmapi.lib")
//#pragma warning(disable:4996)

#define MAX_LOADSTRING 100


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

static LPDRAWITEMSTRUCT lpdis;
static HDC hMemDC;

//포커스 캡쳐 전역 변수
bool isCapturing = false;
FOCUSINFO* fInfo;
int fInfoRetVal;
static HANDLE hTimer;
time_t mytime;
//time_t tnow;
float fTotalSec = 0;
bool isAllowed = true;


//전체 시간
clock_t startTime, endTime;
//비허용 시간 (forbidden app)
clock_t  fStartTime, fEndTime;

int total = 0, tHour = 0, tMin = 0, tSec = 0;
int totalRest = 0, trHour = 0, trMin = 0, trSec = 0;
WCHAR outputMsg[300] = { 0, };

//버튼 핸들
HWND closeBtn, minBtn, settingBtn, startBtn, stopBtn;

// 허용 파일 목록
int numOfFiles = 0;
WCHAR** aFileList;

//Bitmap
NOTIFYICONDATA m_stData;

//GDI init
ULONG_PTR gdiplusToken;

GdiplusStartupInput gdiplusStartupInput;
HMODULE g_hInst;
static int xClick, yClick;

//Text RECT
RECT rt = { 61, 212, 500, 300 };
RECT infoRt = { 70, 160, 380, 198 };

//Images
Image* logo;
static HBITMAP basicBtn, cInputBox;


//FileControl - Common
//const UINT nFileNameMaxLen = 512;
WCHAR szCurrFileName[nFileNameMaxLen] = L"NOFILE\0";
WCHAR szDisplayStr[nFileNameMaxLen * sizeof(WCHAR)] = L"로드된 리스트가 없습니다.";


//FileControl - LoadList
//HWND hButtonLoadListDialog;
//HWND hEditLLDToBeOpened;
OPENFILENAME LLD;
WCHAR szLLDFileName[nFileNameMaxLen];

//FileControl - SaveList
OPENFILENAME SLD;
WCHAR szSLDFileName[nFileNameMaxLen];

//FileControl - AddProg
OPENFILENAME APD;
WCHAR szAPDFileName[nFileNameMaxLen];



//TrayMenu
#define WM_TRAY 0x2345
#define TRAY_ID 0x1234
#define MENU_INFO_MESSAGE   0x101
#define MENU_OPEN_MESSAGE   0x102
#define MENU_QUIT_MESSAGE   0x103

HMENU htMenu, htPopupMenu, htMenubar;

//Setting Menu
HMENU hsMenu, hsPopupMenu, hsMenubar;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

ATOM TrayRegisterClass(HINSTANCE hInstance);
//LONG TrayIconMessage(WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK MainDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK aTest(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

Image* LoadPNG(HMODULE hModule, LPCWSTR ResourceName);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);



    // TODO: 여기에 코드를 입력합니다.
    g_hInst = hInstance;

    // 전역 문자열을 초기화합니다.

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    //TrayRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    GdiplusShutdown(gdiplusToken);

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

Image* LoadPNG(HMODULE hModule, LPCWSTR ResourceName) {
    Image* image = NULL;
    IStream* pStream = NULL;

    HRSRC hResource = FindResource(hModule, ResourceName, L"PNG");
    if (CreateStreamOnHGlobal(NULL, TRUE, &pStream) == S_OK) {
        PVOID pResourceData = LockResource(LoadResource(hModule, hResource));
        DWORD imageSize = SizeofResource(hModule, hResource);
        DWORD dwReadWrite = 0;
        pStream->Write(pResourceData, imageSize, &dwReadWrite);
        image = Image::FromStream(pStream);
        pStream->Release();
    }
    return image;
}


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM TrayRegisterClass(HINSTANCE hInstance)
{
    htMenu = CreateMenu();
    htMenubar = CreateMenu();

    AppendMenu(htMenu, MF_STRING, MENU_INFO_MESSAGE, TEXT("초집중 정보"));
    AppendMenu(htMenu, MF_STRING, MENU_OPEN_MESSAGE, TEXT("열기"));
    AppendMenu(htMenu, MF_STRING, MENU_QUIT_MESSAGE, TEXT("프로그램 종료"));
    AppendMenu(htMenubar, MF_POPUP, (UINT_PTR)htMenu, TEXT("트레이메뉴"));

    memset(&m_stData, 0x00, sizeof(m_stData));

    m_stData.cbSize = sizeof(NOTIFYICONDATA);
    m_stData.hWnd = (HWND)hInstance;
    m_stData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_stData.uCallbackMessage = WM_TRAY;
    m_stData.uID = TRAY_ID;

    m_stData.hIcon = //(HICON)LoadImage(hInstance, TEXT("icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
        LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));

    lstrcpy(m_stData.szTip, TEXT("초집중"));

    return Shell_NotifyIcon(NIM_ADD, &m_stData);
    //SendMessage(WM_SETICON, (WPARAM)True, (LPARAM)nid.hIcon);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_POPUPWINDOW,
        50, 50, 430, 270, nullptr, nullptr, hInstance, nullptr);

    MARGINS margin{ 0, 0, 0, 1 };
    DwmExtendFrameIntoClientArea(hWnd, &margin);
    SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

    fInfo = (FOCUSINFO*)malloc(sizeof(FOCUSINFO));
    //memset(fInfo,0,sizeof(FOCUSINFO));

    //DWORD classLong = GetClassLong(hWnd, GCL_STYLE);
    //classLong = classLong | CS_DROPSHADOW;
    //SetClassLong(hWnd, GCL_STYLE, classLong);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //time_t mytime;

    switch (message)
    {
    case WM_TRAY:
    {
        if (wParam == TRAY_ID) {
            switch (lParam) {
            case WM_RBUTTONUP:
                htPopupMenu = GetSubMenu(htMenubar, 0);
                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hWnd);
                TrackPopupMenu(htPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
                SetForegroundWindow(hWnd);
                PostMessage(hWnd, WM_NULL, 0, 0);
                break;

            case WM_LBUTTONDBLCLK:
                ShowWindow(hWnd, SW_SHOWNORMAL);
            }
        }
    }

    case WM_CREATE:
    {
        TrayRegisterClass((HINSTANCE)hWnd);
        basicBtn = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
        cInputBox = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));

        closeBtn = CreateWindow(L"Button", L"Close", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, 400, 15, 10, 10, hWnd, (HMENU)IDB_CLOSE, hInst, NULL);
        minBtn = CreateWindow(L"Button", L"Minimize", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_OWNERDRAW, 380, 15, 10, 10, hWnd, (HMENU)IDB_MINIMIZE, hInst, NULL);
        settingBtn = CreateWindow(L"Button", L"Setting", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | ES_AUTOHSCROLL, 20, 15, 10, 10, hWnd, (HMENU)IDB_SETTING, hInst, NULL);
        startBtn = CreateWindow(L"Button", L"Start", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | ES_AUTOHSCROLL, 295, 207, 80, 35, hWnd, (HMENU)IDB_START, hInst, NULL);
        stopBtn = CreateWindow(L"Button", L"Start", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | ES_AUTOHSCROLL, 295, 207, 80, 35, hWnd, (HMENU)IDB_STOP, hInst, NULL);

        ShowWindow(stopBtn, SW_HIDE);

        //HWND hCodeInput = CreateWindow(L"edit", L"code here", WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | ES_AUTOHSCROLL, 65, 178, 325, 35, hWnd, (HMENU)IDI_CINPUT, g_hInst, NULL);


        hsMenu = CreateMenu();
        hsMenubar = CreateMenu();

        AppendMenu(hsMenu, MF_STRING, MENU_SETTING_NEWLIST, TEXT("새 리스트"));
        AppendMenu(hsMenu, MF_STRING, MENU_SETTING_LOADLIST, TEXT("리스트 불러오기"));
        AppendMenu(hsMenu, MF_STRING, MENU_SETTING_SAVELIST, TEXT("리스트 저장"));
        AppendMenu(hsMenu, MF_STRING, MENU_SETTING_ADDPROG, TEXT("프로그램 추가"));
        AppendMenu(hsMenu, MF_STRING, IDM_EXIT, TEXT("종료"));
        AppendMenu(hsMenubar, MF_POPUP, (UINT_PTR)hsMenu, TEXT("설정"));

        hTimer = (HANDLE)SetTimer(hWnd, 1, 1000, NULL);

    }
    break;

    case WM_DRAWITEM:
    {
        lpdis = (LPDRAWITEMSTRUCT)lParam;
        hMemDC = CreateCompatibleDC(lpdis->hDC);
        SelectObject(hMemDC, basicBtn);
        switch (lpdis->CtlID)
        {
        case IDB_CLOSE:
            BitBlt(lpdis->hDC, 0, 0, 10, 10, hMemDC, 0, 0, SRCCOPY);
            break;

        case IDB_MINIMIZE:
            BitBlt(lpdis->hDC, -10, 0, 20, 10, hMemDC, 0, 0, SRCCOPY);
            break;

        case IDB_SETTING:
            //BitBlt(lpdis->hDC, -40, 0, 50, 10, hMemDC, 0, 0, SRCCOPY);
            BitBlt(lpdis->hDC, 0, 0, 10, 10, hMemDC, 40, 11, SRCCOPY);
            break;

        case IDB_START:
            SelectObject(hMemDC, cInputBox);
            BitBlt(lpdis->hDC, 0, 0, 80, 36, hMemDC, 0, 66, SRCCOPY);
            break;

        case IDB_STOP:
            SelectObject(hMemDC, cInputBox);
            BitBlt(lpdis->hDC, 0, 0, 80, 36, hMemDC, 160, 66, SRCCOPY);
            break;

            //case IDI_CINPUT:
            //    SelectObject(hMemDC, cInputBox);
            //    if (lpdis->itemState & ODS_SELECTED) {
            //        BitBlt(lpdis->hDC, 0, 0, 323, 33, hMemDC, 0, 0, SRCCOPY);
            //    }
            //    else {
            //        BitBlt(lpdis->hDC, 0, 0, 323, 33, hMemDC, 0, 0, SRCCOPY);
            //    }

            //    break;
        }

        //if (lpdis->itemState & ODS_SELECTED)
        //if (lpdis->itemState & ODS_FOCUS)
        //    SelectObject(hMemDC, cHoverBit);

        DeleteDC(hMemDC);
    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam); //TRAY_DETAIL
        // 메뉴 선택을 구문 분석합니다:
        switch (wmId)
        {
        case IDB_MINIMIZE:
            ShowWindow(hWnd, SW_MINIMIZE);
            break;

        case IDB_CLOSE:
            ShowWindow(hWnd, SW_HIDE);  //트레이 완성 전까지 활성화 금지
            break;

        case IDB_SETTING:
            hsPopupMenu = GetSubMenu(hsMenubar, 0);
            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hWnd);
            TrackPopupMenu(hsPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
            SetForegroundWindow(hWnd);
            PostMessage(hWnd, WM_NULL, 0, 0);
            break;

        case IDB_START:
            if (0 != wcscmp(szCurrFileName, L"NOFILE")) {
                //START CAPTURING ROUTINE
                //버튼 및 활성 상태 전환
                isCapturing = !isCapturing;

                startTime = clock();
                ShowWindow(stopBtn, SW_SHOW);
                ShowWindow(startBtn, SW_HIDE);
            }
            else {
                MessageBox(hWnd, L"로드된 리스트가 없습니다.\n리스트를 로드하거나 새로 만든 후 시작 버튼을 누르십시오.", L"로드된 리스트 없음", MB_OK);
            }
            break;

        case IDB_STOP:
            //버튼 및 활성 상태 전환
            isCapturing = !isCapturing;

            endTime = clock();
            ShowWindow(stopBtn, SW_HIDE);
            ShowWindow(startBtn, SW_SHOW);

            total = (int)(((float)(endTime - startTime)) / 1000);
            totalRest = total - (int)(fTotalSec);

            tMin = total / 60;
            tHour = tMin / 60;
            tSec = total % 60;
            tMin = tMin % 60;

            trMin = totalRest / 60;
            trHour = trMin / 60;
            trSec = totalRest % 60;
            trMin = trMin % 60;

            swprintf(outputMsg, L"추적이 종료되었습니다.\n총 %02d시간 %02d분 %02d초 중\n%02d시간 %02d분 %02d초 집중했습니다.(%.2f%%)\n수고하셨습니다.",
                tHour, tMin, tSec, trHour, trMin, trSec, ((float)totalRest / total) * 100);

            MessageBox(hWnd, outputMsg, L"추적 종료", MB_OK);

            break;


        case MENU_SETTING_NEWLIST:
            aFileList = makeNewList(hWnd, rt, szCurrFileName, szDisplayStr);
            break;

        case MENU_SETTING_LOADLIST:
            memset(&LLD, 0, sizeof(OPENFILENAME));
            LLD.lStructSize = sizeof(OPENFILENAME);
            LLD.hwndOwner = hWnd;
            LLD.lpstrFilter = L"초집중 리스트 파일(*.clist)\0*.clist\0\0";
            LLD.lpstrFile = szLLDFileName; //선택한 파일명 저장
            LLD.nMaxFile = nFileNameMaxLen;

            if (0 != GetOpenFileName(&LLD)) {
                aFileList = loadList(hWnd, LLD.lpstrFile, rt, szCurrFileName, szDisplayStr);
            }

            break;

        case MENU_SETTING_SAVELIST:

            if (0 != wcscmp(szCurrFileName, L"NOFILE")) {
                if (0 == wcscmp(szCurrFileName, L"NEWFILE\0")) {
                    // NEWFILE -> GETSAVEFINENAME DIALOG CALL

                    memset(&SLD, 0, sizeof(OPENFILENAME));
                    SLD.lStructSize = sizeof(OPENFILENAME);
                    SLD.hwndOwner = hWnd;
                    SLD.lpstrFilter = L"초집중 리스트 파일(*.clist)\0*.clist\0\0";
                    SLD.lpstrFile = szSLDFileName; //선택한 파일명 저장
                    SLD.nMaxFile = nFileNameMaxLen;

                    if (0 != GetSaveFileName(&SLD)) {
                        //SAVE
                        saveList(hWnd, SLD.lpstrFile, aFileList, rt, szCurrFileName, szDisplayStr);
                    }
                }
                else {
                    // OLDFILE -> NO ACTION (JUST SAVE)
                    saveList(hWnd, szCurrFileName, aFileList, rt, szCurrFileName, szDisplayStr);
                }
            }
            else {
                MessageBox(hWnd, L" 로드된 리스트가 없습니다.\n 리스트를 로드하거나 새로 만든 후 단추를 누르십시오.", L"로드된 리스트 없음", MB_OK);
            }
            break;

        case MENU_SETTING_ADDPROG:
            if (0 == wcscmp(szCurrFileName, L"NOFILE")) {
                if (IDCANCEL == MessageBox(hWnd, L" 로드된 리스트가 없습니다.\n 새 리스트를 만들까요?", L"로드된 리스트 없음", MB_OKCANCEL)) {
                    break;
                }
                else {
                    aFileList = makeNewList(hWnd, rt, szCurrFileName, szDisplayStr);
                }
            }

            memset(&APD, 0, sizeof(OPENFILENAME));
            APD.lStructSize = sizeof(OPENFILENAME);
            APD.hwndOwner = hWnd;
            APD.lpstrFilter = L"실행 파일(*.exe)\0*.exe\0\0";
            APD.lpstrFile = szAPDFileName;
            APD.nMaxFile = nFileNameMaxLen;

            GetOpenFileName(&APD);
            // ADD TO BUFFER
            addToList(hWnd, APD.lpstrFile, aFileList, rt, szCurrFileName, szDisplayStr);

            break;

        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case MENU_OPEN_MESSAGE:
            ShowWindow(hWnd, SW_SHOWNORMAL);
            break;

        case IDM_EXIT:
        case MENU_QUIT_MESSAGE:
            Shell_NotifyIcon(NIM_DELETE, &m_stData);
            DestroyWindow(hWnd);
            exit(0);
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }



    }
    break;

    case WM_NCCALCSIZE:
    {
        // Setting 0 as the message result when wParam is TRUE removes the
        // standard frame, but keeps the window shadow.
        if (wParam == TRUE)
        {
            SetWindowLong(hWnd, DWL_MSGRESULT, 0);
            return TRUE;
        }
        return FALSE;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps{ 0 };
        HDC hdc = BeginPaint(hWnd, &ps);

        Gdiplus::Graphics gfx{ hdc };
        Gdiplus::SolidBrush brush{ Gdiplus::Color{ 255, 255, 255 } };
        gfx.FillRectangle(&brush, (INT)ps.rcPaint.left, ps.rcPaint.top,
            ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);

        logo = Image::FromFile(L"./rsrc/logo.PNG");

        DrawText(hdc, szDisplayStr, -1, &rt, DT_LEFT | DT_SINGLELINE);

        ::Graphics g(hdc);
        g.DrawImage(logo, 156, 33, 125, 125);

        delete logo;

        //STATUS
        if (isCapturing == true) {
            if (GetCurrExeInfo(fInfo) == 0)
                break;

            //fInfo curruption
            //WCHAR fRoute[nFileNameMaxLen] = { 0, };
            //wcscat_s(fRoute, fInfo.fPath);
            //if (!checkAllowed((WCHAR*)fInfo->fPath, aFileList)) {
            //    isCapturing = false;
            //    if (IDOK == MessageBox(hWnd, L" 허용되지 않은 프로그램을 실행중입니다.\n 이 프로그램을 리스트에 포함할까요?", L"허용되지 않은 프로그램", MB_OKCANCEL)) {
            //        addToList(hWnd, (WCHAR*)fInfo->fPath, aFileList,rt,szCurrFileName,szDisplayStr);
            //    }
            //    isCapturing = true;
            //}

            if (!checkAllowed((WCHAR*)fInfo->fPath, aFileList)) {
                if (!isAllowed) {
                    break; // 이전에도 false였음
                }
                else {
                    isAllowed = false; //처음 진입함
                    fStartTime = clock();
                }
            }
            else {
                if (!isAllowed) {
                    //이전까지 false였음 (false 이후 처음 true 진입함)
                    fEndTime = clock();

                    fTotalSec += ((float)(fEndTime - fStartTime)) / 1000;
                }
                else {
                    //이전에도 true였음 (최초 실행)
                    break;
                }
            }

            wchar_t outText[MAX_TEXT] = L"캡쳐됨 : ";
            wcscat_s(outText, fInfo->appName);
            wcscat_s(outText, L"\n실행 시간 : ");
            wcscat_s(outText, fInfo->datetime);

            DrawText(hdc, outText, -1, &infoRt, DT_WORDBREAK | DT_NOCLIP | DT_RIGHT);
            DrawIcon(hdc, 106, 160, fInfo->hIcon);
        }



        ReleaseDC(hWnd, hdc);
        EndPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
    }
    break;

    case WM_LBUTTONDOWN:
    {
        SetCapture(hWnd);

        xClick = LOWORD(lParam);
        yClick = HIWORD(lParam);
    }
    break;

    case WM_LBUTTONUP:
    {
        ReleaseCapture();
    }
    break;

    case WM_MOUSEMOVE:
        if (GetCapture() == hWnd)
        {
            RECT rcWindow;;

            GetWindowRect(hWnd, &rcWindow);

            int xMouse = LOWORD(lParam);
            int yMouse = HIWORD(lParam);

            int xWindow = rcWindow.left + xMouse - xClick;
            int yWindow = rcWindow.top + yMouse - yClick;

            SetWindowPos(hWnd, NULL, xWindow, yWindow, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        break;

    case WM_TIMER:
        // 매 초마다 화면 갱신
        time(&mytime);

        if (isCapturing) {
            InvalidateRect(hWnd, &infoRt, TRUE);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}