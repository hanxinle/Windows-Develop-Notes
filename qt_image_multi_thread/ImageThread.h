#pragma once
#include <QThread>

//每次重新开多个线程
class ImageThread:public QThread {
public:
    static void DealImage(unsigned char * img, unsigned  int size, float bright, int thread_cnt);
    static int DealPercent();
    static void Wait();
    void run();

    unsigned int begin = 0;
    unsigned int end = 0;
    unsigned int cur = 0;

    unsigned char * img = NULL;
    float bright = 1;
    ImageThread();
    ~ImageThread();
};

