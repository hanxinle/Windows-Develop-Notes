// ch8_dialog_controller.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ch8_dialog_controller.h"

//通过 vc 手写代码，完成所有控件原生生成

#define IDC_BUTTON	10001
//单选按钮
#define IDC_RADIO	10002
//复选按钮
#define IDC_CHECKBOX	10003
//静态文本框
#define IDC_STATIC_TEXT	10004  
//可编辑文本
#define IDC_EDITTEXT	10005

//用于窗口与窗口之间的通信，定义全局 appInstance
HINSTANCE appInstance;
BOOL __stdcall DlgProc(HWND, UINT, WPARAM, LPARAM);

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	//全局变量 appInstance 感知到应用程序
	appInstance = hInstance;
	//绘制窗口，并且返回用户操作
	int nRes = ::DialogBoxParam(
		hInstance,
		(LPCTSTR)IDD_MAIN,//由resoure.h引入的对话框资源号
		NULL,//父窗口句柄为空
		DlgProc,
		NULL
	);
	if (nRes == IDOK) {
		::MessageBox(NULL, "用户单击了IDOK", "第一个对话框程序", MB_OK);
	}
	else {
		::MessageBox(NULL, "用户单击了IDCANCEL", "第一个对话框程序", MB_OK);

	}
	return 0;
}
BOOL __stdcall DlgProc(HWND hDlg, UINT msg,
	WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		//WM_INITDIALOG 是对 窗口创建类消息的封装
	case WM_INITDIALOG:
		//标准按钮
		::CreateWindowEx(0, "button", "标准按钮",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			50, 30, 110, 30, hDlg, (HMENU)IDC_BUTTON,
			appInstance,
			NULL
		);
		//单选框
		::CreateWindowEx(0, "button", "单选框",
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
			50, 70, 110, 30, hDlg, (HMENU)IDC_RADIO,
			appInstance,
			NULL);
		//复选框，与静态文本框内容变更有关
		::CreateWindowEx(0, "button", "复选框",
			WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
			50, 110, 110, 30,
			hDlg, (HMENU)IDC_CHECKBOX,
			appInstance,
			NULL);
		//静态文本
		::CreateWindowEx(0, "static", "静态文本",
			WS_CHILD | WS_VISIBLE | SS_SUNKEN,
			50, 150, 150, 60, hDlg, (HMENU)IDC_STATIC_TEXT,
			appInstance,
			NULL);
		//可编辑文本
		::CreateWindowEx(0, "edit", "编辑文本",
			WS_CHILD | WS_VISIBLE | ES_MULTILINE,
			50, 220, 150, 60, hDlg, (HMENU)IDC_EDITTEXT,
			appInstance,
			NULL);
		break;
	case WM_COMMAND: 
		//[] 消息的类别大方向除外，还需要根据 wParam、lParam 判定控件具体的消息类型
		switch (LOWORD(wParam))	{
		case IDC_BUTTON:
			::MessageBox(hDlg, "Hello", "标准按钮", MB_OK);
			break;
		case IDC_RADIO: {
			//拿到子窗口 hwnd，这里思路很巧妙
			BOOL bChecked = ::IsDlgButtonChecked(hDlg, IDC_RADIO);
			::EnableWindow (::GetDlgItem(hDlg, IDC_BUTTON), bChecked);
			//设置状态
			::CheckDlgButton (hDlg, IDC_RADIO, !bChecked);
		}
		break;
		case IDC_EDITTEXT: {
			//如果我改变了文本框值，那么静态框也有相应动作
			if (HIWORD(wParam) == EN_CHANGE) {
				if (IsDlgButtonChecked(hDlg, IDC_CHECKBOX)) {
					char sz[256];
					::GetWindowText(::GetDlgItem(hDlg, IDC_EDITTEXT), sz, 256);
					::SetWindowText(::GetDlgItem(hDlg, IDC_STATIC_TEXT), sz);
				}
			}
		}
		break;
		case IDOK:
			::EndDialog(hDlg, IDOK);
			break;
		case IDCANCEL:
			::EndDialog(hDlg, IDCANCEL);
			break;
		}
		break;
	}
	return 0;
}

