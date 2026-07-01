// Demo 03: Unhandled Exception Filter + MiniDump 生成.
// 这是工程中最常用的崩溃捕获方案：注册全局最后机会处理 + 生成 dump 文件.

#include <windows.h>
#include <dbghelp.h>
#include <cstdio>
#include <cstring>

#pragma comment(lib, "DbgHelp.lib")

// 保存旧的 UEF，以便链式调用.
LPTOP_LEVEL_EXCEPTION_FILTER g_oldFilter = nullptr;

// 生成 MiniDump 文件.
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
        printf("  创建 dump 文件失败，错误码: %lu\n", GetLastError());
        return false;
    }

    MINIDUMP_EXCEPTION_INFORMATION mei;
    mei.ThreadId = GetCurrentThreadId();
    mei.ExceptionPointers = pExceptionPointers;
    mei.ClientPointers = FALSE;

    // 使用 MiniDumpNormal，体积小，适合线上.
    MINIDUMP_TYPE dumpType = MiniDumpNormal;

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
        printf("  MiniDump 生成成功: ");
        // 简单打印路径（wchar_t 转 char）.
        char narrowPath[512] = {0};
        WideCharToMultiByte(CP_UTF8, 0, path, -1, narrowPath, sizeof(narrowPath), nullptr, nullptr);
        printf("%s\n", narrowPath);
        return true;
    } else {
        printf("  MiniDumpWriteDump 失败，错误码: %lu\n", GetLastError());
        return false;
    }
}

// 全局未处理异常过滤器，这是最后的机会.
LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    printf("\n[UEF] ========== 程序发生未处理异常 ==========\n");
    printf("[UEF] 异常代码: 0x%08X\n", pExceptionInfo->ExceptionRecord->ExceptionCode);
    printf("[UEF] 异常地址: 0x%p\n", pExceptionInfo->ExceptionRecord->ExceptionAddress);

    // 生成 dump 文件到程序所在目录.
    wchar_t dumpPath[MAX_PATH];
    GetModuleFileNameW(nullptr, dumpPath, MAX_PATH);

    // 将 .exe 替换为 .dmp.
    wchar_t* ext = wcsrchr(dumpPath, L'.');
    if (ext) {
        wcscpy_s(ext, MAX_PATH - (ext - dumpPath), L".dmp");
    } else {
        wcscat_s(dumpPath, MAX_PATH, L".dmp");
    }

    printf("[UEF] 正在生成 MiniDump...\n");
    GenerateMiniDump(pExceptionInfo, dumpPath);

    printf("[UEF] ========== 程序即将退出 ==========\n");

    // 调用旧的过滤器.
    if (g_oldFilter != nullptr) {
        return g_oldFilter(pExceptionInfo);
    }

    // 返回 EXCEPTION_EXECUTE_HANDLER 表示我们处理了，程序会退出.
    return EXCEPTION_EXECUTE_HANDLER;
}

void CauseCrash() {
    printf("\n触发崩溃：空指针写入...\n");
    int* p = nullptr;
    *p = 0xDEADBEEF;  // 触发 Access Violation.
}

int main() {
    printf("=== Demo 03: Unhandled Exception Filter + MiniDump ===\n");
    printf("这是实际工程中最常用的崩溃捕获方案.\n\n");

    printf("步骤 1: 注册全局未处理异常过滤器 (SetUnhandledExceptionFilter)...\n");
    g_oldFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
    printf("全局异常过滤器注册成功.\n");

    printf("\n步骤 2: 程序正常运行一段时间...\n");
    printf("模拟一些业务逻辑...\n");

    printf("\n步骤 3: 模拟代码中出现 bug，触发崩溃...\n");
    CauseCrash();

    // 不会执行到这里.
    printf("这行不会打印.\n");
    return 0;
}
