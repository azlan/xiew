#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MyDump;
class MyDisassembly;
class XFile;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MyDump *mMyDump;
    MyDisassembly * mMyDisassembly;
    std::vector<XFile *> mFileInstance;
    int mCurrentFile;


    void renderView();
    void keyPressEvent(QKeyEvent* e);

public slots:
    void keyPressSlot(int key);
private slots:
    void updateOffsetSlot(int offset);
    void updateTableOffsetSlot(int offset);
};

#endif // MAINWINDOW_H

