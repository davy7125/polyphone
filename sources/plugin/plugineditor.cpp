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


#include "plugineditor.h"
#include <iterator>
#include "plugintemperamentkeyboard.h"
#include "BinaryData.h"

// Colors of the theme "Dark, orange" of the editor
static const juce::Colour WINDOW_COLOR(0xff323232);
static const juce::Colour HEADER_COLOR(0xff282828);
static const juce::Colour BUTTON_COLOR(0xff494949);
static const juce::Colour LIST_COLOR(0xff191919);
static const juce::Colour ALTERNATIVE_COLOR(0xff222222);
static const juce::Colour OUTLINE_COLOR(0xff5a5a5a);
static const juce::Colour TEXT_COLOR(0xffe6e6e6);
static const juce::Colour DIM_TEXT_COLOR(0xff9a9a9a);
static const juce::Colour HIGHLIGHT_COLOR(0xffff8234);
static const juce::Colour HIGHLIGHTED_TEXT_COLOR(0xff000000);
static const juce::Colour ERROR_COLOR(0xffff5050);

static const int POLYPHONIES[] = { 64, 128, 256, 512, 1024, 2048, 4096 };

// Size of the window: its height follows the groups on the right side (the room left going to the tuning and the
// velocity), with a little more by default for the presets; its width is the one needed by these groups, the room left
// going to the presets
static const int DEFAULT_EXTRA_HEIGHT = 40;
static const int PRESETS_DEFAULT_WIDTH = 290;
static const int PRESETS_MINIMUM_WIDTH = 240;
static const int MAXIMUM_WIDTH = 1800;
static const int MAXIMUM_HEIGHT = 1400;
static const int FIXED_HEIGHT = 60 + 36 + 90 + 16; // Header, footer, keyboard, margins of the body

// Groups: margin and title (with a switch on its right side for the groups that can be collapsed, their content being
// a bit lower), then their content: rows of the tuning, temperaments (2 rows on the left, the 12 notes on the right),
// knobs of an effect
static const int GROUP_TITLE_HEIGHT = 18;
static const int COLLAPSED_GROUP_HEIGHT = 4 + GROUP_TITLE_HEIGHT + 4;
static const int CONTENT_MARGIN = 6;
static const int GROUP_SPACING = 8;
static const int TUNING_ROW_HEIGHT = 24;
static const int TUNING_ROW_SPACING = 4;
static const int TOP_ROW_HEIGHT = 110; // Tuning and velocity, at least
static const int TEMPERAMENT_KEYBOARD_HEIGHT = 2 * TUNING_ROW_HEIGHT + TUNING_ROW_SPACING;
static const int TEMPERAMENTS_CONTENT_HEIGHT = CONTENT_MARGIN + TEMPERAMENT_KEYBOARD_HEIGHT + 4;
static const int KNOBS_HEIGHT = 90;
static const int KNOB_WIDTH = 64; // Same width for all the knobs of the effects
static const int EFFECT_CONTENT_HEIGHT = CONTENT_MARGIN + KNOBS_HEIGHT;
static const float TOGGLE_FONT_SIZE = 14.0f;

// Parameters of the switches of the groups (PluginEditor::SwitchedGroup)
static const char * const SWITCH_IDS[] = { "temperamentEnabled", "reverbEnabled", "chorusEnabled" };

// Identifiers of the items of the settings menu
static const int MENU_CHANNEL = 10000; // + channel
static const int MENU_STORE_SOUNDFONT = 20000;
static const int MENU_MULTIPLE_OUTPUTS = 20001;
static const int MENU_FIXED_LATENCY = 20002;
static const int MENU_CLIPPING = 20003;
static const int MENU_SYSTEM_LANGUAGE = 30000;
static const int MENU_LANGUAGE = 30001; // + index of the language

// Errors given by the synth core (SynthSoundfontFile, Sf2Reader) and by the processor, translated when they are shown.
// The other ones are "cannot open the file <path>" and "invalid <chunk> index" (see translateError).
static const char * const ERRORS[] = {
    QT_TRANSLATE_NOOP("PluginEditor", "cannot open the file"),
    QT_TRANSLATE_NOOP("PluginEditor", "file recursion (#include)"),
    QT_TRANSLATE_NOOP("PluginEditor", "not enough memory"),
    QT_TRANSLATE_NOOP("PluginEditor", "this is not a DLS file"),
    QT_TRANSLATE_NOOP("PluginEditor", "invalid header"),
    QT_TRANSLATE_NOOP("PluginEditor", "corrupted file (sdta)"),
    QT_TRANSLATE_NOOP("PluginEditor", "corrupted file (pdta)")
};

static int getLabelWidth(const juce::Label &label)
{
    return juce::GlyphArrangement::getStringWidthInt(label.getFont(), label.getText()) + 10;
}

static bool isSoundfontFile(const juce::String &path)
{
    return path.endsWithIgnoreCase(".sf2") || path.endsWithIgnoreCase(".sf3") || path.endsWithIgnoreCase(".sfz") ||
           path.endsWithIgnoreCase(".dls");
}

/// Look and feel

class PluginEditor::LookAndFeel: public juce::LookAndFeel_V4
{
public:
    LookAndFeel() :
        juce::LookAndFeel_V4(juce::LookAndFeel_V4::ColourScheme(WINDOW_COLOR, BUTTON_COLOR, HEADER_COLOR, OUTLINE_COLOR,
                                                                 TEXT_COLOR, HIGHLIGHT_COLOR, HIGHLIGHTED_TEXT_COLOR,
                                                                 HIGHLIGHT_COLOR, TEXT_COLOR))
    {
        setColour(juce::Slider::rotarySliderFillColourId, HIGHLIGHT_COLOR);
        setColour(juce::Slider::rotarySliderOutlineColourId, LIST_COLOR);
        setColour(juce::Slider::thumbColourId, TEXT_COLOR);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::ListBox::backgroundColourId, LIST_COLOR);
        setColour(juce::ListBox::outlineColourId, OUTLINE_COLOR);
        setColour(juce::TextButton::buttonColourId, BUTTON_COLOR);
        setColour(juce::TextButton::buttonOnColourId, HIGHLIGHT_COLOR);
        setColour(juce::TextButton::textColourOnId, HIGHLIGHTED_TEXT_COLOR);
        setColour(juce::ComboBox::backgroundColourId, BUTTON_COLOR);
        setColour(juce::ComboBox::outlineColourId, OUTLINE_COLOR);
        setColour(juce::MidiKeyboardComponent::keyDownOverlayColourId, HIGHLIGHT_COLOR);
        setColour(juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId, HIGHLIGHT_COLOR.withAlpha(0.3f));
        setColour(juce::ScrollBar::thumbColourId, OUTLINE_COLOR);
    }

    // Box on the right side of the text, the text being aligned on the right (option of the temperaments, on the right
    // side of its row)
    void drawToggleButton(juce::Graphics &g, juce::ToggleButton &button, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown) override
    {
        float tickWidth = 1.1f * TOGGLE_FONT_SIZE;
        drawTickBox(g, button, static_cast<float>(button.getWidth()) - tickWidth - 4.0f,
                    (static_cast<float>(button.getHeight()) - tickWidth) * 0.5f, tickWidth, tickWidth,
                    button.getToggleState(), button.isEnabled(), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        g.setColour(button.findColour(juce::ToggleButton::textColourId));
        g.setFont(juce::FontOptions(TOGGLE_FONT_SIZE));
        if (!button.isEnabled())
            g.setOpacity(0.5f);
        g.drawFittedText(button.getButtonText(),
                         button.getLocalBounds().withTrimmedRight(juce::roundToInt(tickWidth) + 10).withTrimmedLeft(2),
                         juce::Justification::centredRight, 1);
    }

    // Knob whose value arc starts from its middle (property "bipolar": the gain, toward the left for an attenuation and
    // toward the right for an amplification), larger in its room (header) with a smaller thumb, otherwise as
    // LookAndFeel_V4
    void drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider &slider) override
    {
        if (!static_cast<bool>(slider.getProperties()["bipolar"]))
        {
            juce::LookAndFeel_V4::drawRotarySlider(g, x, y, width, height, sliderPos, rotaryStartAngle, rotaryEndAngle,
                                                   slider);
            return;
        }

        juce::Rectangle<float> bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(3);
        float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        float toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        float middleAngle = rotaryStartAngle + static_cast<float>(slider.valueToProportionOfLength(0.0)) *
                                                   (rotaryEndAngle - rotaryStartAngle);
        float lineWidth = juce::jmin(8.0f, radius * 0.5f);
        float arcRadius = radius - lineWidth * 0.5f;
        juce::PathStrokeType stroke(lineWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded);

        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
                                    rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.strokePath(backgroundArc, stroke);
        if (slider.isEnabled() && std::abs(toAngle - middleAngle) > 0.01f)
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(), arcRadius, arcRadius, 0.0f,
                                   juce::jmin(middleAngle, toAngle), juce::jmax(middleAngle, toAngle), true);
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            g.strokePath(valueArc, stroke);
        }

        juce::Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                      bounds.getCentreY() + arcRadius * std::sin(toAngle - juce::MathConstants<float>::halfPi));
        g.setColour(slider.findColour(juce::Slider::thumbColourId));
        g.fillEllipse(juce::Rectangle<float>(1.4f * lineWidth, 1.4f * lineWidth).withCentre(thumbPoint));
    }

    // Lists of the combo boxes shown inside the window of the plugin rather than in their own windows: a long list
    // (irregular temperaments, instruments of a large soundfont) is then scrolled and drawn by the window of the plugin,
    // whatever the host and the system (a separate window could not be scrolled with the wheel on Linux)
    juce::PopupMenu::Options getOptionsForComboBoxPopupMenu(juce::ComboBox &box, juce::Label &label) override
    {
        return juce::LookAndFeel_V4::getOptionsForComboBoxPopupMenu(box, label)
            .withParentComponent(box.getTopLevelComponent());
    }

    // Tooltips aligned on the left (texts of several lines), otherwise as LookAndFeel_V4
    juce::TextLayout layoutTooltipText(const juce::String &text, juce::Colour colour)
    {
        juce::AttributedString attributedText;
        attributedText.setWordWrap(juce::AttributedString::WordWrap::byWord);
        attributedText.setJustification(juce::Justification::left);
        attributedText.append(text, juce::FontOptions(13.0f, juce::Font::bold).withMetricsKind(getDefaultMetricsKind()),
                              colour);
        juce::TextLayout layout;
        layout.createLayoutWithBalancedLineLengths(attributedText, 400.0f);
        return layout;
    }

    juce::Rectangle<int> getTooltipBounds(const juce::String &tipText, juce::Point<int> screenPos,
                                          juce::Rectangle<int> parentArea) override
    {
        juce::TextLayout layout = layoutTooltipText(tipText, juce::Colours::black);
        int width = static_cast<int>(layout.getWidth() + 14.0f);
        int height = static_cast<int>(layout.getHeight() + 6.0f);
        return juce::Rectangle<int>(screenPos.x > parentArea.getCentreX() ? screenPos.x - (width + 12) : screenPos.x + 24,
                                    screenPos.y > parentArea.getCentreY() ? screenPos.y - (height + 6) : screenPos.y + 6,
                                    width, height).constrainedWithin(parentArea);
    }

    void drawTooltip(juce::Graphics &g, const juce::String &text, int width, int height) override
    {
        juce::Rectangle<float> bounds(static_cast<float>(width), static_cast<float>(height));
        g.setColour(findColour(juce::TooltipWindow::backgroundColourId));
        g.fillRoundedRectangle(bounds, 5.0f);
        g.setColour(findColour(juce::TooltipWindow::outlineColourId));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 5.0f, 1.0f);
        layoutTooltipText(text, findColour(juce::TooltipWindow::textColourId)).draw(g, bounds.reduced(7.0f, 3.0f));
    }

    // Buttons of a slider with increment and decrement buttons, drawn inside its field (see FieldSlider)
    juce::Button * createSliderButton(juce::Slider &, bool isIncrement) override
    {
        juce::TextButton * button = new juce::TextButton(isIncrement ? "+" : "-");
        button->setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button->setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
        return button;
    }

    static int getToggleButtonWidth(const juce::ToggleButton &button)
    {
        return juce::GlyphArrangement::getStringWidthInt(juce::Font(juce::FontOptions(TOGGLE_FONT_SIZE)),
                                                         button.getButtonText()) +
               juce::roundToInt(1.1f * TOGGLE_FONT_SIZE) + 16;
    }
};

/// Presets of the soundfont, one being played by all channels

class PluginEditor::PresetList: public juce::ListBox,
                                private juce::ListBoxModel
{
public:
    explicit PresetList(const PluginEditor &editor) :
        juce::ListBox("Presets"),
        _editor(editor),
        _isUpdating(false)
    {
        setModel(this);
        setRowHeight(22);
        setOutlineThickness(1);
    }

    std::function<void(int program)> onSelection;

    void setPresets(const std::vector<PluginEngine::Preset> &presets, int selectedProgram)
    {
        _isUpdating = true;
        _presets = presets;
        updateContent();
        deselectAllRows();
        for (size_t i = 0; i < _presets.size(); i++)
        {
            if (_presets[i].program == selectedProgram)
            {
                selectRow(static_cast<int>(i));
                break;
            }
        }
        repaint();
        _isUpdating = false;
    }

private:
    int getNumRows() override
    {
        return static_cast<int>(_presets.size());
    }

    void paintListBoxItem(int row, juce::Graphics &g, int width, int height, bool isSelected) override
    {
        if (row < 0 || row >= getNumRows())
            return;
        g.fillAll(isSelected ? HIGHLIGHT_COLOR : (row % 2 == 0 ? LIST_COLOR : ALTERNATIVE_COLOR));
        g.setColour(isSelected ? HIGHLIGHTED_TEXT_COLOR : TEXT_COLOR);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(_editor.getPresetText(_presets[static_cast<size_t>(row)].program), 8, 0, width - 12, height,
                   juce::Justification::centredLeft, true);
    }

    void selectedRowsChanged(int row) override
    {
        if (!_isUpdating && onSelection && row >= 0 && row < getNumRows())
            onSelection(_presets[static_cast<size_t>(row)].program);
    }

    const PluginEditor &_editor;
    std::vector<PluginEngine::Preset> _presets;
    bool _isUpdating;
};

/// A MIDI channel: preset forced or chosen by the program changes, preset played and activity

class PluginEditor::ChannelRow: public juce::Component
{
public:
    explicit ChannelRow(int channel) :
        _channel(channel),
        _lastActivity(0),
        _litFrames(0),
        _playedProgram(-2)
    {
        _numberLabel.setText(juce::String(channel + 1), juce::dontSendNotification);
        _numberLabel.setJustificationType(juce::Justification::centredRight);
        _numberLabel.setColour(juce::Label::textColourId, DIM_TEXT_COLOR);
        addAndMakeVisible(_numberLabel);
        _programBox.onChange = [this] {
            int id = _programBox.getSelectedId();
            if (onProgramChange && id > 0)
                onProgramChange(_channel, id == 1 ? -1 : id - 2);
        };
        addAndMakeVisible(_programBox);
        _playedLabel.setColour(juce::Label::textColourId, DIM_TEXT_COLOR);
        _playedLabel.setFont(juce::FontOptions(13.0f));
        addAndMakeVisible(_playedLabel);
    }

    std::function<void(int channel, int program)> onProgramChange;

    void setPresets(const PluginEditor &editor, const std::vector<PluginEngine::Preset> &presets, int program)
    {
        _programBox.setTooltip(tr("Instrument played by the channel, or the one chosen by the program changes "
                                  "it receives"));
        _programBox.clear(juce::dontSendNotification);
        _programBox.addItem(_channel == 9 ? tr("Program change (drums)") : tr("Program change"), 1);
        bool found = (program < 0);
        for (const PluginEngine::Preset &preset : presets)
        {
            _programBox.addItem(editor.getPresetText(preset.program), preset.program + 2);
            found = found || preset.program == program;
        }
        if (!found)
            _programBox.addItem(editor.getPresetText(program), program + 2);
        _programBox.setSelectedId(program < 0 ? 1 : program + 2, juce::dontSendNotification);
        _playedProgram = -2; // Updated by the next update
    }

    // Preset played and activity, a light being on when a note starts
    void update(const PluginEditor &editor, int playedProgram, uint32_t activity)
    {
        if (playedProgram != _playedProgram)
        {
            _playedProgram = playedProgram;
            _playedLabel.setText(playedProgram < 0 ? juce::String("-") : editor.getPresetText(playedProgram),
                                 juce::dontSendNotification);
        }
        if (activity != _lastActivity)
        {
            _lastActivity = activity;
            _litFrames = 4;
            repaint();
        }
        else if (_litFrames > 0 && --_litFrames == 0)
            repaint();
    }

    void paint(juce::Graphics &g) override
    {
        g.setColour(_litFrames > 0 ? HIGHLIGHT_COLOR : LIST_COLOR);
        g.fillEllipse(_light.toFloat());
        g.setColour(OUTLINE_COLOR);
        g.drawEllipse(_light.toFloat(), 1.0f);
    }

    void resized() override
    {
        juce::Rectangle<int> area = getLocalBounds().reduced(0, 2);
        _numberLabel.setBounds(area.removeFromLeft(28));
        _light = area.removeFromRight(18).withSizeKeepingCentre(9, 9);
        _programBox.setBounds(area.removeFromLeft(area.getWidth() * 3 / 5).reduced(2, 0));
        _playedLabel.setBounds(area);
    }

private:
    int _channel;
    juce::Label _numberLabel;
    juce::ComboBox _programBox;
    juce::Label _playedLabel;
    juce::Rectangle<int> _light;
    uint32_t _lastActivity;
    int _litFrames;
    int _playedProgram;
};

/// A rotary knob attached to a parameter

struct PluginEditor::Knob
{
    // Possibly moved by steps larger than the ones of its parameter (the gain by 0.5 dB, the host giving any value)
    class Slider: public juce::Slider
    {
    public:
        double step = 0.0;

        double snapValue(double attemptedValue, DragMode dragMode) override
        {
            if (step <= 0.0)
                return attemptedValue;
            double snapped = std::round(attemptedValue / step) * step;

            // A notch of the wheel moves by one step at least
            double current = getValue();
            if (dragMode == notDragging && std::abs(snapped - current) < 1e-9 && std::abs(attemptedValue - current) > 1e-9)
                snapped += attemptedValue > current ? step : -step;
            return juce::jlimit(getMinimum(), getMaximum(), snapped);
        }
    };

    Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

/// Response to the velocity (0 to 127 from left to right): lowering of the filter cutoff by the default modulator, or
/// velocity played after the curve

class PluginEditor::VelocityGraph: public juce::Component,
                                   public juce::SettableTooltipClient
{
public:
    enum Type
    {
        TYPE_FILTER, // Value: modulator 2.01 (0), 2.04 (1) or none (2), the cutoff being lowered by up to 2 octaves
        TYPE_CURVE   // Value: curve from -100 to 100 (PluginEngine::getCurvedVelocity)
    };

    explicit VelocityGraph(Type type) :
        _type(type),
        _value(type == TYPE_FILTER ? 1 : 0)
    {
        setInterceptsMouseClicks(true, false); // Tooltip
    }

    void setValue(int value)
    {
        if (value != _value)
        {
            _value = value;
            repaint();
        }
    }

    void paint(juce::Graphics &g) override
    {
        juce::Rectangle<float> box = getLocalBounds().toFloat().reduced(0.5f);
        g.setColour(LIST_COLOR);
        g.fillRoundedRectangle(box, 3.0f);
        g.setColour(OUTLINE_COLOR);
        g.drawRoundedRectangle(box, 3.0f, 1.0f);
        juce::Rectangle<float> plot = box.reduced(6.0f, 5.0f);
        if (plot.getWidth() < 4.0f || plot.getHeight() < 4.0f)
            return;
        auto getX = [&plot](int velocity) { return plot.getX() + plot.getWidth() * static_cast<float>(velocity) / 127.0f; };
        auto getY = [&plot](float level) { return plot.getBottom() - plot.getHeight() * level; };

        // Reference: unchanged velocity, or the middle velocity (jump of the modulator 2.01)
        const float dashes[2] = { 3.0f, 3.0f };
        g.setColour(OUTLINE_COLOR);
        if (_type == TYPE_CURVE)
            g.drawDashedLine(juce::Line<float>(getX(0), getY(0.0f), getX(127), getY(1.0f)), dashes, 2);
        else
            g.drawDashedLine(juce::Line<float>(getX(64), plot.getY(), getX(64), plot.getBottom()), dashes, 2);

        // Response, the jump of the modulator 2.01 being vertical
        juce::Path path;
        for (int velocity = 0; velocity <= 127; velocity++)
        {
            float x = getX(velocity);
            float y = getY(getLevel(velocity));
            if (velocity == 0)
                path.startNewSubPath(x, y);
            else
            {
                if (_type == TYPE_FILTER && _value == 0 && velocity == 64)
                    path.lineTo(x, getY(getLevel(63)));
                path.lineTo(x, y);
            }
        }
        g.setColour(HIGHLIGHT_COLOR);
        g.strokePath(path, juce::PathStrokeType(1.5f, juce::PathStrokeType::mitered, juce::PathStrokeType::rounded));
    }

private:
    // From 0 (bottom) to 1 (top)
    float getLevel(int velocity) const
    {
        if (_type == TYPE_CURVE)
            return static_cast<float>(PluginEngine::getCurvedVelocity(velocity, _value)) / 127.0f;

        // Linear source (velocity / 128), descending, possibly multiplied by a descending switch (2.01): -2400 cents
        if (_value == 2 || (_value == 0 && velocity >= 64))
            return 1.0f;
        return static_cast<float>(velocity) / 128.0f;
    }

    Type _type;
    int _value;
};

/// Switch on the right side of the title of a group: a track with a thumb on the right, colored, if it is on

class PluginEditor::SwitchButton: public juce::Button
{
public:
    SwitchButton() :
        juce::Button("Switch")
    {
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics &g, bool shouldDrawButtonAsHighlighted, bool) override
    {
        bool isSwitchedOn = getToggleState();
        juce::Rectangle<float> track = getLocalBounds().toFloat().withSizeKeepingCentre(28.0f, 14.0f);
        juce::Colour trackColour = isSwitchedOn ? HIGHLIGHT_COLOR : OUTLINE_COLOR;
        g.setColour(shouldDrawButtonAsHighlighted ? trackColour.brighter(0.2f) : trackColour);
        g.fillRoundedRectangle(track, 7.0f);
        juce::Rectangle<float> thumb = track.reduced(2.0f).withWidth(10.0f);
        if (isSwitchedOn)
            thumb.setX(track.getRight() - 12.0f);
        g.setColour(TEXT_COLOR);
        g.fillEllipse(thumb);
    }
};

/// Slider with increment and decrement buttons drawn as a field (as a combo box): its text and its buttons in a frame,
/// separated by a line

class PluginEditor::FieldSlider: public juce::Slider
{
public:
    void paint(juce::Graphics &g) override
    {
        juce::Rectangle<float> box = getLocalBounds().toFloat();
        g.setColour(BUTTON_COLOR);
        g.fillRoundedRectangle(box, 3.0f);
        g.setColour(OUTLINE_COLOR);
        g.drawRoundedRectangle(box.reduced(0.5f), 3.0f, 1.0f);
        float x = static_cast<float>(getTextBoxWidth()) + 0.5f;
        g.drawLine(x, 4.0f, x, box.getBottom() - 4.0f, 1.0f);
    }
};

/// Information shown by the tooltip of a small "i" in a circle

class PluginEditor::InfoIcon: public juce::Component,
                              public juce::SettableTooltipClient
{
public:
    void paint(juce::Graphics &g) override
    {
        juce::Rectangle<float> circle = getLocalBounds().toFloat().withSizeKeepingCentre(13.0f, 13.0f);
        g.setColour(DIM_TEXT_COLOR);
        g.drawEllipse(circle.reduced(0.5f), 1.0f);
        g.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        g.drawText("i", circle, juce::Justification::centred, false);
    }
};

/// Widths of the groups on the right side

struct PluginEditor::RightWidths
{
    int column; // The widest row
    int tuning;
    int velocityCurve;
    int velocityFilter;
};

/// Editor

PluginEditor::PluginEditor(PluginProcessor &pluginProcessor) :
    AudioProcessorEditor(&pluginProcessor),
    _processor(pluginProcessor),
    _lookAndFeel(new LookAndFeel()),
    _languageVersion(-1),
    _isDragging(false),
    _keyboard(pluginProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(_lookAndFeel.get());
    _logo = juce::ImageCache::getFromMemory(BinaryData::logo_png, BinaryData::logo_pngSize);

    // Soundfont
    _soundfontLabel.setFont(juce::FontOptions(18.0f, juce::Font::bold));
    addAndMakeVisible(_soundfontLabel);
    _messageLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(_messageLabel);
    _openButton.onClick = [this] { openFile(); };
    addAndMakeVisible(_openButton);
    _recentButton.setButtonText(juce::String::fromUTF8("\xe2\x96\xbe"));
    _recentButton.onClick = [this] { showRecentFiles(); };
    addAndMakeVisible(_recentButton);
    _openButton.setConnectedEdges(juce::Button::ConnectedOnRight); // One button with the arrow of the recent files
    _recentButton.setConnectedEdges(juce::Button::ConnectedOnLeft);

    // Presets played
    _onePresetButton.setClickingTogglesState(false);
    _onePresetButton.onClick = [this] { setMode(PluginEngine::MODE_ONE_PRESET); };
    addAndMakeVisible(_onePresetButton);
    _channelsButton.onClick = [this] { setMode(PluginEngine::MODE_CHANNELS); };
    addAndMakeVisible(_channelsButton);
    _mpeButton.setButtonText("MPE");
    _mpeButton.onClick = [this] { setMode(PluginEngine::MODE_MPE); };
    addAndMakeVisible(_mpeButton);
    _presetList.reset(new PresetList(*this));
    _presetList->onSelection = [this](int program) { _processor.getEngine().setProgram(program); };
    addAndMakeVisible(_presetList.get());
    for (int channel = 0; channel < 16; channel++)
    {
        _channelRows.emplace_back(new ChannelRow(channel));
        _channelRows.back()->onProgramChange = [this](int channelNumber, int program) {
            _processor.getEngine().setChannelProgram(channelNumber, program);
        };
        _channelContainer.addAndMakeVisible(_channelRows.back().get());
    }
    _channelViewport.setViewedComponent(&_channelContainer, false);
    _channelViewport.setScrollBarsShown(true, false);
    addChildComponent(_channelViewport);

    // Gain in the header, its value shown when the mouse is over it, knobs of the effects (names in updateTexts)
    for (const char * parameterId : { "gain", "reverbLevel", "reverbSize", "reverbWidth", "reverbDamping",
                                      "chorusLevel", "chorusDepth", "chorusFrequency" })
        addKnob(parameterId);
    Knob::Slider &gainSlider = _knobs[0]->slider;
    gainSlider.step = 0.5;
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 64, 20);
    gainSlider.getProperties().set("bipolar", true);
    gainSlider.textFromValueFunction = [](double gain) { // After the attachment, which sets its own conversions
        return (juce::roundToInt(10.0 * gain) > 0 ? "+" : "") + juce::String(gain, 1) + " dB";
    };
    gainSlider.valueFromTextFunction = [](const juce::String &text) { return text.getDoubleValue(); };
    gainSlider.updateText();
    _knobs[0]->label.setJustificationType(juce::Justification::centredRight);

    // Tuning: note of the reference pitch and its frequency, the parameter being the frequency of A4
    for (juce::Label * label : { &_referencePitchLabel, &_transpositionLabel, &_tonalityLabel })
    {
        label->setFont(juce::FontOptions(13.0f));
        addAndMakeVisible(label);
    }
    for (int key : { 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }) // A#3 to A4, C4 = 60 as the keyboard
        _referencePitchKeyBox.addItem(juce::MidiMessage::getMidiNoteName(key <= 9 ? 60 + key : 48 + key, true, true, 4),
                                      key + 1);
    _referencePitchKeyBox.onChange = [this] {
        if (_referencePitchKeyBox.getSelectedId() > 0)
        {
            _processor.setReferencePitchKey(_referencePitchKeyBox.getSelectedId() - 1);
            _referencePitchSlider->updateText();
            updateTuning();
        }
    };
    addAndMakeVisible(_referencePitchKeyBox);
    _equalLabel.setText("=", juce::dontSendNotification);
    _equalLabel.setJustificationType(juce::Justification::centred);
    _equalLabel.setBorderSize(juce::BorderSize<int>(0));
    addAndMakeVisible(_equalLabel);
    _referencePitchSlider.reset(new FieldSlider());
    _referencePitchSlider->setSliderStyle(juce::Slider::IncDecButtons);
    _referencePitchSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, 70, TUNING_ROW_HEIGHT);
    _referencePitchSlider->setIncDecButtonsMode(juce::Slider::incDecButtonsDraggable_Vertical);
    _referencePitchSlider->setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    _referencePitchSlider->setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(_referencePitchSlider.get());
    _referencePitchAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        _processor.getParameterState(), "tuning", *_referencePitchSlider));
    _referencePitchSlider->textFromValueFunction = [this](double frequencyA4) { // After the attachment, which sets its own
        return juce::String(frequencyA4 * PluginProcessor::getRatioToA4(_processor.getReferencePitchKey()), 1) + " Hz";
    };
    _referencePitchSlider->valueFromTextFunction = [this](const juce::String &text) {
        return text.getDoubleValue() / PluginProcessor::getRatioToA4(_processor.getReferencePitchKey());
    };
    _referencePitchSlider->updateText();

    // Transposition
    for (int semitones = -24; semitones <= 24; semitones++)
        _transpositionBox.addItem((semitones > 0 ? "+" : "") + juce::String(semitones), semitones + 25);
    addAndMakeVisible(_transpositionBox);
    _transpositionAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        _processor.getParameterState(), "transpose", _transpositionBox));

    // Switches of the groups that can be collapsed
    for (int group = 0; group < GROUP_COUNT; group++)
    {
        _switches[group].reset(new SwitchButton());
        addAndMakeVisible(_switches[group].get());
        _switchAttachments[group].reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(
            _processor.getParameterState(), SWITCH_IDS[group], *_switches[group]));
        _switches[group]->onClick = [this] { updateGroups(); }; // After the first state given by the attachment
        _isGroupExpanded[group] = _switches[group]->getToggleState();
    }

    // Temperaments and scales
    addAndMakeVisible(_temperamentBox);
    _temperamentAttachment.reset(new juce::ParameterAttachment(
        *_processor.getParameterState().getParameter("temperament"), [this](float value) {
            _temperamentBox.setSelectedId(juce::roundToInt(value) + 1, juce::dontSendNotification);
        }));
    _temperamentBox.onChange = [this] {
        if (_temperamentBox.getSelectedId() > 0)
            _temperamentAttachment->setValueAsCompleteGesture(static_cast<float>(_temperamentBox.getSelectedId() - 1));
        updateTuning();
    };
    fillTemperamentBox();
    int id = 1;
    for (const char * key : { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" })
        _temperamentKeyBox.addItem(key, id++);
    addAndMakeVisible(_temperamentKeyBox);
    _temperamentKeyAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        _processor.getParameterState(), "temperamentKey", _temperamentKeyBox));
    _playUnusedNotesButton.onClick = [this] { _processor.setPlayUnusedNotes(_playUnusedNotesButton.getToggleState()); };
    addAndMakeVisible(_playUnusedNotesButton);
    _temperamentKeyboard.reset(new PluginTemperamentKeyboard(HIGHLIGHT_COLOR));
    _temperamentKeyboard->onEdit = [this] { onTemperamentEdited(); };
    _temperamentKeyboard->onTonalityRequested = [this](int key) { _temperamentKeyBox.setSelectedItemIndex(key); };
    addAndMakeVisible(_temperamentKeyboard.get());

    // Velocity, the graphs following the parameters (changed by the interface or by the host)
    _velocityToFilterLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(_velocityToFilterLabel);
    _velocityToFilterInfo.reset(new InfoIcon());
    addAndMakeVisible(_velocityToFilterInfo.get());
    juce::StringArray velocityToFilterNames = PluginProcessor::getVelocityToFilterNames();
    for (int i = 0; i < velocityToFilterNames.size(); i++)
        _velocityToFilterBox.addItem(velocityToFilterNames[i], i + 1);
    addAndMakeVisible(_velocityToFilterBox);
    _velocityToFilterGraph.reset(new VelocityGraph(VelocityGraph::TYPE_FILTER));
    addAndMakeVisible(_velocityToFilterGraph.get());
    _velocityToFilterBox.onChange = [this] {
        _velocityToFilterGraph->setValue(_velocityToFilterBox.getSelectedItemIndex());
    };
    _velocityToFilterAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        _processor.getParameterState(), "velocityToFilter", _velocityToFilterBox));
    _velocityCurveLabel.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(_velocityCurveLabel);
    _velocityCurveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    _velocityCurveSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    _velocityCurveSlider.setPopupDisplayEnabled(true, false, this); // Value while dragging
    _velocityCurveSlider.setColour(juce::Slider::trackColourId, LIST_COLOR); // Linear in the middle: no filled part
    _velocityCurveSlider.setColour(juce::Slider::backgroundColourId, LIST_COLOR);
    addAndMakeVisible(_velocityCurveSlider);
    _velocityCurveGraph.reset(new VelocityGraph(VelocityGraph::TYPE_CURVE));
    addAndMakeVisible(_velocityCurveGraph.get());
    _velocityCurveSlider.onValueChange = [this] {
        _velocityCurveGraph->setValue(juce::roundToInt(_velocityCurveSlider.getValue()));
    };
    _velocityCurveAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        _processor.getParameterState(), "velocityCurve", _velocityCurveSlider));
    _velocityToFilterGraph->setValue(_velocityToFilterBox.getSelectedItemIndex());
    _velocityCurveGraph->setValue(juce::roundToInt(_velocityCurveSlider.getValue()));

    // Keyboard and what is played
    _keyboard.setKeyWidth(14.0f);
    _keyboard.setOctaveForMiddleC(4); // C4 = 60
    _keyboard.setLowestVisibleKey(36);
    addAndMakeVisible(_keyboard);
    _statusLabel.setColour(juce::Label::textColourId, DIM_TEXT_COLOR);
    addAndMakeVisible(_statusLabel);
    _panicButton.onClick = [this] { _processor.getEngine().panic(); };
    addAndMakeVisible(_panicButton);
    _settingsButton.onClick = [this] { showSettings(); };
    addAndMakeVisible(_settingsButton);

    _processor.addChangeListener(this);
    timerCallback(); // Texts (giving the width of the groups), soundfont and what is played

    setResizable(true, true);
    updateResizeLimits();
    setSize(getRightWidths().column + PRESETS_DEFAULT_WIDTH + 26, getRequiredHeight() + DEFAULT_EXTRA_HEIGHT);
    updateGroups(); // Content of the groups collapsed hidden
    startTimerHz(20);
}

PluginEditor::~PluginEditor()
{
    _processor.removeChangeListener(this);
    setLookAndFeel(nullptr);
}

PluginEditor::Knob * PluginEditor::addKnob(const juce::String &parameterId)
{
    Knob * knob = new Knob();
    _knobs.emplace_back(knob);
    knob->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
    knob->slider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(knob->slider);
    knob->label.setJustificationType(juce::Justification::centred);
    knob->label.setFont(juce::FontOptions(13.0f));
    addAndMakeVisible(knob->label);
    knob->attachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        _processor.getParameterState(), parameterId, knob->slider));
    return knob;
}

juce::String PluginEditor::getPresetText(int program) const
{
    juce::String text = juce::String(program / 128).paddedLeft('0', 3) + ":" +
                        juce::String(program % 128).paddedLeft('0', 3);
    for (const PluginEngine::Preset &preset : _presets)
        if (preset.program == program)
            return text + " " + juce::String::fromUTF8(preset.name.c_str());
    return text + " " + tr("(no instrument)");
}

void PluginEditor::fillTemperamentBox()
{
    // Equal temperament, then the categories in the order of their first temperament (same identifiers as the indexes
    // of the parameter plus 1)
    std::function<juce::PopupMenu(const juce::StringArray &)> buildMenu = [&buildMenu](const juce::StringArray &path) {
        juce::PopupMenu menu;
        juce::StringArray subCategories;
        for (size_t i = 0; i < PluginProcessor::getTemperaments().size(); i++)
        {
            juce::StringArray category = PluginProcessor::getTemperamentCategory(i);
            bool inPath = category.size() >= path.size();
            for (int level = 0; level < path.size() && inPath; level++)
                inPath = (category[level] == path[level]);
            if (!inPath)
                continue;
            if (category.size() == path.size())
                menu.addItem(static_cast<int>(i) + 1, PluginProcessor::getTemperamentName(i));
            else if (!subCategories.contains(category[path.size()]))
            {
                if (path.isEmpty() && subCategories.isEmpty())
                    menu.addSeparator();
                subCategories.add(category[path.size()]);
                juce::StringArray subPath = path;
                subPath.add(category[path.size()]);
                menu.addSubMenu(category[path.size()], buildMenu(subPath));
            }
        }
        return menu;
    };

    // Custom temperament at the end, as in the editor
    _temperamentBox.clear(juce::dontSendNotification);
    juce::PopupMenu menu = buildMenu(juce::StringArray());
    menu.addSeparator();
    menu.addItem(PluginProcessor::getCustomTemperamentIndex() + 1,
                 PluginProcessor::getTemperamentName(static_cast<size_t>(PluginProcessor::getCustomTemperamentIndex())));
    *_temperamentBox.getRootMenu() = menu;
    _temperamentAttachment->sendInitialUpdate();
}

void PluginEditor::updateTexts()
{
    _languageVersion = PluginTranslator::getLanguageVersion();

    _openButton.setButtonText(tr("Open..."));
    _openButton.setTooltip(tr("Open a soundfont (sf2, sf3, sfz or dls), which can also be dropped on the window"));
    _recentButton.setTooltip(tr("Recent soundfonts"));
    _onePresetButton.setButtonText(tr("One instrument"));
    _onePresetButton.setTooltip(tr("The same instrument is played on all MIDI channels"));
    _channelsButton.setButtonText(tr("16 channels"));
    _channelsButton.setTooltip(tr("Each MIDI channel plays its instrument, or the one chosen by its program "
                                  "changes (General MIDI)"));
    _mpeButton.setTooltip(tr("The same instrument is played by an MPE controller: each note has its own pitch "
                             "bend, pressure and timbre (lower zone of 15 channels, or the zones given by the "
                             "controller)"));

    // Knobs, in the order of their creation
    const juce::String knobNames[] = {
        tr("Gain"),
        tr("Level", "reverb"), tr("Size", "reverb"), tr("Width", "reverb"), tr("Damping", "reverb"),
        tr("Level", "chorus"), tr("Depth", "chorus"), tr("Frequency", "chorus")
    };
    for (size_t i = 0; i < _knobs.size() && i < std::size(knobNames); i++)
        _knobs[i]->label.setText(knobNames[i], juce::dontSendNotification);

    // Tuning, the tooltips being the same for a label and its control
    _referencePitchLabel.setText(tr("Reference pitch"), juce::dontSendNotification);
    juce::String referencePitchTooltip = tr("Note sounding at this frequency, whatever the temperament");
    for (juce::SettableTooltipClient * client : std::initializer_list<juce::SettableTooltipClient *> {
             &_referencePitchLabel, &_referencePitchKeyBox, _referencePitchSlider.get() })
        client->setTooltip(referencePitchTooltip);
    _transpositionLabel.setText(tr("Transposition"), juce::dontSendNotification);
    juce::String transpositionTooltip = tr("Transposition in semitones of the notes played, except the drums");
    _transpositionLabel.setTooltip(transpositionTooltip);
    _transpositionBox.setTooltip(transpositionTooltip);
    fillTemperamentBox();
    _temperamentBox.setTooltip(tr("Temperament or scale, transposed to the chosen tonality"));
    _tonalityLabel.setText(tr("Tonality"), juce::dontSendNotification);
    _temperamentKeyBox.setTooltip(tr("Tonality of the temperament (tonic of a scale)"));
    _tonalityLabel.setTooltip(_temperamentKeyBox.getTooltip());
    _playUnusedNotesButton.setButtonText(tr("Play all"));
    _playUnusedNotesButton.setTooltip(tr("Also play the notes not used by the temperament (scale)"));
    _switches[GROUP_TEMPERAMENTS]->setTooltip(tr("Temperament or scale applied (equal temperament otherwise)"));
    _switches[GROUP_REVERB]->setTooltip(tr("Reverb enabled"));
    _switches[GROUP_CHORUS]->setTooltip(tr("Chorus enabled"));

    // Velocity, the tooltips being the same for the label, the control and the graph of a setting
    _velocityToFilterLabel.setText(tr("Filter cutoff"), juce::dontSendNotification);
    _velocityToFilterBox.changeItemText(3, PluginProcessor::getVelocityToFilterNames()[2]);
    juce::String velocityToFilterTooltip = tr("Default modulator from the velocity to the filter cutoff frequency: the "
                                              "softer the note, the lower the cutoff (up to 2 octaves lower).\nChange "
                                              "this option if notes sound too muffled: SoundFont 2.01 for an old "
                                              "soundfont made with this version (the notes above the velocity 64 are not "
                                              "filtered), Disabled for a soundfont whose samples already sound softer at "
                                              "low velocities (several velocity layers).");
    for (juce::SettableTooltipClient * client : std::initializer_list<juce::SettableTooltipClient *> {
             &_velocityToFilterLabel, _velocityToFilterInfo.get(), &_velocityToFilterBox, _velocityToFilterGraph.get() })
        client->setTooltip(velocityToFilterTooltip);
    _velocityCurveLabel.setText(tr("Curve"), juce::dontSendNotification);
    juce::String velocityCurveTooltip = tr("Velocity curve: linear in the middle, concave to the left (softer velocity), "
                                           "convex to the right (velocity rising quickly)");
    for (juce::SettableTooltipClient * client : std::initializer_list<juce::SettableTooltipClient *> {
             &_velocityCurveLabel, &_velocityCurveSlider, _velocityCurveGraph.get() })
        client->setTooltip(velocityCurveTooltip);

    _panicButton.setButtonText(tr("Panic"));
    _panicButton.setTooltip(tr("Stop all sounds and reset the controllers"));
    _settingsButton.setButtonText(tr("Settings"));

    updateStandaloneTexts();
    refresh();
    if (getHeight() > 0)
    {
        // Width of the groups on the right side in this language
        updateResizeLimits();
        if (getWidth() < getRightWidths().column + PRESETS_MINIMUM_WIDTH + 26)
            setSize(getRightWidths().column + PRESETS_MINIMUM_WIDTH + 26, getHeight());
    }
    resized(); // Widths of the buttons
    repaint(); // Titles of the groups
}

void PluginEditor::parentHierarchyChanged()
{
    AudioProcessorEditor::parentHierarchyChanged();
    updateStandaloneTexts();
}

void PluginEditor::updateStandaloneTexts()
{
    // The button "Options" of the window of the standalone application is not translated by JUCE (the items of its
    // menu are): it is the only text button of the window
    if (_processor.wrapperType != juce::AudioProcessor::wrapperType_Standalone)
        return;
    juce::Component * window = getTopLevelComponent();
    if (window == this)
        return;
    for (juce::Component * child : window->getChildren())
    {
        if (juce::TextButton * button = dynamic_cast<juce::TextButton *>(child))
        {
            juce::NamedValueSet &buttonProperties = button->getProperties();
            if (!buttonProperties.contains("sourceText"))
                buttonProperties.set("sourceText", button->getButtonText());
            if (buttonProperties["sourceText"].toString() == "Options")
                button->setButtonText(PluginTranslator::translate("PluginJuce", "Options"));
        }
    }
}

juce::String PluginEditor::translateError(const juce::String &error)
{
    for (const char * text : ERRORS)
        if (error == text)
            return tr(text);
    if (error == "too many soundfonts are still playing, try again in a moment" || error == "file not found")
        return tr(error.toRawUTF8());

    // "cannot open the file <path>" (sfz), "invalid <chunk> index" (sf2)
    const juce::String cannotOpen = "cannot open the file ";
    if (error.startsWith(cannotOpen))
        return PluginTranslator::arg(tr("cannot open the file %1"), error.substring(cannotOpen.length()));
    if (error.startsWith("invalid ") && error.endsWith(" index"))
        return PluginTranslator::arg(tr("invalid %1 index"), error.substring(8, error.length() - 6));
    return error;
}

void PluginEditor::refresh()
{
    PluginEngine &engine = _processor.getEngine();

    // Soundfont: name, then loading, error or file
    juce::File file = _processor.getSoundfontFile();
    juce::String name = juce::String::fromUTF8(engine.getSoundfontInfo("INAM").c_str()).trim();
    if (!engine.hasSoundfont())
        name = file == juce::File() ? tr("No soundfont") : file.getFileName();
    else if (name.isEmpty())
        name = file.getFileNameWithoutExtension();
    _soundfontLabel.setText(name, juce::dontSendNotification);

    juce::String error = _processor.getError();
    if (_processor.isLoading())
    {
        _messageLabel.setText(tr("Loading..."), juce::dontSendNotification);
        _messageLabel.setColour(juce::Label::textColourId, HIGHLIGHT_COLOR);
    }
    else if (error.isNotEmpty())
    {
        _messageLabel.setText(PluginTranslator::arg(tr("Cannot read the soundfont: %1"), translateError(error)) +
                                  (file == juce::File() ? juce::String() : " (" + file.getFullPathName() + ")"),
                              juce::dontSendNotification);
        _messageLabel.setColour(juce::Label::textColourId, ERROR_COLOR);
    }
    else
    {
        _messageLabel.setText(engine.hasSoundfont() ? file.getFullPathName() : tr("Open a soundfont or drop it here"),
                              juce::dontSendNotification);
        _messageLabel.setColour(juce::Label::textColourId, DIM_TEXT_COLOR);
    }
    _soundfontLabel.setTooltip(juce::String::fromUTF8(engine.getSoundfontInfo("ICOP").c_str()));

    // Presets played
    _presets = engine.getPresets();
    PluginEngine::Mode mode = engine.getMode();
    bool isOnePreset = (mode != PluginEngine::MODE_CHANNELS);
    _onePresetButton.setToggleState(mode == PluginEngine::MODE_ONE_PRESET, juce::dontSendNotification);
    _channelsButton.setToggleState(mode == PluginEngine::MODE_CHANNELS, juce::dontSendNotification);
    _mpeButton.setToggleState(mode == PluginEngine::MODE_MPE, juce::dontSendNotification);
    _presetList->setVisible(isOnePreset);
    _channelViewport.setVisible(!isOnePreset);
    _presetList->setPresets(_presets, engine.getProgram());
    for (int channel = 0; channel < 16; channel++)
    {
        ChannelRow * row = _channelRows[static_cast<size_t>(channel)].get();
        row->setPresets(*this, _presets, engine.getChannelProgram(channel));
        row->update(*this, engine.getPlayedProgram(channel), engine.getChannelActivity(channel));
    }

    // Tuning
    updateTuning();
}

void PluginEditor::setMode(PluginEngine::Mode mode)
{
    _processor.getEngine().setMode(mode);
    refresh();
}

void PluginEditor::changeListenerCallback(juce::ChangeBroadcaster *)
{
    refresh();
}

void PluginEditor::timerCallback()
{
    // Language changed (possibly by another instance)
    if (PluginTranslator::getLanguageVersion() != _languageVersion)
        updateTexts();

    // Switches, temperament and tonality possibly changed by the host
    updateGroups();
    updateTuning();

    PluginEngine &engine = _processor.getEngine();
    juce::String load(juce::roundToInt(100.0f * engine.getLoad()));
    _statusLabel.setText(PluginTranslator::arg(tr("Voices: %1"), juce::String(engine.getVoiceCount())) + "    " +
                             PluginTranslator::arg(tr("CPU: %1 %"), load), juce::dontSendNotification);
    if (_channelViewport.isVisible())
        for (int channel = 0; channel < 16; channel++)
            _channelRows[static_cast<size_t>(channel)]->update(*this, engine.getPlayedProgram(channel),
                                                              engine.getChannelActivity(channel));
}

/// Soundfont

void PluginEditor::openFile()
{
    juce::File file = _processor.getSoundfontFile();
    juce::File directory = file.getParentDirectory().isDirectory() ? file.getParentDirectory() :
                               juce::File::getSpecialLocation(juce::File::userHomeDirectory);
    _fileChooser.reset(new juce::FileChooser(tr("Open a soundfont"), directory, "*.sf2;*.sf3;*.sfz;*.dls"));
    _fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                              [this](const juce::FileChooser &chooser) {
        juce::File chosen = chooser.getResult();
        if (chosen.existsAsFile())
            _processor.loadSoundfont(chosen);
    });
}

void PluginEditor::showRecentFiles()
{
    juce::PopupMenu menu;
    juce::StringArray files = _processor.getRecentFiles();
    if (files.isEmpty())
        menu.addItem(-1, tr("No recent soundfont"), false);
    for (int i = 0; i < files.size(); i++)
        menu.addItem(i + 1, juce::File(files[i]).getFileName() + "   (" + juce::File(files[i]).getParentDirectory().getFullPathName() + ")");
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&_recentButton), [this, files](int result) {
        if (result > 0 && result <= files.size())
            _processor.loadSoundfont(juce::File(files[result - 1]));
    });
}

bool PluginEditor::isInterestedInFileDrag(const juce::StringArray &files)
{
    for (const juce::String &file : files)
        if (isSoundfontFile(file))
            return true;
    return false;
}

void PluginEditor::fileDragEnter(const juce::StringArray &, int, int)
{
    _isDragging = true;
    repaint();
}

void PluginEditor::fileDragExit(const juce::StringArray &)
{
    _isDragging = false;
    repaint();
}

void PluginEditor::filesDropped(const juce::StringArray &files, int, int)
{
    _isDragging = false;
    repaint();
    for (const juce::String &file : files)
    {
        if (isSoundfontFile(file))
        {
            _processor.loadSoundfont(juce::File(file));
            break;
        }
    }
}

/// Settings

void PluginEditor::showSettings()
{
    juce::PopupMenu polyphonyMenu;
    for (int polyphony : POLYPHONIES)
        polyphonyMenu.addItem(polyphony, PluginTranslator::arg(tr("%1 voices"), juce::String(polyphony)), true,
                              _processor.getPolyphony() == polyphony);

    juce::PopupMenu channelMenu;
    for (int channel = 1; channel <= 16; channel++)
        channelMenu.addItem(MENU_CHANNEL + channel, PluginTranslator::arg(tr("Channel %1"), juce::String(channel)),
                            true, _keyboard.getMidiChannel() == channel);

    // Languages in their own names, the language of the system being the default one
    std::vector<PluginTranslator::Language> languages = PluginTranslator::getLanguages();
    juce::String languageSetting = PluginTranslator::getLanguageSetting();
    juce::PopupMenu languageMenu;
    languageMenu.addItem(MENU_SYSTEM_LANGUAGE, tr("System language"), true, languageSetting.isEmpty());
    languageMenu.addSeparator();
    for (size_t i = 0; i < languages.size(); i++)
        languageMenu.addItem(MENU_LANGUAGE + static_cast<int>(i), languages[i].name, true,
                             languages[i].code == languageSetting);

    juce::PopupMenu menu;
    menu.addSubMenu(tr("Polyphony"), polyphonyMenu);
    menu.addSubMenu(tr("Keyboard"), channelMenu);
    menu.addSubMenu(tr("Language"), languageMenu);
    menu.addSeparator();
    menu.addItem(MENU_FIXED_LATENCY, tr("Fixed latency (notes played at their exact position)"), true,
                 _processor.hasFixedLatency());
    menu.addItem(MENU_CLIPPING, tr("Clipping to [-1; 1]"), true, _processor.isClipping());
    menu.addSeparator();
    // An sfz file is not stored: its samples are other files
    bool isSfz = _processor.getSoundfontFile().hasFileExtension("sfz");
    menu.addItem(MENU_STORE_SOUNDFONT,
                 isSfz ? tr("Store the soundfont in the project (not possible with an sfz file)") :
                         tr("Store the soundfont in the project"),
                 !isSfz, _processor.isSoundfontStored() && !isSfz);
    // The standalone application of JUCE only plays the main output (AudioProcessorPlayer), the other ones being
    // disabled: all is mixed in the main output
    bool isStandalone = _processor.wrapperType == juce::AudioProcessor::wrapperType_Standalone;
    menu.addItem(MENU_MULTIPLE_OUTPUTS,
                 isStandalone ? tr("Channels and effects on their own outputs (not possible in the standalone "
                                   "application)") :
                                tr("Channels and effects on their own outputs"),
                 !isStandalone, _processor.hasMultipleOutputs() && !isStandalone);
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&_settingsButton), [this, languages](int result) {
        if (result == MENU_STORE_SOUNDFONT)
            _processor.setSoundfontStored(!_processor.isSoundfontStored());
        else if (result == MENU_MULTIPLE_OUTPUTS)
            _processor.setMultipleOutputs(!_processor.hasMultipleOutputs());
        else if (result == MENU_FIXED_LATENCY)
            _processor.setFixedLatency(!_processor.hasFixedLatency());
        else if (result == MENU_CLIPPING)
            _processor.setClipping(!_processor.isClipping());
        else if (result == MENU_SYSTEM_LANGUAGE ||
                 (result >= MENU_LANGUAGE && result < MENU_LANGUAGE + static_cast<int>(languages.size())))
        {
            // Texts in the language chosen at once (the other instances follow with their timer)
            PluginTranslator::setLanguageSetting(result == MENU_SYSTEM_LANGUAGE ? juce::String() :
                                                     languages[static_cast<size_t>(result - MENU_LANGUAGE)].code);
            updateTexts();
        }
        else if (result > MENU_CHANNEL && result <= MENU_CHANNEL + 16)
        {
            _keyboard.setMidiChannel(result - MENU_CHANNEL);
            _keyboard.setMidiChannelsToDisplay(1 << (result - MENU_CHANNEL - 1));
        }
        else if (result > 0 && result <= 4096)
            _processor.setPolyphony(result);
    });
}

/// Drawing

void PluginEditor::paint(juce::Graphics &g)
{
    g.fillAll(WINDOW_COLOR);
    g.setColour(HEADER_COLOR);
    g.fillRect(0, 0, getWidth(), 60);
    if (_logo.isValid())
        g.drawImage(_logo, juce::Rectangle<float>(12.0f, 8.0f, 44.0f, 44.0f), juce::RectanglePlacement::centred);

    // Groups of parameters
    struct Group
    {
        juce::Rectangle<int> area;
        juce::String title;
    };
    for (const Group &group : { Group { _tuningArea, tr("Tuning") },
                                Group { _temperamentsArea, tr("Temperaments and scales") },
                                Group { _velocityArea, tr("Velocity") }, Group { _reverbArea, tr("Reverb") },
                                Group { _chorusArea, tr("Chorus") } })
    {
        g.setColour(HEADER_COLOR);
        g.fillRoundedRectangle(group.area.toFloat(), 6.0f);
        g.setColour(DIM_TEXT_COLOR);
        g.setFont(juce::FontOptions(13.0f, juce::Font::bold));
        g.drawText(group.title, group.area.withHeight(22).reduced(10, 0), juce::Justification::centredLeft);
    }
}

void PluginEditor::paintOverChildren(juce::Graphics &g)
{
    if (_isDragging)
    {
        g.setColour(HIGHLIGHT_COLOR.withAlpha(0.15f));
        g.fillRect(getLocalBounds());
        g.setColour(HIGHLIGHT_COLOR);
        g.drawRect(getLocalBounds(), 3);
    }
}

void PluginEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds();

    // Header: logo, soundfont, gain and buttons
    // The buttons are as wide as their text, at least (translations)
    auto buttonWidth = [](juce::TextButton &button, int minimumWidth) {
        return juce::jmax(minimumWidth, button.getBestWidthForHeight(28));
    };
    juce::Rectangle<int> header = area.removeFromTop(60).reduced(8, 8);
    header.removeFromLeft(56);
    _recentButton.setBounds(header.removeFromRight(28).withSizeKeepingCentre(28, 28));
    int openWidth = buttonWidth(_openButton, 90);
    _openButton.setBounds(header.removeFromRight(openWidth).withSizeKeepingCentre(openWidth, 28));
    header.removeFromRight(8);
    Knob * gain = _knobs[0].get();
    gain->slider.setBounds(header.removeFromRight(44 + 64));
    gain->label.setBounds(header.removeFromRight(getLabelWidth(gain->label)));
    header.removeFromRight(12);
    _soundfontLabel.setBounds(header.removeFromTop(24));
    _messageLabel.setBounds(header);

    // Footer: what is played and buttons, then the keyboard
    juce::Rectangle<int> footer = area.removeFromBottom(36).reduced(8, 4);
    footer.removeFromRight(10); // Corner for resizing the window
    _settingsButton.setBounds(footer.removeFromRight(buttonWidth(_settingsButton, 90)));
    footer.removeFromRight(6);
    _panicButton.setBounds(footer.removeFromRight(buttonWidth(_panicButton, 80)));
    _statusLabel.setBounds(footer);
    _keyboard.setBounds(area.removeFromBottom(90).reduced(8, 2));

    // Presets on the left, groups on the right from the bottom: effects (side by side if both are expanded),
    // temperaments and scales, then the tuning and the velocity side by side in the room left
    RightWidths widths = getRightWidths();
    juce::Rectangle<int> body = area.reduced(8, 8);
    juce::Rectangle<int> left = body.removeFromLeft(juce::jmax(PRESETS_MINIMUM_WIDTH,
                                                               body.getWidth() - 10 - widths.column));
    body.removeFromLeft(10);
    bool areEffectsSideBySide = this->areEffectsSideBySide();
    if (areEffectsSideBySide)
    {
        // Knobs of the same width: the chorus (3 knobs) narrower than the reverb (4 knobs)
        juce::Rectangle<int> effects = body.removeFromBottom(getGroupHeight(GROUP_REVERB));
        int knobWidth = (effects.getWidth() - GROUP_SPACING - 24) / 7;
        _reverbArea = effects.removeFromLeft(4 * knobWidth + 12);
        effects.removeFromLeft(GROUP_SPACING);
        _chorusArea = effects;
    }
    else
    {
        _chorusArea = body.removeFromBottom(getGroupHeight(GROUP_CHORUS));
        body.removeFromBottom(GROUP_SPACING);
        _reverbArea = body.removeFromBottom(getGroupHeight(GROUP_REVERB));
    }
    body.removeFromBottom(GROUP_SPACING);
    _temperamentsArea = body.removeFromBottom(getGroupHeight(GROUP_TEMPERAMENTS));
    body.removeFromBottom(GROUP_SPACING);
    _tuningArea = body.removeFromLeft(widths.tuning);
    body.removeFromLeft(GROUP_SPACING);
    _velocityArea = body;

    // Switches on the right side of the titles
    juce::Rectangle<int> switchedAreas[GROUP_COUNT] = { _temperamentsArea, _reverbArea, _chorusArea };
    for (int group = 0; group < GROUP_COUNT; group++)
        _switches[group]->setBounds(switchedAreas[group].getRight() - 42, switchedAreas[group].getY() + 3, 34, 20);

    // Tuning: reference pitch (note narrower than the frequency), then the transposition
    int labelWidth = juce::jmax(getLabelWidth(_referencePitchLabel), getLabelWidth(_transpositionLabel));
    juce::Rectangle<int> tuning = _tuningArea.reduced(10, 0).withTrimmedTop(4 + GROUP_TITLE_HEIGHT + CONTENT_MARGIN);
    juce::Rectangle<int> row = tuning.removeFromTop(TUNING_ROW_HEIGHT);
    _referencePitchLabel.setBounds(row.removeFromLeft(labelWidth));
    int noteWidth = juce::jmin(70, (row.getWidth() - 16) * 2 / 5);
    _referencePitchKeyBox.setBounds(row.removeFromLeft(noteWidth));
    _equalLabel.setBounds(row.removeFromLeft(16));
    _referencePitchSlider->setBounds(row);
    _referencePitchSlider->setTextBoxStyle(juce::Slider::TextBoxLeft, false, juce::jmax(40, row.getWidth() - 32),
                                          TUNING_ROW_HEIGHT);
    tuning.removeFromTop(TUNING_ROW_SPACING);
    row = tuning.removeFromTop(TUNING_ROW_HEIGHT);
    _transpositionLabel.setBounds(row.removeFromLeft(labelWidth));
    _transpositionBox.setBounds(row.removeFromLeft(juce::jmin(row.getWidth(), 64)));

    // Temperaments and scales: temperament, then tonality and the option of the notes not used on the left, the 12
    // notes on the right
    juce::Rectangle<int> temperaments = _temperamentsArea.reduced(10, 0)
                                            .withTrimmedTop(4 + GROUP_TITLE_HEIGHT + CONTENT_MARGIN)
                                            .withHeight(TEMPERAMENT_KEYBOARD_HEIGHT);
    _temperamentKeyboard->setBounds(temperaments.removeFromRight(_temperamentKeyboard->getIdealWidth()));
    temperaments.removeFromRight(12);
    _temperamentBox.setBounds(temperaments.removeFromTop(TUNING_ROW_HEIGHT));
    temperaments.removeFromTop(TUNING_ROW_SPACING);
    row = temperaments.removeFromTop(TUNING_ROW_HEIGHT);
    _tonalityLabel.setBounds(row.removeFromLeft(getLabelWidth(_tonalityLabel)));
    _temperamentKeyBox.setBounds(row.removeFromLeft(juce::jmin(row.getWidth(), 60)));
    row.removeFromLeft(8);
    _playUnusedNotesButton.setBounds(row.removeFromRight(
        juce::jmin(row.getWidth(), LookAndFeel::getToggleButtonWidth(_playUnusedNotesButton))));

    // Presets
    juce::Rectangle<int> modes = left.removeFromTop(28);
    int modeWidth = modes.getWidth();
    _onePresetButton.setBounds(modes.removeFromLeft(modeWidth * 2 / 5));
    _channelsButton.setBounds(modes.removeFromLeft(modeWidth * 2 / 5));
    _mpeButton.setBounds(modes);
    _onePresetButton.setConnectedEdges(juce::Button::ConnectedOnRight);
    _channelsButton.setConnectedEdges(juce::Button::ConnectedOnLeft | juce::Button::ConnectedOnRight);
    _mpeButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    left.removeFromTop(6);
    _presetList->setBounds(left);
    _channelViewport.setBounds(left);
    int rowWidth = left.getWidth() - _channelViewport.getScrollBarThickness();
    _channelContainer.setSize(rowWidth, 16 * 28);
    for (int channel = 0; channel < 16; channel++)
        _channelRows[static_cast<size_t>(channel)]->setBounds(0, channel * 28, rowWidth, 28);

    // Velocity: two columns (curve, then filter), each one with its label, its graph and its control
    juce::Rectangle<int> velocity = _velocityArea.reduced(6, 4);
    velocity.removeFromTop(GROUP_TITLE_HEIGHT);
    juce::Rectangle<int> velocityColumns[2] = {
        velocity.removeFromLeft(velocity.getWidth() * widths.velocityCurve / (widths.velocityCurve + widths.velocityFilter)),
        velocity };
    juce::Label * velocityLabels[2] = { &_velocityCurveLabel, &_velocityToFilterLabel };
    juce::Component * velocityControls[2] = { &_velocityCurveSlider, &_velocityToFilterBox };
    VelocityGraph * velocityGraphs[2] = { _velocityCurveGraph.get(), _velocityToFilterGraph.get() };
    for (int i = 0; i < 2; i++)
    {
        juce::Rectangle<int> column = velocityColumns[i].reduced(4, 0);
        juce::Rectangle<int> labelRow = column.removeFromTop(18);
        velocityLabels[i]->setBounds(labelRow.removeFromLeft(juce::jmin(labelRow.getWidth(),
                                                                        getLabelWidth(*velocityLabels[i]))));
        if (velocityLabels[i] == &_velocityToFilterLabel)
            _velocityToFilterInfo->setBounds(labelRow.removeFromLeft(16));
        velocityControls[i]->setBounds(column.removeFromBottom(TUNING_ROW_HEIGHT));
        column.removeFromBottom(4);
        velocityGraphs[i]->setBounds(column);
    }

    // Effects: 4 slots for both if they are one below the other (knobs aligned)
    auto layoutKnobs = [this](juce::Rectangle<int> groupArea, size_t first, size_t count, int slots) {
        juce::Rectangle<int> inside = groupArea.reduced(6, 4);
        inside.removeFromTop(GROUP_TITLE_HEIGHT + CONTENT_MARGIN);
        int width = inside.getWidth() / slots;
        for (size_t i = first; i < first + count; i++)
        {
            juce::Rectangle<int> cell = inside.removeFromLeft(width);
            _knobs[i]->label.setBounds(cell.removeFromBottom(16));
            _knobs[i]->slider.setBounds(cell.reduced(2, 0));
        }
    };
    layoutKnobs(_reverbArea, 1, 4, 4);
    layoutKnobs(_chorusArea, 5, 3, areEffectsSideBySide ? 3 : 4);
}

/// Groups that can be collapsed, tuning

int PluginEditor::getGroupHeight(int group) const
{
    // Title, and the content if it is expanded
    if (!_isGroupExpanded[group])
        return COLLAPSED_GROUP_HEIGHT;
    return COLLAPSED_GROUP_HEIGHT + (group == GROUP_TEMPERAMENTS ? TEMPERAMENTS_CONTENT_HEIGHT : EFFECT_CONTENT_HEIGHT);
}

bool PluginEditor::areEffectsSideBySide() const
{
    return _isGroupExpanded[GROUP_REVERB] && _isGroupExpanded[GROUP_CHORUS];
}

int PluginEditor::getRequiredHeight() const
{
    // Tuning and velocity, temperaments and scales, effects (one height if they are side by side)
    int height = TOP_ROW_HEIGHT + GROUP_SPACING + getGroupHeight(GROUP_TEMPERAMENTS) + GROUP_SPACING +
                 getGroupHeight(GROUP_REVERB);
    if (!areEffectsSideBySide())
        height += GROUP_SPACING + getGroupHeight(GROUP_CHORUS);
    return FIXED_HEIGHT + height;
}

PluginEditor::RightWidths PluginEditor::getRightWidths()
{
    // Tuning: labels, note, "=" and frequency
    RightWidths widths;
    widths.tuning = 20 + juce::jmax(getLabelWidth(_referencePitchLabel), getLabelWidth(_transpositionLabel)) + 50 + 16 + 96;

    // Velocity: curve, then the filter (longest item of its list, or its label and its information)
    widths.velocityCurve = juce::jmax(90, getLabelWidth(_velocityCurveLabel));
    juce::Font boxFont = getLookAndFeel().getComboBoxFont(_velocityToFilterBox);
    widths.velocityFilter = getLabelWidth(_velocityToFilterLabel) + 16;
    for (int item = 0; item < _velocityToFilterBox.getNumItems(); item++)
        widths.velocityFilter = juce::jmax(widths.velocityFilter, juce::GlyphArrangement::getStringWidthInt(
                                                                      boxFont, _velocityToFilterBox.getItemText(item)) + 40);

    // Widest row: tuning and velocity, temperaments and scales, effects side by side
    int topRow = widths.tuning + GROUP_SPACING + 20 + widths.velocityCurve + widths.velocityFilter;
    int temperaments = 20 + _temperamentKeyboard->getIdealWidth() + 12 +
                       juce::jmax(160, getLabelWidth(_tonalityLabel) + 60 + 8 +
                                           LookAndFeel::getToggleButtonWidth(_playUnusedNotesButton));
    int effects = 7 * KNOB_WIDTH + 24 + GROUP_SPACING;
    widths.column = juce::jmax(topRow, temperaments, effects);
    return widths;
}

void PluginEditor::updateResizeLimits()
{
    setResizeLimits(getRightWidths().column + PRESETS_MINIMUM_WIDTH + 26, getRequiredHeight(), MAXIMUM_WIDTH,
                    juce::jmax(MAXIMUM_HEIGHT, getRequiredHeight()));
}

void PluginEditor::updateGroups()
{
    // Groups expanded if they are switched on (by the interface or by the host), the window growing or shrinking by
    // the height of their content so that the other parts keep their sizes
    int previousRequiredHeight = getRequiredHeight();
    bool hasChanged = false;
    for (int group = 0; group < GROUP_COUNT; group++)
    {
        bool isExpanded = _processor.getParameterState().getRawParameterValue(SWITCH_IDS[group])->load() > 0.5f;
        hasChanged = hasChanged || isExpanded != _isGroupExpanded[group];
        _isGroupExpanded[group] = isExpanded;
    }
    for (juce::Component * component : std::initializer_list<juce::Component *> {
             &_temperamentBox, &_tonalityLabel, &_temperamentKeyBox, &_playUnusedNotesButton, _temperamentKeyboard.get() })
        component->setVisible(_isGroupExpanded[GROUP_TEMPERAMENTS]);
    for (size_t knob = 1; knob < _knobs.size(); knob++)
    {
        bool isVisible = _isGroupExpanded[knob <= 4 ? GROUP_REVERB : GROUP_CHORUS];
        _knobs[knob]->slider.setVisible(isVisible);
        _knobs[knob]->label.setVisible(isVisible);
    }

    if (!hasChanged)
        return;
    int delta = getRequiredHeight() - previousRequiredHeight;
    int height = getHeight() + delta;
    if (delta > 0)
    {
        setSize(getWidth(), height);
        updateResizeLimits();
    }
    else
    {
        updateResizeLimits();
        setSize(getWidth(), height);
    }
    resized(); // Also if the size did not change
    repaint();
}

void PluginEditor::updateTuning()
{
    // Temperament shown from C to B, the tonality being grayed for the equal temperament
    juce::AudioProcessorValueTreeState &parameters = _processor.getParameterState();
    int temperament = juce::jlimit(0, PluginProcessor::getCustomTemperamentIndex(),
                                   juce::roundToInt(parameters.getRawParameterValue("temperament")->load()));
    int tonality = juce::jlimit(0, 11, juce::roundToInt(parameters.getRawParameterValue("temperamentKey")->load()));
    bool isTonalityUsed = (temperament != 0);
    float fineTunes[12];
    bool used[12];
    _processor.getTemperament(temperament, tonality, fineTunes, used);
    _temperamentKeyboard->setTemperament(fineTunes, used, isTonalityUsed ? tonality : -1);
    _tonalityLabel.setEnabled(isTonalityUsed);
    _temperamentKeyBox.setEnabled(isTonalityUsed);
    _playUnusedNotesButton.setToggleState(_processor.playsUnusedNotes(), juce::dontSendNotification);

    // Note of the reference pitch, with its frequency
    int referencePitchKey = _processor.getReferencePitchKey();
    _temperamentKeyboard->setReferencePitchKey(referencePitchKey);
    if (_referencePitchKeyBox.getSelectedId() != referencePitchKey + 1)
    {
        _referencePitchKeyBox.setSelectedId(referencePitchKey + 1, juce::dontSendNotification);
        _referencePitchSlider->updateText();
    }
}

void PluginEditor::onTemperamentEdited()
{
    // The temperament shown becomes the custom temperament, stored from its tonic
    int tonality = juce::jlimit(0, 11, juce::roundToInt(
                                           _processor.getParameterState().getRawParameterValue("temperamentKey")->load()));
    const float * values = _temperamentKeyboard->getValues();
    const bool * used = _temperamentKeyboard->getUsed();
    float fineTunes[12];
    bool tonicUsed[12];
    for (int i = 0; i < 12; i++)
    {
        fineTunes[i] = values[(i + tonality) % 12];
        tonicUsed[i] = used[(i + tonality) % 12];
    }
    _processor.setCustomTemperament(fineTunes, tonicUsed);
    if (_temperamentBox.getSelectedId() != PluginProcessor::getCustomTemperamentIndex() + 1)
        _temperamentAttachment->setValueAsCompleteGesture(static_cast<float>(PluginProcessor::getCustomTemperamentIndex()));
    updateTuning();
}
