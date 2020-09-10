//锁导致程序的推进效率降低，两个值相同但是变小了

#include <stdio.h>
#include <process.h>
#include <windows.h>

int g_Cnt_1 = 0;
int g_Cnt_2 = 0;

//全局临界区

CRITICAL_SECTION g_cs_1;


UINT __stdcall ThreadFunc1(LPVOID);
BOOL bFlag1 = FALSE;

int main5(void) {
    UINT uId;
    HANDLE h[2];

    ::InitializeCriticalSection(&g_cs_1);

    h[0] = (HANDLE)::_beginthreadex(NULL, 0, ThreadFunc1, NULL, 0, &uId);
    h[1] = (HANDLE)::_beginthreadex(NULL, 0, ThreadFunc1, NULL, 0, &uId);
    //主线程休息1000ms,让出CPU的使用权，让我们辅助线程有机会运行
    bFlag1 = TRUE;
    Sleep(1000);
    bFlag1 = FALSE;
    ::WaitForMultipleObjects(2, h, TRUE, INFINITE);
    CloseHandle(h[0]);
    CloseHandle(h[1]);

    ::DeleteCriticalSection(&g_cs_1);
    //值应该相同
    printf("g_Cnt_ = %d,g_Cnt_2 = %d\n", g_Cnt_1,g_Cnt_2);
    system("pause");
    return 0;
}

//线程的编程的重点：与时间有关的错误
UINT __stdcall ThreadFunc1(LPVOID) {
    while (bFlag1) {
        ::EnterCriticalSection(&g_cs_1);
        g_Cnt_1++;
        g_Cnt_2++;
        ::LeaveCriticalSection(&g_cs_1);
    }
    return 0;
}
