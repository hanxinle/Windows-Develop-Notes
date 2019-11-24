# 该文档记录如何避免重复#include问题

原因:每次#include就是一次复制,开销很大.

首先,在Visual Studio中,很多文件头有**#program once"**字样,这是VS特有的避免重复包含的方法,和编译器相关.如何在Linux德国其它平台解决重复包含的问题呢?如何在C++的源文件包含C语言写的库呢?

假设头文件为 tools.h,它的C实现文件是 tools.c,C++工程文件为 main.cpp,重点是**tools.h**中的写法,应该有如下形式:

```C++

#ifndef __TOOLS_H     //宏定义ifndef,define,endif 避免重复包含
#define __TOOLS_H

#ifdef __cplusplus    //C++内部定义的,代表C++方式编译的源码
extern "C" {          //extern "C"告诉编译器按照C语言标准编译函数  
#endif // __cplusplus

int func1();
int func2();
int func3();

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !__TOOLS_H


```