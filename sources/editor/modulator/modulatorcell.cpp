/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "modulatorcell.h"
#include "ui_modulatorcell.h"
#include <QPainter>
#include "contextmanager.h"
#include "soundfontmanager.h"
#include <QAbstractItemView>

static const QChar unicodeDoubleArrow[] = { QChar(0xfeff), QChar(0x27F7) };
const QString ModulatorCell::s_doubleArrow = " " + QString::fromRawData(unicodeDoubleArrow, 2) + " ";

ModulatorCell::ModulatorCell(EltID id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell),
    _isSelected(false),
    _id(id),
    _isPrst(id.isPrst()),
    _sm(SoundfontManager::getInstance()),
    _overwrittenBy(false)
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
    ui->labelModNumber->setText(tr("Modulator") + "<br/>#" + QString::number(currentNumber + 1));
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
    ui->comboTransform->setCurrentIndex(SoundfontManager::getInstance()->get(id, champ_sfModTransOper).wValue == absolute_value ? 1 : 0);

    // Compute the range
    onOutputChanged(-1);
}

ModulatorCell::ModulatorCell(const ModulatorData &modulatorData, int index, QWidget * parent) :
    QWidget(parent),
    ui(new Ui::ModulatorCell),
    _isSelected(false),
    _id(elementUnknown),
    _isPrst(false),
    _sm(nullptr),
    _overwrittenBy(false)
{
    ui->setupUi(this);

    // Style
    this->initializeStyle();

    // Load data
    ui->labelModNumber->setText(tr("Default mod.") + "<br/>#" + QString::number(index + 1));
    ui->labelDetails->hide();
    ui->spinAmount->setValue(modulatorData.amount);
    ui->comboSource1->initialize(modulatorData.srcOper);
    ui->widgetShape1->initialize(modulatorData.srcOper);
    ui->comboSource2->initialize(modulatorData.amtSrcOper);
    ui->widgetShape2->initialize(modulatorData.amtSrcOper);
    ui->comboTransform->setCurrentIndex(modulatorData.transOper == 2 ? 1 : 0);
    ui->comboDestination->initialize(modulatorData.destOper);

    // Disabled elements, keeping the same colors
    QString disabledStyleSheet = ":disabled{background-color:" + ContextManager::theme()->getColor(ThemeManager::WINDOW_BACKGROUND).name() +
        ";color:" + ContextManager::theme()->getColor(ThemeManager::WINDOW_TEXT).name() + ";}";
    QString disabledStyleSheet2 = ":disabled{background-color:" + ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name() +
        ";color:" + ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name() + ";}";
    ui->comboSource1->setStyleSheet(disabledStyleSheet);
    ui->comboSource1->setEnabled(false);
    ui->widgetShape1->setStyleSheet(disabledStyleSheet);
    ui->widgetShape1->setEnabled(false);
    ui->comboSource2->setStyleSheet(disabledStyleSheet);
    ui->comboSource2->setEnabled(false);
    ui->widgetShape2->setStyleSheet(disabledStyleSheet);
    ui->widgetShape2->setEnabled(false);
    ui->spinAmount->setStyleSheet(disabledStyleSheet2);
    ui->spinAmount->setEnabled(false);
    ui->comboTransform->setStyleSheet(disabledStyleSheet);
    ui->comboTransform->setEnabled(false);
    ui->comboDestination->setStyleSheet(disabledStyleSheet);
    ui->comboDestination->setEnabled(false);

    // Compute the range
    onOutputChanged(-1);
}

void ModulatorCell::initializeStyle()
{
    // Style
    _lineColor = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    _computationAreaColor = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
        0.25);
    _lineColorSelected = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_TEXT).name();
    _computationAreaColorSelected = ThemeManager::mix(
        ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND),
        ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND),
        0.25);
    _fontHint = QFont(this->font().family(), qMax(7, this->font().pointSize() * 3 / 4));
    ui->labelFinalRange->setFont(_fontHint);

    QString backgroundColor = ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND).name();
}

void ModulatorCell::setOverwrittenBy(int otherModulator)
{
    _overwrittenBy = true;
    setSelected(_isSelected);
    ui->labelDetails->setText(tr("overwritten by %1").arg(otherModulator + 1));
    ui->labelDetails->show();
}

void ModulatorCell::setOverridingDefault()
{
    if (ui->spinAmount->value() == 0)
        ui->labelDetails->setText(tr("disabling\ndefault mod."));
    else
        ui->labelDetails->setText(tr("overriding\ndefault mod."));
    ui->labelDetails->show();
}

void ModulatorCell::setSelected(bool isSelected)
{
    // Save the selected state
    _isSelected = isSelected;

    // Load all possible colors
    QColor colorRed = ContextManager::theme()->getFixedColor(ThemeManager::RED, ThemeManager::LIST_BACKGROUND);
    ui->labelModNumber->setStyleSheet(QString("QLabel{color:%1}").arg((_isSelected ? _lineColorSelected : _lineColor).name()));
    ui->labelFinalRange->setStyleSheet(QString("QLabel{color:%1}").arg((_isSelected ? _lineColorSelected : _lineColor).name()));
    ui->labelDetails->setStyleSheet(QString("QLabel{color:%1}").arg(
        (_isSelected ? _lineColorSelected : (_overwrittenBy ? colorRed : _lineColor)).name()));
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
    p.setPen(QPen(_isSelected ? _lineColorSelected : _lineColor, 3));
    p.drawLine(ui->comboSource2->x() + ui->comboSource2->width(), ui->comboSource2->y() + ui->comboSource2->height() / 2,
                   ui->widgetShape2->x(), ui->comboSource2->y() + ui->comboSource2->height() / 2);
    p.drawLine(ui->widgetShape2->x() + ui->widgetShape2->width(), ui->comboSource2->y() + ui->comboSource2->height() / 2,
                   ui->widgetShape2->x() + ui->widgetShape2->width() + posMultiplicationSign, ui->comboSource2->y() + ui->comboSource2->height() / 2);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign, ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->widgetShape1->x() + ui->widgetShape1->width() + posMultiplicationSign, ui->comboSource2->y() + ui->comboSource2->height() / 2);

    p.drawLine(ui->comboSource1->x() + ui->comboSource1->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->widgetShape1->x(), ui->comboSource1->y() + ui->comboSource1->height() / 2);
    p.drawLine(ui->widgetShape1->x() + ui->widgetShape1->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->spinAmount->x(), ui->comboSource1->y() + ui->comboSource1->height() / 2);
    p.drawLine(ui->spinAmount->x() + ui->spinAmount->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->comboTransform->x(), ui->comboSource1->y() + ui->comboSource1->height() / 2);
    p.drawLine(ui->comboTransform->x() + ui->comboTransform->width(), ui->comboSource1->y() + ui->comboSource1->height() / 2,
               ui->comboDestination->x(), ui->comboSource1->y() + ui->comboSource1->height() / 2);

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
    p.setBrush(_isSelected ? _lineColorSelected : _lineColor);
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
        val.wValue = absolute_value;
    else
        val.wValue = linear;
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
    if (min > max)
    {
        qint16 tmp = max;
        max = min;
        min = tmp;
    }
    _intRange = QString::number(min) + s_doubleArrow + QString::number(max);

    // Absolute value?
    if (ui->comboTransform->currentIndex() == 1)
    {
        qint16 maxTmp = qMax(qAbs(min), qAbs(max));
        if (min <= 0 && max >= 0)
            min = 0;
        else
            min = qMin(qAbs(min), qAbs(max));
        max = maxTmp;
    }

    // Addition or multiplication?
    AttributeType currentAttribute = ui->comboDestination->getCurrentAttribute();
    bool isAddition = true;
    switch (currentAttribute)
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

    // Compute the new range in double
    double dMin = min;
    double dMax = max;
    if (!isAddition)
    {
        dMin = qPow(2.0, 0.000833333 * min);
        dMax = qPow(2.0, 0.000833333 * max);
    }
    else if (currentAttribute != champ_unknown)
    {
        AttributeValue val;
        val.shValue = min;
        dMin = Attribute::toRealValue(currentAttribute, _isPrst, val);
        val.shValue = max;
        dMax = Attribute::toRealValue(currentAttribute, _isPrst, val);
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
        unit = tr("dB", "unit for decibels");
        break;
    case champ_chorusEffectsSend:
    case champ_reverbEffectsSend:
    case champ_sustainModEnv:
        unit = "%";
        break;
    case champ_coarseTune:
        unit = tr("semi-tones");
        break;
    case champ_fineTune:
        unit = tr("cents", "hundredth of semi-tones");
        break;
    case champ_scaleTuning:
        unit = "cent / key";
        break;
    default:
        break;
    }
    if (!unit.isEmpty())
        unit = " " + unit;

    ui->labelFinalRange->setText((isAddition ? tr("Add from:") :
                                               tr("Multiply from:")) +
                                 " " + doubleToString(dMin) + unit + "\n" +
                                 tr("To:") + " " + doubleToString(dMax) + unit);
    this->repaint();
}

QString ModulatorCell::doubleToString(double value)
{
    // Return a value as a string with 3 maximum characters after the decimal separator
    QString txt = QLocale::system().toString(value, 'f', 3);
    while (txt.size() > 1 && (txt.endsWith("0")))
        txt = txt.left(txt.size() - 1);
    if (txt.size() > 1 && (txt.endsWith(".") || txt.endsWith(",")))
        txt = txt.left(txt.size() - 1);
    return txt;
}
