#ifndef CONTROLLERAREA_H
#define CONTROLLERAREA_H

#include <QWidget>

namespace Ui {
class ControllerArea;
}

class ControllerArea : public QWidget
{
    Q_OBJECT

public:
    explicit ControllerArea(QWidget *parent = nullptr);
    ~ControllerArea();

public slots:
    void updateMonoPressure(int value);
    void updateController(int num, int value);
    void updateBend(int value);
    void updateBendSensitivity(double semitones);

signals:
    void monoPressureChanged(int value);
    void controllerChanged(int num, int value);
    void bendChanged(int value);
    void bendSensitivityChanged(double semitones);

private slots:
    void on_sliderPitchWheel_valueChanged(int value);
    void on_sliderSensitivity_valueChanged(int value);
    void on_sliderPressure_valueChanged(int value);
    void on_comboControl1_currentIndexChanged(int index);
    void on_comboControl2_currentIndexChanged(int index);
    void on_comboControl3_currentIndexChanged(int index);
    void on_comboControl4_currentIndexChanged(int index);
    void on_knob1_valueChanged(int value);
    void on_knob2_valueChanged(int value);
    void on_knob3_valueChanged(int value);

private:
    Ui::ControllerArea *ui;
};

#endif // CONTROLLERAREA_H
