// Demo 06: 栈溢出 (Stack Overflow) 演示.
// 包括：无限递归、大数组栈分配、递归深度检查.

#include <windows.h>
#include <cstdio>

// 全局线程局部变量，记录递归深度.
static thread_local int g_recursionDepth = 0;

// 测试 1：无限递归导致栈溢出.
void InfiniteRecursion(int n) {
    // 在栈上分配一些数据，加速栈消耗.
    char buffer[1024];
    buffer[0] = (char)n;
    (void)buffer;

    // 每 100 层打印一次.
    if (n % 100 == 0) {
        printf("  递归深度: %d\n", n);
    }

    // 没有终止条件，无限递归下去.
    InfiniteRecursion(n + 1);
}

void TestInfiniteRecursion() {
    printf("\n[测试 1] 无限递归导致栈溢出...\n");
    printf("  预期：递归到约 1000+ 层后栈溢出 (0xC00000FD).\n");
    printf("  注意：这个测试会导致程序崩溃!\n\n");

    __try {
        InfiniteRecursion(0);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        if (code == STATUS_STACK_OVERFLOW) {
            printf("\n  捕获到栈溢出异常 (STATUS_STACK_OVERFLOW 0xC00000FD)!\n");
        } else {
            printf("\n  捕获到其他异常: 0x%08X\n", code);
        }
    }
}

// 测试 2：递归深度检查（防御性编程）.
void SafeRecursion(int n, int maxDepth) {
    g_recursionDepth++;

    if (g_recursionDepth > maxDepth) {
        printf("  递归深度超过限制 (%d)，安全返回!\n", maxDepth);
        g_recursionDepth--;
        return;
    }

    // 模拟一些工作.
    char buffer[64];
    buffer[0] = (char)n;
    (void)buffer;

    SafeRecursion(n + 1, maxDepth);
    g_recursionDepth--;
}

void TestSafeRecursion() {
    printf("\n[测试 2] 带深度检查的递归...\n");
    printf("  设置最大深度为 500，超过则安全返回.\n");

    g_recursionDepth = 0;
    SafeRecursion(0, 500);
    printf("  安全递归完成，没有崩溃.\n");
}

// 测试 3：大数组栈分配.
void TestLargeStackArray() {
    printf("\n[测试 3] 大数组栈分配...\n");
    printf("  默认栈大小约 1MB，分配 2MB 数组会导致栈溢出.\n");

    printf("  尝试分配 512KB 栈数组...\n");
    __try {
        char buffer[512 * 1024];  // 512KB.
        buffer[0] = 'A';
        printf("  512KB 分配成功.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  512KB 分配触发异常!\n");
    }

    // 更大的分配可能导致溢出.
    printf("  尝试分配 1.5MB 栈数组...\n");
    __try {
        char buffer[1536 * 1024];  // 1.5MB，超过默认栈.
        buffer[0] = 'B';
        printf("  1.5MB 分配成功（可能是编译器优化）.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  1.5MB 分配触发栈溢出!\n");
    }
}

// 测试 4：互相递归导致无限循环.
void MutuallyRecursiveB(int n);

void MutuallyRecursiveA(int n) {
    if (n % 100 == 0) {
        printf("  A -> B 递归深度: %d\n", n);
    }
    MutuallyRecursiveB(n + 1);
}

void MutuallyRecursiveB(int n) {
    MutuallyRecursiveA(n);
}

void TestMutualRecursion() {
    printf("\n[测试 4] 互相递归（A 调用 B，B 调用 A）...\n");
    printf("  这种没有终止条件的互相调用也会导致栈溢出.\n");
    printf("  注意：这个测试会导致程序崩溃!\n\n");

    __try {
        MutuallyRecursiveA(0);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DWORD code = GetExceptionCode();
        printf("\n  捕获到异常: 0x%08X\n", code);
    }
}

int main() {
    printf("=== Demo 06: 栈溢出 (Stack Overflow) ===\n");
    printf("默认线程栈大小约 1MB (Windows 默认).\n");
    printf("可以在链接器设置中调整栈大小.\n\n");

    // 注意：测试 1 和 4 会导致程序崩溃，建议逐个测试.
    // 取消注释你想测试的用例.

    //TestInfiniteRecursion();   // 会导致崩溃!
    //TestSafeRecursion();
    //TestLargeStackArray();
    TestMutualRecursion();     // 会导致崩溃!

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 递归深度不可控时改用循环 + 自己维护栈.\n");
    printf("  2. 大内存用堆分配 (new/malloc) 而不是栈.\n");
    printf("  3. 递归函数加入深度检查作为防御.\n");
    return 0;
}
