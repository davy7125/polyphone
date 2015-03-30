#ifndef DIALOG_TRANSPOSITION_H
#define DIALOG_TRANSPOSITION_H

#include <QDialog>

namespace Ui {
class DialogTransposition;
}

class DialogTransposition : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTransposition(QWidget *parent = 0);
    ~DialogTransposition();

signals:
    void accepted(double shift, bool adaptKeyRange);

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::DialogTransposition *ui;
};

#endif // DIALOG_TRANSPOSITION_H
