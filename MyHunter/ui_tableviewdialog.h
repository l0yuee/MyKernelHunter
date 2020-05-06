/********************************************************************************
** Form generated from reading UI file 'tableviewdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.13.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TABLEVIEWDIALOG_H
#define UI_TABLEVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_TableViewDialog
{
public:
    QGridLayout *gridLayout;
    QTableView *tableView;

    void setupUi(QDialog *TableViewDialog)
    {
        if (TableViewDialog->objectName().isEmpty())
            TableViewDialog->setObjectName(QString::fromUtf8("TableViewDialog"));
        TableViewDialog->resize(662, 324);
        gridLayout = new QGridLayout(TableViewDialog);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableView = new QTableView(TableViewDialog);
        tableView->setObjectName(QString::fromUtf8("tableView"));
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        gridLayout->addWidget(tableView, 0, 0, 1, 1);


        retranslateUi(TableViewDialog);

        QMetaObject::connectSlotsByName(TableViewDialog);
    } // setupUi

    void retranslateUi(QDialog *TableViewDialog)
    {
        TableViewDialog->setWindowTitle(QCoreApplication::translate("TableViewDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TableViewDialog: public Ui_TableViewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TABLEVIEWDIALOG_H
