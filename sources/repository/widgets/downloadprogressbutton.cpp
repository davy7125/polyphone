/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2018 Davy Triponney                                **
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
    this->setMenu(_menu);
}

DownloadProgressButton::~DownloadProgressButton()
{
    delete _menu;
}

void DownloadProgressButton::progressChanged(int percent, int soundfontId, QString soundfontName, QString finalFileName)
{
    if (!_downloadStatus.contains(soundfontName))
    {
        _downloadStatus[soundfontName] = QPair<int, QString>(percent, finalFileName);
        DownloadProgressCell * cell = new DownloadProgressCell(soundfontName, soundfontId);
        _cells[soundfontName] = cell;
        QWidgetAction * wa = new QWidgetAction(_menu);
        wa->setDefaultWidget(cell);
        _menu->addAction(wa);
    }
    else
    {
        _downloadStatus[soundfontName].first = percent;
        _downloadStatus[soundfontName].second = finalFileName;
    }

    // Update the corresponding cell
    _cells[soundfontName]->progressChanged(percent, finalFileName);

    // Update the percentage
    this->updatePercent();
}

void DownloadProgressButton::updatePercent()
{
    // Compute the percentage
    double percent = 0;
    foreach (QString soundfontName, _downloadStatus.keys())
    {
        percent += (double)_downloadStatus[soundfontName].first / _downloadStatus.count();
    }

    // Update the icon
    _svgReplacements["variable_max_1360"] = QString::number(percent >= 100 ? 0 : percent * 13.6);
    this->setIcon(ContextManager::theme()->getColoredSvg(":/icons/download_progress.svg", QSize(28, 28), _svgReplacements));
}
