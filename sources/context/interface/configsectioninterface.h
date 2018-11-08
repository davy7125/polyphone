#ifndef CONFIGSECTIONINTERFACE_H
#define CONFIGSECTIONINTERFACE_H

#include <QWidget>

namespace Ui {
class ConfigSectionInterface;
}

class ConfigSectionInterface : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionInterface(QWidget *parent = 0);
    ~ConfigSectionInterface();

    void initialize();

private slots:
    void on_comboColorTheme_currentIndexChanged(int index);
    void on_pushColorWindowBackground_clicked();
    void on_pushColorButtonBackground_clicked();
    void on_pushColorSelectionBackground_clicked();
    void on_pushColorListBackground_clicked();
    void on_pushColorListAlternativeBackground_clicked();
    void on_pushColorWindowText_clicked();
    void on_pushColorButtonText_clicked();
    void on_pushColorSelectionText_clicked();
    void on_pushColorListText_clicked();
    void on_comboLangue_currentIndexChanged(int index);
    void on_comboKeyName_currentIndexChanged(int index);

private:
    void initComboLanguage();
    void fillColors();

    Ui::ConfigSectionInterface *ui;
};

#endif // CONFIGSECTIONINTERFACE_H
