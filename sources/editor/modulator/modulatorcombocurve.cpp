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

#include "modulatorcombocurve.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include "soundfontmanager.h"

static const QChar unicodeArrow[] = { 0xfeff, 0x279c };
const QString ModulatorComboCurve::s_rightArrow = " " + QString::fromRawData(unicodeArrow, 2) + " ";

ModulatorComboCurve::ModulatorComboCurve(QWidget* parent) : QComboBox(parent)
{
    // New view
    QTableView * view = new QTableView();
    view->viewport()->installEventFilter(this);
    view->horizontalHeader()->setVisible(false);
    view->verticalHeader()->setVisible(false);
    view->setShowGrid(false);
    this->setView(view);

    // Prepare the model
    QStandardItemModel * model = new QStandardItemModel();
    this->setModel(model);
    model->setColumnCount(4);
    model->setRowCount(4);

    // Icons
    int iconWidth = 48;
    int margin = 6;
    QMap<QString, QString> replacement;
    replacement["currentColor"] = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT).name();
    replacement["secondColor"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QString iconName = QString(":/icons/courbe%1.svg").arg(4*j+i+1, 2, 10, QChar('0'));
            QStandardItem * item = new QStandardItem("");
            item->setData(ContextManager::theme()->getColoredSvg(iconName, QSize(iconWidth, iconWidth), replacement), Qt::DecorationRole);
            model->setItem(i, j, item);
        }
    }

    // Size
    for (int i = 0; i < 4; i++)
    {
        view->setColumnWidth(i, iconWidth + margin);
        view->setRowHeight(i, iconWidth + margin);
    }
    view->setFixedSize(4 * (iconWidth + margin), 4 * (iconWidth + margin));

    // Highlight color
    QPalette p = view->palette();
    p.setColor(QPalette::Highlight, ContextManager::theme()->getColor(ThemeManager::LIST_ALTERNATIVE_BACKGROUND));
    view->setPalette(p);
}

ModulatorComboCurve::~ModulatorComboCurve()
{
    delete this->model();
}

bool ModulatorComboCurve::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::MouseButtonPress && object == view()->viewport())
    {
        this->setCurrentIndex(view()->currentIndex().row());
        this->setModelColumn(view()->currentIndex().column());

        // Store the result
        valueSelected(view()->currentIndex().row(), view()->currentIndex().column());
    }
    return false;
}

void ModulatorComboCurve::initialize(EltID id, bool source1)
{
    _id = id;
    _source1 = source1;

    loadValue();
}

void ModulatorComboCurve::initialize(SFModulator mod)
{
    _id.typeElement = elementUnknown;

    _isBipolar = mod.isBipolar;
    _isDescending = mod.isDescending;
    _type = mod.Type;

    this->setCurrentIndex(_type);
    this->setModelColumn((_isDescending ? 1 : 0) + (_isBipolar ? 2 : 0));
}

void ModulatorComboCurve::loadValue()
{
    SoundfontManager * sm = SoundfontManager::getInstance();
    SFModulator mod = sm->get(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper).sfModValue;

    _isBipolar = mod.isBipolar;
    _isDescending = mod.isDescending;
    _type = mod.Type;

    this->setCurrentIndex(_type);
    this->setModelColumn((_isDescending ? 1 : 0) + (_isBipolar ? 2 : 0));
}

void ModulatorComboCurve::valueSelected(int row, int column)
{
    if (_id.typeElement == elementUnknown)
        return;

    _isDescending = (column % 2 > 0);
    _isBipolar = (column / 2 > 0);
    _type = static_cast<quint8>(row);

    // Compare with the old value
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    val.sfModValue = sm->get(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper).sfModValue;
    if (val.sfModValue.isDescending != _isDescending || val.sfModValue.isBipolar != _isBipolar || val.sfModValue.Type != _type)
    {
        val.sfModValue.isDescending = _isDescending;
        val.sfModValue.isBipolar = _isBipolar;
        val.sfModValue.Type = static_cast<quint8>(_type);

        sm->set(_id, _source1 ? champ_sfModSrcOper : champ_sfModAmtSrcOper, val);
        sm->endEditing("modulatorEditor");
    }
}

QString ModulatorComboCurve::getEvolution()
{
    QString result = "";
    if (_isDescending)
        result = _isBipolar ? "1" + s_rightArrow + "-1" : "1" + s_rightArrow + "0";
    else
        result = _isBipolar ? "-1" + s_rightArrow + "1" : "0" + s_rightArrow + "1";
    return result;
}
