/********************************************************************************
** Form generated from reading UI file 'QtWidgetsApp.ui'
**
** Created by: Qt User Interface Compiler version 6.5.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTWIDGETSAPP_H
#define UI_QTWIDGETSAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtWidgetsAppClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtWidgetsAppClass)
    {
        if (QtWidgetsAppClass->objectName().isEmpty())
            QtWidgetsAppClass->setObjectName("QtWidgetsAppClass");
        QtWidgetsAppClass->resize(600, 400);
        menuBar = new QMenuBar(QtWidgetsAppClass);
        menuBar->setObjectName("menuBar");
        QtWidgetsAppClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtWidgetsAppClass);
        mainToolBar->setObjectName("mainToolBar");
        QtWidgetsAppClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtWidgetsAppClass);
        centralWidget->setObjectName("centralWidget");
        QtWidgetsAppClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtWidgetsAppClass);
        statusBar->setObjectName("statusBar");
        QtWidgetsAppClass->setStatusBar(statusBar);

        retranslateUi(QtWidgetsAppClass);

        QMetaObject::connectSlotsByName(QtWidgetsAppClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtWidgetsAppClass)
    {
        QtWidgetsAppClass->setWindowTitle(QCoreApplication::translate("QtWidgetsAppClass", "QtWidgetsApp", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtWidgetsAppClass: public Ui_QtWidgetsAppClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTWIDGETSAPP_H
