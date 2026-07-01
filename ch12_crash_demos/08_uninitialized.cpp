// Demo 08: 未初始化变量演示.
// 未初始化的变量包含垃圾值，导致不可预测的行为.

#ifdef _MSC_VER
// 本文件故意演示未初始化变量，禁用编译期警告和 Debug 运行时检查.
#pragma warning(disable: 4700)
//#pragma runtime_checks("u", off)
#endif

#include <cstdio>

// 测试 1：未初始化局部变量.
void TestUninitializedLocal() {
    printf("\n[测试 1] 未初始化局部变量...\n");

    int x;  // 未初始化.
    printf("  未初始化局部变量 x 的值: %d\n", x);
    printf("  这个值是栈上残留的垃圾数据，每次运行可能不同.\n");

    // 如果在 if 中使用未初始化变量，会导致不可预测的分支.
    if (x > 0) {
        printf("  分支 A：x > 0\n");
    } else {
        printf("  分支 B：x <= 0\n");
    }
    printf("  这个分支走向是随机的，取决于栈上残留的值.\n");
}

// 测试 2：未初始化指针.
void TestUninitializedPointer() {
    printf("\n[测试 2] 未初始化指针...\n");

    int* p;  // 未初始化，包含随机地址.
    printf("  未初始化指针 p 的值: %p\n", (void*)p);
    printf("  如果解引用这个指针，会访问随机地址，大概率崩溃.\n");

    // 不要真的解引用，避免崩溃.
    printf("  正确做法：声明时初始化为 nullptr.\n");
    int* q = nullptr;  // 正确做法.
    printf("  初始化后的指针 q: %p\n", (void*)q);
}

// 测试 3：类成员未初始化.
class BadWidget {
public:
    int width;    // 未初始化.
    int height;   // 未初始化.
    int* data;    // 未初始化.

    // 没有构造函数，成员变量不会被自动初始化.

    void Print() {
        printf("  width = %d, height = %d, data = %p\n", width, height, (void*)data);
    }
};

class GoodWidget {
public:
    int width = 0;     // 类内初始化.
    int height = 0;    // 类内初始化.
    int* data = nullptr;  // 类内初始化.

    GoodWidget() = default;

    GoodWidget(int w, int h) : width(w), height(h) {
        data = new int[w * h]();
    }

    ~GoodWidget() {
        delete[] data;
    }

    void Print() {
        printf("  width = %d, height = %d, data = %p\n", width, height, (void*)data);
    }
};

void TestUninitializedMember() {
    printf("\n[测试 3] 类成员未初始化...\n");

    printf("  BadWidget（无初始化）:\n");
    BadWidget bad;
    bad.Print();
    printf("  成员包含垃圾值!\n");

    printf("\n  GoodWidget（有初始化）:\n");
    GoodWidget good;
    good.Print();
    printf("  成员已正确初始化.\n");
}

// 测试 4：数组未初始化.
void TestUninitializedArray() {
    printf("\n[测试 4] 数组未初始化...\n");

    int arr[5];  // 未初始化.
    printf("  未初始化数组: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n  元素值是栈上的垃圾数据.\n");

    int arr2[5] = {};  // 全部初始化为 0.
    printf("  初始化后的数组: ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr2[i]);
    }
    printf("\n");
}

// 测试 5：统一初始化 {} 的用法.
void TestUniformInitialization() {
    printf("\n[测试 5] 统一初始化 (Uniform Initialization)...\n");

    int a;           // 未初始化.
    int b{};         // 值初始化，b = 0.
    int c = 42;      // 直接初始化.
    int d{42};       // 列表初始化.
    int e = {};      // 值初始化，e = 0.

    printf("  int a;       -> %d (垃圾值)\n", a);
    printf("  int b{};     -> %d\n", b);
    printf("  int c = 42;  -> %d\n", c);
    printf("  int d{42};   -> %d\n", d);
    printf("  int e = {};  -> %d\n", e);

    int* p1;         // 未初始化.
    int* p2{};       // 初始化为 nullptr.
    printf("  int* p1;     -> %p (垃圾值)\n", (void*)p1);
    printf("  int* p2{};   -> %p (nullptr)\n", (void*)p2);
}

int main() {
    printf("=== Demo 08: 未初始化变量 ===\n");
    printf("未初始化变量是 C++ 中最常见的 bug 来源之一.\n");
    printf("Debug 模式下编译器可能帮你填零，但 Release 不会!\n\n");

    TestUninitializedLocal();
    TestUninitializedPointer();
    TestUninitializedMember();
    TestUninitializedArray();
    TestUniformInitialization();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 声明变量时总是初始化.\n");
    printf("  2. 类成员使用类内初始化: int x = 0;.\n");
    printf("  3. 指针初始化为 nullptr.\n");
    printf("  4. 开启编译器警告: /W4 /Wall，把警告当错误.\n");
    printf("  5. 使用 Clang-Tidy 检查: cppcoreguidelines-pro-type-member-init.\n");
    return 0;
}
