// Demo 13: 全局静态变量初始化顺序问题 (SIOF) - 主程序.
// 演示 Meyers' Singleton 解决方案.

#include <cstdio>

// ==================== 问题演示 ====================
// 上面的 a.cpp 和 b.cpp 中的全局对象互相依赖.
// 链接顺序决定了初始化顺序，但顺序不确定，可能导致崩溃.
// 编译命令: cl 13_static_init_order_main.cpp 13_static_init_order_a.cpp 13_static_init_order_b.cpp

// ==================== 解决方案 ====================

// 方案 1: Meyers' Singleton.
// 将全局变量改为函数内静态变量，第一次访问时才初始化.
// C++11 保证线程安全.
class SafeA {
public:
    SafeA() { printf("  SafeA 构造函数（延迟初始化）\n"); }
    void DoWork() { printf("  SafeA::DoWork()\n"); }
};

SafeA& GetSafeA() {
    static SafeA instance;  // 第一次调用时才构造.
    return instance;
}

class SafeB {
public:
    SafeB() { printf("  SafeB 构造函数（延迟初始化）\n"); }
    void DoWork() {
        printf("  SafeB::DoWork()，调用 SafeA...\n");
        GetSafeA().DoWork();  // 安全：如果 SafeA 还没初始化，会先初始化.
    }
};

SafeB& GetSafeB() {
    static SafeB instance;
    return instance;
}

// 方案 2: 显式初始化顺序.
class ResourceManager {
public:
    void Init() { printf("  ResourceManager 初始化\n"); }
};

class Logger {
    ResourceManager* res_ = nullptr;
public:
    void Init(ResourceManager* res) {
        res_ = res;
        printf("  Logger 初始化（依赖 ResourceManager）\n");
    }
};

// 全局指针，初始为 nullptr.
ResourceManager* g_resourceManager = nullptr;
Logger* g_logger = nullptr;

// 显式按顺序初始化.
void InitAllGlobals() {
    printf("  [显式初始化] 开始...\n");
    g_resourceManager = new ResourceManager();
    g_resourceManager->Init();

    g_logger = new Logger();
    g_logger->Init(g_resourceManager);  // 依赖明确.
    printf("  [显式初始化] 完成\n");
}

void CleanupAllGlobals() {
    delete g_logger;
    delete g_resourceManager;
    printf("  [显式清理] 完成\n");
}

// ==================== 主程序 ====================
int main() {
    printf("=== Demo 13: 全局静态变量初始化顺序问题 (SIOF) ===\n");
    printf("问题：全局对象 A 依赖 B，B 依赖 A，初始化顺序不确定.\n\n");

    printf("--- 问题场景（如果编译了 a.cpp 和 b.cpp）---\n");
    printf("  观察上面输出的构造函数顺序.\n");
    printf("  如果 A 在 B 之前初始化，B 还没构造完成，A 调用 B 是未定义行为.\n\n");

    printf("--- 解决方案 1: Meyers' Singleton ---\n");
    printf("  第一次访问时才初始化，顺序由调用链决定.\n");
    GetSafeB().DoWork();
    printf("\n");

    printf("--- 解决方案 2: 显式初始化顺序 ---\n");
    InitAllGlobals();
    CleanupAllGlobals();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 尽量避免全局对象互相依赖.\n");
    printf("  2. 用 Meyers' Singleton 替代全局变量.\n");
    printf("  3. 或者显式控制初始化顺序，在 main 入口统一初始化.\n");
    printf("  4. 减少全局变量，改用依赖注入.\n");
    return 0;
}
