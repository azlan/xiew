#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydump.h"
#include "mydisassembly.h"
#include "xfile.h"
#include "GotoDialog.h"
#include "OpenFileDialog.h"
#include <QMessageBox>
#include <QVector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mCurrentFile (0)
{
    ui->setupUi(this);

    mGotoDialog = new GotoDialog(this);
    mOpenFileDialog = new OpenFileDialog(this);

    mMyDump = new MyDump();
    ui->layoutMain->addWidget(mMyDump);
    mMyDump->show();

    mMyDisassembly = new MyDisassembly();
    ui->layoutMain->addWidget(mMyDisassembly);
    mMyDisassembly->hide();

    mMyDump->setFocus();
    int totalWidth = 0;
    for(int i = 0; i < mMyDump->getColumnCount(); i++)
        totalWidth += mMyDump->getColumnWidth(i);

    this->resize(totalWidth+19,604);

    connect(mMyDump, SIGNAL(keyPressSignal(int)), this, SLOT(keyPressSlot(int)));
    connect(mMyDisassembly, SIGNAL(keyPressSignal(int)), this, SLOT(keyPressSlot(int)));
    connect(mMyDump, SIGNAL(currentOffsetSignal(int)), this, SLOT(updateOffsetSlot(int)));
    connect(mMyDisassembly, SIGNAL(currentOffsetSignal(int)), this, SLOT(updateOffsetSlot(int)));
    connect(mMyDump, SIGNAL(currentTableOffsetSignal(int)), this, SLOT(updateTableOffsetSlot(int)));
    connect(mGotoDialog, SIGNAL(buttonOk_clicked()),this,SLOT(gotoOffsetSlot()));
    connect(mOpenFileDialog, SIGNAL(accepted()),this,SLOT(openFileSlot()));

    mDisplayToggle = true;

//    mFileInstance.push_back(new XFile ("C:\\test\\a.bin"));
//    mFileInstance.push_back(new XFile ("C:\\test\\test.exe"));
//    mFileInstance.push_back(new XFile ("C:\\test\\test.dll"));
//    mFileInstance.push_back(new XFile ("C:\\test\\test2.exe"));


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
    if (mFileInstance.size() == 0)
        return;

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

        if (mUpdateTablePage)
        {
            // Recalculate table row
            tableOffset = offset / mMyDump->getBytePerRowCount();
            // Reset state
            mUpdateTablePage = false;
        }
        mMyDump->setTableOffset(tableOffset);
        updateOffsetSlot(offset);
    }
    else
    {
        mMyDisassembly->mMemPage->setAttributes((duint)base, size);
        mMyDisassembly->setRowCount(size);
        mMyDisassembly->disassembleAt(offset, offset);

    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    keyPressSlot(key);
}

void MainWindow::updateOffsetSlot(int offset)
{
    if (mFileInstance.size() == 0)
        return;

    QFileInfo fileInfo(mFileInstance[mCurrentFile]->getFilename());
    QString filename(fileInfo.fileName());
    this->setWindowTitle(QString("xiew - %1 - %2").arg(offset, 8, 16, QChar('0')).toUpper().arg(filename));

    mFileInstance[mCurrentFile]->mCurrentOffset = offset;
}

void MainWindow::updateTableOffsetSlot(int offset)
{
    if (mFileInstance.size() == 0)
        return;
    mFileInstance[mCurrentFile]->mCurrentTableOffset = offset;
}

void MainWindow::gotoOffsetSlot()
{
    auto expressionText  = mGotoDialog->expressionText;
    auto offset = expressionText.toInt(0,16);
    mFileInstance[mCurrentFile]->mCurrentOffset = offset;
    mUpdateTablePage = true;
    renderView();
}

void MainWindow::openFileSlot()
{
    // TODO, replace container for mFileInstance
    if (mFileInstance.size() > 0)
        ++mCurrentFile;

    auto filename = mOpenFileDialog->getSelectedFile();
    mFileInstance.push_back(new XFile (filename));
    renderView();
}

void MainWindow::keyPressSlot(int key)
{
    if (key ==  Qt::Key_Return || key ==  Qt::Key_Enter)
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
            // Tell hex dump to change its page
            mUpdateTablePage = true;
        }

        mDisplayToggle = !mDisplayToggle;
        renderView();
        return;
    }

    // Switching between loaded files
    if (key ==  Qt::Key_Tab)
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
    if (key ==  Qt::Key_Escape)
    {
        this->close();
        return;
    }

    if (key ==  Qt::Key_F1)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F2)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F3)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F4)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F5)
    {
        mGotoDialog->show();
        return;
    }

    if (key ==  Qt::Key_F6)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F7)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F8)
    {
        QMessageBox msgBox;
        msgBox.setText("Not impemented yet");
        msgBox.exec();
        return;
    }

    if (key ==  Qt::Key_F9)
    {
        mCurrentDirectory = "C:\\test";
        mOpenFileDialog->setDirectory(mCurrentDirectory);
        mOpenFileDialog->listFiles();
        mOpenFileDialog->show();
        return;
    }

    if (key ==  Qt::Key_F10)
    {
        this->close();
        return;
    }

}
