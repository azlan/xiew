#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class XiewHexDump;
class XiewDisassembly;
class XiewFile;
class GotoDialog;
class OpenFileDialog;
class PeHeaderDialog;

enum ViewMode {
    HexMode,
    AsmMode
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    XiewHexDump *mMyDump;
    XiewDisassembly * mMyDisassembly;
    std::vector<XiewFile *> mFileInstance;
    int mCurrentFile;
    bool mUpdateTablePage;
    GotoDialog *mGotoDialog;
    OpenFileDialog *mOpenFileDialog;
    PeHeaderDialog *mPeHeaderDialog;
    QString mCurrentDirectory;
    ViewMode mViewMode;

    void renderView();
    void keyPressEvent(QKeyEvent* e);
    void openFile(QString &filePath);
    
public slots:
    void keyPressSlot(int key);

private slots:
    void updateOffsetSlot(int offset);
    void updateTableOffsetSlot(int offset);
    void gotoOffsetSlot();
    void openFileSlot();
};

#endif // MAINWINDOW_H

