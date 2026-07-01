# 01_veh 实验总结

## 实验目的

验证 Windows VEH (Vectored Exception Handler) 在程序崩溃时的调用时机，以及 Visual Studio 调试器对 VEH 执行流程的影响。

## 实验代码

- 主文件: [01_veh.cpp](01_veh.cpp)
- 核心逻辑:
  1. 调用 `AddVectoredExceptionHandler` 注册全局 VEH 处理函数。
  2. 在 VEH 回调中打印异常代码、异常地址、RIP 寄存器等信息。
  3. 回调返回 `EXCEPTION_CONTINUE_SEARCH`，表示不处理异常，继续向后传递。
  4. 故意执行 `int* p = nullptr; *p = 42;` 触发空指针访问异常 (0xC0000005)。

## 实验现象

### 现象一: 在 Visual Studio 中按 `F5` 调试运行

```text
=== Demo 01: VEH (Vectored Exception Handling) ===
Step 1: Register VEH handler (highest priority)...
VEH registered successfully.

Step 2: Intentionally trigger null pointer access exception...
```

输出到此停止，随后 Visual Studio 弹出调试器异常提示，例如 `p was nullptr` 或 `0xC0000005: Access violation`。

**关键观察**: VEH 注册成功的信息打印了，但 `[VEH] Exception caught!` 等信息没有打印。

### 现象二: 注释掉 VectoredHandler 相关代码后按 `F5` 运行

程序同样会崩溃，调试器同样会提示空指针错误。区别只是少了一行 `VEH registered successfully.` 的注册成功提示。

## 原因分析

Windows 的异常处理调用链路大致如下:

```text
调试器 (如果已附加) → VEH → SEH → UEF → 操作系统 / WER
```

当 Visual Studio 以调试模式 (`F5`) 启动程序时，调试器已经附加到进程。异常发生时，调试器排在最前面拦截，程序会被立即暂停。因此 VEH 回调虽然已注册，但没有机会被执行。

也就是说:

- **VEH 本身没有阻止崩溃的能力**，它的作用是在崩溃瞬间获得一次"抢先通知"的机会，常用于记录日志、生成 dump、上报崩溃等。
- 注释掉 VEH 后仍然崩溃，是因为 `*p = 42` 本身就是非法内存访问，崩溃是操作系统层面的必然结果，不是由 VEH 引起的。
- 调试器提示的异常信息和 VEH 打印的信息是两回事: 前者来自 Visual Studio 调试器，后者来自我们自己注册的 VEH 回调。

## 验证方法

要观察到 VEH 回调实际执行，需要让调试器不抢在 VEH 之前拦截异常。

### 方法 1: 不附加调试器运行

在 Visual Studio 中按 `Ctrl + F5` (开始执行，不调试)，或者直接双击运行编译好的 `.exe`。

预期输出:

```text
=== Demo 01: VEH (Vectored Exception Handling) ===
Step 1: Register VEH handler (highest priority)...
VEH registered successfully.

Step 2: Intentionally trigger null pointer access exception...
[VEH] Exception caught!
[VEH] Exception code: 0xC0000005
[VEH] Exception address: 0x...
[VEH] RIP/EIP = 0x...
[VEH] Not handled, continue searching...
```

### 方法 2: 关闭调试器对 Win32 异常的中断

如果仍然想按 `F5` 运行，可以在 Visual Studio 中调整异常设置:

1. 菜单栏选择 `调试` → `窗口` → `异常设置` (快捷键 `Ctrl + Alt + E`)。
2. 找到 `Win32 异常`。
3. 取消勾选 `引发时中断`。
4. 重新按 `F5` 运行。

这样调试器不会抢在 VEH 之前拦截异常，VEH 回调就能正常执行并打印信息。

## 如何利用 VEH 捕获的信息排查崩溃

当 VEH 成功执行并打印出以下信息时，每个字段都有明确的排查价值:

```text
[VEH] Exception code: 0xC0000005
[VEH] Exception address: 0x00007FF6B57B1A0D
[VEH] RIP/EIP = 0x7FF6B57B1A0D
```

### 字段解析

#### Exception code（异常代码）

异常代码用于判断崩溃类型。`0xC0000005` 即 `EXCEPTION_ACCESS_VIOLATION`，表示访问了非法内存地址。

常见异常代码:

| 代码 | 含义 | 典型原因 |
|------|------|---------|
| `0xC0000005` | Access Violation | 空指针、野指针、数组越界、use-after-free |
| `0xC0000094` | Integer Divide by Zero | 整数除零 |
| `0xC00000FD` | Stack Overflow | 无限递归、栈上分配过大内存 |
| `0xC000001D` | Illegal Instruction | 执行了 CPU 不认识的指令 |
| `0x80000003` | Breakpoint | 调试断点 |
| `0xC0000409` | Stack Buffer Overrun | 栈缓冲区溢出 |

#### Exception address / RIP（异常地址）

异常地址是崩溃发生时 CPU 正在执行的指令地址。它是定位崩溃位置的关键。配合符号文件 (PDB)，可以还原成具体的函数名、文件名和行号，例如:

```text
MyApp.exe!CrashDemo::DoSomething + 0x2d
```

### 真实环境中的使用方法

#### 1. 生成 MiniDump（最重要）

单独打印寄存器的价值有限，真正有价值的是保存崩溃瞬间的完整现场。在 VEH 或 UEF 中调用 `MiniDumpWriteDump` 生成 `.dmp` 文件:

```cpp
MiniDumpWriteDump(
    GetCurrentProcess(),
    GetCurrentProcessId(),
    hFile,
    MiniDumpWithFullMemory,  // 或 MiniDumpNormal
    &mei,
    nullptr,
    nullptr
);
```

拿到 dump 后，使用 WinDbg 或 Visual Studio 配合 PDB 符号可以分析:

- 崩溃时的调用栈
- 每个函数的局部变量
- 寄存器值
- 内存内容
- 所有线程状态

Demo 03 (`03_uef_minidump.cpp`) 演示了如何基于 UEF 生成 MiniDump。

#### 2. 地址转符号

即使没有生成 dump 的条件，也可以使用 `DbgHelp` 库把异常地址解析为函数名:

```cpp
#include <dbghelp.h>

void LogSymbol(void* addr) {
    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME] = {0};
    SYMBOL_INFO* symbol = (SYMBOL_INFO*)buffer;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = MAX_SYM_NAME;

    SymInitialize(GetCurrentProcess(), nullptr, TRUE);

    DWORD64 displacement = 0;
    if (SymFromAddr(GetCurrentProcess(), (DWORD64)addr, &displacement, symbol)) {
        printf("崩溃位置: %s + 0x%llX\n", symbol->Name, displacement);
    }
}
```

配合 `CaptureStackBackTrace` 还可以进一步获取崩溃前的调用栈。

#### 3. 崩溃上报

可以把以下信息打包发送到服务器:

- 异常代码
- 异常地址
- 模块路径和版本号
- 调用栈
- 崩溃前的日志片段
- dump 文件（如果体积允许）

通过后台聚合，可以看到:

- 哪个版本、哪个模块、哪个函数崩溃最多
- 崩溃用户的环境分布
- 按异常类型聚合统计

常用方案包括 Google Breakpad、Google Crashpad，或基于 UEF + MiniDump 自研上报系统。

#### 4. 本地日志兜底

在崩溃前把关键信息写入本地日志文件，方便用户反馈问题时人工收集:

```text
[2026-06-24 10:30:15] CRASH: code=0xC0000005, addr=0x7FF6B57B1A0D
[2026-06-24 10:30:15] Module: MyApp.exe, Version: 1.2.3
```

### 为什么通常用 UEF 而不是 VEH

VEH 是最早被调用的异常处理机制，但它的问题是**每次异常都会被调用**，包括已经被 SEH `__try/__except` 捕获的异常。如果只是想在"真正崩溃"时做处理，通常使用 UEF (Unhandled Exception Filter) 更合适:

```cpp
SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
```

UEF 只在没有任何人处理异常时调用，正好是生成 dump、上报崩溃的最后机会。

### 总结

- `Exception code` 告诉你**为什么崩溃**。
- `Exception address` 告诉你**在哪里崩溃**。
- 结合符号文件可以把地址还原成函数名和行号。
- 最佳实践是生成 MiniDump，离线分析调用栈和内存状态。
- 线上环境通常使用 UEF 而不是 VEH 作为崩溃捕获入口。

## 无 VS 时的崩溃定位：WinDbg + PDB + dump

如果不想使用 Visual Studio，或者在用户环境、服务器环境中没有 VS，可以使用 WinDbg 配合 PDB 符号文件定位崩溃位置。基本思路是：**先保存崩溃现场（dump），再用 WinDbg 离线分析**。

### 为什么必须先生成 dump

程序崩溃后进程会直接退出，内存、寄存器、调用栈全部丢失。打印出来的异常地址是绝对地址，每次运行都不一样（ASLR 随机化），单靠一个地址无法直接对应到源代码。

dump 文件保存了崩溃瞬间的完整现场，配合 PDB 才能做离线分析。

### 工具准备

WinDbg 包含在 Windows SDK 中，默认路径通常在:

```text
C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\windbg.exe
```

命令行版本 cdb 也在同一目录下。

### 方式一：在线调试（适合开发环境一次性定位）

直接用 WinDbg 启动程序，崩溃时自动中断:

```text
windbg.exe -g M:\20260504-CPP-crash\visual_studio_projects\crash_labs\x64\Debug\crash_labs.exe
```

`-g` 表示忽略初始断点，让程序直接运行。当 `*p = 42` 触发访问违例时，WinDbg 会自动停在异常现场。

然后输入:

```text
.sympath M:\20260504-CPP-crash\visual_studio_projects\crash_labs\x64\Debug
.reload
k
```

预期看到类似输出:

```text
 # Child-SP          RetAddr               Call Site
00 000000dd`8831f710 00007ff6`b57b2469     crash_labs!main+0x8d [M:\20260504-CPP-crash\crash_demos\01_veh.cpp @ 40]
01 000000dd`8831f850 00007ff6`b57b2312     crash_labs!invoke_main+0x39 [...]
02 000000dd`8831f8a0 00007ff6`b57b21ce     crash_labs!__scrt_common_main_seh+0x132 [...]
...
```

这表示崩溃发生在 `01_veh.cpp` 第 40 行的 `main` 函数中。

常用命令:

| 命令 | 作用 |
|------|------|
| `k` | 显示当前调用栈 |
| `!analyze -v` | 自动分析崩溃原因，输出异常代码、调用栈等 |
| `lsa .` | 显示当前地址对应的源代码 |
| `dv` | 显示当前函数的局部变量 |
| `~* k` | 显示所有线程的调用栈 |
| `.lastevent` | 查看最后一次异常事件 |

### 方式二：离线调试（适合用户环境、线上环境）

在线调试需要一直开着 WinDbg，不适合用户环境。更实用的做法是在程序崩溃时自动生成 dump 文件，事后用 WinDbg 分析。

#### 1. 在代码中添加 dump 生成逻辑

通常使用 UEF（Unhandled Exception Filter）作为生成 dump 的入口。核心代码:

```cpp
#include <windows.h>
#include <dbghelp.h>
#include <cstdio>

#pragma comment(lib, "DbgHelp.lib")

LONG WINAPI MyUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    wchar_t dumpPath[MAX_PATH] = {0};
    GetModuleFileNameW(nullptr, dumpPath, MAX_PATH);
    wchar_t* ext = wcsrchr(dumpPath, L'.');
    if (ext) {
        wcscpy_s(ext, MAX_PATH - (ext - dumpPath), L".dmp");
    } else {
        wcscat_s(dumpPath, MAX_PATH, L".dmp");
    }

    HANDLE hFile = CreateFileW(dumpPath, GENERIC_WRITE, 0, nullptr,
                               CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        MINIDUMP_EXCEPTION_INFORMATION mei = {0};
        mei.ThreadId = GetCurrentThreadId();
        mei.ExceptionPointers = pExceptionInfo;
        mei.ClientPointers = FALSE;

        MiniDumpWriteDump(
            GetCurrentProcess(),
            GetCurrentProcessId(),
            hFile,
            MiniDumpWithFullMemory,
            &mei,
            nullptr,
            nullptr
        );

        CloseHandle(hFile);
        wprintf(L"Dump 已生成: %s\n", dumpPath);
    }

    return EXCEPTION_EXECUTE_HANDLER;
}

int main() {
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    int* p = nullptr;
    *p = 42;

    return 0;
}
```

编译时需要链接 `DbgHelp.lib`。

#### 2. 运行程序生成 dump

直接双击 exe 或用命令行运行。崩溃后会在 exe 所在目录生成 `crash_labs.dmp`。

#### 3. 用 WinDbg 分析 dump

打开 WinDbg:

```text
文件 → 打开崩溃转储 → 选择 crash_labs.dmp
```

设置符号路径:

```text
.sympath M:\20260504-CPP-crash\visual_studio_projects\crash_labs\x64\Debug
.symfix+ c:\symbols
.reload
```

分析崩溃:

```text
!analyze -v
```

或者手动查看调用栈:

```text
.ecxr
k
```

> 注意: 如果用 `.ecxr` 提示 `Unable to get exception context, HRESULT 0x8000FFFF`，通常是因为当前已经离开异常上下文，或者 dump 没有保存异常上下文。此时直接用 `k` 或 `!analyze -v` 即可。

### 其他辅助工具

#### cdb（命令行版 WinDbg）

```text
cdb -g M:\...\crash_labs.exe
```

崩溃后输入 `.ecxr` 和 `k` 查看调用栈，最后输入 `q` 退出。

#### ProcDump

可以挂接到进程，崩溃时自动抓 dump:

```text
procdump -e -ma crash_labs.exe
```

- `-e`: 捕获未处理异常
- `-ma`: 捕获完整内存 dump

### 无 VS 时的定位流程总结

```text
程序运行 → 崩溃 → 生成 .dmp（或通过 WinDbg/cdb 在线捕获）
    ↓
WinDbg 打开 dump
    ↓
设置符号路径 .sympath <PDB 目录>
    ↓
.reload 加载符号
    ↓
!analyze -v 或 k 查看调用栈
    ↓
lsa . 查看源代码
    ↓
定位到崩溃函数和行号
```

## 分析生成的 dump 文件：WinDbg 实战

本节以实际生成的 `crash_labs.dmp` 为例，演示完整的 WinDbg 分析过程。

### 打开 dump 文件

启动 WinDbg，选择:

```text
文件 → 打开崩溃转储 → 选择 crash_labs.dmp
```

加载后 WinDbg 会显示 dump 的基本信息:

```text
User Mini Dump File with Full Memory: Only application data is available
...
This dump file has an exception of interest stored in it.
The stored exception information can be accessed via .ecxr.
(7188.8fe4): Access violation - code c0000005 (first/second chance not available)
For analysis of this file, run !analyze -v
```

### 设置符号路径并加载

在 WinDbg 命令窗口输入:

```text
.sympath M:\20260504-CPP-crash\visual_studio_projects\crash_labs\x64\Debug
.symfix+ c:\symbols
.reload
```

- 第一行指向项目编译出的 PDB 文件所在目录。
- 第二行设置 Microsoft 公共符号缓存路径，用于解析系统 DLL。
- 第三行重新加载符号。

### 执行自动分析

输入:

```text
!analyze -v
```

### 关键输出解析

`!analyze -v` 会输出大量信息，其中最重要的是:

#### 异常类型和地址

```text
EXCEPTION_RECORD:  (.exr -1)
ExceptionAddress: 00007ff6a18c1f08 (crash_labs!main+0x00000000000000a8)
   ExceptionCode: c0000005 (Access violation)
Attempt to write to address 0000000000000000
```

- `ExceptionCode: c0000005`: 访问违例。
- `ExceptionAddress`: 崩溃时的指令地址，已解析为 `crash_labs!main+0xa8`。
- `Attempt to write to address 0000000000000000`: 尝试向空地址写入。

#### 崩溃时的寄存器状态

```text
rax=0000000000000000 rbx=0000000000000000 rcx=1ffdb6af94060000
...
rip=00007ff6a18c1f08 rsp=00000061bdf9fca0 rbp=00000061bdf9fcc0
```

- `rax=0`: 对应代码中的 `p = nullptr`。
- `rip=00007ff6a18c1f08`: 当前执行指令地址，即 `crash_labs!main+0xa8`。

#### 反汇编确认

```text
crash_labs!main+0xa8:
00007ff6`a18c1f08 c7002a000000    mov     dword ptr [rax],2Ah ds:00000000`00000000=????????
```

`mov dword ptr [rax],2Ah` 就是把十六进制 `0x2A`（十进制 42）写入 `rax` 指向的地址。由于 `rax` 为 0，这条指令等价于 `*p = 42;`。

#### 调用栈

```text
STACK_TEXT:  
00000061`bdf9fca0 00007ff6`a18c2a99     : ... : crash_labs!main+0xa8
00000061`bdf9fde0 00007ff6`a18c2942     : ... : crash_labs!invoke_main+0x39
00000061`bdf9fe30 00007ff6`a18c27fe     : ... : crash_labs!__scrt_common_main_seh+0x132
00000061`bdf9fea0 00007ff6`a18c2b2e     : ... : crash_labs!__scrt_common_main+0xe
00000061`bdf9fed0 00007ffc`ccabe957     : ... : crash_labs!mainCRTStartup+0xe
00000061`bdf9ff00 00007ffc`ce447c1c     : ... : kernel32!BaseThreadInitThunk+0x17
00000061`bdf9ff30 00000000`00000000     : ... : ntdll!RtlUserThreadStart+0x2c
```

最上面一行 `crash_labs!main+0xa8` 就是崩溃点，下面是程序启动到 main 的调用链。

#### 源代码定位

```text
FAULTING_SOURCE_LINE:  M:\20260504-CPP-crash\crash_demos\01_veh.cpp
FAULTING_SOURCE_FILE:  M:\20260504-CPP-crash\crash_demos\01_veh.cpp
FAULTING_SOURCE_LINE_NUMBER:  126

FAULTING_SOURCE_CODE:  
   122: #endif
   123: 
   124:     printf("Step 2: Intentionally trigger null pointer access exception...\n");
   125:     int* p = nullptr;
>  126:     *p = 42;  // ...
   127: 
   128:     // This line will not be reached.
   129:     printf("This line will not be printed.\n");
```

PDB 中的源码信息直接把崩溃定位到了 `01_veh.cpp` 第 126 行的 `*p = 42;`。

#### 出错变量名

```text
FAULTING_LOCAL_VARIABLE_NAME:  p
```

这进一步确认是局部变量 `p` 导致的崩溃。

### 分析结论

通过 `!analyze -v` 一条命令，我们就从 dump 中得到了:

| 信息 | 值 |
|------|-----|
| 异常类型 | Access violation (0xC0000005) |
| 崩溃模块 | crash_labs.exe |
| 崩溃函数 | main |
| 崩溃文件 | 01_veh.cpp |
| 崩溃行号 | 126 |
| 崩溃代码 | `*p = 42;` |
| 出错变量 | `p` |
| 错误原因 | 向空地址写入数据 |

### 直接使用 `k` 查看调用栈

除了 `!analyze -v`，也可以直接输入:

```text
k
```

输出:

```text
 # Child-SP          RetAddr               Call Site
00 00000061`bdf9fca0 00007ff6`a18c2a99     crash_labs!main+0xa8 [M:\20260504-CPP-crash\crash_demos\01_veh.cpp @ 126]
01 00000061`bdf9fde0 00007ff6`a18c2942     crash_labs!invoke_main+0x39
...
```

这也直接显示了崩溃位置。

### 注意事项

- 如果 WinDbg 中源码注释显示为乱码，是因为 WinDbg 默认编码对 UTF-8 中文支持不佳，但**行号和代码本身是正确的**，不影响分析。
- 如果 `k` 命令只显示地址而不显示函数名，说明 PDB 没有正确加载，检查 `.sympath` 路径是否正确，然后重新执行 `.reload`。
- 如果想看崩溃时的局部变量，可以在 `k` 定位到栈帧后输入 `dv`。

### dump 分析的核心价值

之前 VEH 打印的异常地址每次运行都会变化，例如 `0x00007FF6B57B1A0D`。但 dump + PDB 分析后，得到的是固定的、可读的源代码位置:

```text
01_veh.cpp @ 126: *p = 42;
```

这就是 dump 文件配合符号系统的真正价值：把变化的内存地址还原成稳定的代码位置，从而快速定位 bug。

## 实验结论

1. VEH 注册成功不代表一定会被调用，调用顺序取决于是否有调试器附加。
2. 在 Visual Studio 调试模式 (`F5`) 下，调试器优先于 VEH 捕获异常，导致 VEH 回调无法执行。
3. 要验证 VEH 的行为，应使用 `Ctrl + F5` 不调试运行，或关闭调试器对 Win32 异常的中断。
4. VEH 的典型用途是崩溃瞬间的自定义处理 (如打印日志、生成 MiniDump、上报崩溃)，而不是阻止崩溃本身。
5. 真实项目中，应把 VEH/UEF 捕获的信息用于生成 dump、符号解析、崩溃上报和本地日志记录，而不是仅打印到控制台。
6. 不使用 Visual Studio 时，可以通过 WinDbg 在线调试或分析 dump 的方式，配合 PDB 符号文件定位崩溃代码。
7. 通过 `!analyze -v` 命令分析 dump，可以直接定位到崩溃的源文件、行号、出错变量和具体代码。
