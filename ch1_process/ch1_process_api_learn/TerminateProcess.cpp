#include <iostream>
#include <windows.h>
using namespace std;

int main4() {
    DWORD dwid;
    cout << "Input process id:";
    cin >> dwid;
    
    //根据进程 ID 获取进程权限
    HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwid);
    BOOL bRet = FALSE;

    //终止该进程
    if (hProcess != nullptr) {
        bRet = ::TerminateProcess(hProcess, 0);
    }

    if (bRet) {
        cout << "Process end success." << endl;
    }else {
    cout << "Process end failure." << endl;
    }
    ::CloseHandle(hProcess);

    system("pause");

    return 0;
}