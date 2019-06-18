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

#include "downloadprogressbutton.h"
#include "downloadprogresscell.h"
#include "contextmanager.h"
#include <QMenu>
#include <QWidgetAction>

DownloadProgressButton::DownloadProgressButton(QWidget * parent) : QToolButton(parent)
{
    // Icon colors
    _svgReplacements["progress_color"] = ContextManager::theme()->getColor(ThemeManager::HIGHLIGHTED_BACKGROUND).name();
    bool isDark = ContextManager::theme()->isDark(ThemeManager::WINDOW_BACKGROUND, ThemeManager::WINDOW_TEXT);
    _svgReplacements["arrow_color"] = ContextManager::theme()->getColor(isDark ? ThemeManager::WINDOW_TEXT : ThemeManager::WINDOW_BACKGROUND).name();

    // Menu
    _menu = new QMenu(this);
    _menu->setStyleSheet(QString("QMenu::separator {background: ") +
                         ThemeManager::mix(ContextManager::theme()->getColor(ThemeManager::LIST_TEXT),
                                           ContextManager::theme()->getColor(ThemeManager::LIST_BACKGROUND), 0.5).name() +
                         ";margin: 10px 45px; height: 1px}");
    _menu->addSeparator();
    QAction * clearDownloadsAction = _menu->addAction(trUtf8("Clear completed downloads"));
    connect(clearDownloadsAction, SIGNAL(triggered()), this, SLOT(clearCompletedDownloads()));
    this->setMenu(_menu);
}

DownloadProgressButton::~DownloadProgressButton()
{
    delete _menu;
}

void DownloadProgressButton::progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName)
{
    _mutex.lock();
    if (!_cells.contains(soundfontId))
    {
        DownloadProgressCell * cell = new DownloadProgressCell(soundfontId, soundfontName);
        _cells[soundfontId] = cell;
        connect(cell, SIGNAL(closeMenu()), this, SLOT(onCloseMenu()));
        QWidgetAction * wa = new QWidgetAction(_menu);
        wa->setDefaultWidget(cell);

        // Insert the cell
        QList<QAction*> currentActions = _menu->actions();
        _menu->insertAction(currentActions.at(currentActions.count() - 2), wa);
    }

    // Update the corresponding cell
    _cells[soundfontId]->progressChanged(percent, finalFileName);

    // Update the percentage
    this->updatePercent();
    _mutex.unlock();
}

void DownloadProgressButton::updatePercent()
{
    // Compute the percentage
    double percent = 0;
    foreach (int soundfontId, _cells.keys())
    {
        percent += (double)_cells[soundfontId]->getPercent() / _cells.count();
    }

    // Update the icon
    _svgReplacements["variable_max_1360"] = QString::number(percent >= 100 ? 0 : percent * 13.6);
    this->setIcon(ContextManager::theme()->getColoredSvg(":/icons/download_progress.svg", QSize(28, 28), _svgReplacements));
}

void DownloadProgressButton::downloadCanceled(int soundfontId)
{
    _mutex.lock();
    if (_cells.contains(soundfontId))
        _cells[soundfontId]->cancel();
    updatePercent();
    _mutex.unlock();
}

void DownloadProgressButton::clearCompletedDownloads()
{
    _mutex.lock();
    QList<int> soundfontIds = _cells.keys();
    for (int i = 0; i < soundfontIds.count(); i++)
    {
        int soundfontId = soundfontIds[i];
        if (_cells[soundfontId]->getPercent() == 100)
            removeCell(_cells.take(soundfontId));
    }
    updatePercent();
    if (_cells.empty())
        emit(cleared());

    _mutex.unlock();
}

void DownloadProgressButton::removeCell(DownloadProgressCell * cell)
{
    // Search the corresponding QWidgetAction
    QList<QAction*> actions = _menu->actions();
    for (int i = 0; i < actions.count() - 2; i++)
    {
        QWidgetAction * wa = (QWidgetAction*)actions[i];
        if (wa->defaultWidget() == cell)
        {
            _menu->removeAction(wa);
            delete cell;
            break;
        }
    }
}

void DownloadProgressButton::onCloseMenu()
{
    _menu->close();
}
