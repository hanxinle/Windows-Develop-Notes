# 02_seh.cpp 实验总结

## 1. 实验目的

理解 Windows SEH（Structured Exception Handling，结构化异常处理）与 VEH 的区别，验证 SEH 对硬件异常的捕获与恢复能力。

## 2. 实验环境

- Visual Studio 2022
- x64 Debug/Release
- 编译选项：`/EHsc /W4 /Zi /utf-8`
- 依赖库：无（仅需 `<windows.h>`）

## 3. 运行结果

```text
=== Demo 02: SEH (Structured Exception Handling) ===
SEH is the native Windows exception handling mechanism.
C++ try/catch cannot catch hardware exceptions like null pointer access; SEH is required.

[Test 1] Null pointer access...
  [SEH] Exception caught: ACCESS_VIOLATION (0xC0000005)
  [SEH] Program continues, no crash.

[Test 2] Integer divide by zero...
  [SEH] Exception caught: INT_DIVIDE_BY_ZERO (0xC0000094)
  [SEH] Program continues, no crash.

[Test 3] Read invalid memory address...
  [SEH] Exception caught: ACCESS_VIOLATION (0xC0000005)
  [SEH] Program continues, no crash.

[Test 4] Nested SEH...
  Outer __try
  Inner __try, triggering exception...
  Inner __except caught, not rethrowing...
  Back to outer __try
  [SEH] Program continues.

[Test 5] Using exception filter...
  [Filter] This is an access violation, handle it.
  [SEH] Filter decided to handle this exception.
  [SEH] Program continues.

=== All SEH tests passed, program exits normally ===
```

## 4. 核心结论

### 4.1 这些异常都是硬件异常

| 测试 | 异常代码 | 触发原因 |
|------|----------|----------|
| 空指针访问 | `EXCEPTION_ACCESS_VIOLATION (0xC0000005)` | CPU 访问了未映射的内存页 |
| 整数除零 | `EXCEPTION_INT_DIVIDE_BY_ZERO (0xC0000094)` | CPU 执行 `div/idiv` 时除数为 0 |
| 读取无效地址 | `EXCEPTION_ACCESS_VIOLATION (0xC0000005)` | 访问了非法线性地址 |

这些异常首先由 CPU 产生，操作系统内核封装成 Windows 异常记录后，再交给用户态的异常处理链。

### 4.2 `__try/__except` 与 `try/catch` 的区别

| 特性 | `__try/__except` | `try/catch` |
|------|------------------|-------------|
| 来源 | Windows/MSVC 特有 SEH | 标准 C++ |
| 头文件 | 需要 `<windows.h>` | 不需要 `<windows.h>` |
| 捕获硬件异常 | 可以 | 默认不可以 |
| 语法 | `__except (过滤表达式)` | `catch (异常类型)` |
| 跨平台 | 仅 Windows/MSVC | 跨平台 |

注意：`__try/__except` 与 `try/catch` **不能混用在同一个函数中**，但可以在不同函数中嵌套使用。

### 4.3 SEH 与 VEH 的关键区别

这是本次实验最重要的对比。

在 `01_veh.cpp` 中，VEH 回调返回的是：

```cpp
return EXCEPTION_CONTINUE_SEARCH;
```

含义是“我不处理这个异常，继续向上查找处理者”。由于后续没有真正的处理者，程序最终会崩溃。

在 `02_seh.cpp` 中，`__except` 使用的是：

```cpp
__except (EXCEPTION_EXECUTE_HANDLER)
```

含义是“这个异常我处理掉，执行完 `__except` 块后继续运行后续代码”。因此程序没有崩溃，正常退出。

| 机制 | 调用顺序 | 典型用途 | 是否能阻止崩溃 |
|------|----------|----------|----------------|
| VEH | 最早 | 全局日志、崩溃报告、监控 | 理论上能，但工程上通常不处理，因为缺少局部上下文，修复困难且危险 |
| SEH | VEH 之后 | 局部保护、容错、资源清理 | 能，通过 `EXCEPTION_EXECUTE_HANDLER` |
| UEF | 最后 | 生成 MiniDump、崩溃前最后记录 | 不能阻止崩溃，但可实现可控退出 |

### 4.4 SEH 的核心价值

1. **真正的局部恢复能力**  
   可以把可能崩溃的代码段包在 `__try` 中，即使触发硬件异常，也能在 `__except` 中完成清理、记录日志、返回错误码，然后继续执行。

2. **精确控制异常处理范围**  
   VEH 是全局注册的，整个进程的任何异常都会进入 VEH。SEH 是函数级别的，只保护明确需要保护的代码段。

3. **与业务逻辑紧密结合**  
   例如调用第三方 DLL 时担心其内部发生访问违例，可以用 SEH 包裹，捕获后返回失败状态，而不是让整个进程挂掉。

### 4.5 深入理解：VEH 为什么通常返回 `EXCEPTION_CONTINUE_SEARCH`

VEH 回调**不是只能返回 `EXCEPTION_CONTINUE_SEARCH`**。它也可以返回 `EXCEPTION_CONTINUE_EXECUTION`，表示“这个异常我已经处理，继续执行”。

但在 `01_veh.cpp` 的空指针访问例子中：

```cpp
int* p = nullptr;
*p = 42;
```

要让程序继续执行，VEH 必须实际修复问题，例如给 `p` 分配有效内存，并修改 CPU 上下文让 `p` 指向那块内存，然后让程序回到崩溃指令重新执行。

这在 VEH 的全局回调里非常困难，因为：

- 不知道局部变量 `p` 在栈上的地址；
- 不知道崩溃指令执行完后该跳转到哪里；
- 即使硬改上下文让程序继续，也可能造成更隐蔽的 bug。

因此 VEH 的典型工程用法是：**记录异常信息、生成 dump、上报崩溃，然后让程序正常崩溃**。返回 `EXCEPTION_CONTINUE_SEARCH` 是合理且常见的选择。

技术上，VEH 也可以做“跳过指令”这种危险处理，例如：

```cpp
LONG WINAPI VectoredHandler(PEXCEPTION_POINTERS pExceptionInfo) {
    DWORD code = pExceptionInfo->ExceptionRecord->ExceptionCode;
    if (code == EXCEPTION_ACCESS_VIOLATION) {
        // 人为把 RIP/EIP 往后跳，跳过崩溃指令
        pExceptionInfo->ContextRecord->Rip += 3;
        printf("[VEH] Skipping the crashing instruction.\n");
        return EXCEPTION_CONTINUE_EXECUTION;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}
```

这种写法实战中非常危险，仅用于说明 VEH 确实具备处理能力。

### 4.6 SEH 中“处理”的具体含义与示例

在 `TestNullPointer` 中：

```cpp
__try {
    int* p = nullptr;
    *p = 42;
}
__except (EXCEPTION_EXECUTE_HANDLER) {
    DWORD code = GetExceptionCode();
    printf("  [SEH] Exception caught: %s (0x%08X)\n", ExceptionCodeToString(code), code);
}
printf("  [SEH] Program continues, no crash.\n");
```

`__except` 块里的代码就是处理：获取异常代码、打印异常类型、决定让程序继续执行。

**“处理”的核心含义是：异常不再向上传播，程序控制权回到程序员手里。** 这与直接崩溃的区别在于，崩溃时程序终止，而 SEH 处理后程序可以走一条你定义的路径继续。

SEH 的“处理”还可以做得更实质，例如：

**示例 1：修复指针，让后续代码继续**

```cpp
void TestNullPointerWithFix() {
    int* p = nullptr;
    int fallbackValue = 0;

    __try {
        *p = 42;  // 会崩溃
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("[SEH] Null pointer detected, using fallback buffer.\n");
        p = &fallbackValue;  // 修复指针
        *p = 42;             // 重新执行安全写入
    }

    printf("[SEH] Value = %d\n", *p);
}
```

**示例 2：释放资源，返回错误码**

```cpp
bool LoadConfigFile(const char* path) {
    FILE* fp = nullptr;
    bool ok = false;

    __try {
        fopen_s(&fp, path, "r");
        if (!fp) return false;

        char* buffer = (char*)malloc(1024);
        if (!buffer) return false;

        // 假设这里某行代码意外触发了访问违例
        int* p = nullptr;
        *p = 1;

        ok = true;
        free(buffer);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        printf("[SEH] Crash during loading config, cleaning up.\n");
        ok = false;
    }

    if (fp) fclose(fp);
    return ok;
}
```

这里 SEH 的作用是：即使中间某一步崩溃，也能保证资源被关闭，函数返回 `false` 而不是把整个进程带崩。

**示例 3：根据异常类型做不同处理**

```cpp
__try {
    int a = 10, b = 0;
    int c = a / b;
}
__except (GetExceptionCode() == EXCEPTION_INT_DIVIDE_BY_ZERO
          ? EXCEPTION_EXECUTE_HANDLER
          : EXCEPTION_CONTINUE_SEARCH) {
    printf("[SEH] Divide by zero handled, using default value.\n");
}
```

## 5. `__except` 过滤表达式取值

| 值 | 含义 |
|----|------|
| `EXCEPTION_EXECUTE_HANDLER` | 处理该异常，执行 `__except` 块后继续运行 |
| `EXCEPTION_CONTINUE_SEARCH` | 不处理，继续向上查找其他处理者 |
| `EXCEPTION_CONTINUE_EXECUTION` | 忽略异常，从异常发生处继续执行（通常需要修复问题） |

过滤表达式也可以是一个函数调用，如 `02_seh.cpp` 中的 `ExceptionFilter()`，根据异常类型动态决定处理方式。

## 6. 实验中的注意事项

- 运行 `02_seh.cpp` 时**不会崩溃**，这是预期行为，因为 SEH 已经处理了所有异常。
- 如果想观察崩溃效果，可以将 `__except (EXCEPTION_EXECUTE_HANDLER)` 改为 `__except (EXCEPTION_CONTINUE_SEARCH)`。
- 在 Visual Studio 中按 `F5` 调试时，VS 调试器可能会优先截获硬件异常；使用 `Ctrl+F5` 运行或不调试更符合实际生产环境行为。

## 7. 下一步建议

1. 尝试修改 `02_seh.cpp` 中的过滤表达式，观察不同返回值对程序行为的影响。
2. 将 SEH 与 `01_veh.cpp` 的 VEH + MiniDump 流程结合，理解完整异常处理链。
3. 阅读 `crash_blog.md` 中关于 SEH 的章节，加深对底层机制的理解。
