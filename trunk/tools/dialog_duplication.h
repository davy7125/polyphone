#ifndef DIALOG_DUPLICATION_H
#define DIALOG_DUPLICATION_H

#include <QDialog>

namespace Ui {
class DialogDuplication;
}

class DialogDuplication : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogDuplication(bool isPrst, QWidget *parent = 0);
    ~DialogDuplication();
    
signals:
    void accepted(QVector<int> listeVelocite, bool duplicKey, bool duplicVel);

private slots:
    void accept();
    void on_pushAdd_clicked();
    void on_pushRemove_clicked();
    void on_spinMinVel_valueChanged(int arg1);
    void on_spinMaxVel_valueChanged(int arg1);
    void on_checkForEachVelocityRange_clicked();

private:
    Ui::DialogDuplication *ui;
    bool _isPrst;
    QVector<int> _listeVelocites;

    void dispVel();
};

#endif // DIALOG_DUPLICATION_H
