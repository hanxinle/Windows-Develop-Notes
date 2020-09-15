#pragma once

#include <QtWidgets/QWidget>
#include "ui_qt_image_multi_thread.h"

class qt_image_multi_thread : public QWidget
{
    Q_OBJECT

public:
    qt_image_multi_thread(QWidget *parent = Q_NULLPTR);
    ~qt_image_multi_thread();
    void ViewImage(QImage *img);

public slots:
    void Open();
    void Deal();

private:
    Ui::qt_image_multi_threadClass ui;
    QImage *img = NULL;
    QImage ImgDeal;
};

