#ifndef OPENFILEDIALOG_H
#define OPENFILEDIALOG_H

#include <QDialog>
#include <QModelIndex>


namespace Ui {
class OpenFileDialog;
}

class QDir;
class QFileSystemModel;
class QSortFilterProxyModel;

class OpenFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenFileDialog(QWidget *parent = 0);
    ~OpenFileDialog();
    void setDirectory(QString &path);
    QString getDirectory();
    void listFiles();

    QString getSelectedFile() const;
    void setSelectedFile(const QString &selectedFile);

private:
    Ui::OpenFileDialog *ui;
    QDir *mDir;
    QFileSystemModel *mDirModel;
    QSortFilterProxyModel *mProxyModel;
    QString mSelectedFile;
    QString mSelectedDir;

private slots:
    void on_treeView_activated(const QModelIndex &index);
};

#endif // OPENFILEDIALOG_H
