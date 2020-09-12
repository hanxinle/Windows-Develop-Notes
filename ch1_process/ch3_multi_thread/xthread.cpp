#include "xthread.h"
#include <windows.h>
#include <iostream>
#include <process.h>



static void Main(void * para) {
    xthread * obj = (xthread *)para;
    obj->usr_define_task();
    _endthread();
}


bool xthread::start() {
    r_thread_ =  _beginthread(Main, 0, this);
    if (r_thread_ <= 0) return false;

    return true;
}

void xthread::wait() {
    WaitForSingleObject((HANDLE)r_thread_, INFINITE);
}

//void xthread::usr_define_task() {
//
//}

xthread::xthread() {

}

xthread::~xthread() {

}