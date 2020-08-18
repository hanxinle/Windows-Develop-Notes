// ch1dll_exports.cpp : 定义 DLL 的导出函数。
//

#include "pch.h"
#include "framework.h"
#include "ch1dll_exports.h"


// 这是导出变量的一个示例
CH1DLLEXPORTS_API int nch1dllexports=0;

// 这是导出函数的一个示例。
CH1DLLEXPORTS_API int fnch1dllexports(void)
{
    return 0;
}

// 这是已导出类的构造函数。
Cch1dllexports::Cch1dllexports()
{
    return;
}
