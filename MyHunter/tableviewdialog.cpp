#include "tableviewdialog.h"
#include "ui_tableviewdialog.h"

TableViewDialog::TableViewDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TableViewDialog)
{
    ui->setupUi(this);
}

TableViewDialog::~TableViewDialog()
{
    delete ui;
}

void TableViewDialog::set_module(QStandardItemModel *model)
{
    ui->tableView->setModel(model);
}
