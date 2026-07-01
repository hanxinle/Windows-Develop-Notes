# Windows 编码与文件格式问题总结

## 1. UTF-8 中文在 Windows 控制台乱码的原因

Windows 控制台（cmd / PowerShell）有一个"代码页"设置，决定它如何解码输出的字节流：

| 代码页 | 编码 | 说明 |
|--------|------|------|
| 936 | GBK / GB2312 | 中文 Windows 默认 |
| 65001 | UTF-8 | 国际通用编码 |

当源码保存为 UTF-8 时，中文字符按 UTF-8 编码。程序运行时，`printf` 把这些字节原样输出到控制台。如果控制台当前代码页是 GBK（默认），就会用 GBK 规则去解释 UTF-8 字节，导致乱码。

改为 GBK/GB2312 保存源码后，编码和解码双方一致，所以中文能正常显示。

## 2. 解决办法

### 方法 1：程序内设置控制台输出代码页（推荐）

在 `main()` 开头加入：

```cpp
#include <windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);  // 设置控制台为 UTF-8
    // ...
    return 0;
}
```

这样源码可以继续使用 UTF-8，跨平台兼容性更好。

### 方法 2：运行前手动修改代码页

在 cmd 或 PowerShell 中执行：

```text
chcp 65001
```

然后再运行程序。

### 方法 3：源码改用 GBK/GB2312 保存

简单直接，但会破坏跨平台一致性。如果项目只在中文 Windows 环境使用，可以考虑。

## 3. LF 与 CRLF 的区别

| 名称 | 字符 | 使用场景 |
|------|------|----------|
| LF（Line Feed） | `\n` | Linux / macOS / 大多数跨平台项目 |
| CRLF（Carriage Return + Line Feed） | `\r\n` | Windows 传统格式 |

Visual Studio 提示"是否保留 LF"或"转换为 CRLF"，是因为当前文件使用 Unix 风格换行符。

**为什么保留或改为 CRLF 通常都 OK？**

- 现代编译器（MSVC、GCC、Clang）对两种换行符都支持，不影响编译。
- 现代 Windows 记事本也已支持 LF。
- Git 可以通过 `core.autocrlf` 自动转换。

**建议：**

- Windows 本地开发：CRLF 也可以接受。
- 跨平台项目：统一使用 LF，并在 `.gitattributes` 中配置：

```text
*.cpp text eol=lf
*.h text eol=lf
*.md text eol=lf
```

## 4. UTF-8 with BOM vs UTF-8 without BOM

**BOM**（Byte Order Mark）是放在文件开头的三个字节 `EF BB BF`，用于标识"这是一个 UTF-8 文件"。

| 类型 | 说明 | 适用场景 |
|------|------|----------|
| UTF-8 with BOM | 文件开头包含 `EF BB BF` | 微软工具（VS、记事本）识别更稳定 |
| UTF-8 without BOM | 文件开头无 BOM | Linux / macOS / GCC / Clang / Git 更偏好 |

**对 C++ 源码的建议：**

- 推荐使用 **UTF-8 without BOM**，跨平台兼容性最好。
- 部分旧工具或脚本解释器对 BOM 开头的文件处理不友好。
- 如果只在 Windows + VS 环境工作，with BOM 也能正常工作。

## 5. 推荐配置组合

对于 Windows C++ 项目，建议统一采用以下组合：

1. **源码编码**：UTF-8 without BOM。
2. **控制台输出**：程序启动时调用 `SetConsoleOutputCP(CP_UTF8);`。
3. **换行符**：跨平台项目统一使用 LF，并通过 `.gitattributes` 固定。
4. **编译选项**：VS 中可加上 `/utf-8`，强制编译器按 UTF-8 解析源码。

这样可以在保证中文正常显示的同时，获得最好的跨平台一致性。
