// Demo 11: 纯虚函数调用 (Pure Virtual Function Call) 演示.
// 通常在对象已析构后调用虚函数时触发，运行时错误 R6025.

#include <cstdio>

// 基类，包含纯虚函数.
class Base {
public:
    virtual void DoWork() = 0;  // 纯虚函数.
    virtual ~Base() = default;
};

class Derived : public Base {
public:
    void DoWork() override {
        printf("  Derived::DoWork()\n");
    }
    ~Derived() {
        printf("  Derived 析构\n");
    }
};

// 测试 1：正常调用.
void TestNormalCall() {
    printf("\n[测试 1] 正常虚函数调用...\n");

    Derived d;
    Base* ptr = &d;
    ptr->DoWork();  // 正常调用.
    printf("  正常完成.\n");
}

// 测试 2：对象已析构后调用（纯虚函数调用错误）.
// 这个测试演示悬空指针问题，危险调用已默认注释掉.
void TestCallAfterDestruction() {
    printf("\n[测试 2] 对象已析构后调用虚函数...\n");
    printf("  警告：这个测试可能触发 R6025 运行时错误!\n");

    Base* ptr = nullptr;
    {
        Derived d;
        ptr = &d;  // ptr 指向局部对象.
    }  // d 在这里析构，ptr 变成悬空指针.

    // 此时 ptr 指向已析构的对象，vptr 可能已被改写.
    // 在 Debug 模式下，VS 可能检测到并弹出 R6025 错误.
    // 在 Release 模式下，这是未定义行为，可能崩溃也可能不崩溃.
    printf("  尝试调用已析构对象的虚函数...\n");

    // 下面这行是危险的，已经注释掉以避免直接崩溃.
    // 如果取消注释，Debug 下通常会触发 R6025；Release 下可能“碰巧正常”，
    // 也可能访问无效内存而崩溃，具体取决于栈内存是否被覆盖.
    ptr->DoWork();

    //printf("  危险调用已注释掉.\n");
    printf("  如果取消注释，可能输出 Derived::DoWork()（未覆盖内存时），也可能直接崩溃.\n");
}

// 测试 3：构造函数中调用虚函数（不会调用到派生类版本）.
class BaseWithCtorCall : public Base {
public:
    BaseWithCtorCall() {
        printf("  BaseWithCtorCall 构造函数\n");
        // 在基类构造函数中调用虚函数.
        // 此时派生类部分尚未构造，调用的是当前类型的版本（即 BaseWithCtorCall::DoWork）.
        DoWork();
    }
    void DoWork() override {
        printf("  BaseWithCtorCall::DoWork()\n");
    }
};

class DerivedFromCtorCall : public BaseWithCtorCall {
public:
    void DoWork() override {
        printf("  DerivedFromCtorCall::DoWork()\n");
    }
    ~DerivedFromCtorCall() {
        printf("  DerivedFromCtorCall 析构\n");
    }
};

void TestVirtualInConstructor() {
    printf("\n[测试 3] 构造函数中调用虚函数...\n");
    printf("  C++ 规则：构造函数中调用虚函数，不会调用到派生类版本.\n");
    printf("  因为在基类构造期间，派生类部分尚未构造.\n");

    DerivedFromCtorCall obj;  // 构造时，BaseWithCtorCall 构造函数里调用 DoWork().
    obj.DoWork();             // 这里调用 DerivedFromCtorCall::DoWork().
}

// 测试 4：析构函数中调用虚函数.
class BaseWithDtorCall {
public:
    virtual void DoWork() {
        printf("  BaseWithDtorCall::DoWork()\n");
    }
    virtual ~BaseWithDtorCall() {
        printf("  BaseWithDtorCall 析构函数\n");
        // 在析构函数中调用虚函数.
        // 此时派生类部分已经析构，调用的是基类版本.
        DoWork();
    }
};

class DerivedWithDtorCall : public BaseWithDtorCall {
public:
    void DoWork() override {
        printf("  DerivedWithDtorCall::DoWork()\n");
    }
    ~DerivedWithDtorCall() {
        printf("  DerivedWithDtorCall 析构函数\n");
    }
};

void TestVirtualInDestructor() {
    printf("\n[测试 4] 析构函数中调用虚函数...\n");
    printf("  C++ 规则：析构函数中调用虚函数，不会调用到派生类版本.\n");
    printf("  因为在基类析构期间，派生类部分已经析构.\n");

    DerivedWithDtorCall obj;
    printf("  对象构造完成，准备析构...\n");
}  // 析构时观察输出.

// 测试 5：使用智能指针避免生命周期问题.
#include <memory>

void TestSmartPointer() {
    printf("\n[测试 5] 使用智能指针管理生命周期...\n");

    {
        std::unique_ptr<Base> ptr = std::make_unique<Derived>();
        ptr->DoWork();
        printf("  离开作用域，自动析构...\n");
    }
    printf("  对象已安全析构，不会出现悬空指针.\n");
}

int main() {
    printf("=== Demo 11: 纯虚函数调用 (Pure Virtual Call) ===\n");
    printf("R6025 - pure virtual function call 是常见的运行时错误.\n\n");

    TestNormalCall();
    TestCallAfterDestruction();
    TestVirtualInConstructor();
    TestVirtualInDestructor();
    TestSmartPointer();

    printf("\n=== 测试完成 ===\n");
    printf("工程建议：\n");
    printf("  1. 使用智能指针管理对象生命周期.\n");
    printf("  2. 删除对象后置指针为 nullptr.\n");
    printf("  3. 析构函数中不要调用虚函数（或明确调用基类版本）.\n");
    printf("  4. 构造函数中避免调用虚函数.\n");
    printf("  5. 使用观察者模式代替悬空指针回调.\n");
    return 0;
}
