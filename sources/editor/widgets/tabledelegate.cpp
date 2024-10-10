/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#include "tabledelegate.h"
#include "spinboxkey.h"
#include "spinboxrange.h"
#include "nullablespinbox.h"
#include "comboboxloopmode.h"
#include "contextmanager.h"
#include "tablewidget.h"
#include <QApplication>
#include <QPainter>

const char * TableDelegate::DECO_PROPERTY = "deco";
const float TableDelegate::MOD_BORDER_WIDTH = 2;

TableDelegate::TableDelegate(TableWidget * table, QObject * parent): QStyledItemDelegate(parent),
    _table(table),
    _isEditing(false)
{
    _modBorderColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Sample mode icons
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::ColorContext::DISABLED).name();
    _sampleModeIcons[0] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_off.svg", QSize(50, 18), replacement);
    _sampleModeIcons[1] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on.svg", QSize(50, 18), replacement);
    _sampleModeIcons[2] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_release.svg", QSize(50, 18), replacement);
    _sampleModeIcons[3] = ContextManager::theme()->getColoredSvg(":/icons/sample_mode_loop_on_end.svg", QSize(50, 18), replacement);
}

QWidget * TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(index.row(), isNumeric, isKey, nbDecimales, isLoop, isFixed, isAttenuation);
    if (isFixed)
        return nullptr;

    QWidget * widget;
    QColor highlightColor = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
    if (!isNumeric)
    {
        // Range
        SpinBoxRange * spin;
        if (isKey)
            spin = new SpinBoxKeyRange(parent);
        else
            spin = new SpinBoxVelocityRange(parent);
        spin->setStyleSheet("SpinBoxRange{ border: 3px solid " + highlightColor.name() + "; }" +
                            "SpinBoxRange::down-button{width:0px;} SpinBoxRange::up-button{width:0px;} ");
        widget = spin;
    }
    else if (isLoop)
    {
        widget = new ComboBoxLoopMode(parent);
    }
    else if (isKey)
    {
        SpinBoxKey * spin = new SpinBoxKey(parent, true);
        spin->setStyleSheet("SpinBoxKey{ border: 3px solid " + highlightColor.name() + "; }" +
                            "SpinBoxKey::down-button{width:0px;} SpinBoxKey::up-button{width:0px;} ");
        widget = spin;
    }
    else if (nbDecimales > 0)
    {
        NullableDoubleSpinBox * spin = new NullableDoubleSpinBox(parent);
        double min, max;
        this->getLimits(index.row(), min, max);
        spin->setMinimum(min);
        spin->setMaximum(max);
        spin->setSingleStep(isAttenuation ? 0.04 : .1);
        spin->setStyleSheet("QDoubleSpinBox{ border: 3px solid " + highlightColor.name() + "; }" +
                            "QDoubleSpinBox::down-button{width:0px;} QDoubleSpinBox::up-button{width:0px;} ");
        spin->setDecimals(nbDecimales);
        widget = spin;
    }
    else
    {
        QSpinBox * spin = new NullableSpinBox(parent);
        int min, max;
        this->getLimits(index.row(), min, max);
        spin->setMinimum(min);
        spin->setMaximum(max);
        spin->setStyleSheet("QSpinBox{ border: 3px solid " + highlightColor.name() + "; }" +
                            "QSpinBox::down-button{width:0px;} QSpinBox::up-button{width:0px;} ");
        widget = spin;
    }

    return widget;
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(index.row(), isNumeric, isKey, nbDecimales, isLoop, isFixed, isAttenuation);

    if (!isNumeric)
    {
        SpinBoxRange * spin = dynamic_cast<SpinBoxRange *>(editor);
        spin->setText(index.data().isNull() ? ("0" + SpinBoxRange::SEPARATOR + "127") : index.data().toString());
    }
    else if (isLoop)
    {
        QComboBox * combobox = dynamic_cast<QComboBox *>(editor);
        combobox->blockSignals(true);
        if (index.data(Qt::UserRole).isNull())
            combobox->setCurrentIndex(0);
        else
        {
            switch (index.data(Qt::UserRole).toInt())
            {
            case 1:
                combobox->setCurrentIndex(2);
                break;
            case 3:
                combobox->setCurrentIndex(3);
                break;
            case 0: default:
                combobox->setCurrentIndex(1);
                break;
            }
        }
        combobox->blockSignals(false);
    }
    else if (isKey)
    {
        SpinBoxKey * spin = dynamic_cast<SpinBoxKey *>(editor);
        int keyNumber = ContextManager::keyName()->getKeyNum(index.data().toString());
        spin->setValue(index.data().isNull() ? 60 : keyNumber < 0 ? 0 : keyNumber);
    }
    else if (nbDecimales > 0)
    {
        NullableDoubleSpinBox * spin = dynamic_cast<NullableDoubleSpinBox *>(editor);
        spin->setValue(index.data().isNull() ? 0.0 : QLocale::system().toDouble(index.data().toString()));
    }
    else
    {
        NullableSpinBox * spin = dynamic_cast<NullableSpinBox *>(editor);
        spin->setValue(index.data().isNull() ? 0 : QLocale::system().toInt(index.data().toString()));
    }

    _isEditing = true;
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(index.row(), isNumeric, isKey, nbDecimales, isLoop, isFixed, isAttenuation);

    if (!isNumeric)
    {
        // Range
        SpinBoxRange * spin = dynamic_cast<SpinBoxRange *>(editor);
        model->setData(index, spin->isNull() ? QVariant() : spin->text(), Qt::EditRole);
    }
    else if (isLoop)
    {
        ComboBoxLoopMode * combobox = dynamic_cast<ComboBoxLoopMode *>(editor);
        if (combobox->count() > 0)
        {
            int currentData = combobox->itemData(combobox->getIndex(), Qt::UserRole).toInt();
            model->setData(index, currentData == -1 ? QVariant() : currentData, Qt::UserRole);
        }
    }
    else if (isKey)
    {
        SpinBoxKey * spin = dynamic_cast<SpinBoxKey*>(editor);
        model->setData(index, spin->isNull() ? QVariant() : QString::number(spin->value()), Qt::EditRole);
    }
    else if (nbDecimales > 0)
    {
        NullableDoubleSpinBox * spin = dynamic_cast<NullableDoubleSpinBox*>(editor);
        model->setData(index, spin->isNull() ? QVariant() : QLocale::system().toString(spin->value(), 'f', nbDecimales), Qt::EditRole);
    }
    else
    {
        NullableSpinBox * spin = dynamic_cast<NullableSpinBox*>(editor);
        model->setData(index, spin->isNull() ? QVariant() : QString::number(spin->value()), Qt::EditRole);
    }

    _isEditing = false;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void TableDelegate::destroyEditor(QWidget * editor, const QModelIndex & index) const
{
    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(index.row(), isNumeric, isKey, nbDecimales, isLoop, isFixed, isAttenuation);
    QStyledItemDelegate::destroyEditor(editor, index);
}
#endif

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(index.row(), isNumeric, isKey, nbDecimales, isLoop, isFixed, isAttenuation);

    if (isLoop)
    {
        // Background
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        QStyleOptionViewItem opt(option);
#else
        QStyleOptionViewItemV4 opt(option);
#endif
        initStyleOption(&opt, index);

        // Hack: check if the index is selected because option.state might not contain QStyle::State_Selected
        QList<QTableWidgetSelectionRange> ranges = _table->selectedRanges();
        foreach (QTableWidgetSelectionRange range, ranges)
        {
            if (range.topRow() <= index.row() && range.bottomRow() >= index.row() &&
                range.leftColumn() <= index.column() && range.rightColumn() >= index.column())
            {
                opt.backgroundBrush = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND);
                break;
            }
        }

        painter->fillRect(option.rect, opt.backgroundBrush);

        // Image
        bool ok;
        int loopMode = index.data(Qt::UserRole).toInt(&ok);
        if (ok)
        {
            QPixmap pix = _sampleModeIcons[loopMode];
            painter->drawPixmap(opt.rect.left() + (opt.rect.width() - pix.width()) / 2,
                                opt.rect.top() + (opt.rect.height() - pix.height()) / 2,
                                pix.width(), pix.height(), pix);
        }
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);

        // Display mods
        if (_modDisplay.contains(index.column()) && _modDisplay[index.column()].contains(index.row()))
        {
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
            QStyleOptionViewItem opt(option);
#else
            QStyleOptionViewItemV4 opt(option);
#endif
            initStyleOption(&opt, index);
            painter->setPen(QPen(_modBorderColor, 2 * MOD_BORDER_WIDTH));
            painter->setClipRect(opt.rect);
            painter->drawRect(opt.rect);
            painter->setClipping(false);
        }
    }
}

void TableDelegate::getType(int numRow, bool &isNumeric, bool &isKey, int &nbDecimales, bool &isLoop,
                            bool &isFixed, bool &isAttenuation) const
{
    isNumeric = true;
    isKey = false;
    isLoop = false;
    isFixed = false;
    isAttenuation = false;
    nbDecimales = 0;

    if (_table->isInstrumentLevel())
    {
        // Table instrument
        switch (numRow - 1)
        {
        case 0:
            isNumeric = false;
            isKey = true;
            break;
        case 1:
            isNumeric = false;
            break;
        case 2:
            isAttenuation = true;
            nbDecimales = 2;
            break;
        case 3: case 11: case 16: case 24: case 34: case 39: case 40:
            nbDecimales = 1;
            break;
        case 12: case 13: case 14: case 15: case 17:
        case 20: case 21: case 22: case 23: case 25:
        case 30: case 31: case 35: case 36:
            nbDecimales = 3;
            break;
        case 5:
            isKey = true;
            isFixed = true;
            break;
        case 41: case 6:
            isKey = true;
            break;
        case 4:
            // Loop mode
            isLoop = true;
            break;
        case 43: case 46:
            isFixed = true;
            isNumeric = false;
            break;
        }
    }
    else
    {
        // Table preset
        switch (numRow - 1)
        {
        case 0:
            isNumeric = false;
            isKey = true;
            break;
        case 1:
            isNumeric = false;
            break;
        case 2:
            isAttenuation = true;
            nbDecimales = 2;
            break;
        case 3: case 8: case 13: case 21: case 31: case 35: case 36:
            nbDecimales = 1;
            break;
        case 7: case 9: case 10: case 11: case 12: case 14:
        case 17: case 18: case 19: case 20: case 22: case 27:
        case 28: case 32: case 33:
            nbDecimales = 3;
            break;
        }
    }
}

void TableDelegate::getLimits(int numRow, int &min, int &max) const
{
    // Min and max as double values
    double dMin, dMax;
    getLimits(numRow, dMin, dMax);

    // Convert to int
    min = (int)(dMin + 0.5 - (dMin < 0));
    max = (int)(dMax + 0.5 - (dMax < 0));
}

void TableDelegate::getLimits(int numRow, double &min, double &max) const
{
    // Get min / max raw values of the parameter
    AttributeType champ = _table->getChamp(numRow);
    bool isPrst = !_table->isInstrumentLevel();
    int rawMin, rawMax;
    Attribute::getLimit(champ, isPrst, rawMin, rawMax);

    // Value -1 can be excluded as it refers to "not set"
    if (champ == champ_overridingRootKey || champ == champ_keynum || champ == champ_velocity)
        rawMin = 0;

    // Convert to real values
    AttributeValue val;
    val.shValue = rawMin;
    min = Attribute::toRealValue(champ, isPrst, val);

    val.shValue = rawMax;
    max = Attribute::toRealValue(champ, isPrst, val);
}

void TableDelegate::resetModDisplay()
{
    _modDisplay.clear();
}

void TableDelegate::updateModDisplay(int column, QList<int> rows)
{
    _modDisplay[column] = rows;
}
