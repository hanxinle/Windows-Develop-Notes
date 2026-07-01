# Windows C++ Crash 治理 - 实战代码示例

本目录包含博客文章中所有技术点的可编译、可运行代码示例.
每个 demo 都是独立的，可以单独编译运行.

## 文件列表

| 文件 | 对应文章章节 | 说明 |
|------|------------|------|
| `01_veh.cpp` | VEH 向量异常处理 | 注册 VEH，抢先捕获异常 |
| `02_seh.cpp` | SEH 结构化异常处理 | 捕获空指针、除零等多种异常 |
| `03_uef_minidump.cpp` | UEF + MiniDump | 全局崩溃捕获 + 生成 dump 文件 |
| `04_nullptr.cpp` | 空指针/野指针 | C++ catch vs SEH 对比 |
| `05_heap_corruption.cpp` | 堆破坏 | 缓冲区溢出、double free、use-after-free |
| `06_stack_overflow.cpp` | 栈溢出 | 无限递归、大栈分配、递归深度检查 |
| `07_race_condition.cpp` | 多线程竞争 | 无锁竞争、锁保护、原子变量 |
| `08_uninitialized.cpp` | 未初始化变量 | 局部变量、指针、类成员、数组 |
| `09_dll_load.cpp` | DLL 加载失败 | 错误处理、延迟加载概念、用户提示 |
| `10_memory_exhaust.cpp` | 内存耗尽 | 大分配失败、优雅降级、内存泄漏 |
| `11_pure_virtual.cpp` | 纯虚函数调用 | R6025、生命周期、智能指针 |
| `12_assert.cpp` | 断言 | assert 用法、自定义断言、不变量 |
| `13_static_init_order_*.cpp` | 全局初始化顺序 | SIOF 问题、Meyers' Singleton、显式初始化 |
| `14_cross_module_*.cpp` | 跨模块分配 | DLL 分配/释放不匹配、进程堆 |

## 编译方法

### 方法 1: Visual Studio 命令行 (推荐)

打开 "Developer Command Prompt for VS" 或 "x64 Native Tools Command Prompt"，然后：

```bash
# 进入代码目录
cd M:\20260504-CPP-crash\crash_demos

# 编译单个文件（以 01_veh 为例）
cl /EHsc /W4 /Zi /Fe:demo_01_veh.exe 01_veh.cpp DbgHelp.lib

# 运行
demo_01_veh.exe
```

### 方法 2: CMake

```bash
cd M:\20260504-CPP-crash\crash_demos
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Debug

# 运行
Debug\demo_01_veh.exe
```

### 方法 3: Visual Studio IDE

1. 打开 VS，选择 "打开本地文件夹"，选择 `crash_demos` 目录
2. VS 会自动识别 CMakeLists.txt
3. 在解决方案资源管理器中选择要运行的目标
4. 按 F5 调试运行

## 编译选项说明

| 选项 | 说明 |
|------|------|
| `/EHsc` | 启用 C++ 异常处理 |
| `/W4` | 开启最高级别警告 |
| `/Zi` | 生成调试信息 (PDB) |
| `/DEBUG` | 链接器生成调试信息 |
| `/MD` | 动态链接 CRT（跨模块示例需要） |

## 注意事项

1. **部分 demo 会故意崩溃**：如 `01_veh`、`03_uef_minidump`、`06_stack_overflow` 等，建议在调试器中运行 (F5)
2. **Release vs Debug**：某些 bug（如 use-after-free）在 Debug 模式下可能不崩溃，Release 才会暴露
3. **AddressSanitizer**：VS 2019+ 支持，项目属性 -> C/C++ -> 常规 -> 启用 AddressSanitizer = 是
4. **生成 PDB**：所有示例都配置了生成 PDB，方便分析崩溃 dump

## 学习建议

按以下顺序学习，从基础到进阶：

1. **异常处理基础** -> 01, 02, 03
2. **常见 crash 类型** -> 04, 05, 06, 08
3. **并发问题** -> 07
4. **工程实践** -> 09, 10, 11, 12, 13, 14

每个 demo 都包含详细的注释，运行后观察输出，配合博客文章理解原理.
