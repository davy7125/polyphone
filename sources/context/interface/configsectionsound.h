#ifndef CONFIGSECTIONSOUND_H
#define CONFIGSECTIONSOUND_H

#include <QWidget>

namespace Ui {
class ConfigSectionSound;
}

class ConfigSectionSound : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionSound(QWidget *parent = 0);
    ~ConfigSectionSound();
    void initialize();

private slots:
    void on_dialRevNiveau_valueChanged(int value);
    void on_dialRevProfondeur_valueChanged(int value);
    void on_dialRevDensite_valueChanged(int value);
    void on_dialRevAttenuation_valueChanged(int value);
    void on_dialChoNiveau_valueChanged(int value);
    void on_dialChoAmplitude_valueChanged(int value);
    void on_dialChoFrequence_valueChanged(int value);
    void on_sliderGain_valueChanged(int value);

private:
    Ui::ConfigSectionSound *ui;
};

#endif // CONFIGSECTIONSOUND_H
