// ch6_typer_game.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "ch6_typer_game.h"
#include <string>
#define MAX_LOADSTRING 100

LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow) {

    //注册窗口类
    char szClassName[] = "MainWClass";
    WNDCLASSEX wndclass;

    wndclass.cbSize = sizeof(wndclass);
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = ::LoadIcon(hInstance, (LPSTR)ICN_APP);
    wndclass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = (LPSTR)(IDR_TYPER);
    wndclass.lpszClassName = szClassName;
    wndclass.hIconSm = NULL;

    ::RegisterClassEx(&wndclass);

    //创建主窗口
    HWND hWnd = ::CreateWindowEx(0,
                                 szClassName,//类名
                                 "键鼠输入处理",
                                 WS_OVERLAPPEDWINDOW,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 CW_USEDEFAULT,
                                 NULL,
                                 NULL,
                                 hInstance,
                                 NULL
    );
    if (NULL == hWnd) {
        ::MessageBox(NULL, "创建窗口失败", "错误", MB_OK);
        return -1;
    }

    //显示窗口
    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);

    //从操作系统的消息队列中不断的捡取消息
    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) {
        //对底层消息的抓换，更好理解，以 WM_CHAR 形式提供给开发者
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam) {
    static std::string str ="";
    switch (message) {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_COMMAND: {
        //低字节保留菜单消息
        switch (LOWORD(wparam)) {
        case ID_EXIT:
            ::SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = ::BeginPaint(hWnd, &ps);
        ::SetTextColor(hdc, RGB(0, 0, 0));
        ::SetBkColor(hdc, ::GetSysColor(COLOR_3DFACE));
        ::TextOut(hdc, 0, 0, str.c_str(), str.length());
        ::EndPaint(hWnd, &ps);
        break;
    }

    case WM_CHAR: {
        str = str + char(wparam);
        //
        ::InvalidateRect(hWnd, NULL, 0);
        return 0;
    }
    
    case WM_LBUTTONDOWN: {
        char szPoint[56];
        wsprintf(szPoint, "X=%d,Y=%d", LOWORD(lparam), HIWORD(lparam));
        str = szPoint;

        if (wparam & MK_SHIFT)
            str += " Shift 按下了";
        ::InvalidateRect(hWnd, NULL, 1);
        return 0;
    }


    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}