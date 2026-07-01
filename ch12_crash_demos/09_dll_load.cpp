// Demo 09: DLL 加载失败处理演示.
// 演示如何正确加载 DLL，处理失败情况，并给出用户友好的错误提示.

#include <windows.h>
#include <cstdio>

void TestLoadNonExistentDll() {
    printf("\n[测试 1] 加载不存在的 DLL...\n");

    HMODULE hMod = LoadLibraryW(L"this_dll_does_not_exist.dll");
    if (hMod == nullptr) {
        DWORD error = GetLastError();
        printf("  加载失败，错误码: %lu\n", error);
        printf("  错误信息: ");

        // 获取系统错误信息.
        LPWSTR msgBuffer = nullptr;
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            nullptr,
            error,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&msgBuffer,
            0,
            nullptr
        );

        if (msgBuffer) {
            char narrowMsg[512];
            WideCharToMultiByte(CP_UTF8, 0, msgBuffer, -1, narrowMsg, sizeof(narrowMsg), nullptr, nullptr);
            printf("%s", narrowMsg);
            LocalFree(msgBuffer);
        }
    } else {
        printf("  加载成功（不应该）.\n");
        FreeLibrary(hMod);
    }
}

void TestLoadDllWithErrorHandling() {
    printf("\n[测试 2] 正确的 DLL 加载错误处理...\n");

    const wchar_t* dllName = L"kernel32.dll";  // 系统 DLL，一定存在.

    // 方法 1：先检查文件是否存在.
    DWORD attribs = GetFileAttributesW(dllName);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        printf("  文件不存在，不需要尝试加载.\n");
        return;
    }

    // 方法 2：加载并检查返回值.
    HMODULE hMod = LoadLibraryW(dllName);
    if (hMod == nullptr) {
        printf("  加载失败（文件存在但加载失败，可能是位数不匹配或依赖缺失）.\n");
        return;
    }

    printf("  加载成功: %p\n", (void*)hMod);

    // 方法 3：获取函数地址前先检查.
    FARPROC proc = GetProcAddress(hMod, "GetCurrentProcessId");
    if (proc == nullptr) {
        printf("  获取函数地址失败.\n");
        FreeLibrary(hMod);
        return;
    }

    printf("  获取函数地址成功: %p\n", (void*)proc);

    // 使用函数.
    typedef DWORD(WINAPI *pfnGetCurrentProcessId)(void);
    pfnGetCurrentProcessId getId = (pfnGetCurrentProcessId)proc;
    DWORD pid = getId();
    printf("  调用结果: 当前进程 ID = %lu\n", pid);

    FreeLibrary(hMod);
}

// 测试 3：延迟加载 (Delay-Loading) 的概念演示.
// 实际工程中在链接器选项里配置 /DELAYLOAD:xxx.dll.
void TestDelayLoadConcept() {
    printf("\n[测试 3] 延迟加载 (Delay-Load) 概念...\n");
    printf("  传统静态链接：程序启动时加载所有 DLL.\n");
    printf("  延迟加载：第一次调用 DLL 函数时才加载.\n");
    printf("  好处：\n");
    printf("    1. 加快启动速度\n");
    printf("    2. 按需加载，不用的 DLL 不占用内存\n");
    printf("    3. 减少启动时 DLL 注入的机会（IAT 启动时为空）\n");
    printf("  VS 配置: 项目属性 -> 链接器 -> 输入 -> 延迟加载的 DLL\n");
    printf("  需要链接: #pragma comment(lib, \"delayimp.lib\")\n");
}

// 测试 4：用户友好的错误提示.
void ShowUserFriendlyError(const wchar_t* dllName) {
    printf("\n[测试 4] 用户友好的错误提示...\n");

    HMODULE hMod = LoadLibraryW(dllName);
    if (hMod) {
        printf("  DLL 加载成功.\n");
        FreeLibrary(hMod);
        return;
    }

    DWORD error = GetLastError();

    printf("\n  =========================================\n");
    printf("  无法加载组件: ");
    char narrowName[256];
    WideCharToMultiByte(CP_UTF8, 0, dllName, -1, narrowName, sizeof(narrowName), nullptr, nullptr);
    printf("%s\n", narrowName);
    printf("  -----------------------------------------\n");
    printf("  可能原因:\n");
    printf("    1. 文件被杀毒软件误删除\n");
    printf("    2. 安装包损坏，DLL 文件缺失\n");
    printf("    3. 系统 DLL 版本过旧\n");
    printf("    4. 程序位数与 DLL 不匹配（32位/64位）\n");
    printf("  -----------------------------------------\n");
    printf("  建议操作:\n");
    printf("    1. 关闭杀毒软件后重新安装本程序\n");
    printf("    2. 从官网下载最新版本重新安装\n");
    printf("    3. 联系技术支持\n");
    printf("  =========================================\n");
    printf("  系统错误码: %lu\n", error);
}

int main() {
    printf("=== Demo 09: DLL 加载失败处理 ===\n");
    printf("演示如何正确加载 DLL 并处理各种失败情况.\n\n");

    TestLoadNonExistentDll();
    TestLoadDllWithErrorHandling();
    TestDelayLoadConcept();
    ShowUserFriendlyError(L"some_optional_feature.dll");

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 动态加载必须检查返回值.\n");
    printf("  2. 给用户明确的错误信息和解决方案.\n");
    printf("  3. 关键 DLL 启动前预检查是否存在.\n");
    printf("  4. 可选功能用延迟加载，不阻塞主流程.\n");
    printf("  5. 企业级软件使用 Authenticode 数字签名验证 DLL 完整性.\n");
    return 0;
}
