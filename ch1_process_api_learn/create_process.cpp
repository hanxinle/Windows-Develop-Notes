/* 
* CreateProcess
* CloseHandle
* ExitProcess
* TerminateProcess
*/ 

#include <iostream>
#include <windows.h>
using namespace std;

int main() {
    //初始化 CreateProcess 参数
    char szCommandLine[] = "notepad.exe";
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));
    PROCESS_INFORMATION pi;

    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = TRUE;

    bool bRet = ::CreateProcess(
        NULL,			    // 不在此指定可执行文件的文件名
        szCommandLine,	    // 命令行参数
        NULL,			    // 默认进程安全性
        NULL,			    // 默认线程安全性
        FALSE,			    // 指定当前进程内的句柄不可以被子进程继承
        CREATE_NEW_CONSOLE,	// 为新进程创建一个新的控制台窗口
        NULL,			    // 使用本进程的环境变量
        NULL,			    // 使用本进程的驱动器和目录
        &si,
        &pi
    );

    if (bRet) {
        //关闭内核句柄
        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
        cout << "Process ID " << pi.dwProcessId << endl;
        cout << "Thread ID " << pi.dwThreadId << endl;

    }
    system("pause");

    return 0;
}

