/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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

#include "thememanager.h"
#include "confmanager.h"
#include <QApplication>
#include <QFile>
#include <QSvgRenderer>
#include <QPainter>

ThemeManager::ThemeManager(ConfManager * configuration) :
    _configuration(configuration)
{
    // Custom palette enabled?
    QString loadedStyle = _configuration->getValue(ConfManager::SECTION_DISPLAY, "style", "Fusion").toString();
    _customPaletteEnabled = loadedStyle == "Windows" || loadedStyle == "Fusion";

    // Memorize the default theme
    _defaultPalette = QApplication::palette();

    // Check that the alternate base is different
    QColor colorAlternateBase = _defaultPalette.color(QPalette::AlternateBase);
    QColor colorBase = _defaultPalette.color(QPalette::Base);
    if (colorAlternateBase == colorBase)
        _defaultPalette.setColor(QPalette::AlternateBase, mix(colorBase, _defaultPalette.color(QPalette::Text), 0.05));

    // Fix some colors for Mac OS
    if (loadedStyle == "macOS")
    {
        QColor colorTmp = _customPalette.color(QPalette::Window);
        colorTmp.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), (int)(0.7 * colorTmp.value()));
        _defaultPalette.setColor(QPalette::Dark, colorTmp);

        colorTmp = mix(_defaultPalette.color(QPalette::Base), _defaultPalette.color(QPalette::Text), 0.05);
        _defaultPalette.setColor(QPalette::AlternateBase, colorTmp);

        colorTmp = mix(_defaultPalette.color(QPalette::HighlightedText), _defaultPalette.color(QPalette::Highlight), 0.7);
        _defaultPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, colorTmp);
    }

    // Load themes
    _themes << getDefaultTheme() << getThemes();

    // Create the current palette
    computePalette();
}

void ThemeManager::populateCombobox(QComboBox * combobox)
{
    // Prepared themes
    for (int i = 1; i < _themes.count(); i++)
    {
        ColorTheme theme = _themes[i];
        combobox->addItem(theme.getName(), theme.getId());
    }
    combobox->model()->sort(0);

    // Default theme
    combobox->insertItem(0, _themes[0].getName(), _themes[0].getId());

    // Custom
    combobox->addItem(tr("Custom"), -1);

    // Select the current theme
    selectIndex(combobox);
}

void ThemeManager::selectIndex(QComboBox *combobox)
{
    QString configuredStyle = _configuration->getValue(ConfManager::SECTION_DISPLAY, "style", "Fusion").toString();
    bool customStyleDisabled = (configuredStyle != "Windows" && configuredStyle != "Fusion");
    if (customStyleDisabled)
    {
        combobox->setCurrentIndex(0);
        return;
    }

    int themeId = -1;
    for (int i = 0; i < _themes.count(); i++)
    {
        if (_themes[i].getColor(WINDOW_BACKGROUND) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(WINDOW_BACKGROUND), QColor()).value<QColor>() &&
                _themes[i].getColor(WINDOW_TEXT) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(WINDOW_TEXT), QColor()).value<QColor>() &&
                _themes[i].getColor(BUTTON_BACKGROUND) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(BUTTON_BACKGROUND), QColor()).value<QColor>() &&
                _themes[i].getColor(BUTTON_TEXT) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(BUTTON_TEXT), QColor()).value<QColor>() &&
                _themes[i].getColor(LIST_BACKGROUND) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(LIST_BACKGROUND), QColor()).value<QColor>() &&
                _themes[i].getColor(LIST_ALTERNATIVE_BACKGROUND) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(LIST_ALTERNATIVE_BACKGROUND), QColor()).value<QColor>() &&
                _themes[i].getColor(LIST_TEXT) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(LIST_TEXT), QColor()).value<QColor>() &&
                _themes[i].getColor(HIGHLIGHTED_BACKGROUND) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(HIGHLIGHTED_BACKGROUND), QColor()).value<QColor>() &&
                _themes[i].getColor(HIGHLIGHTED_TEXT) == _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(HIGHLIGHTED_TEXT), QColor()).value<QColor>())
        {
            themeId = _themes[i].getId();
            break;
        }
    }

    if (themeId == -1)
    {
        // Custom theme
        combobox->blockSignals(true);
        combobox->setCurrentIndex(combobox->count() - 1);
        combobox->blockSignals(false);
    }
    else
    {
        for (int i = 0; i < combobox->count() - 1; i++)
        {
            if (combobox->itemData(i).toInt() == themeId)
            {
                combobox->blockSignals(true);
                combobox->setCurrentIndex(i);
                combobox->blockSignals(false);
                break;
            }
        }
    }
}

QColor ThemeManager::getColor(ThemeManager::ColorType type, ColorContext context, bool fromSettingFile)
{
    // Color role
    QPalette::ColorRole role = getColorRole(type);

    if (fromSettingFile)
    {
        // Read the color directly in the setting file
        return _configuration->getValue(
                    ConfManager::SECTION_DISPLAY, getName(type),
                    _defaultPalette.color(role)).value<QColor>();
    }

    // Take the right palette and get the color
    QPalette currentPalette = _customPaletteEnabled ? _customPalette : _defaultPalette;
    QColor color = currentPalette.color(context == DISABLED ? QPalette::Disabled : QPalette::Active, role);

    // Apply a change if hovered
    if (context == HOVERED)
        color = mix(color, currentPalette.color(getColorRole(getAssociatedColorType(type))), 0.2);

    // If AlternateBase, check that it's different from Base
    if (role == QPalette::AlternateBase)
    {
        QColor baseColor = currentPalette.color(QPalette::Base);
        if (color == baseColor)
            color = mix(color, currentPalette.color(QPalette::Text), 0.05);
    }

    // If disabled, check it is different from enabled
    if (context == DISABLED)
    {
        QColor enabledColor = currentPalette.color(QPalette::Active, role);
        if (color == enabledColor)
            color = mix(color, currentPalette.color(getColorRole(getAssociatedColorType(type))), 0.7);
    }

    return color;
}

QColor ThemeManager::getFixedColor(FixedColorType type, ColorType background)
{
    return getFixedColor(type, this->isDark(background, getAssociatedColorType(background)));
}

QColor ThemeManager::getFixedColor(FixedColorType type, bool darkBackground)
{
    QColor color;

    switch (type)
    {
    case FixedColorType::RED:
        color = darkBackground ? QColor(255, 80, 80) : QColor(210, 50, 50);
        break;
    case FixedColorType::GREEN:
        color = darkBackground ? QColor(0, 180, 40) : QColor(0, 160, 60);
        break;
    case FixedColorType::YELLOW:
        color = darkBackground ? QColor(250, 250, 130) : QColor(200, 200, 120);
        break;
    case FixedColorType::BLUE:
        color = darkBackground ? QColor(50, 150, 255) : QColor(50, 100, 180);
        break;
    }

    return color;
}

void ThemeManager::setColor(ColorType type, QColor color)
{
    _configuration->setValue(ConfManager::SECTION_DISPLAY, getName(type), color);
}

void ThemeManager::resetTheme()
{
    // Restore default colors
    setColor(WINDOW_BACKGROUND, _defaultPalette.color(getColorRole(WINDOW_BACKGROUND)));
    setColor(WINDOW_TEXT, _defaultPalette.color(getColorRole(WINDOW_TEXT)));
    setColor(BUTTON_BACKGROUND, _defaultPalette.color(getColorRole(BUTTON_BACKGROUND)));
    setColor(BUTTON_TEXT, _defaultPalette.color(getColorRole(BUTTON_TEXT)));
    setColor(LIST_BACKGROUND, _defaultPalette.color(getColorRole(LIST_BACKGROUND)));
    setColor(LIST_ALTERNATIVE_BACKGROUND, _defaultPalette.color(getColorRole(LIST_ALTERNATIVE_BACKGROUND)));
    setColor(LIST_TEXT, _defaultPalette.color(getColorRole(LIST_TEXT)));
    setColor(HIGHLIGHTED_BACKGROUND, _defaultPalette.color(getColorRole(HIGHLIGHTED_BACKGROUND)));
    setColor(HIGHLIGHTED_TEXT, _defaultPalette.color(getColorRole(HIGHLIGHTED_TEXT)));
}

void ThemeManager::applyTheme(int id)
{
    ColorTheme * theme = nullptr;
    for (int i = 0; i < _themes.count(); i++)
    {
        if (_themes[i].getId() == id)
        {
            theme = &_themes[i];
            break;
        }
    }

    if (theme != nullptr)
    {
        setColor(WINDOW_BACKGROUND, theme->getColor(WINDOW_BACKGROUND));
        setColor(WINDOW_TEXT, theme->getColor(WINDOW_TEXT));
        setColor(BUTTON_BACKGROUND, theme->getColor(BUTTON_BACKGROUND));
        setColor(BUTTON_TEXT, theme->getColor(BUTTON_TEXT));
        setColor(LIST_BACKGROUND, theme->getColor(LIST_BACKGROUND));
        setColor(LIST_ALTERNATIVE_BACKGROUND, theme->getColor(LIST_ALTERNATIVE_BACKGROUND));
        setColor(LIST_TEXT, theme->getColor(LIST_TEXT));
        setColor(HIGHLIGHTED_BACKGROUND, theme->getColor(HIGHLIGHTED_BACKGROUND));
        setColor(HIGHLIGHTED_TEXT, theme->getColor(HIGHLIGHTED_TEXT));
    }
}

QString ThemeManager::getName(ColorType type)
{
    QString ret = "";

    switch (type)
    {
    case WINDOW_BACKGROUND:
        ret = "window_background";
        break;
    case WINDOW_TEXT:
        ret = "window_text";
        break;
    case BUTTON_BACKGROUND:
        ret = "button_background";
        break;
    case BUTTON_TEXT:
        ret = "button_text";
        break;
    case LIST_BACKGROUND:
        ret = "list_background";
        break;
    case LIST_ALTERNATIVE_BACKGROUND:
        ret = "list_alternative_background";
        break;
    case LIST_TEXT:
        ret = "list_text";
        break;
    case HIGHLIGHTED_BACKGROUND:
        ret = "highlighted_background";
        break;
    case HIGHLIGHTED_TEXT:
        ret = "highlighted_text";
        break;
    default:
        break;
    }

    return ret;
}

QPalette::ColorRole ThemeManager::getColorRole(ColorType type)
{
    QPalette::ColorRole ret = QPalette::NoRole;

    switch (type)
    {
    case WINDOW_BACKGROUND:
        ret = QPalette::Window;
        break;
    case WINDOW_TEXT:
        ret = QPalette::WindowText;
        break;
    case BUTTON_BACKGROUND:
        ret = QPalette::Button;
        break;
    case BUTTON_TEXT:
        ret = QPalette::ButtonText;
        break;
    case LIST_BACKGROUND:
        ret = QPalette::Base;
        break;
    case LIST_ALTERNATIVE_BACKGROUND:
        ret = QPalette::AlternateBase;
        break;
    case LIST_TEXT:
        ret = QPalette::Text;
        break;
    case HIGHLIGHTED_BACKGROUND:
        ret = QPalette::Highlight;
        break;
    case HIGHLIGHTED_TEXT:
        ret = QPalette::HighlightedText;
        break;
    case BORDER:
        ret = QPalette::Dark;
        break;
    case LINK:
        ret = QPalette::Link;
        break;
    }

    return ret;
}

bool ThemeManager::isDark(ColorType backgroundType, ColorType textType)
{
    QColor backgroundColor = getColor(backgroundType);
    int backgroundValue = qMax(qMax(backgroundColor.red(), backgroundColor.green()), backgroundColor.blue());

    QColor textColor = getColor(textType);
    int textValue = qMax(qMax(textColor.red(), textColor.green()), textColor.blue());

    return backgroundValue < textValue;
}

QColor ThemeManager::disabled1(QColor color, QColor limit)
{
    int m1 = (color.red() + color.green() + color.blue()) / 3;
    int m2 = (limit.red() + limit.green() + limit.blue()) / 3;
    int newVal = qMax(0, (m1 + 2 * m2) / 3);
    return QColor(newVal, newVal, newVal);
}

QColor ThemeManager::disabled2(QColor color, QColor limit)
{
    int m1 = (color.red() + color.green() + color.blue()) / 3;
    int m2 = (limit.red() + limit.green() + limit.blue()) / 3;
    int newVal = qMin(255, (7 * m1 + m2) / 8);
    return QColor(newVal, newVal, newVal);
}

void ThemeManager::computePalette()
{
    _customPalette = _defaultPalette;

    // Window
    _customPalette.setColor(QPalette::Window, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(WINDOW_BACKGROUND),
                                _defaultPalette.color(getColorRole(WINDOW_BACKGROUND))).value<QColor>());
    _customPalette.setColor(QPalette::WindowText, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(WINDOW_TEXT),
                                _defaultPalette.color(getColorRole(WINDOW_TEXT))).value<QColor>());
    _customPalette.setColor(QPalette::BrightText, isDark(WINDOW_BACKGROUND, WINDOW_TEXT) ?
                                _customPalette.color(QPalette::WindowText).lighter() :
                                _customPalette.color(QPalette::WindowText).darker());

    // Button / Tooltip
    _customPalette.setColor(QPalette::Button, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(BUTTON_BACKGROUND),
                                _defaultPalette.color(getColorRole(BUTTON_BACKGROUND))).value<QColor>());
    _customPalette.setColor(QPalette::ButtonText, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(BUTTON_TEXT),
                                _defaultPalette.color(getColorRole(BUTTON_TEXT))).value<QColor>());
    _customPalette.setColor(QPalette::ToolTipBase, _customPalette.color(QPalette::Button));
    _customPalette.setColor(QPalette::ToolTipText, _customPalette.color(QPalette::ButtonText));
    QColor colorPlaceHolder = mix(_customPalette.color(QPalette::Button), _customPalette.color(QPalette::ButtonText), 0.3);
    _customPalette.setColor(QPalette::PlaceholderText, colorPlaceHolder);

    // List
    _customPalette.setColor(QPalette::Base, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(LIST_BACKGROUND),
                                _defaultPalette.color(getColorRole(LIST_BACKGROUND))).value<QColor>());
    _customPalette.setColor(QPalette::AlternateBase, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(LIST_ALTERNATIVE_BACKGROUND),
                                _defaultPalette.color(getColorRole(LIST_ALTERNATIVE_BACKGROUND))).value<QColor>());
    _customPalette.setColor(QPalette::Text, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(LIST_TEXT),
                                _defaultPalette.color(getColorRole(LIST_TEXT))).value<QColor>());
    _customPalette.setColor(QPalette::Highlight, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(HIGHLIGHTED_BACKGROUND),
                                _defaultPalette.color(getColorRole(HIGHLIGHTED_BACKGROUND))).value<QColor>());
    _customPalette.setColor(QPalette::HighlightedText, _configuration->getValue(
                                ConfManager::SECTION_DISPLAY, getName(HIGHLIGHTED_TEXT),
                                _defaultPalette.color(getColorRole(HIGHLIGHTED_TEXT))).value<QColor>());

    // Automatically complete colors
    QColor colorTmp = _customPalette.color(QPalette::Window);
    QColor colorLight = colorTmp;
    colorLight.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), qMin(255, (int)(1.3 * colorTmp.value())));
    _customPalette.setColor(QPalette::Light, colorLight);

    QColor colorMidLight = colorTmp;
    colorMidLight.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), qMin(255, (int)(1.15 * colorTmp.value())));
    _customPalette.setColor(QPalette::Midlight, colorMidLight);

    QColor colorDark = colorTmp;
    colorDark.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), (int)(0.7 * colorTmp.value()));
    _customPalette.setColor(QPalette::Dark, colorDark);

    QColor colorMid = colorTmp;
    colorMid.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), qMin(255, (int)(0.4 * colorTmp.value())));
    _customPalette.setColor(QPalette::Mid, colorMid);

    QColor colorShadow = colorTmp;
    colorShadow.setHsv(colorTmp.hue(), (int)(0.75 * colorTmp.saturation()), qMin(255, (int)(0.2 * colorTmp.value())));
    _customPalette.setColor(QPalette::Shadow, colorShadow);

    // Disabled colors
    _customPalette.setColor(QPalette::Disabled, QPalette::WindowText,
                            disabled1(_customPalette.color(QPalette::WindowText), _customPalette.color(QPalette::Window)));
    _customPalette.setColor(QPalette::Disabled, QPalette::Window,
                            disabled2(_customPalette.color(QPalette::Window), _customPalette.color(QPalette::WindowText)));
    _customPalette.setColor(QPalette::Disabled, QPalette::ButtonText,
                            disabled1(_customPalette.color(QPalette::ButtonText), _customPalette.color(QPalette::Button)));
    _customPalette.setColor(QPalette::Disabled, QPalette::Button,
                            disabled2(_customPalette.color(QPalette::Button), _customPalette.color(QPalette::ButtonText)));
    _customPalette.setColor(QPalette::Disabled, QPalette::Text,
                            disabled1(_customPalette.color(QPalette::Text), _customPalette.color(QPalette::Base)));
    _customPalette.setColor(QPalette::Disabled, QPalette::Base,
                            disabled2(_customPalette.color(QPalette::Base), _customPalette.color(QPalette::Text)));
    _customPalette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                            disabled1(_customPalette.color(QPalette::HighlightedText), _customPalette.color(QPalette::Highlight)));
    _customPalette.setColor(QPalette::Disabled, QPalette::Highlight,
                            disabled2(_customPalette.color(QPalette::Highlight), _customPalette.color(QPalette::HighlightedText)));

    // Hyperlink
    if (isDark(LIST_BACKGROUND, LIST_TEXT))
    {
        _customPalette.setColor(QPalette::Link, QColor(100, 150, 255));
        _customPalette.setColor(QPalette::LinkVisited, QColor(120, 20, 220));
    }
    else
    {
        _customPalette.setColor(QPalette::Link, QColor(0, 0, 255));
        _customPalette.setColor(QPalette::LinkVisited, QColor(80, 0, 140));
    }
}

ThemeManager::ColorTheme ThemeManager::getDefaultTheme()
{
    ColorTheme theme = ColorTheme(tr("Default"), 0);

    theme.setColor(WINDOW_BACKGROUND, _defaultPalette.color(getColorRole(WINDOW_BACKGROUND)));
    theme.setColor(WINDOW_TEXT, _defaultPalette.color(getColorRole(WINDOW_TEXT)));
    theme.setColor(BUTTON_BACKGROUND, _defaultPalette.color(getColorRole(BUTTON_BACKGROUND)));
    theme.setColor(BUTTON_TEXT, _defaultPalette.color(getColorRole(BUTTON_TEXT)));
    theme.setColor(LIST_BACKGROUND, _defaultPalette.color(getColorRole(LIST_BACKGROUND)));
    theme.setColor(LIST_ALTERNATIVE_BACKGROUND, _defaultPalette.color(getColorRole(LIST_ALTERNATIVE_BACKGROUND)));
    theme.setColor(LIST_TEXT, _defaultPalette.color(getColorRole(LIST_TEXT)));
    theme.setColor(HIGHLIGHTED_BACKGROUND, _defaultPalette.color(getColorRole(HIGHLIGHTED_BACKGROUND)));
    theme.setColor(HIGHLIGHTED_TEXT, _defaultPalette.color(getColorRole(HIGHLIGHTED_TEXT)));

    return theme;
}

QList<ThemeManager::ColorTheme> ThemeManager::getThemes()
{
    QList<ColorTheme> themes;

    // Gnome theme
    ColorTheme theme1 = ColorTheme(tr("Gnome"), 1);
    theme1.setColor(WINDOW_BACKGROUND, QColor(237, 237, 237));
    theme1.setColor(WINDOW_TEXT, QColor(50, 50, 50));
    theme1.setColor(BUTTON_BACKGROUND, QColor(220, 220, 220));
    theme1.setColor(BUTTON_TEXT, QColor(0, 0, 0));
    theme1.setColor(LIST_BACKGROUND, QColor(255, 255, 255));
    theme1.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(210, 230, 255));
    theme1.setColor(LIST_TEXT, QColor(50, 50, 50));
    theme1.setColor(HIGHLIGHTED_BACKGROUND, QColor(74, 144, 217));
    theme1.setColor(HIGHLIGHTED_TEXT, QColor(255, 255, 255));
    themes << theme1;

    // Dark blue theme
    ColorTheme theme2 = ColorTheme(tr("Dark, blue"), 2);
    theme2.setColor(WINDOW_BACKGROUND, QColor(50, 50, 50));
    theme2.setColor(WINDOW_TEXT, QColor(255, 255, 255));
    theme2.setColor(BUTTON_BACKGROUND, QColor(73, 73, 73));
    theme2.setColor(BUTTON_TEXT, QColor(255, 255, 255));
    theme2.setColor(LIST_BACKGROUND, QColor(25, 25, 25));
    theme2.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(60, 60, 83));
    theme2.setColor(LIST_TEXT, QColor(220, 220, 220));
    theme2.setColor(HIGHLIGHTED_BACKGROUND, QColor(82, 130, 180));
    theme2.setColor(HIGHLIGHTED_TEXT, QColor(0, 0, 0));
    themes << theme2;

    // Dark orange theme
    ColorTheme theme3 = ColorTheme(tr("Dark, orange"), 3);
    theme3.setColor(WINDOW_BACKGROUND, QColor(50, 50, 50));
    theme3.setColor(WINDOW_TEXT, QColor(255, 255, 255));
    theme3.setColor(BUTTON_BACKGROUND, QColor(73, 73, 73));
    theme3.setColor(BUTTON_TEXT, QColor(255, 255, 255));
    theme3.setColor(LIST_BACKGROUND, QColor(25, 25, 25));
    theme3.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(118, 67, 0));
    theme3.setColor(LIST_TEXT, QColor(220, 220, 220));
    theme3.setColor(HIGHLIGHTED_BACKGROUND, QColor(255, 130, 52));
    theme3.setColor(HIGHLIGHTED_TEXT, QColor(0, 0, 0));
    themes << theme3;

    // Dark green theme
    ColorTheme theme4 = ColorTheme(tr("Dark, green"), 4);
    theme4.setColor(WINDOW_BACKGROUND, QColor(50, 50, 50));
    theme4.setColor(WINDOW_TEXT, QColor(255, 255, 255));
    theme4.setColor(BUTTON_BACKGROUND, QColor(73, 73, 73));
    theme4.setColor(BUTTON_TEXT, QColor(189, 255, 187));
    theme4.setColor(LIST_BACKGROUND, QColor(25, 25, 25));
    theme4.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(19, 104, 61));
    theme4.setColor(LIST_TEXT, QColor(220, 220, 220));
    theme4.setColor(HIGHLIGHTED_BACKGROUND, QColor(32, 205, 80));
    theme4.setColor(HIGHLIGHTED_TEXT, QColor(0, 0, 0));
    themes << theme4;

    // Dark yellow theme
    ColorTheme theme5 = ColorTheme(tr("Dark, yellow"), 5);
    theme5.setColor(WINDOW_BACKGROUND, QColor(50, 50, 50));
    theme5.setColor(WINDOW_TEXT, QColor(255, 255, 245));
    theme5.setColor(BUTTON_BACKGROUND, QColor(73, 73, 73));
    theme5.setColor(BUTTON_TEXT, QColor(255, 255, 245));
    theme5.setColor(LIST_BACKGROUND, QColor(25, 25, 25));
    theme5.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(50, 50, 50));
    theme5.setColor(LIST_TEXT, QColor(225, 225, 220));
    theme5.setColor(HIGHLIGHTED_BACKGROUND, QColor(235, 235, 50));
    theme5.setColor(HIGHLIGHTED_TEXT, QColor(5, 5, 0));
    themes << theme5;

    // Aqua theme
    ColorTheme theme6 = ColorTheme(tr("Aqua"), 6);
    theme6.setColor(WINDOW_BACKGROUND, QColor(213, 209, 235));
    theme6.setColor(WINDOW_TEXT, QColor(0, 0, 0));
    theme6.setColor(BUTTON_BACKGROUND, QColor(158, 151, 213));
    theme6.setColor(BUTTON_TEXT, QColor(0, 0, 0));
    theme6.setColor(LIST_BACKGROUND, QColor(255, 255, 255));
    theme6.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(178, 254, 207));
    theme6.setColor(LIST_TEXT, QColor(0, 0, 0));
    theme6.setColor(HIGHLIGHTED_BACKGROUND, QColor(26, 99, 173));
    theme6.setColor(HIGHLIGHTED_TEXT, QColor(255, 255, 255));
    themes << theme6;

    // Spring theme
    ColorTheme theme7 = ColorTheme(tr("Spring"), 7);
    theme7.setColor(WINDOW_BACKGROUND, QColor(206, 254, 217));
    theme7.setColor(WINDOW_TEXT, QColor(50, 50, 50));
    theme7.setColor(BUTTON_BACKGROUND, QColor(99, 224, 138));
    theme7.setColor(BUTTON_TEXT, QColor(60, 60, 60));
    theme7.setColor(LIST_BACKGROUND, QColor(255, 255, 255));
    theme7.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(254, 254, 155));
    theme7.setColor(LIST_TEXT, QColor(20, 20, 20));
    theme7.setColor(HIGHLIGHTED_BACKGROUND, QColor(32, 187, 94));
    theme7.setColor(HIGHLIGHTED_TEXT, QColor(255, 255, 255));
    themes << theme7;

    // Windows 10 theme
    ColorTheme theme8 = ColorTheme(tr("Windows 10"), 8);
    theme8.setColor(WINDOW_BACKGROUND, QColor(255, 255, 255));
    theme8.setColor(WINDOW_TEXT, QColor(60, 60, 60));
    theme8.setColor(BUTTON_BACKGROUND, QColor(240, 240, 240));
    theme8.setColor(BUTTON_TEXT, QColor(0, 0, 0));
    theme8.setColor(LIST_BACKGROUND, QColor(245, 246, 247));
    theme8.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(248, 248, 210));
    theme8.setColor(LIST_TEXT, QColor(0, 0, 0));
    theme8.setColor(HIGHLIGHTED_BACKGROUND, QColor(25, 121, 202));
    theme8.setColor(HIGHLIGHTED_TEXT, QColor(255, 255, 255));
    themes << theme8;

    // Ubuntu theme
    ColorTheme theme9 = ColorTheme(tr("Ubuntu"), 9);
    theme9.setColor(WINDOW_BACKGROUND, QColor(242, 241, 240));
    theme9.setColor(WINDOW_TEXT, QColor(76, 76, 76));
    theme9.setColor(BUTTON_BACKGROUND, QColor(248, 248, 248));
    theme9.setColor(BUTTON_TEXT, QColor(76, 76, 76));
    theme9.setColor(LIST_BACKGROUND, QColor(255, 255, 255));
    theme9.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(247, 247, 247));
    theme9.setColor(LIST_TEXT, QColor(76, 76, 76));
    theme9.setColor(HIGHLIGHTED_BACKGROUND, QColor(240, 131, 87));
    theme9.setColor(HIGHLIGHTED_TEXT, QColor(255, 255, 255));
    themes << theme9;

    // Good night theme
    ColorTheme theme10 = ColorTheme(tr("Good night"), 10);
    theme10.setColor(WINDOW_BACKGROUND, QColor(41, 38, 50));
    theme10.setColor(WINDOW_TEXT, QColor(240, 240, 240));
    theme10.setColor(BUTTON_BACKGROUND, QColor(51, 51, 73));
    theme10.setColor(BUTTON_TEXT, QColor(212, 212, 226));
    theme10.setColor(LIST_BACKGROUND, QColor(24, 21, 18));
    theme10.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(50, 48, 40));
    theme10.setColor(LIST_TEXT, QColor(255, 255, 255));
    theme10.setColor(HIGHLIGHTED_BACKGROUND, QColor(224, 205, 169));
    theme10.setColor(HIGHLIGHTED_TEXT, QColor(18, 17, 16));
    themes << theme10;

    // Pink theme
    ColorTheme theme11 = ColorTheme(tr("Pink"), 11);
    theme11.setColor(WINDOW_BACKGROUND, QColor(255, 140, 177));
    theme11.setColor(WINDOW_TEXT, QColor(0, 0, 0));
    theme11.setColor(BUTTON_BACKGROUND, QColor(245, 185, 193));
    theme11.setColor(BUTTON_TEXT, QColor(95, 1, 65));
    theme11.setColor(LIST_BACKGROUND, QColor(255, 255, 255));
    theme11.setColor(LIST_ALTERNATIVE_BACKGROUND, QColor(227, 224, 225));
    theme11.setColor(LIST_TEXT, QColor(0, 76, 76));
    theme11.setColor(HIGHLIGHTED_BACKGROUND, QColor(157, 48, 99));
    theme11.setColor(HIGHLIGHTED_TEXT, QColor(255, 222, 223));
    themes << theme11;

    return themes;
}

QPixmap ThemeManager::getColoredSvg(QString filename, QSize maxSize, ColorType currentColor)
{
    QMap<QString, QString> replacement;
    replacement["currentColor"] = this->getColor(currentColor).name();
    return getColoredSvg(filename, maxSize, replacement);
}

QPixmap ThemeManager::getColoredSvg(QString filename, QSize maxSize, QMap<QString, QString> &replacement)
{
    // Read the content of the file
    QFile stream(filename);
    QByteArray fileContent;
    if (stream.open(QFile::ReadOnly))
    {
        fileContent = stream.readAll();
        stream.close();
    }

    // Replace keywords
    foreach (QString key, replacement.keys())
        fileContent.replace(key.toUtf8(), replacement[key].toUtf8());

    // Compute the size
    QSvgRenderer rr(fileContent);
    QSize size = rr.defaultSize();
    size.scale(maxSize.width(), maxSize.height(), Qt::KeepAspectRatio);

    // Create the image
    QImage image(size, QImage::Format_ARGB32);
    QPainter painter(&image);
    image.fill(Qt::transparent);
    rr.render(&painter);
    return QPixmap::fromImage(image);
}

QColor ThemeManager::mix(QColor color1, QColor color2, double fade)
{
    double fade2 = 1.0 - fade;
    return QColor(fade2 * color1.red() + fade * color2.red(),
                  fade2 * color1.green() + fade * color2.green(),
                  fade2 * color1.blue() + fade * color2.blue());
}

ThemeManager::ColorType ThemeManager::getAssociatedColorType(ColorType type)
{
    switch (type)
    {
    case WINDOW_BACKGROUND:
        return WINDOW_TEXT;
    case WINDOW_TEXT:
        return WINDOW_BACKGROUND;
    case BUTTON_BACKGROUND:
        return BUTTON_TEXT;
    case BUTTON_TEXT:
        return BUTTON_BACKGROUND;
    case LIST_BACKGROUND:
    case LIST_ALTERNATIVE_BACKGROUND:
        return LIST_TEXT;
    case LIST_TEXT:
        return LIST_BACKGROUND;
    case HIGHLIGHTED_BACKGROUND:
        return HIGHLIGHTED_TEXT;
    case HIGHLIGHTED_TEXT:
        return HIGHLIGHTED_BACKGROUND;
    default:
        break;
    }
    return type;
}

QString ThemeManager::getMenuTheme()
{
    QString middleColor = this->mix(this->getColor(LIST_TEXT),
                                    this->getColor(LIST_BACKGROUND), 0.5).name();
    return QString("\
QMenu {\
  background-color: %1;\
  border: 1px solid %4;\
}\
QMenu::item {\
  padding: 5px 15px;\
}\
QMenu::icon {\
  padding-left: 20px;\
}\
QMenu::item:selected {\
  background: %2;\
  color: %3;\
}\
QMenu::separator {\
  background: %4;\
  margin: 10px 45px;\
  height: 1px;\
}")
       .arg(this->getColor(LIST_BACKGROUND).name())
       .arg(this->getColor(HIGHLIGHTED_BACKGROUND).name())
       .arg(this->getColor(HIGHLIGHTED_TEXT).name())
       .arg(middleColor);
}

QString ThemeManager::getTableTheme()
{
    return QString("\
QTableWidget QTableCornerButton::section {\
  background: %1;\
  border: 1px solid %2;\
  border-top: 0;\
  border-left: 0;\
}\
QTableWidget {\
  border:1px solid %2;\
  gridline-color: %2;\
}\
QHeaderView::section {\
  border:1px solid %2;\
  padding-left: 2px;\
  padding-right: 2px;\
  border-top: 0;\
  border-left: 0;\
}\
QHeaderView::down-arrow {\
  image: url(:/icons/empty);\
}\
QHeaderView::up-arrow {\
  image: url(:/icons/empty);\
}")
       .arg(this->getColor(ThemeManager::BUTTON_BACKGROUND).name())
       .arg(this->getColor(ThemeManager::BORDER).name());
}
