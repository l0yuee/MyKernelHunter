#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <process.h>
#include <QMenu>
#include <QCursor>
#include "msgboxunit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    myhunter_ = new MyHunter(this);
    pop_menu_process_ = new QMenu(this);

    // 设置信号槽
    connect(myhunter_, &MyHunter::child_wnd_set_modle, this, [=]() {
        if (child_dialog_ == nullptr)
            return;
        child_dialog_->set_module(myhunter_->get_child_tableView_model());
        });
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::initialization()
{
    if(!myhunter_->initialization()) {
        return false;
    }

    // 初始化界面
    ui->tabWidgetMain->setCurrentIndex(0);

    // 切换到选项卡第一子选项卡
    emit ui->tabWidgetMain->currentChanged(0);


    // 初始化进程列表右键菜单
    pop_menu_process_->addAction(new QAction(tr("进程模块列表"), this));
    pop_menu_process_->addAction(new QAction(tr("进程线程列表"), this));
    pop_menu_process_->addAction(new QAction(tr("进程句柄列表"), this));
    connect(pop_menu_process_, static_cast<void (QMenu::*)(QAction *)>(&QMenu::triggered),
        this, static_cast<void (MainWindow::*)(QAction *)>(&MainWindow::triger_popmenu_process));

    return true;
}

void MainWindow::uninitialization()
{
    myhunter_->uninitialization();
}

void MainWindow::popmenu_process()
{
    // 判断是否选择了列表项
    auto index = ui->tableViewProcess->selectionModel()->currentIndex();
    if (!index.isValid())
        return;
    // 弹出右键菜单
    pop_menu_process_->exec(QCursor::pos());
}


void MainWindow::triger_popmenu_process(QAction *action)
{
    QStringList labels;
    // 获取当前pid
    QString pid_text = myhunter_->get_module_index_text(myhunter_->get_tableView_model(), ui->tableViewProcess->selectionModel()->currentIndex(), M_PROCESS_PID);

    // 静态函数成员的指针，线程回调
    typedef unsigned (__stdcall *THREAD_CALL)(void *arg);
    THREAD_CALL thread_call = nullptr;

    if(action->text() == tr("进程模块列表")) {
        // 设置子窗口表头
        labels << "Module Path" << "Base" << "Size";
        myhunter_->set_child_tableView_header(labels);

        // 获取进程线程列表
        thread_call = &MyHunter::get_process_module_list;
    } else if(action->text() == tr("进程线程列表")) {
        // 设置子窗口表头
        labels << "Thread ID" << "ETHREAD" << "teb"/* << "Priority" << "Module" << "Switch Count" << "Status"*/;
        myhunter_->set_child_tableView_header(labels);

        // 获取进程线程列表
        thread_call = &MyHunter::get_process_thread_list;
    } else if(action->text() == tr("进程句柄列表")) {
        // 设置子窗口表头
        labels << "Handle Type" << "Handle Name" << "Handle" << "Handle Object" << "Type Code" << "Reference Count";
        myhunter_->set_child_tableView_header(labels);

        // 获取进程句柄列表
        thread_call = &MyHunter::get_process_handle_list;
    }

    if (thread_call == nullptr) {
        // 清空子窗口的模型数据
        myhunter_->tableView_child_model_clearall();
        return;
    }

    // 设置相关参数
    myhunter_->param_pid = pid_text.toUInt();

    // 创建子窗口
    if(child_dialog_ == nullptr) {
        child_dialog_ = new TableViewDialog(this);
        child_dialog_->setAttribute(Qt::WA_DeleteOnClose);

        // 设置窗口标题
        child_dialog_->setWindowTitle(action->text() + " [pid: " + pid_text + "]");

        // 设置模型
        //child_dialog_->set_module(myhunter_->get_child_tableView_model());

        // 开启线程，执行功能
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, *thread_call, myhunter_, 0, NULL));

        // 显示窗口
        child_dialog_->exec();
        child_dialog_ = nullptr;
    }

    // 清空子窗口的模型数据
    myhunter_->tableView_child_model_clearall();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    uninitialization();

    event->accept();
}

void MainWindow::on_tabWidgetMain_currentChanged(int index)
{
    // 获取选项卡标题
    QString tab_title = ui->tabWidgetMain->tabText(index);

    // 判断是哪个选项卡
    if(tab_title == tr("进程")) {
        // 清空模型数据
        myhunter_->tableView_model_clearall();
        // 设置表头
        QStringList labels;
        labels << "Process Name" << "PID" << "Parent PID" << "Image Path" << "EPROCESS" << "Privilege";
        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewProcess->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewGDT->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取进程列表
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_process_list, myhunter_, 0, NULL));
    } else if(tab_title == tr("内核")) {
        emit ui->tabWidgetKernel->currentChanged(0);
    } /*else if() {
        
    }*/
}

void MainWindow::on_tabWidgetKernel_currentChanged(int index)
{
    // 清空模型数据
    myhunter_->tableView_model_clearall();
    // 获取选项卡标题
    QString tab_title = ui->tabWidgetKernel->tabText(index);

    if (tab_title == tr("GDT")) {
        // 设置表头
        QStringList labels;
        labels << "CPU Number" << "Seg Selector" << "Base" << "Limit" << "Seg Granularity" << "Seg Privilege" << "Type";
        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewGDT->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewGDT->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取GDT数据
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_gdt, myhunter_, 0, NULL));
    } else if(tab_title == tr("IDT")) {
        // 设置表头
        QStringList labels;
        labels << "CPU Number" << "Gate Number" << "Seg Selector" << "Func Address";
        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewIDT->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewIDT->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取GDT数据
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_idt, myhunter_, 0, NULL));
    } else if(tab_title == tr("SSDT")) {
        // 设置表头
        QStringList labels;
        labels << "Number" << "Function Address";

        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewSSDT->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewSSDT->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取SSDT数据
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_ssdt, myhunter_, 0, NULL));
    } else if (tab_title == tr("ShadowSSDT")) {
        // 设置表头
        QStringList labels;
        labels << "Number" << "Function Address";

        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewShadowSSDT->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewShadowSSDT->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取SSDT数据
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_shadow_ssdt, myhunter_, 0, NULL));
    } else if (tab_title == tr("驱动模块")) {
        // 设置表头
        QStringList labels;
        labels << "Driver Name" << "Base" << "Size" << "Path";

        myhunter_->set_tableView_header(labels);
        // 设置模型
        ui->tableViewDirverModule->setModel(myhunter_->get_tableView_model());
        // 表头均匀填充
        ui->tableViewDirverModule->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        // 开线程工作，获取SSDT数据
        ::CloseHandle((HANDLE)_beginthreadex(NULL, 0, MyHunter::get_driver_module, myhunter_, 0, NULL));
    }
}


void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    // 获取主选项卡标题
    QString tab_title = ui->tabWidgetMain->tabText(ui->tabWidgetMain->currentIndex());
    if(tab_title == tr("进程")) {
        // 弹出进程列表右键菜单
        popmenu_process();
    } else if(tab_title == tr("内核")) {
        // 获取内核子选项卡标题
        tab_title = ui->tabWidgetKernel->tabText(ui->tabWidgetKernel->currentIndex());
        if(tab_title == tr("GDT")) {
            // 弹出GDT表右键菜单
        } else if(tab_title == tr("IDT")) {
            // 弹出IDT表右键菜单
        }
    }
}