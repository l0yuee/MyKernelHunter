#include "msgboxunit.h"

MsgBoxUnit::MsgBoxUnit()
{

}

void MsgBoxUnit::msgbox_information(const QString &title, const QString &text)
{
    QMessageBox msg(QMessageBox::Information, title, text, QMessageBox::Ok);
    msg.exec();
}

void MsgBoxUnit::msgbox_warning(const QString &title, const QString &text)
{
    QMessageBox msg(QMessageBox::Warning, title, text, QMessageBox::Ok);
    msg.exec();
}

void MsgBoxUnit::msgbox_critical(const QString &title, const QString &text)
{
    QMessageBox msg(QMessageBox::Critical, title, text, QMessageBox::Ok);
    msg.exec();
}
