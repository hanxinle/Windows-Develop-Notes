//线程同步，线程A、B交替打印字母
#include <stdio.h>
#include <windows.h>
#include <process.h>


HANDLE g_hEventA, g_hEventB;

UINT __stdcall thread1(LPVOID) {
    while (TRUE) {
        WaitForSingleObject(g_hEventA, INFINITE);
        printf("A\n");
        SetEvent(g_hEventB);
        Sleep(200);
    }
    return 0;
}

UINT __stdcall thread2(LPVOID) {
    while (TRUE) {
        WaitForSingleObject(g_hEventB, INFINITE);
        printf("B\n");
        SetEvent(g_hEventA);
        Sleep(200);
    }

    return 0;
}

int main(void) {
    HANDLE h[2];
    h[0] = (HANDLE)_beginthreadex(NULL,0,thread1,NULL,0,NULL);
    h[1] = (HANDLE)_beginthreadex(NULL, 0, thread2, NULL, 0, NULL);

    g_hEventA = CreateEvent(NULL, FALSE, TRUE, NULL);
    g_hEventB = CreateEvent(NULL, FALSE, FALSE,NULL );

    WaitForMultipleObjects(2, h, TRUE, INFINITE);

    system("pause");
  
    return 0;
}