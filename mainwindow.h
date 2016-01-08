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
class GotoDialog;

enum ViewMode {
    Hex,
    Asm
};

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
    bool mDisplayToggle;
    bool mUpdateTablePage;
    GotoDialog *mGotoDialog;

    void renderView();
    void keyPressEvent(QKeyEvent* e);

public slots:
    void keyPressSlot(int key);
private slots:
    void updateOffsetSlot(int offset);
    void updateTableOffsetSlot(int offset);
    void gotoOffsetSlot();
};

#endif // MAINWINDOW_H

