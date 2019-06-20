#ifndef MODULATORCELL_H
#define MODULATORCELL_H

#include <QWidget>

namespace Ui {
class ModulatorCell;
}

class ModulatorCell : public QWidget
{
    Q_OBJECT

public:
    explicit ModulatorCell(QWidget *parent = 0);
    ~ModulatorCell();

private:
    Ui::ModulatorCell *ui;
};

#endif // MODULATORCELL_H
