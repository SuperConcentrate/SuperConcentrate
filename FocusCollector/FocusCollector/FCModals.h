#ifndef __FCMODALS_H__
#define __FCMODALS_H__

#include <Windows.h>

#define ID_MOL_B_OK		3000

//MODAL
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MakeOrLoad(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

#endif