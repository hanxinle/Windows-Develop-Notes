# 03_uef_minidump 实验总结

## 1. 实验目的

理解 Windows **Unhandled Exception Filter (UEF)** 的工作机制，掌握在程序崩溃时自动生成 **MiniDump** 文件的方法。这是生产环境中最常用的崩溃捕获与现场保存方案。

## 2. 实验代码

- 主文件: [03_uef_minidump.cpp](03_uef_minidump.cpp)
- 核心逻辑:
  1. 调用 `SetUnhandledExceptionFilter` 注册全局 UEF 回调 `MyUnhandledExceptionFilter`。
  2. 保存旧的 UEF 指针，便于链式调用。
  3. UEF 回调中打印异常代码、异常地址，并调用 `MiniDumpWriteDump` 生成 `.dmp` 文件。
  4. 故意触发空指针写入，观察 UEF 被调用和 dump 生成的完整流程。

## 3. 运行现象

程序按 `Ctrl + F5`（不调试运行）或双击 exe 运行时，典型输出如下：

```text
=== Demo 03: Unhandled Exception Filter + MiniDump ===
This is the most commonly used crash-capture solution in production.

Step 1: Register global unhandled exception filter (SetUnhandledExceptionFilter)...
Global exception filter registered successfully.

Step 2: Program is running normally for a while...
Simulating some business logic...

Step 3: Simulating a bug in the code, triggering a crash...
Triggering crash: null pointer write...

[UEF] ========== Unhandled exception occurred ==========
[UEF] Exception code: 0xC0000005
[UEF] Exception address: 0x...
[UEF] Generating MiniDump...
  MiniDump generated successfully: ...\crash_labs.dmp
[UEF] ========== Application is about to exit ==========
```

运行结束后，程序所在目录会生成一个与 exe 同名的 `.dmp` 文件。

## 4. 关键技术点

### 4.1 UEF 是什么

**UEF** 是 **Unhandled Exception Filter** 的缩写，中文叫**未处理异常过滤器**。

当程序中发生了一个异常，但这个异常没有被任何 `try/catch`、`__except`、线程级处理等机制捕获时，Windows 会给这个“没人管”的异常最后一次被处理的机会，这就是 UEF。

注册方式：

```cpp
g_oldFilter = SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
```

一旦发生未处理异常，Windows 就会调用你指定的 `MyUnhandledExceptionFilter` 函数。你可以在里面做最后一件事，比如生成 dump、写日志、弹提示框，然后决定程序是继续运行还是退出。

所以 UEF 是**最后机会**的异常处理机制。

### 4.2 为什么用 UEF 生成 MiniDump

在 UEF 中生成 MiniDump 有几个好处：

- **只处理真正未处理的异常**：UEF 只在没有任何人处理异常时被调用，避免了 VEH 那样“每次异常都触发”的噪音。
- **全局统一入口**：一个回调函数就能覆盖整个进程的未处理异常。
- **可控性强**：dump 文件名、路径、类型、附加信息都可以自定义。
- **适合线上**：`MiniDumpNormal` 体积小，配合自定义回调添加日志后，上报速度快。

### 4.3 MiniDump 生成要点

代码中关键 API：

```cpp
MiniDumpWriteDump(
    GetCurrentProcess(),      // 当前进程句柄
    GetCurrentProcessId(),    // 当前进程 ID
    hFile,                    // dump 文件句柄
    MiniDumpNormal,           // dump 类型
    &mei,                     // 异常信息
    nullptr,                  // 用户流
    nullptr                   // 回调信息
);
```

`MINIDUMP_EXCEPTION_INFORMATION` 需要填入：

- `ThreadId`：崩溃线程 ID。
- `ExceptionPointers`：异常指针，包含异常记录和上下文记录。
- `ClientPointers`：设为 `FALSE`，表示指针是进程自身地址空间中的地址。

## 5. VEH / SEH / VEC / UEF / WER 对比

Windows 提供了多层异常处理机制，下面是它们的区别：

| 机制 | 简称 | 调用顺序 | 作用 | 典型用途 |
|------|------|----------|------|----------|
| **Vectored Exception Handling** | VEH | 1（最先调用） | 第三方可以抢先处理异常 | 全局日志、崩溃报告、调试注入 |
| **Structured Exception Handling** | SEH | 2 | 基于栈帧的异常处理，`__try/__except` 语法 | 局部保护、容错、资源清理 |
| **Vectored Continue Execution** | VEC | 3（SEH 之后） | 当 SEH 不处理时继续交给 VEH 处理 | 很少单独用，一般和 VEH 配合 |
| **Unhandled Exception Filter** | UEF | 4（最后机会） | 全局最后机会处理 | 生成 MiniDump、崩溃前最后记录 |
| **Windows Error Reporting** | WER | 5（都没处理时） | 系统帮你上报崩溃 | 系统自动备份、生成 dump |

### 5.1 调用链总结

```text
VEH → SEH → VEC → UEF → WER
```

任何一层决定处理异常（返回 `EXCEPTION_EXECUTE_HANDLER` 或 `EXCEPTION_CONTINUE_EXECUTION`），就不会继续往后传。

### 5.2 各自特点

| 机制 | 优点 | 缺点 | 工程建议 |
|------|------|------|----------|
| VEH | 最早被调用，灵活，不需要修改栈帧 | 每次异常都会触发，包括已被 SEH 捕获的异常 | 需要抢先处理或注入调试时使用 |
| SEH | 语言集成 `__try/__except`，局部控制强 | 仅 Windows/MSVC，基于栈帧 | 包裹可疑代码块，实现局部恢复 |
| VEC | 补充 SEH 之后继续处理的能力 | 使用场景较少 | 一般不单独使用 |
| UEF | 只处理真正未处理异常，可控性强 | 是最后机会，不能阻止崩溃 | **最常用**：生成 dump、崩溃上报 |
| WER | 不需要写代码，系统自动处理 | 可控性差，dump 位置和类型依赖注册表 | 开启作为备份方案 |

### 5.3 工程选择

- **大多数客户端程序**：只需要注册 `SetUnhandledExceptionFilter` 就够了。
- **需要更灵活的抢先处理**：再加 VEH。
- **WER 保持默认开启**：作为双重保险。
- **复杂项目**：可以考虑 Google Crashpad 等成熟崩溃捕获库。

## 6. 实验结论

1. UEF 是 Windows 提供的**最后机会**异常处理机制，只在异常未被任何上层处理时调用。
2. 在 UEF 中调用 `MiniDumpWriteDump` 可以保存崩溃现场，是生产环境定位问题的核心手段。
3. `MiniDumpNormal` 体积小，适合线上上报；配合自定义回调添加日志等信息，可以在体积和信息量之间取得平衡。
4. 完整的异常处理链路是 `VEH → SEH → VEC → UEF → WER`，UEF 是自定义崩溃捕获的推荐入口。
5. 生成的 `.dmp` 文件需要配合 PDB 符号文件，用 WinDbg 或 Visual Studio 分析，才能还原崩溃的源代码位置。

## 7. 下一步建议

1. 用 WinDbg 打开生成的 `.dmp` 文件，练习 `!analyze -v` 和 `k` 命令。
2. 尝试修改 `MiniDumpNormal` 为 `MiniDumpWithFullMemory`，对比 dump 文件大小。
3. 结合 `01_veh.cpp` 和 `02_seh.cpp`，理解完整异常处理链的调用顺序。
4. 阅读 `crash_blog.md` 中“Windows 异常处理机制全景”和“各层异常处理对比总结”章节，获得更系统的对比。

## 8. 参考资料

- [01_veh_实验总结.md](01_veh_实验总结.md)
- [02_seh_实验总结.md](02_seh_实验总结.md)
- [crash_blog.md](../crash_blog.md)
