#ifndef DIALOGSELECTION_H
#define DIALOGSELECTION_H

#include <QDialog>
#include "sf2_types.h"

class Pile_sf2;

namespace Ui {
class DialogSelection;
}

class DialogSelection : public QDialog
{
    Q_OBJECT

public:
    DialogSelection(Pile_sf2 *sf2, EltID id, QWidget *parent = 0);
    ~DialogSelection();

private slots:
    void on_pushSelectAll_clicked();
    void on_pushSelectNone_clicked();
    void on_pushCancel_clicked();
    void on_pushOk_clicked();

signals:
    void listChosen(QList<int> listIndex);

private:
    Ui::DialogSelection *ui;
};

#endif // DIALOGSELECTION_H
