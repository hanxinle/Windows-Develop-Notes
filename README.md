#  Windows程序设计


For: windows 10 + Visual Studio 2017.vc6.0 not recommend. 


## MFC工程:

* add 

Dialog base. Show how to use EDIT Control , Button and CString.
 
* use console in MFC 

add consoleInMfc.cpp to the MFC (Dialog base) project.In the xxxDlg.cpp file:    
1. add "# include <istream.> ";   
2. and a button.in function on button add "cout<<""<<endl;";   
3. set Project-Setting-Link-sbusystem:windows as subsystem:console;   
OK,run !      

   (PS:VC6.0 + windows10 or WindowsXP,VS2017 is bad.)

* CFile

CString store file name,write,read.CFile object ops 1.txt including write and read.

* RegEdit

RegEdit could open registry.See rootkey,subkey values.Create new Subkey.Delete Subkey,Subkey value.  

## 语法汇编

通过汇编，学习C++语言的语法，在汇编层面解释语法。

* [环境配置](https://github.com/hanxinle/windows-dev/blob/master/Cpp_Assembly/README.md) 
* [extern "C"及避免头文件重复包含](https://github.com/hanxinle/windows-dev/blob/master/Cpp_Assembly/extern_C_and_multi_files_include.md)
* [内联函数和const常量](Cpp_Assembly/const_inline.md)
* [引用](Cpp_Assembly/reference.md)