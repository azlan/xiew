#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}
class MyDump;
class MyDisassembly;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void keyPressEvent(QKeyEvent* e);

private:
    Ui::MainWindow *ui;
    MyDump *mMyDump;
    MyDisassembly * mMyDisassembly;

public slots:
    void keyPressSlot(int key);
};

#endif // MAINWINDOW_H

