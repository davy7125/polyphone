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

#ifndef SOUNDFONTCELLFULL_H
#define SOUNDFONTCELLFULL_H

#include <QWidget>
class SoundfontInformation;
class SoundfontFilter;

namespace Ui {
class SoundfontCellFull;
}

class SoundfontCellFull : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontCellFull(SoundfontInformation* soundfontInfo, QWidget *parent = nullptr);
    ~SoundfontCellFull();
    void setActive(bool isActive);
    bool hasHeightForWidth() const override;
    int heightForWidth(int width) const override;
    int getSoundfontId() { return _soundfontId; }

signals:
    void itemClicked(SoundfontFilter * filter);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void on_labelAuthor_linkActivated(const QString &link);

private:
    class IconContainer
    {
    public:
        IconContainer();

        QPixmap _downloadIcon;
        QPixmap _downloadIconSelected;
        QPixmap _commentIcon;
        QPixmap _commentIconSelected;
        QPixmap _userIcon;
        QPixmap _userIconSelected;
        QPixmap _dateIcon;
        QPixmap _dateIconSelected;
        QPixmap _copyrightIcon;
        QPixmap _copyrightIconSelected;
    };

    void initIcons();

    QString _title;
    Ui::SoundfontCellFull *ui;
    QString _normalStyleSheet;
    QString _activeStyleSheet;
    bool _active;
    QString _authorTextNoColor;
    int _soundfontId;
    static IconContainer * s_icons;
};

#endif // SOUNDFONTCELLFULL_H
