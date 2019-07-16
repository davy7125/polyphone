/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "modulatorcell.h"
#include "ui_modulatorcell.h"
#include <QPainter>
#include "contextmanager.h"
#include "soundfontmanager.h"

static const QChar unicodeDoubleArrow[] = { 0xfeff, 0x27F7 };
const QString ModulatorCell::s_doubleArrow = " " + QString::fromRawData(unicodeDoubleArrow, 2) + " ";

ModulatorCell::ModulatorCell(EltID id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell),
    _isSelected(false),
    _id(id),
    _isPrst(id.isPrst()),
    _sm(SoundfontManager::getInstance()),
    _overridenBy(false)
{
    ui->setupUi(this);

    // Style
    this->initializeStyle();

    // Current number
    int currentNumber = 0;
    foreach (int i, SoundfontManager::getInstance()->getSiblings(id))
    {
        if (i == _id.indexMod)
            break;
        currentNumber++;
    }
    ui->labelModNumber->setText(trUtf8("Modulator") + "<br/>#" + QString::number(currentNumber + 1));
    ui->labelDetails->hide();

    // Comboboxes
    ui->comboSource1->initialize(id, true);
    ui->comboSource2->initialize(id, false);
    ui->comboDestination->initialize(id);
    connect(ui->comboDestination, SIGNAL(currentIndexChanged(int)), this, SLOT(onOutputChanged(int)));

    // Shapes
    ui->widgetShape1->initialize(id, true);
    ui->widgetShape2->initialize(id, false);

    // Coefficient
    ui->spinAmount->blockSignals(true);
    ui->spinAmount->setValue(SoundfontManager::getInstance()->get(id, champ_modAmount).shValue);
    ui->spinAmount->blockSignals(false);
    connect(ui->spinAmount, SIGNAL(valueChanged(int)), this, SLOT(onOutputChanged(int)));

    // Transform
    ui->comboTransform->setCurrentIndex(SoundfontManager::getInstance()->get(id, champ_sfModTransOper).wValue == 2 ? 1 : 0);

    // Compute the range
    onOutputChanged(-1);
}

ModulatorCell::ModulatorCell(ModulatorData modulatorData, QWidget * parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell),
    _isSelected(false),
    _id(elementUnknown),
    _isPrst(false),
    _sm(nullptr),
    _overridenBy(false)
{
    ui->setupUi(this);

    // Style
    this->initializeStyle();

    // Load data
    ui->labelModNumber->setText(trUtf8("Default mod.") + "<br/>#" + QString::number(modulatorData.index + 1));
    ui->labelDetails->hide();
    ui->spinAmount->setValue(modulatorData.amount);
    ui->comboSource1->initialize(modulatorData.srcOper);
    ui->widgetShape1->initialize(modulatorData.srcOper);
    ui->comboSource2->initialize(modulatorData.amtSrcOper);
    ui->widgetShape2->initialize(modulatorData.amtSrcOper);
    ui->comboTransform->setCurrentIndex(modulatorData.transOper == 2 ? 1 : 0);
    ui->comboDestination->initialize(modulatorData.destOper);

    // Disable elements
    ui->comboSource1->setEnabled(false);
    ui->widgetShape1->setEnabled(false);
    ui->comboSource2->setEnabled(false);
    ui->widgetShape2->setEnabled(false);
    ui->spinAmount->setEnabled(false);
    ui->comboTransform->setEnabled(false);
    ui->comboDestination->setEnabled(false);

    // Compute the range
    onOutputChanged(-1);
}

void ModulatorCell::initializeStyle()
{
    // Style
    _labelColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.75);
    _computationAreaColor = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.35);
    _labelColorSelected = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.25);
    _computationAreaColorSelected = ThemeManager::mix(
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT),
                ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
                0.65);
    _fontHint = QFont(this->font().family(), qMax(7, this->font().pointSize() * 3 / 4));
    ui->labelFinalRange->setFont(_fontHint);
}

void ModulatorCell::setOverridenBy(int otherModulator)
{
    _overridenBy = true;
    setSelected(_isSelected);
    ui->labelDetails->setText(trUtf8("overriden by %1").arg(otherModulator + 1));
    ui->labelDetails->show();
}

void ModulatorCell::setOverridingDefault()
{
    if (ui->spinAmount->value() == 0)
        ui->labelDetails->setText(trUtf8("disabling\ndefault mod."));
    else
        ui->labelDetails->setText(trUtf8("overriding\ndefault mod."));
    ui->labelDetails->show();
}

void ModulatorCell::setSelected(bool isSelected)
{
    // Save the selected state
    _isSelected = isSelected;

    // Load all possible colors
    QString colorRed = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND).name();
    QString colorSelected = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    QString colorNormal = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();

    ui->labelModNumber->setStyleSheet(QString("QLabel{color:%1}").arg(_isSelected ? colorSelected : colorNormal));
    ui->labelFinalRange->setStyleSheet(QString("QLabel{color:%1}").arg(_isSelected ? colorSelected : colorNormal));
    ui->labelDetails->setStyleSheet(QString("QLabel{color:%1}").arg(
                                        _isSelected ? colorSelected : (_overridenBy ? colorRed : colorNormal)));
}

ModulatorCell::~ModulatorCell()
{
    delete ui;
}

ModulatorData ModulatorCell::getModulatorData()
{
    ModulatorData mod;

    mod.srcOper.CC = ui->comboSource1->isCC();
    mod.srcOper.Index = static_cast<quint8>(ui->comboSource1->getIndex());
    mod.srcOper.Type = ui->widgetShape1->getType();
    mod.srcOper.isBipolar = ui->widgetShape1->isBipolar();
    mod.srcOper.isDescending = ui->widgetShape1->isDescending();

    mod.amtSrcOper.CC = ui->comboSource2->isCC();
    mod.amtSrcOper.Index = static_cast<quint8>(ui->comboSource2->getIndex());
    mod.amtSrcOper.Type = ui->widgetShape2->getType();
    mod.amtSrcOper.isBipolar = ui->widgetShape2->isBipolar();
    mod.amtSrcOper.isDescending = ui->widgetShape2->isDescending();

    mod.amount = static_cast<qint16>(ui->spinAmount->value());
    mod.transOper = ui->comboTransform->currentIndex() == 0 ? linear : absolute_value;
    mod.destOper = static_cast<quint16>(ui->comboDestination->getCurrentAttribute());

    return mod;
}

void ModulatorCell::paintEvent(QPaintEvent* event)
{
    QPainter p(this);

    // Highlight the computation area
    int x1 = ui->widgetShape1->x() - 6;
    int x2 = ui->comboTransform->x() + ui->comboTransform->width() + 6;
    int y1 = ui->widgetShape1->y() - 6;
    int y2 = ui->widgetShape2->y() + ui->widgetShape2->height() + 6;
    p.fillRect(QRect(x1, y1, x2 - x1, y2 - y1), _isSelected ? _computationAreaColorSelected : _computationAreaColor);

    // Draw lines
    int posMultiplicationSign = 44;
    p.setPen(QPen(_isSelected ? _labelColorSelected : _labelColor, 3));
    p.drawLine(ui->comboSource2->x() + ui->comboSource2->width(), ui->comboSource2->y() + ui->comboSource2->height() / 2,
               ui->widgetShape2->x() + ui->widgetShape2->width() + posMultiplicationSign, ui->comboSource2->y() + ui->comboSource2->height() / 2);
    p.drawLine(ui->comboSource1->x() + ui->comboSource1->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->comboDestination->x(), ui->comboSource1->y() + ui->comboSource1->height() / 2);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign, ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign, ui->comboSource2->y() + ui->comboSource2->height() / 2);

    // Add input range
    p.setFont(_fontHint);
    p.drawText(ui->widgetShape1->x() + ui->widgetShape1->width() + 2, ui->comboSource1->y() + ui->comboSource1->height() / 2 + 5 + _fontHint.pointSize(),
               ui->comboSource1->isOne() ? "1" : ui->widgetShape1->getEvolution());
    p.drawText(ui->widgetShape2->x() + ui->widgetShape2->width() + 2, ui->comboSource2->y() + ui->comboSource2->height() / 2 + 5 + _fontHint.pointSize(),
               ui->comboSource2->isOne() ? "1" : ui->widgetShape2->getEvolution());

    // Add output range
    p.drawText(ui->spinAmount->x(), ui->spinAmount->y() + ui->spinAmount->height() + 5 + _fontHint.pointSize(), _intRange);

    // Draw a multiplication sign
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.setBrush(_isSelected ? _labelColorSelected : _labelColor);
    p.drawEllipse(ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign - 8, ui->comboSource1->y() + ui->comboSource1->height() / 2 - 8, 16, 16);
    p.setPen(QPen(_isSelected ? _computationAreaColorSelected : _computationAreaColor, 2));
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign - 3, ui->comboSource1->y() + ui->comboSource1->height() / 2 - 3,
               ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign + 3, ui->comboSource1->y() + ui->comboSource1->height() / 2 + 3);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign - 3, ui->comboSource1->y() + ui->comboSource1->height() / 2 + 3,
               ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign + 3, ui->comboSource1->y() + ui->comboSource1->height() / 2 - 3);

    QWidget::paintEvent(event);
}

AttributeType ModulatorCell::getTargetAttribute()
{
    return ui->comboDestination->getCurrentAttribute();
}

void ModulatorCell::on_spinAmount_editingFinished()
{
    if (_sm == nullptr)
        return;

    // Compare with the old value
    AttributeValue val;
    val.shValue = static_cast<qint16>(ui->spinAmount->value());
    if (_sm->get(_id, champ_modAmount).shValue != val.shValue)
    {
        _sm->set(_id, champ_modAmount, val);
        _sm->endEditing("modulatorEditor");
    }
}

void ModulatorCell::on_comboTransform_currentIndexChanged(int index)
{
    if (_sm == nullptr)
        return;

    // Compare with the old value
    AttributeValue val;
    if (index == 1)
        val.wValue = 2;
    else
        val.wValue = 0;
    if (_sm->get(_id, champ_sfModTransOper).wValue != val.wValue)
    {
        _sm->set(_id, champ_sfModTransOper, val);
        _sm->endEditing("modulatorEditor");
    }
}

void ModulatorCell::onOutputChanged(int dummy)
{
    Q_UNUSED(dummy)

    // Compute min / max as integer
    qint16 min1 = (ui->comboSource1->isOne() ? 1 : (ui->widgetShape1->isBipolar() ? -1 : 0));
    qint16 min2 = (ui->comboSource2->isOne() ? 1 : (ui->widgetShape2->isBipolar() ? -1 : 0));
    qint16 max = static_cast<qint16>(ui->spinAmount->value());
    qint16 min = max * qMin(min1, min2);
    if (ui->comboTransform->currentIndex() == 1 && min < 0)
        min = 0;
    if (min > max)
    {
        qint16 tmp = max;
        max = min;
        min = tmp;
    }
    _intRange = QString::number(min) + s_doubleArrow + QString::number(max);

    // Compute the new range in double
    AttributeValue val;
    val.shValue = min;
    double dMin = Attribute::toRealValue(ui->comboDestination->getCurrentAttribute(), _isPrst, val);
    val.shValue = max;
    double dMax = Attribute::toRealValue(ui->comboDestination->getCurrentAttribute(), _isPrst, val);

    // Addition or multiplication?
    bool isAddition = true;
    switch (ui->comboDestination->getCurrentAttribute())
    {
    case champ_delayModEnv: case champ_delayVolEnv:
    case champ_attackModEnv: case champ_attackVolEnv:
    case champ_holdModEnv: case champ_holdVolEnv:
    case champ_decayModEnv: case champ_decayVolEnv:
    case champ_releaseModEnv: case champ_releaseVolEnv:
    case champ_delayModLFO: case champ_delayVibLFO:
    case champ_initialFilterFc: case champ_freqModLFO: case champ_freqVibLFO:
        isAddition = false;
        break;
    default:
        break;
    }

    // Get the unit
    QString unit = "";
    switch (ui->comboDestination->getCurrentAttribute())
    {
    case champ_modLfoToPitch:
    case champ_vibLfoToPitch:
    case champ_modEnvToPitch:
    case champ_modLfoToFilterFc:
    case champ_modEnvToFilterFc:
    case champ_keynumToModEnvHold:
    case champ_keynumToModEnvDecay:
    case champ_keynumToVolEnvHold:
    case champ_keynumToVolEnvDecay:
        unit = "(c)"; // No better way to display it...
        break;
    case champ_initialFilterQ:
    case champ_modLfoToVolume:
    case champ_sustainVolEnv:
    case champ_initialAttenuation:
        unit = trUtf8("dB");
        break;
    case champ_chorusEffectsSend:
    case champ_reverbEffectsSend:
    case champ_sustainModEnv:
        unit = "%";
        break;
    case champ_coarseTune:
        unit = trUtf8("semi-tones");
        break;
    case champ_fineTune:
        unit = trUtf8("cents", "hundredth of semi-tones");
        break;
    case champ_scaleTuning:
        unit = "cent / key";
        break;
    default:
        break;
    }

    ui->labelFinalRange->setText((isAddition ? trUtf8("Add from: ") : trUtf8("Multiply from: ")) + QString::number(dMin) + " " + unit + "\n" +
                                 trUtf8("To: ") + QString::number(dMax) + " " + unit);
    this->repaint();
}
