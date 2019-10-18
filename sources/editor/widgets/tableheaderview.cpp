/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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

#include "tableheaderview.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QMenu>
#include "soundfontmanager.h"
#include <QPainter>

const int TableHeaderView::MARGIN = 2;

TableHeaderView::TableHeaderView(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
    this->setSectionsClickable(true);
    this->setHighlightSections(true);
    this->setDefaultSectionSize(70);

    // Mute icon
    _muteIcon = ContextManager::theme()->getColoredSvg(":/icons/volume-mute.svg", QSize(12, 12), ThemeManager::HIGHLIGHTED_BACKGROUND);

    // Menu
    _menu = new QMenu(this);
    _muteAction = _menu->addAction(trUtf8("mute"));
    _muteAction->setCheckable(true);
    connect(_muteAction, SIGNAL(toggled(bool)), this, SLOT(mute(bool)));

    QAction * action1 = _menu->addAction(trUtf8("activate solo"));
    connect(action1, SIGNAL(triggered(bool)), this, SLOT(activateSolo(bool)));

    QAction * action2 = _menu->addAction(trUtf8("unmute all"));
    connect(action2, SIGNAL(triggered(bool)), this, SLOT(unmuteAll(bool)));

    // Height of the header
    QFontMetrics fm(this->font());
    _height = fm.height() * 2;
}

TableHeaderView::~TableHeaderView()
{
    delete _menu;
}

void TableHeaderView::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::RightButton)
    {
        // Id associated to the column
        _currentSection = this->logicalIndexAt(e->pos());
        _currentId = this->model()->headerData(_currentSection, Qt::Horizontal, Qt::UserRole).value<EltID>();

        // Possibly display the menu
        if (_currentId.typeElement == elementInstSmpl || _currentId.typeElement == elementPrstInst)
        {
            _muteAction->blockSignals(true);
            _muteAction->setChecked(SoundfontManager::getInstance()->get(_currentId, champ_mute).bValue > 0);
            _muteAction->blockSignals(false);
            _menu->exec(e->globalPos());
        }
    }
    else
        QHeaderView::mousePressEvent(e);
}

void TableHeaderView::mute(bool isMute)
{
    AttributeValue val;
    val.bValue = (isMute ? 1 : 0);
    SoundfontManager::getInstance()->set(_currentId, champ_mute, val);

    // Update the header
    this->model()->setHeaderData(_currentSection, Qt::Horizontal,
                                 isMute ? QVariant::fromValue(_muteIcon) : QVariant(), Qt::DecorationRole);
}

void TableHeaderView::activateSolo(bool unused)
{
    Q_UNUSED(unused)
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;

    // Current id is a division of a preset?
    if (_currentId.typeElement == elementPrstInst)
    {
        // First unmute all divisions of the targeted instrument
        val.bValue = false;

        EltID idInst(elementInst, _currentId.indexSf2);
        idInst.indexElt = sm->get(_currentId, champ_instrument).wValue;
        EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
        foreach (int subIndex, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = subIndex;
            sm->set(idInstSmpl, champ_mute, val);
        }

        // Unmute all preset divisions except current id
        EltID idPrstInst(elementPrstInst, _currentId.indexSf2, _currentId.indexElt);
        foreach (int subIndex, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            val.bValue = (subIndex != _currentId.indexElt2);
            sm->set(idPrstInst, champ_mute, val);
        }
    }
    else
    {
        // Unmute all instrument divisions except current id
        EltID idPrstInst(elementInstSmpl, _currentId.indexSf2, _currentId.indexElt);
        foreach (int subIndex, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            val.bValue = (subIndex != _currentId.indexElt2);
            sm->set(idPrstInst, champ_mute, val);
        }
    }

    // Update all decorations
    for (int i = 0; i < this->model()->columnCount(); i++)
    {
        bool isMute = (i != _currentSection && i != 0);
        this->model()->setHeaderData(i, Qt::Horizontal,
                                     isMute ? QVariant::fromValue(_muteIcon) : QVariant(), Qt::DecorationRole);
    }
}

void TableHeaderView::unmuteAll(bool unused)
{
    Q_UNUSED(unused)

    // Unmute all divisions of all instruments
    SoundfontManager * sm = SoundfontManager::getInstance();
    AttributeValue val;
    val.bValue = 0;
    EltID idInst(elementInst, _currentId.indexSf2);
    foreach (int index, sm->getSiblings(idInst))
    {
        idInst.indexElt = index;
        EltID idInstSmpl(elementInstSmpl, idInst.indexSf2, idInst.indexElt);
        foreach (int subIndex, sm->getSiblings(idInstSmpl))
        {
            idInstSmpl.indexElt2 = subIndex;
            sm->set(idInstSmpl, champ_mute, val);
        }
    }

    // Unmute all divisions of all presets
    EltID idPrst(elementPrst, _currentId.indexSf2);
    foreach (int index, sm->getSiblings(idPrst))
    {
        idPrst.indexElt = index;
        EltID idPrstInst(elementPrstInst, idPrst.indexSf2, idPrst.indexElt);
        foreach (int subIndex, sm->getSiblings(idPrstInst))
        {
            idPrstInst.indexElt2 = subIndex;
            sm->set(idPrstInst, champ_mute, val);
        }
    }

    // Remove all decorations
    for (int i = 0; i < this->model()->columnCount(); i++)
        this->model()->setHeaderData(i, Qt::Horizontal, QVariant(), Qt::DecorationRole);
}

QSize TableHeaderView::sizeHint() const
{
    // Override the height with a custom value
    QSize size = QHeaderView::sizeHint();
    size.setHeight(_height);
    return size;
}

void TableHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    // Get the text and icon to display
    QString text = this->model()->headerData(logicalIndex, this->orientation(), Qt::DisplayRole).toString();
    QPixmap icon = this->model()->headerData(logicalIndex, this->orientation(), Qt::DecorationRole).value<QPixmap>();

    // Icon and text rect
    QRect iconRect = icon.rect();
    QRect textRect = rect;
    textRect.setWidth(textRect.width() - 2 * MARGIN);
    if (iconRect.width() > 0)
        textRect.setWidth(textRect.width() - iconRect.width() - MARGIN);
    textRect.translate(MARGIN, 0);

    // Adapt the text
    QString adaptedText = text;
    int lengthLine1 = text.length();
    QFontMetrics fm(this->font());
    while (fm.width(text.left(lengthLine1)) > textRect.width() && lengthLine1 > 0)
        lengthLine1--;
    if (lengthLine1 < text.length())
        adaptedText = text.left(lengthLine1) + "\n" + fm.elidedText(text.mid(lengthLine1), Qt::ElideRight, textRect.width());

    // First draw the cell without text or icon for the background and border
    this->model()->setHeaderData(logicalIndex, this->orientation(), "", Qt::DisplayRole);
    this->model()->setHeaderData(logicalIndex, this->orientation(), QVariant(), Qt::DecorationRole);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    this->model()->setHeaderData(logicalIndex, this->orientation(), text, Qt::DisplayRole);
    this->model()->setHeaderData(logicalIndex, this->orientation(), icon, Qt::DecorationRole);

    // Then draw the text
    QVariant foregroundBrush = model()->headerData(logicalIndex, this->orientation(), Qt::ForegroundRole);
    if (foregroundBrush.canConvert<QBrush>())
        painter->setPen(foregroundBrush.value<QBrush>().color());
    painter->setClipRect(rect);
    painter->drawText(textRect, Qt::AlignCenter, adaptedText);

    // Finally draw the icon on the right and vertically centered
    int iconHeight = iconRect.height();
    int headerHeight = rect.height();
    int offsetY = iconHeight < headerHeight ? (headerHeight - iconHeight) / 2 : 0;
    painter->setClipRect(rect);
    painter->drawPixmap(textRect.right() + MARGIN, rect.top() + offsetY, iconRect.width(), iconRect.height(), icon);
}
