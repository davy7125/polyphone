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
#include <QTableWidget>
#include <QApplication>
#include <QPainter>

const char * TableDelegate::DECO_PROPERTY = "deco";
const float TableDelegate::MOD_BORDER_WIDTH = 2;

TableDelegate::TableDelegate(QTableWidget * table, QObject * parent): QStyledItemDelegate(parent),
    _table(table),
    _isEditing(false)
{
    _modBorderColor = ContextManager::theme()->getColor(ThemeManager::BORDER);
}

QWidget * TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)

    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop, isFixed, isAttenuation);
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
        // Remove the icon from the model
        QVariant previousDecoration = index.data(Qt::DecorationRole);
        QAbstractItemModel * model = const_cast<QAbstractItemModel *>(index.model());
        model->blockSignals(true);
        model->setData(index, QImage(), Qt::DecorationRole);
        model->blockSignals(false);

        QComboBox * combobox = new ComboBoxLoopMode(parent);
        combobox->setProperty(DECO_PROPERTY, previousDecoration);
        widget = combobox;
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
        spin->setMinimum(-1000000);
        spin->setMaximum(1000000);
        spin->setSingleStep(isAttenuation ? 0.04 : .1);
        spin->setStyleSheet("QDoubleSpinBox{ border: 3px solid " + highlightColor.name() + "; }" +
                            "QDoubleSpinBox::down-button{width:0px;} QDoubleSpinBox::up-button{width:0px;} ");
        spin->setDecimals(nbDecimales);
        widget = spin;
    }
    else
    {
        QSpinBox * spin = new NullableSpinBox(parent);
        spin->setMinimum(-2147483647);
        spin->setMaximum(2147483647);
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
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop, isFixed, isAttenuation);

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
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop, isFixed, isAttenuation);

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

            bool isDark = ContextManager::theme()->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT);
            switch (currentData)
            {
            case -1:
                model->setData(index, QVariant(), Qt::DecorationRole);
                break;
            case 0:
                if (isDark)
                    model->setData(index, QImage(":/icons/loop_off_w.png"), Qt::DecorationRole);
                else
                    model->setData(index, QImage(":/icons/loop_off.png"), Qt::DecorationRole);
                break;
            case 1:
                if (isDark)
                    model->setData(index, QImage(":/icons/loop_on_w.png"), Qt::DecorationRole);
                else
                    model->setData(index, QImage(":/icons/loop_on.png"), Qt::DecorationRole);
                break;
            case 3:
                if (isDark)
                    model->setData(index, QImage(":/icons/loop_on_end_w.png"), Qt::DecorationRole);
                else
                    model->setData(index, QImage(":/icons/loop_on_end.png"), Qt::DecorationRole);
                break;
            default:
                break;
            }
            editor->setProperty(DECO_PROPERTY, QVariant());
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
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop, isFixed, isAttenuation);

    if (isLoop && !editor->property(DECO_PROPERTY).isNull())
    {
        // Restore the previous decoration
        QAbstractItemModel * model = const_cast<QAbstractItemModel *>(index.model());
        model->blockSignals(true);
        model->setData(index, editor->property(DECO_PROPERTY), Qt::DecorationRole);
        model->blockSignals(false);
    }

    QStyledItemDelegate::destroyEditor(editor, index);
}
#endif

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    bool isNumeric, isKey, isLoop, isFixed, isAttenuation;
    int nbDecimales;
    getType(isNumeric, isKey, nbDecimales, index.row(), isLoop, isFixed, isAttenuation);

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
        QImage image = index.data(Qt::DecorationRole).value<QImage>();
        painter->drawPixmap(opt.rect.left() + (opt.rect.width() - image.width()) / 2,
                            opt.rect.top() + (opt.rect.height() - image.height()) / 2,
                            image.width(), image.height(), QPixmap::fromImage(image));
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
            QRectF rect1 = opt.rect.toRectF();

            painter->setBrush(_modBorderColor);
            painter->setPen(Qt::transparent);
            painter->drawRect(QRectF(rect1.left(), rect1.top(), rect1.width(), MOD_BORDER_WIDTH));
            painter->drawRect(QRectF(rect1.left(), rect1.bottom() - MOD_BORDER_WIDTH, rect1.width(), MOD_BORDER_WIDTH));
            painter->drawRect(QRectF(rect1.left(), rect1.top(), MOD_BORDER_WIDTH, rect1.height()));
            painter->drawRect(QRectF(rect1.right() - MOD_BORDER_WIDTH, rect1.top(), MOD_BORDER_WIDTH, rect1.height()));
        }
    }
}

void TableDelegate::getType(bool &isNumeric, bool &isKey, int &nbDecimales, int numRow, bool &isLoop,
                            bool &isFixed, bool &isAttenuation) const
{
    isNumeric = true;
    isKey = false;
    isLoop = false;
    isFixed = false;
    isAttenuation = false;
    nbDecimales = 0;

    if (_table->rowCount() == 50)
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

void TableDelegate::resetModDisplay()
{
    _modDisplay.clear();
}

void TableDelegate::updateModDisplay(int column, QList<int> rows)
{
    _modDisplay[column] = rows;
}
