#ifndef WAITINGTOOLDIALOG_H
#define WAITINGTOOLDIALOG_H

#include <QDialog>

namespace Ui {
class WaitingToolDialog;
}

class WaitingToolDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitingToolDialog(QString title, int stepNumber, QWidget *parent = 0);
    ~WaitingToolDialog();

    void setValue(int value);

signals:
    void canceled();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushCancel_clicked();

private:
    void cancel();

    Ui::WaitingToolDialog *ui;
    int _stepNumber;
    bool _isCanceled;
};

#endif // WAITINGTOOLDIALOG_H
