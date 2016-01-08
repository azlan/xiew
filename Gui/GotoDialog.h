#ifndef GOTODIALOG_H
#define GOTODIALOG_H

#include <QDialog>
//#include "ValidateExpressionThread.h"
#include "Imports.h"

namespace Ui
{
    class GotoDialog;
}

class GotoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GotoDialog(QWidget* parent = 0);
    ~GotoDialog();
    QString expressionText;
    duint validRangeStart;
    duint validRangeEnd;
    bool fileOffset;
    QString modName;
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    void validateExpression(QString expression);

private slots:
    void expressionChanged(bool validExpression, bool validPointer, dsint value);
    void on_buttonOk_clicked();
    void finishedSlot(int result);

private:
    Ui::GotoDialog* ui;
    //ValidateExpressionThread* mValidateThread;
    bool IsValidMemoryRange(duint addr);
signals:
    void buttonOk_clicked();
};

#endif // GOTODIALOG_H
