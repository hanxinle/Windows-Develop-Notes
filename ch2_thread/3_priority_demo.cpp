#include <stdio.h>
#include <Windows.h>

DWORD WINAPI ThreadIdle(LPVOID lpParam) {
    for (int i = 0; i < 10; ++i) {
        printf("Idle thread running\n");
    }
    return  0;
}

DWORD WINAPI threadNormal(LPVOID lpParam) {
    for (int i = 0; i < 10; ++i) {
        printf("Normal thread running\n");
    }
    return 0;
}

int main3(void) {
    DWORD dwThreadId;
    HANDLE h[2];
    
    h[0] = ::CreateThread (NULL, 0, ThreadIdle, NULL, CREATE_SUSPENDED, &dwThreadId);
    ::SetThreadPriority(h[0], THREAD_PRIORITY_IDLE);
    ::ResumeThread(h[0]);

    h[1] = ::CreateThread(NULL, 0, threadNormal, NULL, 0, &dwThreadId);

    //授信（通知），通知主线程任务完成
    ::WaitForMultipleObjects(
        2,                  //数量
        h,
        TRUE,               //都完成了，才能结束
        INFINITE
    );


    return 0;
}