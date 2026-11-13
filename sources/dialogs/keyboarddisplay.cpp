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

#include "keyboarddisplay.h"
#include "contextmanager.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QPainter>
#include <QToolButton>
#include <QtMath>

// Dot matrix: number of rows of each line and distance between two dots, in logical pixels (the distance in physical
// pixels is an integer, so that all dots look the same). The file name is smaller, its dots touching each other.
static const int FILE_ROWS = 11;
static const int FILE_PITCH = 1;
static const int NAME_ROWS = 12;
static const int NAME_PITCH = 3;
static const int MARGIN_TOP = 4;
static const int LINE_SPACING = 3;
static const int MARGIN_BOTTOM = MARGIN_TOP + FILE_ROWS * FILE_PITCH + LINE_SPACING; // Same space as above the name
static const int ICON_SPACING = 3; // Number of columns between the icon and the text
static const int WORD_SPACING = 4; // Number of columns between the number of a preset and its name

// One line of the dot matrix, one pixel per dot (transparent if the dot is off)
class DotLine
{
public:
    DotLine(int columns, int rows) :
        _image(qMax(1, columns), rows, QImage::Format_ARGB32),
        _x(0)
    {
        _image.fill(Qt::transparent);
    }

    const QImage &getImage() const { return _image; }
    int getRemainingColumns() const { return qMax(0, _image.width() - _x); }
    void addSpace(int columns) { _x += columns; }

    void addText(QString text, QColor color)
    {
        if (text.isEmpty() || getRemainingColumns() <= 0)
            return;

        // Font without antialiasing, the largest whose height is the number of rows
        QFont font = getFont(_image.height());
        QFontMetrics fm(font);
        text = fm.elidedText(text, Qt::ElideMiddle, getRemainingColumns());

        QPainter painter(&_image);
        painter.setRenderHint(QPainter::TextAntialiasing, false);
        painter.setFont(font);
        painter.setPen(color);
        painter.drawText(_x, (_image.height() - fm.ascent() - fm.descent()) / 2 + fm.ascent(), text);
        _x += fm.horizontalAdvance(text);
    }

    void addIcon(QString svgPath, QColor color)
    {
        // Icon rendered bigger, a dot being on if at least 25% of its surface is covered (thin lines are kept)
        const int factor = 4;
        QMap<QString, QString> replacement;
        replacement["currentColor"] = "#000000";
        QImage icon = ContextManager::theme()->getColoredSvg(
                          svgPath, QSize(_image.height() * factor, _image.height() * factor), replacement).toImage();
        int columns = (icon.width() + factor - 1) / factor;
        int rows = (icon.height() + factor - 1) / factor;
        int top = (_image.height() - rows) / 2;
        for (int column = 0; column < columns && _x + column < _image.width(); column++)
        {
            for (int row = 0; row < rows; row++)
            {
                int coverage = 0;
                for (int x = column * factor; x < qMin(icon.width(), (column + 1) * factor); x++)
                    for (int y = row * factor; y < qMin(icon.height(), (row + 1) * factor); y++)
                        coverage += qAlpha(icon.pixel(x, y));
                if (coverage > 0.25 * 255 * factor * factor && top + row >= 0 && top + row < _image.height())
                    _image.setPixel(_x + column, top + row, color.rgba());
            }
        }
        _x += columns;
    }

private:
    static QFont getFont(int rows)
    {
        QFont font = QApplication::font();
        font.setStyleStrategy(QFont::NoAntialias);
        font.setHintingPreference(QFont::PreferFullHinting);
        for (int pixelSize = rows; pixelSize > 4; pixelSize--)
        {
            font.setPixelSize(pixelSize);
            QFontMetrics fm(font);
            if (fm.ascent() + fm.descent() <= rows)
                break;
        }
        return font;
    }

    QImage _image;
    int _x;
};

// Area of the display where the dots are drawn
class KeyboardDisplayScreen : public QWidget
{
public:
    KeyboardDisplayScreen(QColor backgroundColor, QColor textColor, QWidget * parent) : QWidget(parent),
        _textColor(textColor),
        _numberColor(ThemeManager::mix(backgroundColor, textColor, 0.75)),
        _fileColor(ThemeManager::mix(backgroundColor, textColor, 0.6)),
        _messageColor(ThemeManager::mix(backgroundColor, textColor, 0.5)),
        _offColor(ThemeManager::mix(backgroundColor, textColor, 0.09)),
        _elementType(elementUnknown)
    {
        this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        this->setFixedHeight(MARGIN_TOP + FILE_ROWS * FILE_PITCH + LINE_SPACING + NAME_ROWS * NAME_PITCH + MARGIN_BOTTOM);
    }

    void setContent(QString fileName, ElementType elementType, QString number, QString name)
    {
        _fileName = fileName;
        _elementType = elementType;
        _number = number;
        _name = name;
        _cache = QImage();
        this->update();
    }

protected:
    void paintEvent(QPaintEvent * event) override
    {
        Q_UNUSED(event)
        qreal ratio = this->devicePixelRatioF();
        if (_cache.isNull() || _cache.devicePixelRatio() != ratio || _cacheSize != this->size())
            render(ratio);

        QPainter painter(this);
        painter.drawImage(0, 0, _cache);
    }

private:
    void render(qreal ratio)
    {
        _cacheSize = this->size();
        _cache = QImage(qCeil(this->width() * ratio), qCeil(this->height() * ratio), QImage::Format_ARGB32_Premultiplied);
        _cache.fill(Qt::transparent);
        QPainter painter(&_cache);

        // File
        int pitch = getPhysicalPitch(FILE_PITCH, ratio);
        DotLine fileLine(_cache.width() / pitch, FILE_ROWS);
        fileLine.addText(_fileName, _fileColor);
        drawLine(painter, fileLine.getImage(), pitch, qRound(MARGIN_TOP * ratio), qRound(FILE_ROWS * FILE_PITCH * ratio), false);

        // Element played: icon, number if any, name
        pitch = getPhysicalPitch(NAME_PITCH, ratio);
        DotLine nameLine(_cache.width() / pitch, NAME_ROWS);
        switch (_elementType)
        {
        case elementSmpl:
            nameLine.addIcon(":/icons/sample.svg", ContextManager::theme()->getFixedColor(ThemeManager::YELLOW, true));
            break;
        case elementInst:
            nameLine.addIcon(":/icons/instrument.svg", ContextManager::theme()->getFixedColor(ThemeManager::BLUE, true));
            break;
        case elementPrst:
            nameLine.addIcon(":/icons/preset.svg", ContextManager::theme()->getFixedColor(ThemeManager::RED, true));
            break;
        default:
            break;
        }
        if (_elementType == elementUnknown)
            nameLine.addText(_name, _messageColor);
        else
        {
            nameLine.addSpace(ICON_SPACING);
            if (!_number.isEmpty())
            {
                nameLine.addText(_number, _numberColor);
                nameLine.addSpace(WORD_SPACING);
            }
            nameLine.addText(_name, _textColor);
        }
        drawLine(painter, nameLine.getImage(), pitch, qRound((MARGIN_TOP + FILE_ROWS * FILE_PITCH + LINE_SPACING) * ratio),
                 qRound(NAME_ROWS * NAME_PITCH * ratio), true);

        painter.end();
        _cache.setDevicePixelRatio(ratio);
    }

    static int getPhysicalPitch(int pitch, qreal ratio)
    {
        return qMax(1, qFloor(pitch * ratio + 0.01));
    }

    void drawLine(QPainter &painter, const QImage &dots, int pitch, int top, int height, bool separatedDots)
    {
        // Dot matrix centered in the area
        int left = (_cache.width() - dots.width() * pitch) / 2;
        top += (height - dots.height() * pitch) / 2;
        QRect area(left, top, dots.width() * pitch, dots.height() * pitch);

        // Dots touching each other: the line is only enlarged
        if (!separatedDots)
        {
            painter.drawImage(area, dots);
            return;
        }

        // Dots off
        QBrush brush(getDot(_offColor, pitch));
        painter.setBrushOrigin(left, top);
        painter.fillRect(area, brush);

        // Dots on
        for (int row = 0; row < dots.height(); row++)
        {
            const QRgb * line = reinterpret_cast<const QRgb *>(dots.constScanLine(row));
            for (int column = 0; column < dots.width(); column++)
                if (qAlpha(line[column]) > 127)
                    painter.drawImage(left + column * pitch, top + row * pitch, getDot(QColor::fromRgb(line[column]), pitch));
        }
    }

    QImage getDot(QColor color, int pitch)
    {
        // One cell of the matrix, with a dot separated from the next ones by a space (if possible)
        QString key = color.name() + QString::number(pitch);
        if (!_dots.contains(key))
        {
            QImage image(pitch, pitch, QImage::Format_ARGB32_Premultiplied);
            image.fill(Qt::transparent);
            QPainter painter(&image);
            int dotSize = pitch <= 1 ? 1 : (pitch <= 3 ? pitch - 1 : qRound(0.75 * pitch));
            if (pitch < 4)
                painter.fillRect(QRect(0, 0, dotSize, dotSize), color);
            else
            {
                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(Qt::NoPen);
                painter.setBrush(color);
                double offset = 0.5 * (pitch - dotSize);
                painter.drawRoundedRect(QRectF(offset, offset, dotSize, dotSize), 0.3 * dotSize, 0.3 * dotSize);
            }
            painter.end();
            _dots[key] = image;
        }
        return _dots[key];
    }

    QColor _textColor, _numberColor, _fileColor, _messageColor, _offColor;
    QString _fileName;
    ElementType _elementType;
    QString _number;
    QString _name;
    QImage _cache;
    QSize _cacheSize;
    QMap<QString, QImage> _dots;
};

KeyboardDisplay::KeyboardDisplay(QWidget * parent) : QWidget(parent)
{
    // Colors of the lists, the darker being the background
    ThemeManager * theme = ContextManager::theme();
    _backgroundColor = theme->getColor(ThemeManager::LIST_BACKGROUND);
    _textColor = theme->getColor(ThemeManager::LIST_TEXT);
    if (!theme->isDark(ThemeManager::LIST_BACKGROUND, ThemeManager::LIST_TEXT))
        std::swap(_backgroundColor, _textColor);
    this->setStyleSheet("QToolButton{border:0;background:transparent}");

    // Buttons around the screen
    _buttonPreviousFile = createButton(":/icons/file_previous.svg", QSize(18, 22), 30, tr("Previous file"),
                                       QKeySequence(Qt::SHIFT | Qt::Key_PageUp));
    _buttonPreviousElement = createButton(":/icons/arrow_left.svg", QSize(10, 22), 24, tr("Previous element"),
                                          QKeySequence(Qt::Key_PageUp));
    _buttonNextElement = createButton(":/icons/arrow_right.svg", QSize(10, 22), 24, tr("Next element"),
                                      QKeySequence(Qt::Key_PageDown));
    _buttonNextFile = createButton(":/icons/file_next.svg", QSize(18, 22), 30, tr("Next file"),
                                   QKeySequence(Qt::SHIFT | Qt::Key_PageDown));
    connect(_buttonPreviousFile, &QToolButton::clicked, this, &KeyboardDisplay::previousFileRequested);
    connect(_buttonPreviousElement, &QToolButton::clicked, this, &KeyboardDisplay::previousElementRequested);
    connect(_buttonNextElement, &QToolButton::clicked, this, &KeyboardDisplay::nextElementRequested);
    connect(_buttonNextFile, &QToolButton::clicked, this, &KeyboardDisplay::nextFileRequested);
    _screen = new KeyboardDisplayScreen(_backgroundColor, _textColor, this);

    QHBoxLayout * layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(0);
    layout->addWidget(_buttonPreviousFile);
    layout->addWidget(_buttonPreviousElement);
    layout->addSpacing(4);
    layout->addWidget(_screen, 1);
    layout->addSpacing(4);
    layout->addWidget(_buttonNextElement);
    layout->addWidget(_buttonNextFile);
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

    // Initial state: nothing is playable
    _content.hasNextElement = true; // So that the content is different
    setContent(KeyboardDisplayContent());
}

QToolButton * KeyboardDisplay::createButton(QString svgPath, QSize iconSize, int width, QString toolTip, QKeySequence shortcut)
{
    // Icon in the text color when hovered (active mode of an auto-raise button), dimmed otherwise
    QIcon icon;
    QList<QPair<QIcon::Mode, QColor>> colors;
    colors << QPair<QIcon::Mode, QColor>(QIcon::Normal, ThemeManager::mix(_backgroundColor, _textColor, 0.7))
           << QPair<QIcon::Mode, QColor>(QIcon::Active, _textColor)
           << QPair<QIcon::Mode, QColor>(QIcon::Disabled, ThemeManager::mix(_backgroundColor, _textColor, 0.2));
    for (int i = 0; i < colors.count(); i++)
    {
        QMap<QString, QString> replacement;
        replacement["currentColor"] = colors[i].second.name();
        QPixmap pixmap = ContextManager::theme()->getColoredSvg(svgPath, 2 * iconSize, replacement);
        pixmap.setDevicePixelRatio(2);
        icon.addPixmap(pixmap, colors[i].first);
    }

    QToolButton * button = new QToolButton(this);
    button->setIcon(icon);
    button->setIconSize(iconSize);
    button->setAutoRaise(true);
    button->setFocusPolicy(Qt::NoFocus);
    button->setFixedWidth(width);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    button->setToolTip(toolTip + " (" + shortcut.toString(QKeySequence::NativeText) + ")");
    return button;
}

void KeyboardDisplay::setContent(const KeyboardDisplayContent &content)
{
    if (content == _content)
        return;
    _content = content;

    // Buttons, always visible
    QList<QPair<QToolButton *, bool>> states;
    states << QPair<QToolButton *, bool>(_buttonPreviousFile, content.hasPreviousFile)
           << QPair<QToolButton *, bool>(_buttonPreviousElement, content.hasPreviousElement)
           << QPair<QToolButton *, bool>(_buttonNextElement, content.hasNextElement)
           << QPair<QToolButton *, bool>(_buttonNextFile, content.hasNextFile);
    for (int i = 0; i < states.count(); i++)
    {
        states[i].first->setEnabled(states[i].second);
        states[i].first->setCursor(states[i].second ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }

    // Texts
    if (content.elementType == elementSmpl || content.elementType == elementInst || content.elementType == elementPrst)
        _screen->setContent(content.fileName, content.elementType,
                            content.elementType == elementPrst ? content.elementNumber : "", content.elementName);
    else
        _screen->setContent(content.fileName, elementUnknown, "", tr("No playable element selected"));
}

bool KeyboardDisplay::triggerButton(bool next, bool file)
{
    QToolButton * button = file ? (next ? _buttonNextFile : _buttonPreviousFile) :
                               (next ? _buttonNextElement : _buttonPreviousElement);
    if (!button->isEnabled())
        return false;
    button->click();
    return true;
}

void KeyboardDisplay::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(this->rect(), _backgroundColor);
}
