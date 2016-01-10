#include "OpenFileDialog.h"
#include "ui_OpenFileDialog.h"
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

OpenFileDialog::OpenFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenFileDialog)
{
    ui->setupUi(this);
    setModal(true);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);
#endif
    mDir = new QDir;
    mDirModel = new QFileSystemModel(this);
    mProxyModel = new QSortFilterProxyModel(this);
}

OpenFileDialog::~OpenFileDialog()
{
    delete mDir;
    delete mDirModel;
    delete ui;
}

void OpenFileDialog::setDirectory(QString &path)
{
    mDir->setPath(path);
}

QString OpenFileDialog::getDirectory() {
    return mDir->path();
}

void OpenFileDialog::listFiles()
{
    mDirModel->setRootPath(QDir::currentPath());
    mDirModel->setFilter(QDir::NoDot | QDir::AllEntries);
    ui->treeView->setModel(mDirModel);
    ui->treeView->setRootIndex(mDirModel->index(getDirectory()));

//    ui->treeView->setSortingEnabled(true);
//    mProxyModel->setSourceModel(mDirModel);
//    ui->treeView->setModel(mProxyModel);
//    ui->treeView->setRootIndex(mProxyModel->mapFromSource(mDirModel->index(getDirectory())));

//    mProxyModel->sort(0, Qt::AscendingOrder);
}

QString OpenFileDialog::getSelectedFile() const
{
    return mSelectedFile;
}

void OpenFileDialog::setSelectedFile(const QString &selectedFile)
{
    mSelectedFile = selectedFile;
}

void OpenFileDialog::on_treeView_activated(const QModelIndex &index)
{
    if (mDirModel->isDir(index))
    {
        auto mSelectedDir = mDirModel->filePath(index);
        if (mSelectedDir.endsWith(".."))
        {
            // remove "/.."
            mSelectedDir = mSelectedDir.left(mSelectedDir.length()-3);
            QFileInfo info = (mSelectedDir);
            // get parent directory
            mSelectedDir = info.path();
        }

        setDirectory(mSelectedDir);
        mDirModel->setRootPath(getDirectory());
        mDirModel->setFilter(QDir::NoDot | QDir::AllEntries);
        ui->treeView->setRootIndex(mDirModel->index(getDirectory()));
    }
    else
    {
        mSelectedFile = mDirModel->filePath(index);
        this->accept();
    }
}
