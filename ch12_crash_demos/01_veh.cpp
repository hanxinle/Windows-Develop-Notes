// Demo 01: VEH (Vectored Exception Handling) 向量异常处理演示.
// 本程序注册 VEH 处理程序，然后故意触发空指针访问异常.
// VEH 会比 SEH 更早被调用，可以抢先处理异常.
//
// 宏 ENABLE_MINIDUMP 控制是否在崩溃时生成 MiniDump:
//   #define ENABLE_MINIDUMP 1  // 启用 dump 生成
//   #define ENABLE_MINIDUMP 0  // 禁用 dump 生成

#define ENABLE_MINIDUMP 1

#include <windows.h>
#include <cstdio>

#if ENABLE_MINIDUMP
#include <dbghelp.h>
#pragma comment(lib, "DbgHelp.lib")
#endif

// VEH 回调函数，异常发生时最先被调用.
LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    printf("[VEH] Exception caught!\n");
    printf("[VEH] Exception code: 0x%08X\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
    printf("[VEH] Exception address: 0x%p\n", pExceptionInfo->ExceptionRecord->ExceptionAddress);

    // 可以查看 CPU 寄存器上下文.
    PCONTEXT ctx = pExceptionInfo->ContextRecord;
    printf("[VEH] RIP/EIP = 0x%llX\n", ctx->Rip);

    // 返回 EXCEPTION_CONTINUE_SEARCH 让后续处理者继续处理.
    // 如果返回 EXCEPTION_CONTINUE_EXECUTION，则需要修复异常（如修改 PC 指针）.
    printf("[VEH] Not handled, continue searching...\n\n");
    return EXCEPTION_CONTINUE_SEARCH;
}

#if ENABLE_MINIDUMP
// 生成 MiniDump 文件到指定路径.
bool GenerateMiniDump(EXCEPTION_POINTERS* pExceptionPointers, const wchar_t* path) {
    HANDLE hFile = CreateFileW(
        path,
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to create dump file, error: %lu\n", GetLastError());
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION mei;
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = pExceptionPointers;
    mei.ClientPointers = FALSE;

    MINIDUMP_TYPE dumpType = MiniDumpWithFullMemory;

    BOOL result = MiniDumpWriteDump(
        GetCurrentProcess(),
        GetCurrentProcessId(),
        hFile,
        dumpType,
        &mei,
        nullptr,
        nullptr
    );

    CloseHandle(hFile);

    if (result == TRUE) {
        return true;
    } else {
        printf("MiniDumpWriteDump failed, error: %lu\n", GetLastError());
        return false;
    }
}

// 未处理异常过滤器，崩溃前的最后机会，用于生成 dump.
LONG WINAPI MiniDumpExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    printf("\n[UEF] Unhandled exception detected, generating MiniDump...\n");

    // dump 路径与 exe 同名，扩展名改为 .dmp.
    wchar_t dumpPath[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, dumpPath, MAX_PATH);

    wchar_t* ext = wcsrchr(dumpPath, L'.');
    if (ext) {
        wcscpy_s(ext, MAX_PATH - (ext - dumpPath), L".dmp");
    } else {
        wcscat_s(dumpPath, MAX_PATH, L".dmp");
    }

    if (GenerateMiniDump(pExceptionInfo, dumpPath)) {
        wprintf(L"[UEF] MiniDump saved to: %s\n", dumpPath);
    } else {
        printf("[UEF] Failed to generate MiniDump.\n");
    }

    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

int main() {
    printf("=== Demo 01: VEH (Vectored Exception Handling) ===\n");
    printf("Step 1: Register VEH handler (highest priority)...\n");

    // 注册 VEH，第一个参数为 1 表示放在链表最前面，最先调用.
    PVOID vehHandle = AddVectoredExceptionHandler(1, VectoredHandler);
    if (!vehHandle) {
        printf("Failed to register VEH!\n");
        return 1;
    }
    printf("VEH registered successfully.\n\n");

#if ENABLE_MINIDUMP
    // 注册全局未处理异常过滤器，在崩溃时生成 dump.
    SetUnhandledExceptionFilter(MiniDumpExceptionFilter);
    printf("MiniDump generation enabled (ENABLE_MINIDUMP = 1).\n");
#else
    printf("MiniDump generation disabled (ENABLE_MINIDUMP = 0).\n");
#endif

    printf("Step 2: Intentionally trigger null pointer access exception...\n");
    int* p = nullptr;
    *p = 42;  // 这里会触发 Access Violation (0xC0000005).

    // This line will not be reached.
    printf("This line will not be printed.\n");

    RemoveVectoredExceptionHandler(vehHandle);
    return 0;
}
