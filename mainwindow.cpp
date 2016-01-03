#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydump.h"
#include "mydisassembly.h"
#include "xfile.h"
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mMyDump = new MyDump();
    ui->layoutMain->addWidget(mMyDump);
    mMyDump->show();

    mMyDisassembly = new MyDisassembly();
    ui->layoutMain->addWidget(mMyDisassembly);
    mMyDisassembly->hide();

   mMyDump->setFocus();

   this->resize(mMyDump->width(),600);
   connect(mMyDump, SIGNAL(keyPressSignal(int)), this, SLOT(keyPressSlot(int)));
   connect(mMyDisassembly, SIGNAL(keyPressSignal(int)), this, SLOT(keyPressSlot(int)));

    XFile *file1 = new XFile("C:\\test\\test.exe");
    XFile *file2 = new XFile("C:\\test\\test.dll");

    mMyDump->mMemPage->setAttributes((duint)file1->getBase(),file1->getSize());
    mMyDump->printDumpAt((dsint)file1->getBase());

    mMyDisassembly->mMemPage->setAttributes((duint)file1->getBase(),file1->getSize());
    mMyDisassembly->setRowCount(file1->getSize());

    files.append(file1);
    files.append(file2);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    keyPressSlot(key);
}

void MainWindow::keyPressSlot(int key)
{
    if (key ==  Qt::Key_Return || key ==  Qt::Key_Enter  )
    {
        if (mMyDump->isVisible())
        {
            mMyDump->hide();
            mMyDisassembly->setFocus();
            mMyDisassembly->show();
        }
        else if (mMyDisassembly->isVisible())
        {
            mMyDump->show();
            mMyDump->setFocus();
            mMyDisassembly->hide();
        }
        return;
    }

    if (key ==  Qt::Key_Tab  )
    {
        mMyDump->mMemPage->setAttributes((duint)files[1]->getBase(),files[1]->getSize());
        mMyDump->printDumpAt((dsint)files[1]->getBase());
    }
}
