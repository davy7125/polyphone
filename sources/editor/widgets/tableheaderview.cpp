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

#include "tableheaderview.h"
#include "contextmanager.h"
#include <QMouseEvent>
#include <QMenu>
#include "soundfontmanager.h"
#include "solomanager.h"
#include <QLabel>
#include <QPainter>
#include <QWidgetAction>

const int TableHeaderView::MARGIN = 2;

TableHeaderView::TableHeaderView(QWidget *parent) : QHeaderView(Qt::Horizontal, parent),
    _menuWidth(0),
    _solo(SoundfontManager::getInstance()->solo())
{
    this->setSectionsClickable(true);
    this->setHighlightSections(true);
    this->setDefaultSectionSize(80);

    // Icons
    _muteIcon = ContextManager::theme()->getColoredSvg(":/icons/volume-mute.svg", QSize(12, 12), ThemeManager::HIGHLIGHTED_BACKGROUND);
    _menuCheckIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::LIST_TEXT), QIcon::Normal);
    _menuCheckIcon.addPixmap(ContextManager::theme()->getColoredSvg(":/icons/check.svg", QSize(24, 24), ThemeManager::HIGHLIGHTED_TEXT), QIcon::Active);

    // Menu with actions
    _menu = new QMenu(this);
    _menu->setStyleSheet(ContextManager::theme()->getMenuTheme());
    QFontMetrics fm(_menu->font());

    _muteAction = _menu->addAction(tr("mute"));
    connect(_muteAction, SIGNAL(triggered(bool)), this, SLOT(mute(bool)));
    _menuWidth = qMax(_menuWidth, fm.horizontalAdvance(_muteAction->text()));

    _soloAction = _menu->addAction(tr("solo"));
    connect(_soloAction, SIGNAL(triggered(bool)), this, SLOT(activateSolo(bool)));
    _menuWidth = qMax(_menuWidth, fm.horizontalAdvance(_soloAction->text()));

    _clearAction = _menu->addAction(tr("unmute all"));
    connect(_clearAction, SIGNAL(triggered(bool)), this, SLOT(unmuteAll(bool)));
    _menuWidth = qMax(_menuWidth, fm.horizontalAdvance(_clearAction->text()));

    _menu->addSeparator();
    _soloSelectionAction = _menu->addAction(tr("solo on selection"));
    connect(_soloSelectionAction, SIGNAL(triggered(bool)), this, SLOT(soloOnSelection(bool)));
    _menuWidth = qMax(_menuWidth, fm.horizontalAdvance(_soloSelectionAction->text()));

    // Height of the header
    QFontMetrics fm2(this->font());
    _height = fm2.height() * 2 + 8;
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
            _soloSelectionAction->blockSignals(true);
            if (_solo->isSoloOnSelectionEnabled(_currentId.indexSf2))
            {
                _muteAction->setEnabled(false);
                _soloAction->setEnabled(false);
                _clearAction->setEnabled(false);
                _soloSelectionAction->setIcon(_menuCheckIcon);
                _muteAction->setIcon(QIcon());
                _menu->setMinimumWidth(_menuWidth + 32 + 20);
            }
            else
            {
                _muteAction->setEnabled(true);
                _soloAction->setEnabled(true);
                _clearAction->setEnabled(true);
                _soloSelectionAction->setIcon(QIcon());
                bool isMute = _solo->isMute(_currentId);
                _muteAction->setIcon(isMute ? _menuCheckIcon : QIcon());
                _menu->setMinimumWidth(_menuWidth + 32 + (isMute ? 20 : 0));
            }
            _soloSelectionAction->blockSignals(false);
            _muteAction->blockSignals(false);
#if QT_VERSION < 0x060000
            _menu->exec(e->globalPos());
#else
            _menu->exec(e->globalPosition().toPoint());
#endif
        }
    }
    else
        QHeaderView::mousePressEvent(e);
}

void TableHeaderView::mute(bool unused)
{
    Q_UNUSED(unused)
    _solo->setMute(_currentId, !_solo->isMute(_currentId));
    this->viewport()->update();
}

void TableHeaderView::activateSolo(bool unused)
{
    Q_UNUSED(unused)
    _solo->activateSolo(_currentId);
    this->viewport()->update();
}

void TableHeaderView::unmuteAll(bool unused)
{
    Q_UNUSED(unused)
    _solo->unmuteAll(_currentId.indexSf2);
    this->viewport()->update();
}

void TableHeaderView::soloOnSelection(bool unused)
{
    Q_UNUSED(unused)
    _solo->setSoloOnSelection(!_solo->isSoloOnSelectionEnabled(_currentId.indexSf2), _currentId.indexSf2);
    this->viewport()->update();
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
    EltID currentId = this->model()->headerData(logicalIndex, Qt::Horizontal, Qt::UserRole).value<EltID>();
    QPixmap icon = _solo->isMute(currentId) ? _muteIcon : QPixmap();

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
    while (fm.horizontalAdvance(text.left(lengthLine1)) > textRect.width() - 2 * MARGIN && lengthLine1 > 0)
        lengthLine1--;
    if (lengthLine1 < text.length())
        adaptedText = text.left(lengthLine1) + "\n" + fm.elidedText(text.mid(lengthLine1), Qt::ElideRight, textRect.width());

    // First draw the cell without text for the background and border
    this->model()->blockSignals(true);
    this->model()->setHeaderData(logicalIndex, this->orientation(), "", Qt::DisplayRole);
    QHeaderView::paintSection(painter, rect, logicalIndex);
    this->model()->setHeaderData(logicalIndex, this->orientation(), text, Qt::DisplayRole);
    this->model()->blockSignals(false);

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
    painter->drawPixmap(textRect.right() + MARGIN, rect.top() + offsetY, iconRect.width(), iconRect.height(), icon);
}
