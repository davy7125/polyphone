#ifndef DIALOGCREATEELEMENTS_H
#define DIALOGCREATEELEMENTS_H

#include <QDialog>
#include "basetypes.h"

namespace Ui {
class DialogCreateElements;
}

class DialogCreateElements : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCreateElements(QWidget *parent = 0);
    ~DialogCreateElements();

    void initialize(IdList ids);

signals:
    void createElements(IdList ids, bool oneForEach);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogCreateElements *ui;
    IdList _ids;
};

#endif // DIALOGCREATEELEMENTS_H
