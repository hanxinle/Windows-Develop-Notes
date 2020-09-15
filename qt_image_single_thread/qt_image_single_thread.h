#pragma once

#include <QtWidgets/QWidget>
#include "ui_qt_image_single_thread.h"

class qt_image_single_thread : public QWidget
{
    Q_OBJECT

public:
    qt_image_single_thread(QWidget *parent = Q_NULLPTR);
    void ViewImage(QImage *img);
public slots:
    void Open();
    void Deal();

private:
    Ui::qt_image_single_threadClass ui;

    QImage *img = NULL;
    QImage imgDeal;
};
