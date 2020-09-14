//对项目 ch3_multi_thread 中类 xthread 进行封装
#include "xthread.h"
#include <process.h>
#include <windows.h>
#include <iostream>
using namespace std;

static char *arr = NULL;

class mythread :public xthread {
public:
    int b_ = 0;
    int e_ = 0;

    void usr_define_task() {
        for (int i = b_; i < e_; ++i) {
            arr[i] = i;
        }
    }

};

int main1() {
    const int length = 1000 * 1000 * 1000;
    arr = new char[length];

    int thread_cnt = 6;
    mythread * p_thread = new mythread[thread_cnt];
    //10+ms ~ 20+ ms 计算一次
    long long start = GetTickCount();

    for (int i = 0; i < thread_cnt; ++i) {
        p_thread[i].b_ = i * (length / thread_cnt);
        p_thread[i].e_ = p_thread[i].b_ + (length / thread_cnt);

        if (p_thread[i].e_ >= length) {
            p_thread[i].e_ = length - 1;
        }

        p_thread[i].start();
    }
    /*
     * 下面 for 语句，参考 ch3_multi_thread 中 2_single_thread 
     * 本示例中将其操作放于 usr_define_task 中，借助笔记记住这种设计
     */

    //for (auto i = 0; i < length; ++i) {
    //    arr[i] = i;
    //}

    for (int i = 0; i < thread_cnt; ++i) {
        p_thread[i].wait();
    }

    long long end = GetTickCount();

    cout << "Multi thread use " << end - start << " ms" << endl;

    system("pause");
    return 0;
}