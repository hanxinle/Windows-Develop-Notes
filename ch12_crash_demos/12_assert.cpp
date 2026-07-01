// Demo 12: 断言 (Assertion) 演示.
// 断言用于检查程序逻辑假设，帮助在开发阶段发现问题.

#include <cstdio>
#include <cassert>
#include <cstring>

// 自定义断言，在失败前打印更多信息.
#define MY_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("\n========== ASSERTION FAILED ==========\n"); \
            printf("文件: %s\n", __FILE__); \
            printf("行号: %d\n", __LINE__); \
            printf("条件: %s\n", #cond); \
            printf("信息: %s\n", msg); \
            printf("======================================\n\n"); \
            assert(cond); \
        } \
    } while(0)

// 测试 1：基本断言.
void TestBasicAssert() {
    printf("\n[测试 1] 基本 assert...\n");

    int x = 10;
    assert(x > 0);  // 通过.
    printf("  assert(x > 0) 通过.\n");

    int* p = &x;
    assert(p != nullptr);  // 通过.
    printf("  assert(p != nullptr) 通过.\n");
}

// 测试 2：自定义断言（带详细信息）.
void TestCustomAssert() {
    printf("\n[测试 2] 自定义断言（带详细信息）...\n");

    int divisor = 5;
    MY_ASSERT(divisor != 0, "除数不能为零，会导致除零错误.");
    printf("  自定义断言通过: divisor = %d\n", divisor);

    // 模拟一个可能失败的场景.
    int index = 3;
    int arraySize = 5;
    MY_ASSERT(index >= 0 && index < arraySize,
              "数组索引越界，会导致内存访问错误.");
    printf("  自定义断言通过: index = %d, size = %d\n", index, arraySize);
}

// 测试 3：断言 vs 错误处理.
void Divide(int a, int b) {
    printf("\n[测试 3] 断言 vs 错误处理...\n");

    // 断言用于检查程序逻辑假设（内部错误）.
    // 这是编程错误，不应该发生.
    assert(b != 0 && "除数不应该为零，这是调用者的 bug");

    // 如果程序继续执行，说明断言通过了.
    int result = a / b;
    printf("  %d / %d = %d\n", a, b, result);
}

// 运行时检查（用户输入等）应该用错误处理，而不是断言.
bool SafeDivide(int a, int b, int* out) {
    if (b == 0) {
        printf("  错误：除数为零\n");
        return false;  // 返回错误，不要断言.
    }
    *out = a / b;
    return true;
}

void TestRuntimeErrorHandling() {
    printf("\n[测试 3b] 运行时错误处理（用户输入场景）...\n");

    int result;
    if (SafeDivide(10, 2, &result)) {
        printf("  10 / 2 = %d\n", result);
    }

    if (!SafeDivide(10, 0, &result)) {
        printf("  安全地处理了除零错误，程序没有崩溃.\n");
    }
}

// 测试 4：不变量检查（函数前后条件）.
class Counter {
    int count_ = 0;

public:
    void Increment() {
        // 前置条件.
        assert(count_ >= 0 && "计数器不能为负数");

        count_++;

        // 后置条件.
        assert(count_ > 0 && "递增后计数器必须大于零");
    }

    void Decrement() {
        // 前置条件.
        assert(count_ > 0 && "计数器已经为零，不能递减");

        count_--;

        // 后置条件.
        assert(count_ >= 0 && "递减后计数器不能为负数");
    }

    int Get() const { return count_; }
};

void TestInvariant() {
    printf("\n[测试 4] 不变量检查（前置/后置条件）...\n");

    Counter c;
    c.Increment();
    c.Increment();
    printf("  计数器 = %d\n", c.Get());

    c.Decrement();
    printf("  计数器 = %d\n", c.Get());

    printf("  不变量检查通过.\n");
}

// 测试 5：NDebug 的影响.
// 在 Release 配置中定义 NDEBUG 会禁用 assert.
void TestNDebugEffect() {
    printf("\n[测试 5] NDEBUG 对断言的影响...\n");

#ifdef NDEBUG
    printf("  当前定义了 NDEBUG，assert 被禁用.\n");
    printf("  assert 中的代码不会执行.\n");
#else
    printf("  当前未定义 NDEBUG，assert 启用.\n");
    printf("  注意：Release 版本中如果定义了 NDEBUG，断言会被移除.\n");
#endif
}

int main() {
    printf("=== Demo 12: 断言 (Assertion) ===\n");
    printf("断言用于检查程序逻辑假设，帮助在开发阶段发现 bug.\n");
    printf("不要把断言用于运行时错误处理（如用户输入验证）.\n\n");

    TestBasicAssert();
    TestCustomAssert();
    Divide(10, 2);
    TestRuntimeErrorHandling();
    TestInvariant();
    TestNDebugEffect();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 用 assert 检查程序内部逻辑假设（不变量、前后条件）.\n");
    printf("  2. 不要用 assert 处理用户输入或外部数据错误.\n");
    printf("  3. 自定义断言，失败前打印足够上下文信息.\n");
    printf("  4. 线上保留断言，不要 NDEBUG 删掉（可用更轻量的检查）.\n");
    printf("  5. 断言失败时记录日志，方便定位问题.\n");
    return 0;
}
