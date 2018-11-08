#ifndef CONFIGSECTIONGENERAL_H
#define CONFIGSECTIONGENERAL_H

#include <QWidget>

namespace Ui {
class ConfigSectionGeneral;
}

class ConfigSectionGeneral : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigSectionGeneral(QWidget *parent = 0);
    ~ConfigSectionGeneral();

    void initialize();

private slots:
    void on_comboAudioOuput_currentIndexChanged(int index);
    void on_comboBufferSize_currentIndexChanged(int index);
    void on_comboMidiInput_currentIndexChanged(int index);

    void on_checkBoucle_toggled(bool checked);
    void on_checkBlanc_toggled(bool checked);
    void on_checkRepercussionStereo_toggled(bool checked);

private:
    void initializeAudio();
    void initializeMidi();

    Ui::ConfigSectionGeneral *ui;
};

#endif // CONFIGSECTIONGENERAL_H
