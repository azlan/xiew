#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydump.h"
#include "mydisassembly.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mMyDump = new MyDump();
    ui->verticalLayout->addWidget(mMyDump);
    mMyDump->hide();

    mMyDisassembly = new MyDisassembly();
    ui->verticalLayout->addWidget(mMyDisassembly);
    mMyDisassembly->show();




}

MainWindow::~MainWindow()
{
    delete ui;
}
