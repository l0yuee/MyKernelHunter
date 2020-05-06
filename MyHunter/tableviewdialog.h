#ifndef TABLEVIEWDIALOG_H
#define TABLEVIEWDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class TableViewDialog;
}

class TableViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableViewDialog(QWidget *parent = nullptr);
    ~TableViewDialog();

    void set_module(QStandardItemModel *model);     // …Ë÷√ƒ£–Õ

private:
    Ui::TableViewDialog *ui;
};

#endif // TABLEVIEWDIALOG_H
