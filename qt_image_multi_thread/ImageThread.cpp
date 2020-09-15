#include "ImageThread.h"

#include <vector>
using namespace std;

static vector<ImageThread*> threads;

void ImageThread::run() {
    if (!img || begin < 0 || end <=0 || (end-begin) <= 0)   return;
    for (int i = begin; i <= end; ++i) {
        int B = img[i * 4] * bright;
        int G = img[i * 4 + 1] * bright;
        int R = img[i * 4 + 2] * bright;

        if (B >= 255) B = 255;
        if (G >= 255) G = 255;
        if (R >= 255) R = 255;

        img[i * 4]     = B;
        img[i * 4 + 1] = G;
        img[i * 4 + 2] = R;

        cur = i;
    }
}
//每次调用把上一次线程对象清理
void ImageThread::DealImage(unsigned char * img, unsigned  int size, float bright,int thread_cnt) {
    for (int i = 0; i < threads.size(); i++) {
        threads[i]->wait();
        delete  threads[i];
    }

    threads.clear();

    int tsize = size / thread_cnt;

    for (int i = 0; i < thread_cnt; i++) {
        ImageThread * pThread = new ImageThread();
        pThread->begin = i * tsize;
        pThread->end = pThread->begin + tsize;
        pThread->img = img;
        pThread->bright = bright;

        if (i == thread_cnt - 1) {
            pThread->end = size - 1;
        }

        threads.push_back(pThread);
    }

    for (int i = 0; i < threads.size(); i++) {
        threads[i]->start();
    }
}
int ImageThread::DealPercent() {
    int cur = 0;
    int total = 0;
    for (int i = 0; i < threads.size(); ++i) {
        cur   += (threads[i]->cur - threads[i]->begin);
        total += (threads[i]->end - threads[i]->begin);
    }

    if (total == 0) return 0;
    int per = cur * 100 / total;
    return per;
}

void ImageThread::Wait() {
    for (int i = 0; i < threads.size(); ++i) {
        threads[i]->wait();
    }
}
ImageThread::ImageThread() {}
ImageThread::~ImageThread() {}