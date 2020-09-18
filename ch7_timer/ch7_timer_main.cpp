#include <stdio.h>
#include <windows.h>
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
    wndclass.hIcon = ::LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szClassName;
    wndclass.hIconSm = NULL;

    ::RegisterClassEx(&wndclass);

    //创建主窗口
    HWND hWnd = ::CreateWindowEx(0,
                                 szClassName,//类名
                                 "定时器程序",
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
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    return msg.wParam;
}

#define IDT_TIMER 100801
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam) {
    static int nNUM;
    static int bSetTimer;
    char szText[56];
    PAINTSTRUCT  ps;
    HDC hdc;

    switch (message) {
    case WM_CREATE:
        bSetTimer = false;
        break;

    case WM_PAINT:
        hdc = ::BeginPaint(hWnd, &ps);
        wsprintf(szText, "计数： %d", nNUM);
        ::TextOut(hdc, 10, 10, szText, strlen(szText));
        ::EndPaint(hWnd, &ps);
        break;
    case WM_LBUTTONDOWN:
        if (bSetTimer) {
            ::KillTimer(hWnd, IDT_TIMER);
            bSetTimer = false;
        } else {
            if (::SetTimer(hWnd, IDT_TIMER, 1000, NULL) == 0) {
                ::MessageBox(hWnd, "安装定时器失败","Timer Error",MB_OK);
            } else {
                bSetTimer = TRUE;
            }
        }
        break;
    case WM_CLOSE:
        if (bSetTimer)
            ::KillTimer(hWnd, IDT_TIMER);
                break;
    case WM_TIMER:
        if (wparam == IDT_TIMER) {
            hdc = GetDC(hWnd);
            wsprintf(szText, "计数： %d", nNUM++);
            ::TextOut(hdc, 10, 10, szText, strlen(szText));        }
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}