// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 CH1DLLEXPORTS_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// CH1DLLEXPORTS_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef CH1DLLEXPORTS_EXPORTS
#define CH1DLLEXPORTS_API __declspec(dllexport)
#else
#define CH1DLLEXPORTS_API __declspec(dllimport)
#endif

// 此类是从 dll 导出的
class CH1DLLEXPORTS_API Cch1dllexports {
public:
	Cch1dllexports(void);
	// TODO: 在此处添加方法。
};

extern CH1DLLEXPORTS_API int nch1dllexports;

CH1DLLEXPORTS_API int fnch1dllexports(void);
