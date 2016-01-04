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

    mFileInstance.push_back(new XFile ("C:\\test\\test.exe"));
    mFileInstance.push_back(new XFile ("C:\\test\\test.dll"));
    mFileInstance.push_back(new XFile ("C:\\test\\test2.exe"));

    mCurrentFile = 0;
    renderView();
}

MainWindow::~MainWindow()
{
    for(auto file : mFileInstance)
    {
        delete file;
    }
    delete ui;
}

void MainWindow::renderView()
{

    auto file = mFileInstance[mCurrentFile];
    auto base = file->getBase();
    auto size = file->getSize();

    mMyDump->mMemPage->setAttributes((duint)base, size);
    mMyDump->printDumpAt((duint)base);

    mMyDisassembly->mMemPage->setAttributes((duint)base, size);
    mMyDisassembly->setRowCount(size);
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

    // Switching between loaded files
    if (key ==  Qt::Key_Tab  )
    {
        mCurrentFile;

        if (++mCurrentFile == mFileInstance.size())
        {
            // reset the cycle
            mCurrentFile = 0;
        }

        renderView();
        return;
    }

    // Escape to quit
    if (key ==  Qt::Key_Escape  )
    {
        this->close();
        return;
    }
}
