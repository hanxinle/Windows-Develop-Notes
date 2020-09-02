#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
using namespace std;

int main2() {
    //保存快照的信息
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(pe32);

    HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        cout << "Create failure" << endl;
        return -1;

    }
    //遍历进程快照信息
    BOOL bMore = ::Process32First(hProcessSnap, &pe32);
    while (bMore) {
        cout << "Process Name: " << pe32.szExeFile << endl;
        cout << "Process ID: " << pe32.th32ProcessID << endl;
        bMore = ::Process32Next(hProcessSnap, &pe32);
    }
    ::CloseHandle(hProcessSnap);
    system("pause");

    return 0;
}