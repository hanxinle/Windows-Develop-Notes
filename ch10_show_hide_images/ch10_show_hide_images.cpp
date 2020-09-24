#include <Windows.h>
#include "resource.h"

BOOL __stdcall DlgProc(HWND, UINT, WPARAM, LPARAM);

//第一个图片，第二个图片句柄
HBITMAP g_hBitmap1;
HBITMAP g_hBitmap2;
HICON	g_hIcon;
HBRUSH	g_hBgBrush;

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	g_hBitmap1 = ::LoadBitmap(hInstance, (LPCTSTR)IDB_BITMAP1);
	g_hBitmap2 = ::LoadBitmap(hInstance, (LPCTSTR)IDB_BITMAP2);
	g_hIcon = ::LoadIcon(hInstance, (LPCTSTR)IDI_ICON1);
	//创建窗体背景刷子
	g_hBgBrush = ::CreateSolidBrush(RGB(0xa6, 0xca, 0xf0));
	int nRes = ::DialogBoxParam(
		hInstance,
		(LPCTSTR)IDD_DIALOG1,
		NULL,
		DlgProc,
		NULL
	);

	return 0;

}

BOOL __stdcall DlgProc(HWND hDlg, UINT mes, WPARAM wParam, LPARAM lParam)
{
	switch (mes) {
	case WM_INITDIALOG: {
		//加载图标
		::SendMessage(hDlg, WM_SETICON, ICON_BIG, (long)g_hIcon);
		//设置IDC_BMP风格，以便于位图加载
		HWND hWndBMP = ::GetDlgItem(hDlg, IDC_BMP);
		//SS_BTIMAP
		//向内核请求，找到句柄的 GWL_STYLE 项
		LONG nStyle = ::GetWindowLong(hWndBMP, GWL_STYLE);
		::SetWindowLong(hWndBMP, GWL_STYLE, nStyle | SS_BITMAP);
		//设置完毕
		::SendDlgItemMessage(hDlg, IDC_BMP, STM_SETIMAGE, IMAGE_BITMAP, (long)g_hBitmap1);
		//设置复选框状态
		::CheckDlgButton(hDlg, IDC_BMP1, BST_CHECKED);
	}
		break;
	case WM_CTLCOLORDLG://设置背景色
	{
		HDC hdc = (HDC)wParam;
		::SetBkColor(hdc, RGB(0xa6, 0xca, 0xf0));
		return (int)g_hBgBrush;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			HWND hWndEdit = ::GetDlgItem(hDlg, IDC_INPUT);
			//取得文本
			char szText[56];
			int nLen = ::GetWindowText(hWndEdit, szText, 56);
			if (nLen > 0) {
				//添加文本
				::SendDlgItemMessage(hDlg, IDC_LIST, LB_ADDSTRING, NULL, (long)szText);
				::SetWindowText(hWndEdit, "");
			}
		}
				   break;
		case IDCANCEL:
			::EndDialog(hDlg, IDCANCEL);
			break;
		case IDC_TOPMOST: {
			HWND hWndCheck = ::GetDlgItem(hDlg, IDC_TOPMOST);
			int nRet = ::SendMessage(hWndCheck, BM_GETCHECK, 0, 0);
			if (nRet == BST_CHECKED) {
				//置顶
				::SetWindowPos(hDlg, HWND_TOPMOST, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			}
			else {
				::SetWindowPos(hDlg, HWND_NOTOPMOST, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			}
		}
						  break;
		case IDC_BMP1: {
			int nRet = ::IsDlgButtonChecked(hDlg, IDC_BMP1);
			if (nRet == BST_CHECKED) {
				::SendDlgItemMessage(hDlg, IDC_BMP,
					STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hBitmap1);

			}
		}
					   break;
		case IDC_BMP2: {
			int nRet = ::IsDlgButtonChecked(hDlg, IDC_BMP2);
			if (nRet == BST_CHECKED) {
				::SendDlgItemMessage(hDlg, IDC_BMP,
					STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)g_hBitmap2);

			}
		}
					   break;
		case IDC_HIDE: {
			HWND hWndBmp = ::GetDlgItem(hDlg, IDC_BMP);
			HWND hWndCHK = ::GetDlgItem(hDlg, IDC_HIDE);
			int nRet = ::SendMessage(hWndCHK, BM_GETCHECK, 0, 0);
			if (nRet == BST_CHECKED)
				::ShowWindow(hWndBmp, SW_HIDE);
			else
				::ShowWindow(hWndBmp, SW_SHOW);
		}
					   break;
		}
		break;
	}
	return 0;
}