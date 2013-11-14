#ifndef DIALOG_EXPORT_H
#define DIALOG_EXPORT_H

#include <QDialog>
#include "sf2_types.h"

class Pile_sf2;

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogExport(Pile_sf2 * sf2, EltID idSf2, QWidget *parent = 0);
    ~DialogExport();
    
private slots:
    void on_pushTick_clicked();
    void on_pushUntick_clicked();
    void on_pushFolder_clicked();
    void on_pushAnnuler_clicked();
    void on_pushExport_clicked();

signals:
    void accepted(QList<EltID> listID, QString dir, int format);

private:
    Ui::DialogExport *ui;
    EltID _initialID;
};

#endif // DIALOG_EXPORT_H
