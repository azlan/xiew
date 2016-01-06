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
    connect(mMyDump, SIGNAL(currentOffsetSignal(int)), this, SLOT(updateOffsetSlot(int)));
    connect(mMyDump, SIGNAL(currentTableOffsetSignal(int)), this, SLOT(updateTableOffsetSlot(int)));

    mDisplayToggle = true;

    mFileInstance.push_back(new XFile ("C:\\test\\a.bin"));
    mFileInstance.push_back(new XFile ("C:\\test\\test.exe"));
    mFileInstance.push_back(new XFile ("C:\\test\\test.dll"));
//    mFileInstance.push_back(new XFile ("C:\\test\\test2.exe"));

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
    auto offset = file->mCurrentOffset;
    auto tableOffset = file->mCurrentTableOffset;

    if(mDisplayToggle)
    {
        mMyDump->mMemPage->setAttributes((duint)base, size);
        mMyDump->printDumpAt((duint)base);
        mMyDump->setSingleSelection(offset);
        mMyDump->setTableOffset(tableOffset);
        updateOffsetSlot(offset);
    }
    else
    {
        mMyDisassembly->mMemPage->setAttributes((duint)base, size);
        mMyDisassembly->setRowCount(size);
        mMyDisassembly->reloadData();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    keyPressSlot(key);
}

void MainWindow::updateOffsetSlot(int offset)
{
    this->setWindowTitle(QString("xiew - %1").arg(offset, 8, 16, QChar('0')));
    mFileInstance[mCurrentFile]->mCurrentOffset = offset;
}

void MainWindow::updateTableOffsetSlot(int offset)
{
    mFileInstance[mCurrentFile]->mCurrentTableOffset = offset;
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

        mDisplayToggle = !mDisplayToggle;
        renderView();
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
