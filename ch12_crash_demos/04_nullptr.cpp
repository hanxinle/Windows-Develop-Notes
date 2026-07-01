// Demo 04: 空指针/野指针问题 - C++ catch vs SEH 对比.
// 本示例验证标准 C++ try/catch 无法捕获空指针硬件异常，必须使用 SEH.
//
// 修复说明：原代码在同一个函数中混用了 try/catch 与 __try/__except，
// 导致 MSVC 报错 C2713/C2712. 现将 C++ 异常处理封装到独立函数中，
// 外层使用 SEH 兜底，既保留了演示效果，也能正常编译.

#include <windows.h>
#include <cstdio>

// 内部函数：仅使用 C++ try/catch 尝试捕获空指针访问.
// 由于硬件异常不走 C++ 异常机制，catch (...) 不会执行，异常会向上传播.
void TestCppCatchInner() {
    try {
        int* p = nullptr;
        *p = 42;  // 硬件异常，C++ catch 无法捕获.
    }
    catch (...) {
        printf("  C++ catch (...) 捕获了!\n");
    }
}

void TestCppCatch() {
    printf("\n[测试 1] 用 C++ try/catch 捕获空指针访问...\n");
    printf("  预期结果：catch (...) 无法捕获硬件异常.\n");

    // 外层用 SEH 兜底，防止整个程序崩溃，以便继续后续演示.
    __try {
        TestCppCatchInner();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  外层 SEH 捕获：C++ catch 没抓到，SEH 兜底了!\n");
        printf("  结论：硬件异常必须走 SEH 或 UEF 捕获.\n");
    }
}

void TestSEHCatch() {
    printf("\n[测试 2] 用 SEH __try/__except 捕获空指针访问...\n");
    printf("  预期结果：成功捕获，程序继续运行.\n");

    __try {
        int* p = nullptr;
        *p = 42;  // 触发 Access Violation.
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        printf("  SEH 成功捕获异常 (0x%08X)!\n", code);
    }
    printf("  程序继续执行，没有崩溃.\n");
}

void TestWildPointer() {
    printf("\n[测试 3] 野指针访问（指向已释放的栈内存）...\n");

    int* wild = nullptr;
    {
        int local = 100;
        wild = &local;  // wild 指向局部变量.
    }  // local 超出作用域，wild 变为野指针.

    __try {
        // 访问野指针，行为未定义，可能崩溃也可能不崩溃.
        int value = *wild;
        printf("  野指针访问偶然未崩溃，值为 %d，但这是未定义行为!\n", value);
        printf("  在 Debug 模式下通常能捕获，Release 下可能不被发现.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  SEH 捕获野指针访问异常.\n");
    }
}

void TestUseAfterFree() {
    printf("\n[测试 4] use-after-free（释放后使用）...\n");

    int* p = new int(42);
    delete p;  // 内存已释放.

    __try {
        // 访问已释放的内存，行为未定义.
        *p = 100;
        printf("  use-after-free 偶然未崩溃，但这是未定义行为!\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  SEH 捕获 use-after-free 异常.\n");
    }

    // 再次释放会导致 double-free，也是严重问题.
    printf("\n[测试 4b] double-free（重复释放）...\n");
    int* p2 = new int(42);
    delete p2;
    // delete p2;  // 不要重复释放，实际工程中会造成崩溃.
    printf("  double-free 已注释掉，实际工程中不要重复释放.\n");
}

int main() {
    printf("=== Demo 04: 空指针/野指针问题 ===\n");
    printf("核心知识点：C++ catch 无法捕获硬件异常，必须使用 SEH.\n");

    TestCppCatch();
    TestSEHCatch();
    TestWildPointer();
    TestUseAfterFree();

    printf("\n=== 所有测试结束 ===\n");
    printf("建议：工程中尽量使用 std::unique_ptr / std::shared_ptr 管理裸指针.\n");
    return 0;
}
