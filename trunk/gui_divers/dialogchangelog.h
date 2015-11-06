#ifndef DIALOGCHANGELOG_H
#define DIALOGCHANGELOG_H

#include <QDialog>

namespace Ui {
class DialogChangeLog;
}

class DialogChangeLog : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangeLog(QWidget *parent = 0);
    ~DialogChangeLog();

private slots:
    void on_pushDonate_clicked();

    void on_pushOk_clicked();

private:
    Ui::DialogChangeLog *ui;
};

#endif // DIALOGCHANGELOG_H
