/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidgetMain;
    QWidget *tab_2;
    QGridLayout *gridLayout_4;
    QTableView *tableViewProcess;
    QWidget *tab_1;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidgetKernel;
    QWidget *tab_3;
    QGridLayout *gridLayout_3;
    QTableView *tableViewGDT;
    QWidget *tab_4;
    QGridLayout *gridLayout_5;
    QTableView *tableViewIDT;
    QWidget *tab_5;
    QGridLayout *gridLayout_6;
    QTableView *tableViewSSDT;
    QWidget *tab_6;
    QGridLayout *gridLayout_7;
    QTableView *tableViewShadowSSDT;
    QWidget *tab_7;
    QGridLayout *gridLayout_8;
    QTableView *tableViewDirverModule;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(936, 386);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tabWidgetMain = new QTabWidget(centralwidget);
        tabWidgetMain->setObjectName(QString::fromUtf8("tabWidgetMain"));
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        gridLayout_4 = new QGridLayout(tab_2);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        tableViewProcess = new QTableView(tab_2);
        tableViewProcess->setObjectName(QString::fromUtf8("tableViewProcess"));
        tableViewProcess->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewProcess->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewProcess->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_4->addWidget(tableViewProcess, 0, 0, 1, 1);

        tabWidgetMain->addTab(tab_2, QString());
        tab_1 = new QWidget();
        tab_1->setObjectName(QString::fromUtf8("tab_1"));
        gridLayout_2 = new QGridLayout(tab_1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        tabWidgetKernel = new QTabWidget(tab_1);
        tabWidgetKernel->setObjectName(QString::fromUtf8("tabWidgetKernel"));
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        gridLayout_3 = new QGridLayout(tab_3);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        tableViewGDT = new QTableView(tab_3);
        tableViewGDT->setObjectName(QString::fromUtf8("tableViewGDT"));
        tableViewGDT->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewGDT->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewGDT->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableViewGDT->verticalHeader()->setVisible(false);

        gridLayout_3->addWidget(tableViewGDT, 0, 0, 1, 1);

        tabWidgetKernel->addTab(tab_3, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        gridLayout_5 = new QGridLayout(tab_4);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        tableViewIDT = new QTableView(tab_4);
        tableViewIDT->setObjectName(QString::fromUtf8("tableViewIDT"));
        tableViewIDT->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewIDT->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewIDT->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_5->addWidget(tableViewIDT, 0, 0, 1, 1);

        tabWidgetKernel->addTab(tab_4, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        gridLayout_6 = new QGridLayout(tab_5);
        gridLayout_6->setObjectName(QString::fromUtf8("gridLayout_6"));
        tableViewSSDT = new QTableView(tab_5);
        tableViewSSDT->setObjectName(QString::fromUtf8("tableViewSSDT"));
        tableViewSSDT->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewSSDT->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewSSDT->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_6->addWidget(tableViewSSDT, 0, 0, 1, 1);

        tabWidgetKernel->addTab(tab_5, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        gridLayout_7 = new QGridLayout(tab_6);
        gridLayout_7->setObjectName(QString::fromUtf8("gridLayout_7"));
        tableViewShadowSSDT = new QTableView(tab_6);
        tableViewShadowSSDT->setObjectName(QString::fromUtf8("tableViewShadowSSDT"));
        tableViewShadowSSDT->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewShadowSSDT->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewShadowSSDT->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_7->addWidget(tableViewShadowSSDT, 0, 0, 1, 1);

        tabWidgetKernel->addTab(tab_6, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        gridLayout_8 = new QGridLayout(tab_7);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        tableViewDirverModule = new QTableView(tab_7);
        tableViewDirverModule->setObjectName(QString::fromUtf8("tableViewDirverModule"));
        tableViewDirverModule->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableViewDirverModule->setSelectionMode(QAbstractItemView::SingleSelection);
        tableViewDirverModule->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout_8->addWidget(tableViewDirverModule, 0, 0, 1, 1);

        tabWidgetKernel->addTab(tab_7, QString());

        gridLayout_2->addWidget(tabWidgetKernel, 0, 0, 1, 1);

        tabWidgetMain->addTab(tab_1, QString());

        gridLayout->addWidget(tabWidgetMain, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        tabWidgetMain->setCurrentIndex(0);
        tabWidgetKernel->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MyHunter", nullptr));
        tabWidgetMain->setTabText(tabWidgetMain->indexOf(tab_2), QCoreApplication::translate("MainWindow", "\350\277\233\347\250\213", nullptr));
        tabWidgetKernel->setTabText(tabWidgetKernel->indexOf(tab_3), QCoreApplication::translate("MainWindow", "GDT", nullptr));
        tabWidgetKernel->setTabText(tabWidgetKernel->indexOf(tab_4), QCoreApplication::translate("MainWindow", "IDT", nullptr));
        tabWidgetKernel->setTabText(tabWidgetKernel->indexOf(tab_5), QCoreApplication::translate("MainWindow", "SSDT", nullptr));
        tabWidgetKernel->setTabText(tabWidgetKernel->indexOf(tab_6), QCoreApplication::translate("MainWindow", "ShadowSSDT", nullptr));
        tabWidgetKernel->setTabText(tabWidgetKernel->indexOf(tab_7), QCoreApplication::translate("MainWindow", "\351\251\261\345\212\250\346\250\241\345\235\227", nullptr));
        tabWidgetMain->setTabText(tabWidgetMain->indexOf(tab_1), QCoreApplication::translate("MainWindow", "\345\206\205\346\240\270", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
