#include "qt_image_multi_thread.h"
#include "ImageThread.h"
#include <QFileDialog>
#include <Qtime>

qt_image_multi_thread::qt_image_multi_thread(QWidget *parent)
    : QWidget(parent) {
    ui.setupUi(this);
}

void qt_image_multi_thread::ViewImage(QImage *img) {
    QImage simg;
    if (img->width() > img->height()) {
        simg = img->scaledToWidth(ui.view->width());
    } else {
        simg = img->scaledToHeight(ui.view->height());
    }
    ui.view->setPixmap(QPixmap::fromImage(simg));
}
void qt_image_multi_thread::Open() {
    QString filename = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图片文件"));

    if (filename.isEmpty()) return;

    if (!img) {
        delete img;
        img = NULL;
    }

    img = new QImage(filename);
    if (!img)   return;

    ViewImage(img);

}

void qt_image_multi_thread::Deal() {
    float b = (float)ui.bright->value() / 100;
    if (!img)   return;
    ImgDeal = img->copy();

    unsigned char * data = ImgDeal.bits();
    int pix_cnt = img->width() * img->height();

    int thread_count = ui.thread_cnt->value();
    QTime t;
    t.start();
    ImageThread::DealImage(data, pix_cnt, b, thread_count);
    QThread::msleep(1000);

    for (;;) {
        int percent = ImageThread::DealPercent();
        ui.progress->setValue(percent);

        if (percent >= 100) 
            break;
    }

    ImageThread::Wait();
    int ms = t.elapsed();
    char buf[128] = { 0 };
    sprintf(buf, "%d ms", ms);
    ui.dealtime->setText(QString::fromLocal8Bit(buf));
    //for (int i = 0; i < pix_cnt; ++i) {
    //    int B = data[i * 4] * b;
    //    int G = data[i * 4 + 1] * b;
    //    int R = data[i * 4 + 2] * b;

    //    if (B >= 255) B = 255;
    //    if (G >= 255) B = 255;
    //    if (R >= 255) B = 255;

    //    data[i * 4] = B;
    //    data[i * 4 + 1] = G;
    //    data[i * 4 + 2] = R;
    //}
    ViewImage(&ImgDeal);
}



qt_image_multi_thread::~qt_image_multi_thread() {

}
