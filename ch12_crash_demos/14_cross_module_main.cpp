// Demo 14: 跨模块内存分配/释放不匹配 - 主程序端.
// 演示跨模块分配和释放导致的堆损坏问题.

#include <windows.h>
#include <cstdio>

// DLL 导出的函数声明.
extern "C" __declspec(dllimport) char* DLL_AllocBuffer_Bad(int size);
extern "C" __declspec(dllimport) char* DLL_AllocBuffer_Good(int size);
extern "C" __declspec(dllimport) void DLL_FreeBuffer_Good(char* ptr);
extern "C" __declspec(dllimport) void* DLL_HeapAlloc(int size);
extern "C" __declspec(dllimport) void DLL_HeapFree(void* ptr);

// 加载 DLL 的辅助函数.
HMODULE LoadMyDll() {
    // 先尝试加载当前目录下的 DLL.
    HMODULE hMod = LoadLibraryA("CrossModuleDll.dll");
    if (!hMod) {
        printf("  加载 DLL 失败，错误码: %lu\n", GetLastError());
        printf("  请确保 DLL 已编译并放在同一目录下.\n");
    }
    return hMod;
}

void TestBadPattern() {
    printf("\n[测试 1] 错误做法：DLL 分配，主程序释放...\n");
    printf("  如果 DLL 和主程序使用不同的 CRT，这会导致堆损坏.\n");

    HMODULE hMod = LoadMyDll();
    if (!hMod) return;

    // 从 DLL 获取函数指针（如果直接链接失败）.
    typedef char* (*pfnAlloc)(int);
    pfnAlloc allocBad = (pfnAlloc)GetProcAddress(hMod, "DLL_AllocBuffer_Bad");

    if (!allocBad) {
        printf("  获取函数地址失败\n");
        FreeLibrary(hMod);
        return;
    }

    char* buffer = allocBad(100);
    if (buffer) {
        strcpy_s(buffer, 100, "Hello from DLL");
        printf("  DLL 分配内存成功，内容: %s\n", buffer);

        printf("  尝试在主程序中释放（这是错误的）...\n");
         delete[] buffer;  // 危险！注释掉避免崩溃.
        printf("  （已注释掉危险操作）\n");
        printf("  实际工程中，这种跨模块释放会导致堆损坏或崩溃.\n");

        // 正确的做法：让 DLL 自己释放.
        /*
        typedef void (*pfnFree)(char*);
        pfnFree freeGood = (pfnFree)GetProcAddress(hMod, "DLL_FreeBuffer_Good");
        if (freeGood) {
            freeGood(buffer);
            printf("  通过 DLL 的释放函数正确释放.\n");
        }
        */
    }

    FreeLibrary(hMod);
}

void TestGoodPattern() {
    printf("\n[测试 2] 正确做法：DLL 提供配套的分配/释放函数...\n");

    HMODULE hMod = LoadMyDll();
    if (!hMod) return;

    typedef char* (*pfnAlloc)(int);
    typedef void (*pfnFree)(char*);

    pfnAlloc alloc = (pfnAlloc)GetProcAddress(hMod, "DLL_AllocBuffer_Good");
    pfnFree freeBuf = (pfnFree)GetProcAddress(hMod, "DLL_FreeBuffer_Good");

    if (!alloc || !freeBuf) {
        printf("  获取函数地址失败\n");
        FreeLibrary(hMod);
        return;
    }

    char* buffer = alloc(100);
    if (buffer) {
        strcpy_s(buffer, 100, "Safe allocation");
        printf("  分配成功: %s\n", buffer);

        // 使用 DLL 提供的释放函数.
        freeBuf(buffer);
        printf("  通过配套函数正确释放.\n");
    }

    FreeLibrary(hMod);
}

void TestProcessHeap() {
    printf("\n[测试 3] 使用进程默认堆（最安全的跨模块方案）...\n");

    HMODULE hMod = LoadMyDll();
    if (!hMod) return;

    typedef void* (*pfnHeapAlloc)(int);
    typedef void (*pfnHeapFree)(void*);

    pfnHeapAlloc myAlloc = (pfnHeapAlloc)GetProcAddress(hMod, "DLL_HeapAlloc");
    pfnHeapFree myFree = (pfnHeapFree)GetProcAddress(hMod, "DLL_HeapFree");

    if (!myAlloc || !myFree) {
        printf("  获取函数地址失败\n");
        FreeLibrary(hMod);
        return;
    }

    void* buffer = myAlloc(100);
    if (buffer) {
        strcpy_s((char*)buffer, 100, "Process heap allocation");
        printf("  进程堆分配成功: %s\n", (char*)buffer);

        myFree(buffer);
        printf("  正确释放.\n");
    }

    FreeLibrary(hMod);
}

void TestDynamicLinkCRT() {
    printf("\n[测试 4] 动态链接 CRT 方案...\n");
    printf("  如果所有模块都使用 /MD 动态链接 CRT，\n");
    printf("  它们共享同一个 CRT 堆，跨模块分配释放是安全的.\n");
    printf("  在 VS 中设置: 项目属性 -> C/C++ -> 代码生成 -> 运行库 = 多线程 DLL (/MD)\n");
}

int main() {
    printf("=== Demo 14: 跨模块内存分配/释放不匹配 ===\n");
    printf("每个链接了静态 CRT 的 DLL 都有自己的堆.\n");
    printf("A 的 CRT 分配，B 的 CRT 释放，会导致崩溃.\n\n");

    TestBadPattern();
    TestGoodPattern();
    TestProcessHeap();
    TestDynamicLinkCRT();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 谁分配谁释放，不跨模块.\n");
    printf("  2. 提供配套的分配/释放函数.\n");
    printf("  3. 所有模块统一使用 /MD 动态链接 CRT.\n");
    printf("  4. 使用进程默认堆 HeapAlloc/HeapFree.\n");
    printf("  5. 不要跨模块传递 std::string/vector（它们的内存管理在模块内部）.\n");
    return 0;
}
