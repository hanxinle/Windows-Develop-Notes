# 05_heap_corruption 实验总结

## 实验代码

主文件: [05_heap_corruption.cpp](05_heap_corruption.cpp)

本次实验重点关注 `TestBufferOverflow()` 函数：

```cpp
void TestBufferOverflow() {
    char* buffer = new char[16];

    // 写入超过缓冲区大小的数据，破坏堆块头.
    for (int i = 0; i < 32; i++) {
        buffer[i] = 'A';  // 越界写入.
    }

    __try {
        delete[] buffer;
        printf("  释放成功（但堆可能已经损坏）.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  释放时触发异常，堆已被破坏!\n");
    }

    // 再次分配一次，验证堆是否稳定.
    __try {
        char* buffer2 = new char[16];
        buffer2[0] = 'X';
        delete[] buffer2;
        printf("  二次分配释放成功.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  二次分配/释放触发异常，确认堆已坏!\n");
    }
}
```

## 实验现象

### Release x64 模式（Ctrl + F5）

```text
[测试 1] 堆缓冲区溢出...
  分配 16 字节，写入 32 字节，覆盖堆管理结构.
  越界写入完成，现在尝试释放...
  释放成功（但堆可能已经损坏）.
  再次分配内存验证堆状态...
  二次分配释放成功.

=== 测试完成 ===
```

### Debug x64 模式（Ctrl + F5）

```text
[测试 1] 堆缓冲区溢出...
  分配 16 字节，写入 32 字节，覆盖堆管理结构.
  越界写入完成，现在尝试释放...
```

输出到此停止，程序没有继续打印后续内容.

## 原因分析

### Release 模式为什么能跑完？

Release 模式下 CRT 堆不会做严格的边界检查。越界写入 32 字节到 16 字节缓冲区后，堆管理元数据虽然已经被覆盖，但：

- `delete[]` 在 Release 下可能只是简单归还内存，没有立即校验堆块完整性；
- 第二次 `new/delete` 可能分配到了堆的其他区域，恰好没有使用到被损坏的堆块；
- 因此程序"看起来正常"跑完了。

这是堆破坏最危险的地方：**Release 下往往不立即崩溃，但程序的内部状态已经损坏**，崩溃可能被延迟到完全无关的代码位置才暴露。

### Debug 模式为什么停在释放时？

Debug 模式下 CRT 堆会插入**围栏字节**（guard bytes / fence bytes），分配和释放时都会做完整性检查。流程如下：

```text
new char[16]  → 实际分配 16 + 围栏区域 + 调试头
越界写入 32 字节 → 覆盖了围栏区域
delete[]      → CRT 检查围栏，发现被破坏
              → 触发断言失败或调用 abort()
```

由于堆断言通常直接调用 `abort()` 或 `__debugbreak()`，而不是抛出一个可捕获的 SEH 异常，因此 `__try/__except` 无法拦截。在 Ctrl+F5 不调试运行时，进程会直接终止，所以后续输出没有打印。

## 核心结论

| 模式 | 行为 | 原因 |
|------|------|------|
| Release | 可能继续运行 | 堆检查宽松，损坏被延迟 |
| Debug | 释放时终止 | 围栏检测发现越界写入 |

**Release 下"没崩溃"不等于"没问题"**。堆破坏在 Release 中的典型表现包括：

- 运行一段时间后才崩溃；
- 在看似无关的代码处崩溃；
- 不同机器、不同运行次数表现不一致；
- 崩溃栈远离真正的破坏现场。

因此，Debug 模式下的早期崩溃其实是好事，它把问题暴露在了发生现场。

## 进一步验证建议

1. **多次运行 Release 版本**：加大越界写入范围，或增加后续内存分配操作，观察是否会在后期崩溃。
2. **开启 AddressSanitizer**：在 VS 项目属性中启用 ASan，可以更早、更精确地定位越界写入位置。
3. **使用 gflags + page heap**：即使在 Release 环境下也能强制 Windows 做严格的堆检查。

```text
gflags /p /enable YourProgram.exe /full
```

page heap 会为每个堆块分配独立页面并在后面放置保护页，越界写入会立即触发访问违例，从而精确定位破坏点。

## 工程启示

- 不要依赖 Release 模式下"没崩溃"来判断内存操作安全。
- 堆破坏类 bug 应尽量在开发阶段用 Debug + ASan / gflags 等手段暴露。
- 线上捕获到崩溃时，如果调用栈看起来 unrelated，要考虑是否是早期堆破坏的延迟表现。

## 补充实验：TestDoubleFree（重复释放）

### 实验代码

```cpp
void TestDoubleFree() {
    printf("\n[测试 2] Double Free（重复释放）...\n");

    int* p = new int(42);
    printf("  分配内存: %p\n", (void*)p);

    delete p;
    printf("  第一次释放成功.\n");

    printf("  尝试第二次释放同一块内存...\n");
    __try {
        delete p;  // 重复释放，可能破坏堆.
        printf("  第二次释放竟然成功了，Release 模式下可能不被发现.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  第二次释放触发异常!\n");
    }
}
```

### 实验现象

#### Debug x64 模式（Ctrl + F5）

```text
[测试 2] Double Free（重复释放）...
  分配内存: 0000023B4F9C8E30
  第一次释放成功.
  尝试第二次释放同一块内存...
  第二次释放触发异常!

=== 测试完成 ===
```

#### Release x64 模式（Ctrl + F5）

```text
[测试 2] Double Free（重复释放）...
  分配内存: 000002152189CAA0
  第一次释放成功.
  尝试第二次释放同一块内存...
```

输出到此停止，程序没有继续打印后续内容.

### 原因分析

#### Debug 模式为什么能捕获？

Debug 模式下 CRT 堆会记录每个堆块的分配/释放状态。第二次 `delete p` 时，CRT 发现这个指针已经被释放过，于是主动抛出一个 SEH 异常（通常是 `STATUS_HEAP_CORRUPTION`，代码 `0xC0000374`）。这个异常可以被 `__try/__except` 捕获，因此程序继续运行并打印了测试完成。

#### Release 模式为什么直接停止？

Release 模式下 CRT 没有维护这么多调试状态。第一次 `delete p` 后，这块内存被堆管理器回收，可能已经被重新分配给其他用途，或者空闲链表结构已经被修改。

第二次 `delete p` 时，堆管理器按这个"已失效"的地址去操作空闲链表，直接破坏了堆的内部结构。这时候触发的不是 Debug 那种"主动抛出的 SEH 异常"，而是更底层的访问违例、不可恢复的堆损坏，或者 CRT 直接调用 `abort()` / `__fastfail()`。

因此 `__try/__except` 要么没机会捕获，要么在异常处理过程中进程就终止了，没有后续输出。

### 与 TestBufferOverflow 的对比

| 对比项 | TestBufferOverflow（越界写入） | TestDoubleFree（重复释放） |
|--------|-------------------------------|---------------------------|
| 破坏对象 | 堆块周围的元数据/围栏 | 堆空闲链表 |
| Debug 行为 | 释放时触发断言/abort，进程终止 | 抛出可捕获的 SEH 异常，程序继续 |
| Release 行为 | 可能延迟崩溃，甚至"正常"跑完 | 通常立即崩溃或进程终止 |
| 核心原因 | Debug 有围栏检查，Release 检查宽松 | Debug 有指针状态记录，Release 没有 |
| 共同特点 | **Debug 检查更严格，Release 更宽松** | **Debug 检查更严格，Release 更宽松** |

### 关键区别

- **TestBufferOverflow** 是"写坏了堆的元数据"，Release 下可能碰巧不触发崩溃，属于**延迟暴露型**问题。
- **TestDoubleFree** 是"操作一个已经被回收的堆块"，Release 下几乎一定会立即破坏堆结构，属于**快速崩溃型**问题。

两者都说明：**Release 下堆的行为比 Debug 更不可预测**，很多问题在 Debug 下能稳定复现和捕获，在 Release 下却变成"偶发崩溃"或"程序直接消失"。

## 补充实验：TestUseAfterFree（释放后使用）

### 实验代码

```cpp
void TestUseAfterFree() {
    printf("\n[测试 3] Use After Free（释放后使用）...\n");

    int* p = new int(42);
    printf("  分配内存: %p, 值 = %d\n", (void*)p, *p);

    delete p;
    printf("  内存已释放.\n");

    // 尝试读写已释放的内存.
    printf("  尝试读取已释放的内存...\n");
    __try {
        int value = *p;  // 未定义行为.
        printf("  读取成功，值 = %d（这是未定义行为，可能读到任意值）.\n", value);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  读取已释放内存触发异常!\n");
    }

    printf("  尝试写入已释放的内存...\n");
    __try {
        *p = 100;  // 未定义行为，可能破坏堆.
        printf("  写入成功，但堆可能已损坏.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  写入已释放内存触发异常!\n");
    }
}
```

### 实验现象

#### Release x64 模式（Ctrl + F5）

```text
[测试 3] Use After Free（释放后使用）...
  分配内存: 00000189305EECF0, 值 = 42
  内存已释放.
  尝试读取已释放的内存...
  读取已释放内存触发异常!
  尝试写入已释放的内存...
  写入已释放内存触发异常!

=== 测试完成 ===
```

#### Debug x64 模式（Ctrl + F5）

```text
[测试 3] Use After Free（释放后使用）...
  分配内存: 0000022502DA8230, 值 = 42
  内存已释放.
  尝试读取已释放的内存...
  读取已释放内存触发异常!
  尝试写入已释放内存...
  写入已释放内存触发异常!

=== 测试完成 ===
```

### 原因分析

这次 Release 和 Debug 的行为**一致**：读写已释放内存都触发了可捕获的 SEH 异常，程序继续运行并打印"测试完成"。

#### 为什么这里 Release 也能捕获？

`use-after-free` 访问的是一个已经被堆管理器回收的地址。在 Windows 堆管理器（包括 Debug 和 Release 的 CRT 堆）中，释放后的内存通常不会立即交还给操作系统，而是被标记为"空闲"，可能被填充为特定模式（如 `0xFEEE` 等调试填充），并受到堆元数据的保护。

当程序再次访问这块空闲内存时，堆的完整性检查会触发一个 SEH 异常（如 `STATUS_ACCESS_VIOLATION` 或 `STATUS_HEAP_CORRUPTION`），这个异常可以被 `__try/__except` 捕获。

#### 与 TestBufferOverflow / TestDoubleFree 的区别

| 对比项 | TestBufferOverflow | TestDoubleFree | TestUseAfterFree |
|--------|-------------------|----------------|------------------|
| 操作类型 | 越界写入 | 重复释放 | 读写已释放内存 |
| Debug 行为 | 释放时 abort / 终止 | 抛出可捕获 SEH | 抛出可捕获 SEH |
| Release 行为 | 可能延迟崩溃 | 通常直接终止 | 抛出可捕获 SEH |
| 是否可预测 | Release 下更隐蔽 | Release 下更剧烈 | Release/Debug 都较一致 |
| 根本原因 | 覆盖堆块元数据 | 破坏堆空闲链表 | 访问被堆管理器回收的地址 |

### 核心结论

- `use-after-free` 在 Release 和 Debug 下都更容易被及时捕获，因为访问的是堆管理器已回收的地址，而不是单纯地覆盖堆块头或空闲链表。
- 但这并不意味着 `use-after-free` 不危险。实际场景中，如果释放后的内存被立即重新分配给其他对象，读写操作可能落到新对象上，造成**数据污染**或**安全漏洞**，而不会触发访问违例。
- 因此，即使这次实验"看起来"捕获了异常，也要避免任何形式的 `use-after-free`。

## 补充实验：TestHeapEnableTermination（启用堆破坏即终止）

### 实验代码

```cpp
void TestHeapEnableTermination() {
    printf("\n[测试 4] 启用 HeapEnableTerminationOnCorruption...\n");
    printf("  这个 API 可以让堆破坏时尽早崩溃，调用栈更准确.\n");

    BOOL result = HeapSetInformation(
        GetProcessHeap(),
        HeapEnableTerminationOnCorruption,
        nullptr,
        0
    );

    if (result) {
        printf("  HeapEnableTerminationOnCorruption 启用成功.\n");
        printf("  现在如果发生堆破坏，程序会立即终止而不是继续运行.\n");
    } else {
        printf("  启用失败，错误码: %lu\n", GetLastError());
    }
}
```

### 实验现象

#### Debug x64 模式（Ctrl + F5）

```text
[测试 4] 启用 HeapEnableTerminationOnCorruption...
  这个 API 可以让堆破坏时尽早崩溃，调用栈更准确.
  HeapEnableTerminationOnCorruption 启用成功.
  现在如果发生堆破坏，程序会立即终止而不是继续运行.

=== 测试完成 ===
```

#### Release x64 模式（Ctrl + F5）

```text
[测试 4] 启用 HeapEnableTerminationOnCorruption...
  这个 API 可以让堆破坏时尽早崩溃，调用栈更准确.
  HeapEnableTerminationOnCorruption 启用成功.
  现在如果发生堆破坏，程序会立即终止.

=== 测试完成 ===
```

### 原因分析

这个测试本身只是**启用策略**，并没有真的触发堆破坏，所以 Release 和 Debug 的输出几乎一样：`HeapSetInformation` 调用成功，程序正常结束。

#### HeapEnableTerminationOnCorruption 是什么？

`HeapEnableTerminationOnCorruption` 是 Windows 提供的一个进程级堆策略。启用后，当系统堆管理器检测到堆破坏时，会立即终止进程，而不是尝试继续运行或让异常处理程序有机会恢复。

对应的 API：

```cpp
BOOL HeapSetInformation(
    HANDLE HeapHandle,                       // 要设置的堆句柄
    HEAP_INFORMATION_CLASS HeapInformationClass, // HeapEnableTerminationOnCorruption
    PVOID HeapInformation,                   // 无需额外信息，传 nullptr
    SIZE_T HeapInformationLength             // 传 0
);
```

通常作用于进程默认堆：

```cpp
HeapSetInformation(GetProcessHeap(), HeapEnableTerminationOnCorruption, nullptr, 0);
```

#### 它改变了什么？

在默认情况下，某些堆破坏可能不会立即终止进程，而是抛出 SEH 异常、返回错误码，甚至被忽略。启用这个策略后：

- 堆破坏一旦发生，进程立即被强制终止（通常调用 `TerminateProcess` 或 `__fastfail`）。
- 不会有 `__try/__except` 的捕获机会。
- 崩溃的调用栈更接近破坏现场，便于定位问题。

#### 为什么 Release 和 Debug 行为一致？

因为这个测试只是启用了一个系统策略，没有实际触发堆破坏。`HeapSetInformation` 本身是一个 Windows API，行为由操作系统决定，和 Debug/Release 的 CRT 堆调试机制无关。

### 对比：启用前后遇到堆破坏的表现

| 场景 | 未启用 HeapEnableTerminationOnCorruption | 启用 HeapEnableTerminationOnCorruption |
|------|------------------------------------------|----------------------------------------|
| 检测到堆破坏 | 可能抛 SEH、返回错误、继续运行 | 立即终止进程 |
| 调用栈准确性 | 可能延迟崩溃，栈远离现场 | 更接近破坏现场 |
| 是否可恢复 | 有机会被 `__except` 捕获 | 不可捕获，直接终止 |
| 线上价值 | 可能隐藏问题 | 快速暴露问题，防止数据污染 |

### 工程启示

- 对于服务端程序或长期运行的进程，建议在启动早期启用 `HeapEnableTerminationOnCorruption`，让堆破坏尽快暴露，而不是带病运行。
- 该策略和 AddressSanitizer、gflags page heap 等手段可以叠加使用，进一步提高堆破坏的发现率。
- 但启用后，某些原本可以被 `__try/__except` 捕获的堆相关异常会变成进程直接终止，需要在设计崩溃上报、日志收集时考虑这一点。

### 进一步验证建议

可以在启用 `HeapEnableTerminationOnCorruption` 后再调用 `TestBufferOverflow()` 或 `TestDoubleFree()`，观察是否还会被 `__try/__except` 捕获，还是进程直接终止。

示例：

```cpp
int main() {
    HeapSetInformation(GetProcessHeap(), HeapEnableTerminationOnCorruption, nullptr, 0);
    TestDoubleFree();  // 观察行为是否从"捕获异常"变成"直接终止"
    return 0;
}
```

通常启用该策略后，`TestDoubleFree` 在 Debug 下原本可捕获的 SEH 异常可能会变成不可捕获的进程终止，从而验证策略效果。

### 实际验证结果

在 `main()` 中先调用 `TestHeapEnableTermination()`，再调用 `TestDoubleFree()`：

```cpp
int main() {
    // ...
    TestHeapEnableTermination();
    TestDoubleFree();
    // ...
}
```

#### Debug x64 模式（Ctrl + F5）

```text
[测试 2] Double Free（重复释放）...
  分配内存: 000001B4A24988F0
  第一次释放成功.
  尝试第二次释放同一块内存...
  第二次释放触发异常!

=== 测试完成 ===
```

#### Release x64 模式（Ctrl + F5）

```text
[测试 2] Double Free（重复释放）...
  分配内存: 000001AF9DD9CAA0
  第一次释放成功.
  尝试第二次释放同一块内存...
```

输出到此停止，进程终止。

### 结果分析

| 模式 | 行为 | 原因 |
|------|------|------|
| Debug | 仍被 `__except` 捕获 | CRT Debug 堆会主动检测 double-free 并抛出 SEH，这个检查发生在系统堆 `HeapEnableTerminationOnCorruption` 策略触发之前 |
| Release | 直接终止进程 | 没有 CRT 前置检查，double-free 直接破坏系统堆空闲链表，系统堆检测到破坏后立即终止 |

这说明：`HeapEnableTerminationOnCorruption` 影响的是**系统堆管理器检测到破坏后的行为**，而不是 CRT Debug 堆自己的前置校验。Debug 下 CRT 的校验会先抛 SEH，因此 `__try/__except` 仍有机会捕获；Release 下没有这个校验，系统堆策略直接生效。
