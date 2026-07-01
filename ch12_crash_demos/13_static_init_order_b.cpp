// Demo 13: 全局静态变量初始化顺序问题 (SIOF).
// b.cpp: 定义全局对象 B，其构造函数依赖全局对象 A.

#include <cstdio>

// 前向声明 A 的获取函数（在 a.cpp 中定义）.
class A {
public:
    A() { printf("  A 构造函数\n"); }
    void DoSomething() { printf("  A::DoSomething()\n"); }
};

A& GetA();

// 全局对象 B: 构造函数调用 A 的方法.
class B {
public:
    B() {
        printf("  B 构造函数，尝试调用 A::DoSomething()...\n");
        GetA().DoSomething();  // 如果 A 还没初始化，这是未定义行为.
        printf("  B 构造函数完成\n");
    }
    void DoSomething() { printf("  B::DoSomething()\n"); }
};

B g_b;  // 全局对象 B.

B& GetB() {
    return g_b;
}
