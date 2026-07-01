// Demo 13: 全局静态变量初始化顺序问题 (SIOF - Static Initialization Order Fiasco).
// a.cpp: 定义全局对象 A，其构造函数依赖全局对象 B.

#include <cstdio>

// 前向声明 B 的获取函数（在 b.cpp 中定义）.
class B {
public:
    B() { printf("  B 构造函数\n"); }
    void DoSomething() { printf("  B::DoSomething()\n"); }
};

B& GetB();

// 全局对象 A: 构造函数调用 B 的方法.
// 如果链接器先初始化 A 再初始化 B，B 尚未构造，调用是未定义行为.
class A {
public:
    A() {
        printf("  A 构造函数，尝试调用 B::DoSomething()...\n");
        GetB().DoSomething();  // 如果 B 还没初始化，这是未定义行为.
        printf("  A 构造函数完成\n");
    }
    void DoSomething() { printf("  A::DoSomething()\n"); }
};

A g_a;  // 全局对象 A.

A& GetA() {
    return g_a;
}
