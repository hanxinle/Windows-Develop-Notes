/////////////////////////////////////////////////////////////////////////////
//显式动态链接
//////////////////////////////////////////////////////////////////////////
//#include "myfunc.h"
#include <windows.h>
#include <iostream>
using namespace std;

typedef int(*PFNEXPORTFUNC)(int, int);


int main() {

    int c = 12, d = 20;
    int result=0;
    int rusult2=0;

   
    HMODULE hModule = LoadLibraryA ("CH1DLLEXPORTS.dll");
 
    PFNEXPORTFUNC call_dll_func = (PFNEXPORTFUNC)GetProcAddress(hModule, "sub");
    if (call_dll_func != nullptr) {
        result = call_dll_func(c, d);
        cout << "result1 = " << result << endl;
    }

    PFNEXPORTFUNC call_dll_func2 = (PFNEXPORTFUNC)GetProcAddress(hModule, "sum");
    if (call_dll_func2 != nullptr) {
        rusult2 = call_dll_func2(c, d);
        cout << "result2 = " << rusult2 << endl;
    }

    FreeLibrary(hModule);

    //system("pause");
    return 0;
}


