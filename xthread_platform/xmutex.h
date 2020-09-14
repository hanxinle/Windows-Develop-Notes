#pragma once
#ifdef XTHREADPLATFORM_EXPORTS
#define XTHREADPLATFORM_API __declspec(dllexport)
#else
#define XTHREADPLATFORM_API __declspec(dllimport)
#endif


class XTHREADPLATFORM_API xmutex {
public:
    xmutex();
    ~xmutex();
    void lock();
    void unlock();

    void *section;
};

