#include <stdio.h>
#include <windows.h>

DWORD WINAPI SubThread(LPVOID lpParam) {
    int i = 0;
    while (i < 10) {
        printf("sub thread loop %d.\n", i);
        i++;
    }
    return 0;
}

int main1(void) {
    HANDLE hthread;
    DWORD dwThreadId;

    hthread = ::CreateThread(
        NULL,                   //默认的安全属性
        NULL,                   //默认的堆栈
        SubThread,               //线程的入口地址
        NULL,                   //默认的线程参数
        0,                      //线程立即执行（推进到线程就绪系统，通知系统可以调度）
        &dwThreadId
    );

    printf("Main thread will run its subthread,subthread id is %d.\n", dwThreadId);

    system("pause");

    return 0;
}