#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <stdio.h>
using namespace std;

BOOL findFirst (DWORD dwValue);      //对目标程序进程空间首次遍历
BOOL findNext  (DWORD dwValue);       //找到多个地址，更改value的值再次遍历

//暂存匹配数字的地址，匹配数字的个数，目标进程的句柄
//4k b最多有 DWORD 类型 1024 个
DWORD g_arList[1024]; 
int g_ListCnt;
HANDLE g_hProcess;

void showList ();
BOOL WriteMemory (DWORD dwAddr, DWORD dwValue);


int main() {
    char szFileName[] = "02Testor.exe";
    STARTUPINFO si;
    memset(&si, 0, sizeof(si));

    PROCESS_INFORMATION pi;

    BOOL bRet = ::CreateProcess(
        NULL,
        szFileName,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    );
    g_hProcess = pi.hProcess;

    int target_value;
    cout << "Input value you want to change: ";
    cin >> target_value;


    findFirst(target_value);
    showList();

    while (g_ListCnt > 1) {
        cout << "Input another value try to locate it: ";
        cin >> target_value;
        findNext(target_value);
        showList();
    }

    cout << "Address of value " << target_value << " is: " << g_arList[0] << endl;
    cout << "Input the value you want to set for: " ;
    cin >> target_value;
    
    if (WriteMemory(g_arList[0], target_value)) {
        cout << "UPDATE SUCCESS.\n";
    }

    return 0;
}

BOOL compareInOnePage(DWORD dwBaseAddr, DWORD dwValue) {
    BYTE arBytes[4096];
    //验证进程内存空间可读否
    if (!::ReadProcessMemory(g_hProcess,
                             (LPVOID)dwBaseAddr,
                             arBytes,
                             4096,
                             NULL))
        return FALSE;

    DWORD * pdw;
    for (int i = 0; i < (int)4 * 1024 - 3; ++i) {
        pdw = (DWORD*)&arBytes[i];
        if (pdw[0] == dwValue) {
            if (g_ListCnt >= 1024) 
                return FALSE;
            g_arList[g_ListCnt++] = dwBaseAddr + i;
        }
    }
    return TRUE;
}

BOOL findFirst(DWORD dwValue) {
    const DWORD dwOneGB = 1024 * 1024 * 1024;

    const DWORD dwOnePage = 4 * 1024;

    if (g_hProcess == nullptr)
        return FALSE;

    DWORD dwBase = 640 * 1024;
    BOOL bRet = FALSE;
    //用户空间 640kb~2GB
    for (; dwBase < 2 * dwOneGB; dwBase += dwOnePage) {  
        compareInOnePage(dwBase, dwValue);
    }
    return TRUE;
}

BOOL findNext(DWORD dwValue) {
    //保存当前找到有效地址个数
    int nOrgCnt = g_ListCnt;
    g_ListCnt = 0;

    BOOL bRet = FALSE;
    DWORD dwReadValue;
    
    for (int i = 0; i < nOrgCnt; ++i) {
        if (::ReadProcessMemory(g_hProcess,
            (LPVOID)g_arList[i],
                                &dwReadValue,
                                sizeof(DWORD),
                                NULL
        )) {
            if (dwReadValue == dwValue) {
                g_arList[g_ListCnt++] = g_arList[i];
                bRet = TRUE;
            }
        }
    }
    
    return bRet;
}

void showList() {
    for (int i = 0; i < g_ListCnt; ++i) {
        printf("%p\n", g_arList[i]);
    }
}

BOOL WriteMemory(DWORD dwAddr, DWORD dwValue) {
    return ::WriteProcessMemory(g_hProcess,
                                (LPVOID)dwAddr,
                                &dwValue,
                                sizeof(DWORD),
                                NULL
                                );
}