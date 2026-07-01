# Demo 06: 栈溢出（Stack Overflow）实验总结

## 1. 文件说明

- 源文件：[06_stack_overflow.cpp](06_stack_overflow.cpp)
- 目标：演示 Windows 下 C++ 程序中常见的栈溢出场景，以及对应的防御思路。

## 2. 核心技术点

### 2.1 无限递归

- 函数 [InfiniteRecursion](06_stack_overflow.cpp#L11) 没有终止条件，每递归一次在栈上分配 1KB 缓冲区。
- 最终耗尽默认约 1MB 的线程栈，触发异常 `0xC00000FD`（`STATUS_STACK_OVERFLOW`）。
- 通过 `__try / __except` 捕获该异常并打印提示。

### 2.2 互相递归

- [MutuallyRecursiveA](06_stack_overflow.cpp#L102) 与 [MutuallyRecursiveB](06_stack_overflow.cpp#L109) 循环调用，同样没有终止条件。
- 说明栈溢出不一定来自单个函数自递归，也可能来自循环调用链。

### 2.3 大数组栈分配

- [TestLargeStackArray](06_stack_overflow.cpp#L73) 尝试在栈上分配 512KB、1.5MB 等大块内存。
- 当分配超过默认栈大小时，也会触发栈溢出。
- 教学点：大内存应使用堆分配（`new` / `malloc`），而不是栈。

### 2.4 安全递归与深度检查

- [SafeRecursion](06_stack_overflow.cpp#L45) 使用 `thread_local` 计数器 `g_recursionDepth` 跟踪当前递归深度。
- 超过 `maxDepth` 后安全返回，避免崩溃。
- [TestSafeRecursion](06_stack_overflow.cpp#L63) 设置最大深度为 500，程序可正常结束。

## 3. Debug / Release 运行差异讨论

针对 [TestSafeRecursion](06_stack_overflow.cpp#L63) 的预测：

- **功能结果一致**：Debug 和 Release 下都能正常结束，不会栈溢出。500 层递归远低于默认 1MB 栈空间。
- **栈帧大小不同**：Debug 保留帧指针、安全检查、未初始化填充等，单帧更大；Release 可能去掉局部数组 `buffer`，单帧更小。
- **崩溃用例的行为可能不同**：`TestInfiniteRecursion` 在 Release 下每层栈帧更小，可能多跑若干层才崩溃；而 `TestMutualRecursion` 在 Release 下会被尾调用优化成循环，栈不再增长，可能永远运行下去。详见第 7 节。
- **`TestLargeStackArray` 实测表现**：在默认 1MB 栈的 Debug/Release x64 下，程序都会直接以 `0xC00000FD` 退出，看不到 512KB 分配成功的日志。原因不是 Debug/Release 优化差异，而是函数入口一次性分配大数组导致溢出发生在 `__try` 生效之前。详见第 6 节。

## 4. 工程建议

1. 递归深度不可控时，改用循环 + 自己维护栈结构。
2. 大内存优先使用堆分配，而不是栈上分配大数组。
3. 递归函数应加入深度检查等防御性措施。
4. Windows 下可通过链接器设置调整线程栈大小，但根本解决之道是避免无限制递归和大栈分配。
5. 如果确实需要测试或捕获大栈分配导致的异常，应把每次分配放到独立函数中，或使用 `_alloca` / 堆分配，而不是把多个大局部变量堆在同一个函数里。

## 5. 关键异常码

- `0xC00000FD`：`STATUS_STACK_OVERFLOW`，即 Windows 下的栈溢出异常。

## 6. 实测现象：`TestLargeStackArray` 在 Debug/Release 下都直接崩溃

### 6.1 现象

单独运行 [`TestLargeStackArray()`](crash_demos/06_stack_overflow.cpp#L72) 时，控制台只打印了 `main` 中的两行标题：

```text
=== Demo 06: 栈溢出 (Stack Overflow) ===
默认线程栈大小约 1MB (Windows 默认).
```

随后进程直接退出，返回码：

```text
-1073741571 (0xc00000fd)  // STATUS_STACK_OVERFLOW
```

无论是 **Debug x64** 还是 **Release x64**，输出都相同，并且没有看到任何“512KB 分配成功”的日志。

### 6.2 原以为的行为

代码里把 512KB 和 1.5MB 的分配分别包在 [`__try / __except`](crash_demos/06_stack_overflow.cpp#L78) 中，期望的执行流程是：

1. 打印“尝试分配 512KB 栈数组...”
2. 512KB 分配成功，打印成功
3. 打印“尝试分配 1.5MB 栈数组...”
4. 1.5MB 触发栈溢出，被 `__except` 捕获，打印异常

### 6.3 实际原理

这个预期没有实现，根本原因是 **C/C++ 局部变量的栈空间通常是在函数入口处一次性分配的**，而不是执行到声明语句时才分配。

[`TestLargeStackArray()`](crash_demos/06_stack_overflow.cpp#L72) 内部有两个大数组：

```cpp
char buffer[512 * 1024];   // 512KB
char buffer[1536 * 1024];  // 1.5MB
```

虽然它们写在不同的 `__try` 块里，但 MSVC 编译器在进入函数时会为整个栈帧预留空间。这个函数局部变量总需求至少为 1.5MB（Release 下可能复用空间），而 Windows 默认线程栈只有约 1MB，因此在 **函数 prologue 阶段** 调用 `__chkstk` 进行页探测时就会失败，直接触发 `STATUS_STACK_OVERFLOW`。

此时：

- `printf("  尝试分配 512KB 栈数组...\n")` 尚未执行。
- `__try` 的异常处理作用域尚未开始，对应的 SEH 范围还没有生效。
- 所以 `__except` **无法捕获**这次溢出。

这就是“程序一进入函数就崩溃”的原因。

### 6.4 为什么 Debug 和 Release 表现一样？

这个问题一开始被猜测为“Debug/Release 优化差异”，但实测表现相同，说明它**不是优化级别的问题**，而是：

> 只要函数局部变量所需的最大栈空间超过默认栈大小，就会在函数入口溢出。

Release 编译器虽然会做如下优化：

- 去掉没有实际作用的死存储
- 复用不同作用域的栈空间
- 内联简单函数

但在本例中，512KB 和 1.5MB 两个数组中较大的那个已经是 1.5MB，仍然大于默认 1MB。因此即使 Release 复用了空间，进入函数时仍然需要预留超过 1MB 的栈帧，最终还是会溢出。

### 6.5 正确写法示例

如果想让 512KB 的测试正常完成，并让 1.5MB 的溢出被 `__except` 捕获，应该把每个大数组放到**独立的函数**中：

```cpp
void Test512KB() {
    __try {
        char buffer[512 * 1024];
        buffer[0] = 'A';
        printf("  512KB 分配成功.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  512KB 分配触发异常!\n");
    }
}

void Test1_5MB() {
    __try {
        char buffer[1536 * 1024];
        buffer[0] = 'B';
        printf("  1.5MB 分配成功.\n");
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("  1.5MB 分配触发栈溢出!\n");
    }
}

void TestLargeStackArray() {
    printf("\n[测试 3] 大数组栈分配...\n");
    Test512KB();
    Test1_5MB();
}
```

这样每个函数进入时只分配自己的数组，512KB 那个函数可以正常执行；1.5MB 那个函数虽然也会溢出，但异常发生在函数体内部，`__try / __except` 已经生效，因此能被捕获。

### 6.6 另一个可行方案：使用 `_alloca` 在 `__try` 内部动态分配

如果希望保留“在同一个函数里测试”的形式，可以使用 `_alloca`，它的栈分配发生在调用点：

```cpp
__try {
    char* buffer = (char*)_alloca(512 * 1024);
    buffer[0] = 'A';
    printf("  512KB 分配成功.\n");
}
__except (EXCEPTION_EXECUTE_HANDLER) {
    printf("  512KB 分配触发异常!\n");
}
```

因为 `_alloca` 的分配发生在 `__try` 块内部，溢出时 `__except` 已经生效。但 `_alloca` 本身也有风险（不支持在循环中重复调用、不支持异常安全等），工程上仍推荐用堆分配。

### 6.7 核心教训

1. **`__try / __except` 无法捕获函数入口阶段因分配自身局部变量导致的栈溢出。** 栈帧建立失败时，异常处理框架还没有准备好。
2. **不要把多个大数组或大块局部变量放在同一个函数里**，即使它们位于不同的 `__try` 块中。
3. **真正安全的做法**：大块内存用堆分配（`new` / `malloc` / `std::vector`），或把每个大栈分配隔离到独立函数中。

## 7. 实测现象：`TestMutualRecursion` 在 Release 下一直运行

### 7.1 现象

- **Debug 模式**：运行 [`TestMutualRecursion()`](crash_demos/06_stack_overflow.cpp#L113)，打印到约 `A -> B 递归深度: 1900` 后，捕获到异常 `0xC00000FD`，程序继续执行后续代码。
- **Release 模式**：程序开始后一直运行，持续打印递归深度，不会停止；手动中断前没有看到 `0xC00000FD`，也没有执行到 `printf("=== 测试完成 ===")`。

### 7.2 原因：Release 下的尾调用优化

互相递归的两个函数：

```cpp
void MutuallyRecursiveA(int n) {
    if (n % 100 == 0) {
        printf("  A -> B 递归深度: %d\n", n);
    }
    MutuallyRecursiveB(n + 1);   // A 的最后一条语句
}

void MutuallyRecursiveB(int n) {
    MutuallyRecursiveA(n);       // B 的最后一条语句
}
```

它们对彼此的调用都位于**尾位置**（调用之后函数直接返回）。在 **Release** 模式下，MSVC 会执行**尾调用优化（Tail Call Optimization）**，把：

```asm
call MutuallyRecursiveB
ret
```

替换成：

```asm
jmp MutuallyRecursiveB
```

`jmp` 不会压入新的返回地址，而是复用当前函数的栈帧。于是 `A -> B -> A -> B ...` 在汇编层面等价于一段循环：

```cpp
for (int n = 0; ; n++) {
    if (n % 100 == 0) printf(...);
    // 继续循环
}
```

栈空间不再增长，因此永远不会达到 1MB 上限，也就不会触发 `STATUS_STACK_OVERFLOW`。

### 7.3 为什么 Debug 会栈溢出

Debug 模式默认关闭尾调用优化，每次调用都会：

- 压入返回地址
- 保存寄存器
- 建立新的栈帧

因此栈深度不断增加，最终超过默认 1MB，触发 `0xC00000FD`，被 [`TestMutualRecursion`](crash_demos/06_stack_overflow.cpp#L113) 中的 `__except` 捕获。

### 7.4 核心教训

1. **尾调用优化会让某些递归在 Release 下变成循环**，从而不再栈溢出。这是 Debug/Release 行为差异的一个经典来源。
2. **不要依赖“栈溢出”来终止递归**。在 Release 优化下，这种“保护机制”可能失效。
3. **可预期的终止条件必须由程序逻辑保证**。例如 [`SafeRecursion`](crash_demos/06_stack_overflow.cpp#L45) 中使用 `maxDepth` 限制深度，才是最可靠的做法。

