////对项目 ch3_multi_thread 中类 xthread 进行封装
/*
 * 不要再一个项目里面定义多个同名的类，或者不同的子类并且重写虚函数
 * 编译时可能会去找其它文件的定义和实现，而不是当前 main 函数所在的文件
 * 如果不注释掉 str_print_t ，文件 3_xxx 会来此文件查找同名子类及其 usr_define_task 实现
 */
//#define _CRT_SECURE_NO_WARNINGS
//#include "xthread.h"
//#include <process.h>
//#include <windows.h>
//#include <iostream>
//using namespace std;
//
//CRITICAL_SECTION g_cs;
//
//static char buf[1024] = { 0 };
//class str_print_t :public xthread {
//public:
//    char c;
//
//    void usr_define_task() {
//        cout << "mythread..." << endl;
//        while (TRUE) {
//        int sz = sizeof(buf);
//        ::EnterCriticalSection(&g_cs);
//        for (int i = 0; i < sz; ++i) {
//            buf[i] = c;
//            Sleep(1);
//        }
//        buf[sz - 1] = '\0';
//        cout << "[" << buf << "]" << endl;
//        ::LeaveCriticalSection(&g_cs);
//        Sleep(1);
//        }
//    }
//
//};
//
//int main2() {
//    //long long start = GetTickCount();
//    ::InitializeCriticalSection(&g_cs);
//    str_print_t t1;
//    str_print_t t2;
//    t1.c = 'A';
//    t2.c = 'B';
//    t1.start();
//    t2.start();
// 
//    ::DeleteCriticalSection(&g_cs);
//    //Sleep(3000);
//    //t2.suspend();
//    //Sleep(2000);
//    //t2.resume();
//
//
//
//    //long long end = GetTickCount();
//
//    //cout << "Multi thread use " << end - start << " ms" << endl;
//
//    system("pause");
//    return 0;
//}