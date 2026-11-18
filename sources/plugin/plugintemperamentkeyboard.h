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


#ifndef PLUGINTEMPERAMENTKEYBOARD_H
#define PLUGINTEMPERAMENTKEYBOARD_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "plugintranslator.h"

// Keys from C to B placed as on a keyboard, each one showing its fine tune in cents (same as TemperamentKeyboard in the
// editor): flat black keys with their value at the top, the values of the white keys at the bottom, the height being
// given by the layout (at least getMinimumHeight).
// A value is changed with the mouse wheel or a vertical drag, a double click sets it back to 0.
// The keys not used are gray, a dot marks the tonality. The value of the note of the reference pitch is orange if it is
// not 0 (all the notes being shifted). The context menu of a key changes its use and the tonality.
class PluginTemperamentKeyboard: public juce::Component,
                                 public juce::TooltipClient
{
    Q_DECLARE_TR_FUNCTIONS(PluginTemperamentKeyboard)

public:
    explicit PluginTemperamentKeyboard(juce::Colour highlightColour);

    // Fine tunes in cents and use of the keys from C to B, tonality (0 = C, -1 if not used)
    void setTemperament(const float values[12], const bool used[12], int tonality);
    const float * getValues() const { return _values; }
    const bool * getUsed() const { return _used; }

    // Note of the reference pitch (0 = C), sounding at this pitch whatever its fine tune
    void setReferencePitchKey(int key);

    // Width given by the font of the values, height keeping them readable
    int getIdealWidth() const;
    int getMinimumHeight() const;

    // A value or the use of a key has been changed by the user, another tonality is asked by the context menu
    std::function<void()> onEdit;
    std::function<void(int key)> onTonalityRequested;

    static constexpr float MIN_VALUE = -50.0f;
    static constexpr float MAX_VALUE = 50.0f;

    void paint(juce::Graphics &g) override;
    void mouseMove(const juce::MouseEvent &event) override;
    void mouseExit(const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
    void mouseDoubleClick(const juce::MouseEvent &event) override;
    void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;
    juce::String getTooltip() override;

private:
    juce::Font getValueFont() const;
    juce::Rectangle<float> getTextInk() const; // Around the baseline
    float getWhiteKeyWidth() const;
    float getBlackKeyHeight() const;
    float getDotSize() const;
    juce::Rectangle<float> getKeyRect(int key) const;
    void getTopGap(int key, float &left, float &right) const;
    int getKeyAt(juce::Point<float> position) const;
    int getEditableKeyAt(juce::Point<float> position) const;
    bool hasWarning(int key) const;
    void setHoveredKey(int key);
    void editValue(int key, float value);
    void showContextMenu(int key);
    static juce::String formatValue(float value);

    juce::Colour _highlightColour;
    float _values[12];
    bool _used[12];
    int _tonality;
    int _referencePitchKey;
    int _hoveredKey;
    int _draggedKey;
    float _dragValue; // Not rounded, so that slow movements are not lost
    float _lastDragY;
    float _wheelDelta; // Remainder of the movements of a smooth wheel (trackpad)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginTemperamentKeyboard)
};

#endif // PLUGINTEMPERAMENTKEYBOARD_H
