/********************************************************************************
** Form generated from reading UI file 'qt_image_single_thread.ui'
**
** Created by: Qt User Interface Compiler version 5.12.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_IMAGE_SINGLE_THREAD_H
#define UI_QT_IMAGE_SINGLE_THREAD_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_image_single_threadClass
{
public:
    QLabel *view;
    QPushButton *open;
    QPushButton *deal;
    QLabel *label;
    QSpinBox *bright;
    QLabel *label_2;
    QLabel *dealtime;

    void setupUi(QWidget *qt_image_single_threadClass)
    {
        if (qt_image_single_threadClass->objectName().isEmpty())
            qt_image_single_threadClass->setObjectName(QString::fromUtf8("qt_image_single_threadClass"));
        qt_image_single_threadClass->resize(600, 400);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/qt_image_single_thread/Resources/logo.png"), QSize(), QIcon::Normal, QIcon::Off);
        qt_image_single_threadClass->setWindowIcon(icon);
        view = new QLabel(qt_image_single_threadClass);
        view->setObjectName(QString::fromUtf8("view"));
        view->setGeometry(QRect(40, 20, 261, 351));
        view->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 221, 181);"));
        open = new QPushButton(qt_image_single_threadClass);
        open->setObjectName(QString::fromUtf8("open"));
        open->setGeometry(QRect(340, 50, 81, 31));
        open->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        deal = new QPushButton(qt_image_single_threadClass);
        deal->setObjectName(QString::fromUtf8("deal"));
        deal->setGeometry(QRect(460, 50, 81, 31));
        deal->setStyleSheet(QString::fromUtf8("font: 14pt \"ADMUI3Lg\";"));
        label = new QLabel(qt_image_single_threadClass);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(340, 140, 31, 21));
        label->setStyleSheet(QString::fromUtf8("font: 15pt \"ADMUI3Lg\";"));
        bright = new QSpinBox(qt_image_single_threadClass);
        bright->setObjectName(QString::fromUtf8("bright"));
        bright->setGeometry(QRect(340, 170, 81, 31));
        bright->setMaximum(200);
        bright->setValue(100);
        label_2 = new QLabel(qt_image_single_threadClass);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(340, 220, 71, 21));
        label_2->setStyleSheet(QString::fromUtf8("font: 15pt \"ADMUI3Lg\";"));
        dealtime = new QLabel(qt_image_single_threadClass);
        dealtime->setObjectName(QString::fromUtf8("dealtime"));
        dealtime->setGeometry(QRect(340, 240, 91, 21));
        dealtime->setStyleSheet(QString::fromUtf8("font: 15pt \"ADMUI3Lg\";"));

        retranslateUi(qt_image_single_threadClass);
        QObject::connect(open, SIGNAL(clicked()), qt_image_single_threadClass, SLOT(Open()));
        QObject::connect(deal, SIGNAL(clicked()), qt_image_single_threadClass, SLOT(Deal()));

        QMetaObject::connectSlotsByName(qt_image_single_threadClass);
    } // setupUi

    void retranslateUi(QWidget *qt_image_single_threadClass)
    {
        qt_image_single_threadClass->setWindowTitle(QApplication::translate("qt_image_single_threadClass", "qt_image_single_thread", nullptr));
        view->setText(QApplication::translate("qt_image_single_threadClass", "TextLabel", nullptr));
        open->setText(QApplication::translate("qt_image_single_threadClass", "\346\211\223\345\274\200", nullptr));
        deal->setText(QApplication::translate("qt_image_single_threadClass", "\345\244\204\347\220\206", nullptr));
        label->setText(QApplication::translate("qt_image_single_threadClass", "\344\272\256\345\272\246", nullptr));
        label_2->setText(QApplication::translate("qt_image_single_threadClass", "\346\227\266\351\227\264\357\274\210ms\357\274\211", nullptr));
        dealtime->setText(QApplication::translate("qt_image_single_threadClass", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class qt_image_single_threadClass: public Ui_qt_image_single_threadClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_IMAGE_SINGLE_THREAD_H
