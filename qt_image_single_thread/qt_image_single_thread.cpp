#include "qt_image_single_thread.h"
#include <qfiledialog.h>
#include <QTime>
qt_image_single_thread::qt_image_single_thread(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
}

void qt_image_single_thread::ViewImage(QImage *img) {
    QImage simg;
    if (img->width() > img->height()) {
        simg = img->scaledToWidth(ui.view->width());
    } else {
        simg = img->scaledToHeight(ui.view->height());
    }
    ui.view->setPixmap(QPixmap::fromImage(simg));
}

void qt_image_single_thread::Open() {
    QString filename = QFileDialog::getOpenFileName(this,QString::fromLocal8Bit("选择图片文件"));
    if (filename.isEmpty())return;
    if (!img) {
        delete img;
        img = NULL;
    }
    img = new QImage(filename);
    if (!img)return;
    ViewImage(img);

}

void qt_image_single_thread::Deal() {
    float b = (float)ui.bright->value() / 100;
    if (!img)return;
    imgDeal = img->copy();
    unsigned char *data = imgDeal.bits();
    int pixCount = img->width() * img->height();
    QTime t;
    t.start();
    for (int i = 0; i < pixCount; i++) {
        int B = data[i * 4] * b;
        int G = data[i * 4 + 1] * b;
        int R = data[i * 4 + 2] * b;
        if (B > 255) B = 255;
        if (G > 255) G = 255;
        if (R > 255) R = 255;
        data[i * 4] = B;
        data[i * 4 + 1] = G;
        data[i * 4 + 2] = R;
    }
    int ms = t.elapsed();
    char buf[127] = { 0 };
    sprintf(buf, "%d", ms);
    ui.dealtime->setText(buf);
    ViewImage(&imgDeal);
}
