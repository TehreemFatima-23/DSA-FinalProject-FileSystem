/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QTabWidget *tabWidget;
    QWidget *tab;
    QTreeView *treeView;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *pushButton_6;
    QLabel *label;
    QWidget *tab_2;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName("tabWidget");
        tabWidget->setGeometry(QRect(10, 20, 701, 351));
        tab = new QWidget();
        tab->setObjectName("tab");
        treeView = new QTreeView(tab);
        treeView->setObjectName("treeView");
        treeView->setGeometry(QRect(0, 0, 141, 321));
        pushButton = new QPushButton(tab);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(10, 40, 75, 24));
        pushButton_2 = new QPushButton(tab);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(10, 70, 91, 24));
        pushButton_3 = new QPushButton(tab);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setGeometry(QRect(10, 100, 75, 24));
        pushButton_4 = new QPushButton(tab);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(10, 130, 75, 24));
        pushButton_5 = new QPushButton(tab);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(10, 160, 75, 24));
        pushButton_6 = new QPushButton(tab);
        pushButton_6->setObjectName("pushButton_6");
        pushButton_6->setGeometry(QRect(10, 190, 91, 24));
        label = new QLabel(tab);
        label->setObjectName("label");
        label->setGeometry(QRect(10, 10, 121, 16));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName("tab_2");
        tabWidget->addTab(tab_2, QString());
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "\342\236\225 Add File", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "\360\237\223\201 Add Folder", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "\342\235\214 Delete", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "\360\237\224\215 Search", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "\360\237\224\201 Move", nullptr));
        pushButton_6->setText(QCoreApplication::translate("MainWindow", "\360\237\227\221 Recycle Bin", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "File System Simulator", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QCoreApplication::translate("MainWindow", "Tab 1", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QCoreApplication::translate("MainWindow", "Tab 2", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
