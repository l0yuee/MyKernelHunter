#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myhunter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool initialization();      // 初始化
    void uninitialization();    // 反初始化

    void popmenu_process();     // 进程列表右键菜单

private:
    Ui::MainWindow *ui;
    QMenu *pop_menu_process_ = nullptr;     // 进程列表右键菜单
    MyHunter *myhunter_ = nullptr;

    TableViewDialog *child_dialog_ = nullptr;    // 子窗口

protected slots:
    void triger_popmenu_process(QAction *action);       // 进程列表右键菜单响应信号槽


    // QWidget interface
protected:
    void closeEvent(QCloseEvent *event);
private slots:
    void on_tabWidgetMain_currentChanged(int index);    // 主选项卡改变
    void on_tabWidgetKernel_currentChanged(int index);  // 内部选项卡改变

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event);    // 根据不同的窗口弹出不同的菜单
};
#endif // MAINWINDOW_H
