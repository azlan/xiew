#ifndef PEHEADERDIALOG_H
#define PEHEADERDIALOG_H

#include <QDialog>

namespace Ui {
class PeHeaderDialog;
}

class PeHeaderDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PeHeaderDialog(QWidget *parent = 0);
    ~PeHeaderDialog();

private:
    Ui::PeHeaderDialog *ui;
};

#endif // PEHEADERDIALOG_H
