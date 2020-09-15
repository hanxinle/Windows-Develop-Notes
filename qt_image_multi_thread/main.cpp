#include "qt_image_multi_thread.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qt_image_multi_thread w;
    w.show();
    return a.exec();
}
