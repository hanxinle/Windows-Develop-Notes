// Demo 02: SEH (Structured Exception Handling) 结构化异常处理演示.
// 演示 SEH 捕获各种硬件异常，包括空指针、除零、非法内存访问等.

#include <windows.h>
#include <cstdio>
// 辅助函数：将异常代码转换为可读字符串.
const char* ExceptionCodeToString(DWORD code) {
    switch (code) {
        // 访问违例.
        case EXCEPTION_ACCESS_VIOLATION:         return "ACCESS_VIOLATION";
        // 数组越界.
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "ARRAY_BOUNDS_EXCEEDED";
        // 浮点除零.
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "FLT_DIVIDE_BY_ZERO";
        // 整数除零.
        case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "INT_DIVIDE_BY_ZERO";
        // 栈溢出.
        case EXCEPTION_STACK_OVERFLOW:           return "STACK_OVERFLOW";
        // 非法指令.
        case EXCEPTION_ILLEGAL_INSTRUCTION:      return "ILLEGAL_INSTRUCTION";
        // 特权指令.
        case EXCEPTION_PRIV_INSTRUCTION:         return "PRIV_INSTRUCTION";
        // 未知异常.
        default:                                 return "UNKNOWN";
    }
}

// 测试 1：空指针访问.
void TestNullPointer() {
    // 英文输出：测试 1，空指针访问.
    printf("\n[Test 1] Null pointer access...\n");
    __try {
        int* p = nullptr;
        *p = 42;  // 触发访问违例.
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        // 英文输出：SEH 捕获到异常：异常类型（异常代码）.
        printf("  [SEH] Exception caught: %s (0x%08X)\n", ExceptionCodeToString(code), code);
    }
    // 英文输出：程序继续执行，没有崩溃.
    printf("  [SEH] Program continues, no crash.\n");
}

// 测试 2：整数除零.
void TestDivideByZero() {
    // 英文输出：测试 2，整数除零.
    printf("\n[Test 2] Integer divide by zero...\n");
    __try {
        int a = 10;
        int b = 0;
        int c = a / b;  // 触发除零异常.
        (void)c;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        // 英文输出：SEH 捕获到异常：异常类型（异常代码）.
        printf("  [SEH] Exception caught: %s (0x%08X)\n", ExceptionCodeToString(code), code);
    }
    // 英文输出：程序继续执行，没有崩溃.
    printf("  [SEH] Program continues, no crash.\n");
}

// 测试 3：读取无效内存地址.
void TestReadInvalidMemory() {
    // 英文输出：测试 3，读取无效内存地址.
    printf("\n[Test 3] Read invalid memory address...\n");
    __try {
        // 读取一个几乎不可能有效的地址.
        volatile int* p = (volatile int*)0x12345678;
        int value = *p;  // 触发访问违例.
        (void)value;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        // 英文输出：SEH 捕获到异常：异常类型（异常代码）.
        printf("  [SEH] Exception caught: %s (0x%08X)\n", ExceptionCodeToString(code), code);
    }
    // 英文输出：程序继续执行，没有崩溃.
    printf("  [SEH] Program continues, no crash.\n");
}

// 测试 4：嵌套 SEH.
void TestNestedSEH() {
    // 英文输出：测试 4，嵌套 SEH.
    printf("\n[Test 4] Nested SEH...\n");
    __try {
        // 英文输出：外层 __try.
        printf("  Outer __try\n");
        __try {
            // 英文输出：内层 __try，触发异常.
            printf("  Inner __try, triggering exception...\n");
            int* p = nullptr;
            *p = 1;
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            // 英文输出：内层 __except 捕获，不重新抛出.
            printf("  Inner __except caught, not rethrowing...\n");
            // 内层处理后继续，不重新抛出.
        }
        // 英文输出：回到外层 __try.
        printf("  Back to outer __try\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // 英文输出：外层 __except 捕获.
        printf("  Outer __except caught.\n");
    }
    // 英文输出：程序继续执行.
    printf("  [SEH] Program continues.\n");
}

// 使用过滤函数根据异常类型决定处理方式.
LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    DWORD code = pExceptionInfo->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        // 英文输出：这是访问违例，决定处理它.
        printf("  [Filter] This is an access violation, handle it.\n");
        return EXCEPTION_EXECUTE_HANDLER;  // 处理.
    }
    // 英文输出：这不是访问违例，继续搜索处理者.
    printf("  [Filter] This is not an access violation, continue searching.\n");
    return EXCEPTION_CONTINUE_SEARCH;  // 不处理，继续搜索.
}

// 测试 5：使用过滤函数.
void TestFilterFunction() {
    // 英文输出：测试 5，使用过滤函数.
    printf("\n[Test 5] Using exception filter...\n");
    __try {
        int* p = nullptr;
        *p = 1;
    }
    __except (ExceptionFilter(GetExceptionInformation())) {
        // 英文输出：过滤函数决定处理此异常.
        printf("  [SEH] Filter decided to handle this exception.\n");
    }
    // 英文输出：程序继续执行.
    printf("  [SEH] Program continues.\n");
}

int main() {
    // 英文输出：标题.
    printf("=== Demo 02: SEH (Structured Exception Handling) ===\n");
    // 英文输出：SEH 是 Windows 原生的异常处理机制.
    printf("SEH is the native Windows exception handling mechanism.\n");
    // 英文输出：C++ try/catch 无法捕获空指针访问等硬件异常，必须使用 SEH.
    printf("C++ try/catch cannot catch hardware exceptions like null pointer access; SEH is required.\n");

    TestNullPointer();
    TestDivideByZero();
    TestReadInvalidMemory();
    TestNestedSEH();
    TestFilterFunction();

    // 英文输出：所有 SEH 测试通过，程序正常退出.
    printf("\n=== All SEH tests passed, program exits normally ===\n");
    return 0;
}
