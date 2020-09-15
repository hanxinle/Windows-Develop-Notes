/********************************************************************************
** Form generated from reading UI file 'qt_image_multi_thread.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_IMAGE_MULTI_THREAD_H
#define UI_QT_IMAGE_MULTI_THREAD_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_image_multi_threadClass
{
public:
    QLabel *view;
    QPushButton *open;
    QPushButton *deal;
    QSpinBox *bright;
    QLabel *label_2;
    QLabel *label_3;
    QSpinBox *thread_cnt;
    QLabel *label_4;
    QLabel *dealtime;
    QProgressBar *progress;

    void setupUi(QWidget *qt_image_multi_threadClass)
    {
        if (qt_image_multi_threadClass->objectName().isEmpty())
            qt_image_multi_threadClass->setObjectName(QString::fromUtf8("qt_image_multi_threadClass"));
        qt_image_multi_threadClass->resize(600, 400);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/qt_image_multi_thread/Resources/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        qt_image_multi_threadClass->setWindowIcon(icon);
        view = new QLabel(qt_image_multi_threadClass);
        view->setObjectName(QString::fromUtf8("view"));
        view->setGeometry(QRect(20, 30, 261, 331));
        view->setStyleSheet(QString::fromUtf8("\n"
"background-color: rgb(194, 255, 165);"));
        open = new QPushButton(qt_image_multi_threadClass);
        open->setObjectName(QString::fromUtf8("open"));
        open->setGeometry(QRect(300, 40, 81, 41));
        open->setStyleSheet(QString::fromUtf8(""));
        deal = new QPushButton(qt_image_multi_threadClass);
        deal->setObjectName(QString::fromUtf8("deal"));
        deal->setGeometry(QRect(390, 40, 91, 41));
        bright = new QSpinBox(qt_image_multi_threadClass);
        bright->setObjectName(QString::fromUtf8("bright"));
        bright->setGeometry(QRect(350, 110, 131, 31));
        bright->setMinimum(1);
        bright->setMaximum(200);
        bright->setValue(80);
        label_2 = new QLabel(qt_image_multi_threadClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(300, 120, 31, 16));
        label_2->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        label_3 = new QLabel(qt_image_multi_threadClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(300, 180, 61, 16));
        label_3->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        thread_cnt = new QSpinBox(qt_image_multi_threadClass);
        thread_cnt->setObjectName(QString::fromUtf8("thread_cnt"));
        thread_cnt->setGeometry(QRect(390, 170, 91, 31));
        thread_cnt->setValue(2);
        label_4 = new QLabel(qt_image_multi_threadClass);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(300, 235, 121, 21));
        label_4->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        dealtime = new QLabel(qt_image_multi_threadClass);
        dealtime->setObjectName(QString::fromUtf8("dealtime"));
        dealtime->setGeometry(QRect(300, 280, 51, 21));
        dealtime->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        progress = new QProgressBar(qt_image_multi_threadClass);
        progress->setObjectName(QString::fromUtf8("progress"));
        progress->setGeometry(QRect(300, 330, 118, 23));
        progress->setValue(0);

        retranslateUi(qt_image_multi_threadClass);
        QObject::connect(open, SIGNAL(clicked()), qt_image_multi_threadClass, SLOT(Open()));
        QObject::connect(deal, SIGNAL(clicked()), qt_image_multi_threadClass, SLOT(Deal()));

        QMetaObject::connectSlotsByName(qt_image_multi_threadClass);
    } // setupUi

    void retranslateUi(QWidget *qt_image_multi_threadClass)
    {
        qt_image_multi_threadClass->setWindowTitle(QApplication::translate("qt_image_multi_threadClass", "qt_image_multi_thread", nullptr));
        view->setText(QApplication::translate("qt_image_multi_threadClass", "TextLabel", nullptr));
        open->setText(QApplication::translate("qt_image_multi_threadClass", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        deal->setText(QApplication::translate("qt_image_multi_threadClass", "\345\244\204\347\220\206", nullptr));
        label_2->setText(QApplication::translate("qt_image_multi_threadClass", "\344\272\256\345\272\246", nullptr));
        label_3->setText(QApplication::translate("qt_image_multi_threadClass", "\347\272\277\347\250\213\346\225\260\351\207\217", nullptr));
        label_4->setText(QApplication::translate("qt_image_multi_threadClass", "\345\244\204\347\220\206\346\227\266\351\227\264\357\274\210ms\357\274\211", nullptr));
        dealtime->setText(QApplication::translate("qt_image_multi_threadClass", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_image_multi_threadClass: public Ui_qt_image_multi_threadClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_IMAGE_MULTI_THREAD_H
