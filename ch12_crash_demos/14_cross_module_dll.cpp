// Demo 14: 跨模块内存分配/释放不匹配 - DLL 端.
// 这个文件编译为 DLL，演示跨模块分配和释放的问题.

#include <windows.h>
#include <cstdio>

#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

// 错误做法：分配内存，让调用者释放.
extern "C" MYDLL_API char* DLL_AllocBuffer_Bad(int size) {
    return new char[size];  // 在 DLL 的 CRT 堆中分配.
}

// 正确做法：提供配套的释放函数.
extern "C" MYDLL_API char* DLL_AllocBuffer_Good(int size) {
    return new char[size];
}

extern "C" MYDLL_API void DLL_FreeBuffer_Good(char* ptr) {
    delete[] ptr;  // 在 DLL 中释放，确保使用同一个 CRT 堆.
}

// 使用进程默认堆（跨模块安全）.
extern "C" MYDLL_API void* DLL_HeapAlloc(int size) {
    return HeapAlloc(GetProcessHeap(), 0, size);
}

extern "C" MYDLL_API void DLL_HeapFree(void* ptr) {
    HeapFree(GetProcessHeap(), 0, ptr);
}

// DLL 入口点.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            printf("  [DLL] 加载到进程\n");
            break;
        case DLL_PROCESS_DETACH:
            printf("  [DLL] 从进程卸载\n");
            break;
    }
    return TRUE;
}
