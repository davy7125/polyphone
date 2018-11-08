#ifndef CONFIGSECTIONKEYBOARD_H
#define CONFIGSECTIONKEYBOARD_H

#include <QWidget>

namespace Ui {
class ConfigSectionKeyboard;
}

class ConfigSectionKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionKeyboard(QWidget *parent = 0);
    ~ConfigSectionKeyboard();
    void initialize();

private slots:
    void on_comboDo_currentIndexChanged(int index);
    void on_spinDefaultVelocity_editingFinished();
    void combinaisonChanged(int key, int numOctave, QString combinaison);

private:
    void renameComboDo();
    Ui::ConfigSectionKeyboard *ui;
};

#endif // CONFIGSECTIONKEYBOARD_H
