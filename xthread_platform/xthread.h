#pragma once
#ifdef XTHREADPLATFORM_EXPORTS
#define XTHREADPLATFORM_API __declspec(dllexport)
#else
#define XTHREADPLATFORM_API __declspec(dllimport)
#endif

class XTHREADPLATFORM_API xthread {
public:
    bool start();
    void wait();
    void suspend();
    void resume();
    virtual void usr_define_task() = 0; //继承类要实现
    xthread();
    virtual ~xthread();

private:
    unsigned int r_thread_;
};

