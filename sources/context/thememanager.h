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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QList>
#include <QColor>
#include <QComboBox>
#include <QMap>
class ConfManager;

class ThemeManager: QObject
{
    Q_OBJECT
    
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

    /// Fixed colors that depends on the background but that cannot be changed by the user
    enum FixedColorType
    {
        GREEN,
        RED,
        YELLOW,
        BLUE
    };

    enum ColorContext
    {
        NORMAL,
        HOVERED,
        DISABLED
    };

    ThemeManager(ConfManager * configuration);

    /// Fill a combobox with all possible theme names with id
    void populateCombobox(QComboBox * combobox);

    /// Select the theme corresponding to the current colors
    void selectIndex(QComboBox * combobox);

    /// Get a color from the current theme
    QColor getColor(ColorType type, ColorContext context = NORMAL);

    /// Get a fixed color depending on a background or explicitely mentioned light or dark
    QColor getFixedColor(FixedColorType type, ColorType background);
    QColor getFixedColor(FixedColorType type, bool darkBackground);

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

    /// Convenient function to color a svg file from the resources
    /// The color that will be renamed is "currentColor"
    QPixmap getColoredSvg(QString filename, QSize maxSize, ColorType currentColor);
    QPixmap getColoredSvg(QString filename, QSize maxSize, QMap<QString, QString> &replacement);

    /// Function to mix two colors
    static QColor mix(QColor color1, QColor color2, double fade);

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
    ColorType getAssociatedColorType(ColorType type);

    ConfManager * _configuration;
    QList<ColorTheme> _themes;
    QPalette _defaultPalette;
};

#endif // THEMEMANAGER_H
