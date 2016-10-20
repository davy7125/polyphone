/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2016 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://polyphone-soundfonts.com                      **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QList>
#include <QColor>
#include <QComboBox>
#include <QMap>

class ThemeManager: QObject
{
public:
    /// Color type, each type being used at specific locations in the interface
    enum ColorType
    {
        WINDOW_BACKGROUND,
        WINDOW_TEXT,
        BUTTON_BACKGROUND,
        BUTTON_TEXT,
        LIST_BACKGROUND,
        LIST_ALTERNATIVE_BACKGROUND,
        LIST_TEXT,
        HIGHLIGHTED_BACKGROUND,
        HIGHLIGHTED_TEXT
    };

    /// Get an instance
    static ThemeManager * getInstance()
    {
        if (_instance == NULL)
            _instance = new ThemeManager();
        return _instance;
    }

    /// Destroy the singleton
    void kill()
    {
        delete _instance;
        _instance = NULL;
    }

    /// Fill a combobox with all possible theme names with id
    void populateCombobox(QComboBox * combobox);

    /// Select the theme corresponding to the current colors
    void selectIndex(QComboBox * combobox);

    /// Get a color from the current theme
    QColor getColor(ColorType type);

    /// Change a color
    void setColor(ColorType type, QColor color);

    /// Apply a theme
    void applyTheme(int id);

    /// Reset the color theme
    void resetTheme();

    /// Get the palette of the application
    QPalette getPalette();

    /// Return true if the background is darker than the text
    bool isDark(ColorType backgroundType, ColorType textType);

private:
    class ColorTheme
    {
    public:
        ColorTheme(QString name, int id) : _name(name), _id(id) {}
        QString getName() { return _name; }
        int getId() { return _id; }
        void setColor(ColorType colorType, QColor color) { _colors[colorType] = color; }
        QColor getColor(ColorType colorType) { return _colors.contains(colorType) ? _colors[colorType] : QColor(); }

    private:
        QString _name;
        int _id;
        QMap<ColorType, QColor> _colors;
    };

    ColorTheme getDefaultTheme();
    QList<ColorTheme> getThemes();
    QString getName(ColorType type);
    QPalette::ColorRole getColorRole(ColorType type);
    QColor disabled1(QColor color, QColor limit);
    QColor disabled2(QColor color, QColor limit);
    ThemeManager();
    static ThemeManager * _instance;
    QList<ColorTheme> _themes;

    QPalette _defaultPalette;
};

#endif // THEMEMANAGER_H
