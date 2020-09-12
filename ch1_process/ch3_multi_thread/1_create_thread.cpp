#include <process.h>
#include <iostream>

using namespace std;

void thread_main(void * para) {
    int *p = (int *)para;
    cout << dec << *p << endl;;
    cout << hex <<"0x"<< *p << endl;

    _endthread();
}
int main1() {
    int i = 1024;
    //最后一个参数是线程要处理的对象，不然创建子线程做什么
    //要保证该对象生命周期
    _beginthread(thread_main, 0, &i);  

    system("pause");
    return 0;
}

