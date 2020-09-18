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
                                 "我的第一个GUI程序",
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

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam) {
    switch (message) {
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, message, wparam, lparam);
}