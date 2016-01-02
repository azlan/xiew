#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydump.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mMyDump = new MyDump();
    ui->verticalLayout->addWidget(mMyDump);
    mMyDump->hexAsciiSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}
