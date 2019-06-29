#include "controllerarea.h"
#include "ui_controllerarea.h"
#include <QDebug>

ControllerArea::ControllerArea(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControllerArea)
{
    ui->setupUi(this);
}

ControllerArea::~ControllerArea()
{
    delete ui;
}

void ControllerArea::updateMonoPressure(int value)
{

}

void ControllerArea::updateController(int num, int value)
{

}

void ControllerArea::updateBend(int value)
{

}

void ControllerArea::updateBendSensitivity(double semitones)
{

}

void ControllerArea::on_sliderPitchWheel_valueChanged(int value)
{

}

void ControllerArea::on_sliderSensitivity_valueChanged(int value)
{

}

void ControllerArea::on_sliderPressure_valueChanged(int value)
{

}

void ControllerArea::on_comboControl1_currentIndexChanged(int index)
{

}

void ControllerArea::on_comboControl2_currentIndexChanged(int index)
{

}

void ControllerArea::on_comboControl3_currentIndexChanged(int index)
{

}

void ControllerArea::on_comboControl4_currentIndexChanged(int index)
{

}

void ControllerArea::on_knob1_valueChanged(int value)
{

}

void ControllerArea::on_knob2_valueChanged(int value)
{

}

void ControllerArea::on_knob3_valueChanged(int value)
{

}
