//对项目 ch3_multi_thread 中类 xthread 进行封装
#include "xthread.h"
#include "xmutex.h"
#include <process.h>
#include <windows.h>
#include <iostream>
using namespace std;

static xmutex mutex;


static char buf[1024] = { 0 };

class str_print_t :public xthread {
public:
    char c;

    void usr_define_task() {
        cout << "mythread..." << endl;
        for (;;) {
            int sz = sizeof(buf);
            /*::EnterCriticalSection(&g_cs);*/
            mutex.lock();
            for (int i = 0; i < sz; i++) {
                buf[i] = c;
                Sleep(1);
            }
            buf[sz - 1] = '\0';
            cout << "[" << buf << "]" << endl;
           /* ::LeaveCriticalSection(&g_cs);*/
            mutex.unlock();
            Sleep(1);
        }
    }

};

int main() {
 /*   long long start = GetTickCount();*/
    //::InitializeCriticalSection(&g_cs);
    str_print_t t1;
    str_print_t t2;
    t1.c = 'A';
    t2.c = 'B';

    t1.start();
 
 
    t2.start();
    //Sleep(3000);
    //t2.suspend();
    //Sleep(2000);
    //t2.resume();



    //long long end = GetTickCount();

    //cout << "Multi thread % mutex use " << end - start << " ms" << endl;

    system("pause");
    return 0;
}