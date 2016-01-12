#include "PeHeaderDialog.h"
#include "ui_PeHeaderDialog.h"

PeHeaderDialog::PeHeaderDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeHeaderDialog)
{
    ui->setupUi(this);
    setModal(true);
#if QT_VERSION < QT_VERSION_CHECK(5,0,0)
    setWindowFlags(Qt::Dialog | Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::MSWindowsFixedSizeDialogHint);
#endif
}

PeHeaderDialog::~PeHeaderDialog()
{
    delete ui;
}
