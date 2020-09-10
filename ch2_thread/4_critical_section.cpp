#include <stdio.h>
#include <process.h>
#include <windows.h>

int g_Cnt = 0;
//全局临界区
CRITICAL_SECTION g_cs;


UINT __stdcall ThreadFunc(LPVOID);
BOOL bFlag = FALSE;

int main4(void) {
    UINT uId;
    HANDLE h[2];
    
    ::InitializeCriticalSection(&g_cs);

    h[0] = (HANDLE)::_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, &uId);
    h[1] = (HANDLE)::_beginthreadex(NULL, 0, ThreadFunc, NULL, 0, &uId);
    //主线程休息1000ms,让出CPU的使用权，让我们辅助线程有机会运行
    bFlag = TRUE;
    Sleep(1000);
    bFlag = FALSE;

    ::WaitForMultipleObjects(2, h, TRUE, INFINITE);
    
    CloseHandle(h[0]);
    CloseHandle(h[1]);

    ::DeleteCriticalSection(&g_cs);

    printf("g_Cnt=%d\n", g_Cnt);
    system("pause");
    return 0;
}
//线程的编程的重点：与时间有关的错误
UINT __stdcall ThreadFunc(LPVOID) {
    if (bFlag) {
        //::EnterCriticalSection(&g_cs);

        for (int i = 0; i < 10000; i++)
            ////g_Cnt++;                          
            //将 g_Cnt++ 替换为 互锁函数
            ::InterlockedIncrement((long*)&g_Cnt);  

        //::LeaveCriticalSection(&g_cs);
    }
    return 0;
}
