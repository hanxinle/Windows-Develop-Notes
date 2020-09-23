// ch9_windows_communication.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ch9_windows_communication.h"
#include <windows.h>
#include "resource.h"

HINSTANCE appInstance;
//消息处理
BOOL __stdcall DlgProc(HWND, UINT, WPARAM, LPARAM);
char sz[256];//缓冲区
char szBuff[256];


//主窗口绘制
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	appInstance = hInstance;
	int nRes = ::DialogBoxParam(
		hInstance,
		(LPCTSTR)IDD_MAIN,
		NULL,
		DlgProc,
		NULL
	);
	return 0;
}

// dialog IDD_WIN 消息处理
BOOL __stdcall DlgProcWin(HWND hDlgWin, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message)
	{
	case WM_INITDIALOG:

		//获得编辑控件
		::SetWindowText(::GetDlgItem(hDlgWin, IDC_EDIT),
			szBuff);
		break;
	}
	return 0;
}

//IDD_MAIN消息处理
BOOL __stdcall DlgProc(HWND hDlg, UINT mes, WPARAM wParam, LPARAM lParam) {
	int nRes = 0;
	switch (mes)
	{
	case WM_COMMAND://侦测我们的子窗口中的控件ID的
		switch (LOWORD(wParam)) {
		case IDC_BTNConfirm:


			szBuff[0] = '\0';
			::GetWindowText(::GetDlgItem(hDlg, IDC_NAME), sz, 256);
			wsprintf(szBuff, "您的姓名是 %s", sz);
			memset(sz, 0, 256);
			::GetWindowText(::GetDlgItem(hDlg, IDC_AGE), sz, 256);
			wsprintf(szBuff + strlen(szBuff), "您的年龄是%d", atoi(sz));
			MessageBox(NULL, szBuff, "Hello", MB_OK);
			nRes = ::DialogBoxParam(
				appInstance,
				(LPCTSTR)IDD_WIN,
				NULL,
				DlgProcWin,//=>IDD_WIN,所以我们需要一个新的回调函数
				NULL
			);


			break;
		case IDOK:
			::EndDialog(hDlg, IDOK);
			break;
		case IDC_BTNRest:
			char szTEMP[] = "";
			::SetWindowText(::GetDlgItem(hDlg, IDC_NAME), szTEMP);
			::SetWindowText(::GetDlgItem(hDlg, IDC_AGE), szTEMP);
			break;
		}
		break;
	}
	return 0;
}

