#ifndef MSGBOXUNIT_H
#define MSGBOXUNIT_H

#include <QMessageBox>


class MsgBoxUnit
{
public:
    MsgBoxUnit();

    // 一般信息
    static void msgbox_information(const QString &title, const QString &text);

    // 警告信息
    static void msgbox_warning(const QString &title, const QString &text);

    // 错误信息
    static void msgbox_critical(const QString &title, const QString &text);
};

#endif // MSGBOXUNIT_H
