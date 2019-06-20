#ifndef MODULATOREDITOR_H
#define MODULATOREDITOR_H

#include <QWidget>

namespace Ui {
class ModulatorEditor;
}

class ModulatorEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ModulatorEditor(QWidget *parent = 0);
    ~ModulatorEditor();

private slots:
    void on_pushExpand_clicked();

    void on_pushCollapse_clicked();

private:
    Ui::ModulatorEditor *ui;
    static QList<ModulatorEditor *> s_instances;
};

#endif // MODULATOREDITOR_H
