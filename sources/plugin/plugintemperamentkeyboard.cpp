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


#include "plugintemperamentkeyboard.h"

// Change of a value: one cent per notch of the wheel and half a cent per pixel when dragging, ten times less with Ctrl
// (Cmd on macOS). Values are rounded to a tenth of cent.
static const float WHEEL_STEP = 1.0f;
static const float DRAG_STEP = 0.5f;
static const float FINE_RATIO = 0.1f;

// Width of the black keys as on a keyboard, preferred height (if the texts fit), margin around the texts and the dots
static const float BLACK_KEY_WIDTH = 0.8f;
static const float BLACK_KEY_HEIGHT = 0.6f;
static const float MARGIN = 3.0f;

// Colors of the value of the reference pitch note (not 0) on a white key and on a black key
static const juce::Colour WARNING_ON_WHITE(0xffe07800);
static const juce::Colour WARNING_ON_BLACK(0xffffaf28);

// Position of the black keys between the white keys, as on the virtual keyboard of the editor (PianoScene::getOffsetX)
static float getOffsetX(int key)
{
    switch (key % 12)
    {
    case 1: return 0.425f;
    case 3: return 0.575f;
    case 6: return 0.35f;
    case 8: return 0.5f;
    case 10: return 0.65f;
    default: return 0.0f;
    }
}

static bool isBlack(int key)
{
    int note = key % 12;
    return note == 1 || note == 3 || note == 6 || note == 8 || note == 10;
}

static float roundValue(float value)
{
    return static_cast<float>(juce::roundToInt(10.0f * juce::jlimit(PluginTemperamentKeyboard::MIN_VALUE,
                                                                     PluginTemperamentKeyboard::MAX_VALUE, value))) / 10.0f;
}

PluginTemperamentKeyboard::PluginTemperamentKeyboard(juce::Colour highlightColour) :
    _highlightColour(highlightColour),
    _tonality(-1),
    _referencePitchKey(9),
    _hoveredKey(-1),
    _draggedKey(-1),
    _dragValue(0.0f),
    _lastDragY(0.0f),
    _wheelDelta(0.0f)
{
    for (int i = 0; i < 12; i++)
    {
        _values[i] = 0.0f;
        _used[i] = true;
    }
}

void PluginTemperamentKeyboard::setTemperament(const float values[12], const bool used[12], int tonality)
{
    // A drag continues from the new value of its key if it changed (otherwise the remainder is kept)
    if (_draggedKey != -1 && !juce::exactlyEqual(values[_draggedKey], _values[_draggedKey]))
        _dragValue = values[_draggedKey];

    bool hasChanged = (tonality != _tonality);
    for (int i = 0; i < 12; i++)
    {
        hasChanged = hasChanged || !juce::exactlyEqual(values[i], _values[i]) || used[i] != _used[i];
        _values[i] = values[i];
        _used[i] = used[i];
    }
    _tonality = tonality;
    if (_draggedKey != -1 && !_used[_draggedKey])
        _draggedKey = -1;
    if (_hoveredKey != -1 && !_used[_hoveredKey])
        setHoveredKey(-1);
    if (hasChanged)
        repaint();
}

void PluginTemperamentKeyboard::setReferencePitchKey(int key)
{
    if (key != _referencePitchKey)
    {
        _referencePitchKey = key;
        repaint();
    }
}

juce::Font PluginTemperamentKeyboard::getValueFont() const
{
    return juce::Font(juce::FontOptions(10.5f));
}

juce::Rectangle<float> PluginTemperamentKeyboard::getTextInk() const
{
    // Ink of the values around their baseline (signs, digits, decimal point), smaller than the line height
    juce::GlyphArrangement glyphs;
    glyphs.addLineOfText(getValueFont(), "+-0123456789.", 0.0f, 0.0f);
    return glyphs.getBoundingBox(0, -1, false);
}

float PluginTemperamentKeyboard::getWhiteKeyWidth() const
{
    // The widest value must fit in a black key
    float textWidth = static_cast<float>(juce::GlyphArrangement::getStringWidthInt(getValueFont(), formatValue(-88.8f)));
    return std::ceil(juce::jmax(30.0f, (textWidth + 2.0f * MARGIN) / BLACK_KEY_WIDTH));
}

float PluginTemperamentKeyboard::getDotSize() const
{
    return juce::jmax(5.0f, 0.12f * getWhiteKeyWidth());
}

float PluginTemperamentKeyboard::getBlackKeyHeight() const
{
    // Value and dot inside, the values of the white keys below
    float height = static_cast<float>(getHeight()) - 1.0f;
    float inkHeight = getTextInk().getHeight();
    float minimum = inkHeight + getDotSize() + 3.0f * MARGIN;
    float maximum = height - inkHeight - 2.0f * MARGIN;
    return std::round(juce::jmax(minimum, juce::jmin(BLACK_KEY_HEIGHT * height, maximum)));
}

int PluginTemperamentKeyboard::getIdealWidth() const
{
    return juce::roundToInt(7.0f * getWhiteKeyWidth()) + 1;
}

int PluginTemperamentKeyboard::getMinimumHeight() const
{
    return static_cast<int>(std::ceil(2.0f * getTextInk().getHeight() + getDotSize() + 5.0f * MARGIN)) + 2;
}

juce::Rectangle<float> PluginTemperamentKeyboard::getKeyRect(int key) const
{
    float whiteWidth = getWhiteKeyWidth();
    int whiteIndex = key >= 5 ? (key + 1) / 2 : key / 2; // White key, or the one just before a black key
    if (!isBlack(key))
        return juce::Rectangle<float>(static_cast<float>(whiteIndex) * whiteWidth + 0.5f, 0.5f, whiteWidth,
                                      static_cast<float>(getHeight()) - 1.0f);
    float x = std::round((static_cast<float>(whiteIndex) + (1.0f - BLACK_KEY_WIDTH) / 2.0f + getOffsetX(key)) * whiteWidth);
    return juce::Rectangle<float>(x + 0.5f, 0.5f, std::round(BLACK_KEY_WIDTH * whiteWidth), getBlackKeyHeight());
}

void PluginTemperamentKeyboard::getTopGap(int key, float &left, float &right) const
{
    // Part of a key not covered by the black keys at the top
    juce::Rectangle<float> rect = getKeyRect(key);
    left = rect.getX();
    right = rect.getRight();
    if (!isBlack(key))
    {
        if (key > 0 && isBlack(key - 1))
            left = getKeyRect(key - 1).getRight();
        if (key < 11 && isBlack(key + 1))
            right = getKeyRect(key + 1).getX();
    }
}

int PluginTemperamentKeyboard::getKeyAt(juce::Point<float> position) const
{
    // Black keys first since they are above the white keys
    for (int pass = 0; pass < 2; pass++)
        for (int key = 0; key < 12; key++)
            if (isBlack(key) == (pass == 0) && getKeyRect(key).contains(position))
                return key;
    return -1;
}

int PluginTemperamentKeyboard::getEditableKeyAt(juce::Point<float> position) const
{
    // The value of a key not used cannot be changed
    int key = getKeyAt(position);
    return (key != -1 && _used[key]) ? key : -1;
}

bool PluginTemperamentKeyboard::hasWarning(int key) const
{
    // The note of the reference pitch keeps its pitch: a fine tune shifts all the notes
    return key == _referencePitchKey && _used[key] && juce::roundToInt(10.0f * _values[key]) != 0;
}

juce::String PluginTemperamentKeyboard::formatValue(float value)
{
    int tenths = juce::roundToInt(10.0f * value);
    if (tenths == 0)
        return "0";
    return (tenths > 0 ? "+" : "-") + juce::String(static_cast<float>(std::abs(tenths)) / 10.0f, 1);
}

void PluginTemperamentKeyboard::paint(juce::Graphics &g)
{
    juce::Font font = getValueFont();
    g.setFont(font);
    juce::Rectangle<float> ink = getTextInk();
    float dot = getDotSize();
    float blackBottom = 0.5f + getBlackKeyHeight();
    int activeKey = _draggedKey != -1 ? _draggedKey : _hoveredKey;
    juce::Colour gray = juce::Colours::black.interpolatedWith(juce::Colours::white, 0.5f);

    // White keys then black keys
    for (int pass = 0; pass < 2; pass++)
    {
        bool black = (pass == 1);
        for (int key = 0; key < 12; key++)
        {
            if (isBlack(key) != black)
                continue;

            // Key, highlighted if it is hovered or edited, gray if it is not used, the white keys with a shadow on
            // their right and bottom sides as on the virtual keyboard of the editor
            juce::Rectangle<float> rect = getKeyRect(key);
            juce::Colour keyColour = black ? juce::Colours::black : juce::Colours::white;
            if (!_used[key])
                keyColour = gray;
            else if (key == activeKey && isEnabled())
                keyColour = keyColour.interpolatedWith(_highlightColour, black ? 0.5f : 0.3f);
            g.setColour(keyColour);
            g.fillRoundedRectangle(rect, 2.0f);
            g.setColour(juce::Colours::black);
            g.drawRoundedRectangle(rect, 2.0f, 1.0f);
            if (!black)
            {
                juce::Path shadow;
                shadow.startNewSubPath(rect.getX() + 1.5f, rect.getBottom() - 1.0f);
                shadow.lineTo(rect.getRight() - 1.0f, rect.getBottom() - 1.0f);
                shadow.lineTo(rect.getRight() - 1.0f, rect.getY() + 1.0f);
                g.setColour(juce::Colours::grey);
                g.strokePath(shadow, juce::PathStrokeType(1.0f));
            }

            // Value at the top of a black key, below the black keys on a white key, the dots of the tonality on a
            // line between them (in the gap between the black keys for a white key)
            juce::Point<float> dotCenter(rect.getCentreX(), blackBottom - MARGIN - dot / 2.0f);
            float baseline;
            if (black)
                baseline = rect.getY() + MARGIN - ink.getY();
            else
            {
                baseline = blackBottom + (rect.getBottom() - blackBottom - ink.getHeight()) / 2.0f - ink.getY();
                float left, right;
                getTopGap(key, left, right);
                dotCenter.setX((left + right) / 2.0f);
            }

            if (_used[key])
            {
                // 0 is lighter, the note of the reference pitch with a fine tune is orange
                juce::Colour textColour = black ? juce::Colours::white : juce::Colours::black;
                if (hasWarning(key))
                    textColour = black ? WARNING_ON_BLACK : WARNING_ON_WHITE;
                else if (juce::roundToInt(10.0f * _values[key]) == 0)
                    textColour = textColour.withAlpha(0.45f);
                g.setColour(textColour);
                juce::String text = formatValue(_values[key]);
                float textWidth = static_cast<float>(juce::GlyphArrangement::getStringWidthInt(font, text));
                g.drawSingleLineText(text, juce::roundToInt(rect.getCentreX() - textWidth / 2.0f),
                                     juce::roundToInt(baseline));
            }

            // Tonality
            if (key == _tonality)
            {
                juce::Rectangle<float> dotRect(dot, dot);
                dotRect.setCentre(dotCenter);
                g.setColour(_highlightColour);
                g.fillEllipse(dotRect);
                g.setColour(black ? juce::Colours::white : juce::Colours::black);
                g.drawEllipse(dotRect, 1.0f);
            }
        }
    }
}

void PluginTemperamentKeyboard::setHoveredKey(int key)
{
    if (key != _hoveredKey)
    {
        _hoveredKey = key;
        setMouseCursor(key == -1 ? juce::MouseCursor::NormalCursor : juce::MouseCursor::UpDownResizeCursor);
        repaint();
    }
}

void PluginTemperamentKeyboard::editValue(int key, float value)
{
    value = roundValue(value);
    if (juce::exactlyEqual(value, _values[key]))
        return;
    _values[key] = value;
    repaint();
    if (onEdit)
        onEdit();
}

void PluginTemperamentKeyboard::mouseMove(const juce::MouseEvent &event)
{
    if (_draggedKey == -1)
        setHoveredKey(getEditableKeyAt(event.position));
}

void PluginTemperamentKeyboard::mouseExit(const juce::MouseEvent &)
{
    if (_draggedKey == -1)
        setHoveredKey(-1);
}

void PluginTemperamentKeyboard::mouseDown(const juce::MouseEvent &event)
{
    if (event.mods.isPopupMenu())
    {
        int key = getKeyAt(event.position);
        if (key != -1)
            showContextMenu(key);
        return;
    }

    _draggedKey = getEditableKeyAt(event.position);
    if (_draggedKey != -1)
    {
        _dragValue = _values[_draggedKey];
        _lastDragY = event.position.y;
        setHoveredKey(_draggedKey);
    }
}

void PluginTemperamentKeyboard::mouseDrag(const juce::MouseEvent &event)
{
    if (_draggedKey == -1)
        return;

    // Up: higher value
    float step = event.mods.isCommandDown() ? DRAG_STEP * FINE_RATIO : DRAG_STEP;
    _dragValue = juce::jlimit(MIN_VALUE, MAX_VALUE, _dragValue + (_lastDragY - event.position.y) * step);
    _lastDragY = event.position.y;
    editValue(_draggedKey, _dragValue);
}

void PluginTemperamentKeyboard::mouseUp(const juce::MouseEvent &event)
{
    if (_draggedKey == -1)
        return;
    _draggedKey = -1;
    setHoveredKey(getEditableKeyAt(event.position));
    repaint();
}

void PluginTemperamentKeyboard::mouseDoubleClick(const juce::MouseEvent &event)
{
    int key = getEditableKeyAt(event.position);
    if (key != -1 && !event.mods.isPopupMenu())
    {
        _draggedKey = -1;
        editValue(key, 0.0f);
    }
}

void PluginTemperamentKeyboard::mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel)
{
    int key = getEditableKeyAt(event.position);
    if (key == -1)
    {
        juce::Component::mouseWheelMove(event, wheel);
        return;
    }

    // One step per notch of a wheel, the movements of a trackpad being accumulated
    float delta = (wheel.isReversed ? -1.0f : 1.0f) * (juce::exactlyEqual(wheel.deltaY, 0.0f) ? -wheel.deltaX : wheel.deltaY);
    int steps = 0;
    if (wheel.isSmooth)
    {
        _wheelDelta += delta;
        steps = static_cast<int>(_wheelDelta / 0.1f);
        _wheelDelta -= 0.1f * static_cast<float>(steps);
    }
    else if (delta > 0.0f || delta < 0.0f)
        steps = delta > 0.0f ? 1 : -1;
    if (steps != 0)
    {
        float step = event.mods.isCommandDown() ? WHEEL_STEP * FINE_RATIO : WHEEL_STEP;
        editValue(key, _values[key] + static_cast<float>(steps) * step);
    }
}

void PluginTemperamentKeyboard::showContextMenu(int key)
{
    juce::PopupMenu menu;
    menu.addItem(1, tr("Note used"), true, _used[key]);
    menu.addItem(2, tr("Tonality on this note"), _tonality != -1 && key != _tonality);
    juce::Component::SafePointer<PluginTemperamentKeyboard> safeThis(this);
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this).withMousePosition(),
                       [safeThis, key](int result) {
        if (safeThis == nullptr)
            return;
        if (result == 1)
        {
            safeThis->_used[key] = !safeThis->_used[key];
            if (!safeThis->_used[key] && safeThis->_hoveredKey == key)
                safeThis->setHoveredKey(-1);
            safeThis->repaint();
            if (safeThis->onEdit)
                safeThis->onEdit();
        }
        else if (result == 2 && safeThis->onTonalityRequested)
            safeThis->onTonalityRequested(key);
    });
}

juce::String PluginTemperamentKeyboard::getTooltip()
{
    // Explanation of the value of the reference pitch note, otherwise the help of the keyboard
    static const char * const KEY_NAMES[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    int key = getKeyAt(getMouseXYRelative().toFloat());
    if (key != -1 && hasWarning(key))
        return PluginTranslator::arg(tr("%1 is the note of the reference pitch: its fine tune of %2 cents is compensated by "
                                        "shifting all the notes by %3 cents."), KEY_NAMES[key])
            .replace("%2", formatValue(_values[key])).replace("%3", formatValue(-_values[key]));
    return tr("Fine tune of each key in cents, compared to the equal temperament.") + "\n" +
           tr("Mouse wheel or vertical drag: change a value (finer with Ctrl).") + "\n" +
           tr("Double click: back to 0.") + "\n" +
           tr("Right click: note used or not, tonality.") + "\n" +
           tr("Dot: tonality.");
}
