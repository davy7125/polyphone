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
        _programBox.setTooltip("Preset played by the channel, or the one chosen by the program changes it receives");
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
        _programBox.clear(juce::dontSendNotification);
        _programBox.addItem(_channel == 9 ? "Program change (drums)" : "Program change", 1);
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
    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
};

/// Editor

PluginEditor::PluginEditor(PluginProcessor &pluginProcessor) :
    AudioProcessorEditor(&pluginProcessor),
    _processor(pluginProcessor),
    _lookAndFeel(new LookAndFeel()),
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
    _openButton.setButtonText("Open...");
    _openButton.setTooltip("Open a soundfont (sf2, sf3, sfz or dls), which can also be dropped on the window");
    _openButton.onClick = [this] { openFile(); };
    addAndMakeVisible(_openButton);
    _recentButton.setButtonText(juce::String::fromUTF8("\xe2\x96\xbe"));
    _recentButton.setTooltip("Recent soundfonts");
    _recentButton.onClick = [this] { showRecentFiles(); };
    addAndMakeVisible(_recentButton);

    // Presets played
    _onePresetButton.setButtonText("One preset");
    _onePresetButton.setTooltip("The same preset is played on all MIDI channels");
    _onePresetButton.setClickingTogglesState(false);
    _onePresetButton.onClick = [this] { setMode(PluginEngine::MODE_ONE_PRESET); };
    addAndMakeVisible(_onePresetButton);
    _channelsButton.setButtonText("16 channels");
    _channelsButton.setTooltip("Each MIDI channel plays its preset, or the one chosen by its program changes (General MIDI)");
    _channelsButton.onClick = [this] { setMode(PluginEngine::MODE_CHANNELS); };
    addAndMakeVisible(_channelsButton);
    _mpeButton.setButtonText("MPE");
    _mpeButton.setTooltip("The same preset is played by an MPE controller: each note has its own pitch bend, pressure and "
                          "timbre (lower zone of 15 channels, or the zones given by the controller)");
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

    // Parameters
    addKnob("gain", "Gain");
    addKnob("transpose", "Transpose");
    addKnob("tuning", "Tuning");
    addKnob("reverbLevel", "Level");
    addKnob("reverbSize", "Size");
    addKnob("reverbWidth", "Width");
    addKnob("reverbDamping", "Damping");
    addKnob("chorusLevel", "Level");
    addKnob("chorusDepth", "Depth");
    addKnob("chorusFrequency", "Frequency");
    _temperamentLabel.setText("Temperament", juce::dontSendNotification);
    _temperamentLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(_temperamentLabel);
    int id = 1;
    for (const PluginProcessor::Temperament &temperament : PluginProcessor::getTemperaments())
        _temperamentBox.addItem(temperament.name, id++);
    _temperamentBox.setTooltip("Temperament, its fine tunes being relative to the key chosen on the right");
    addAndMakeVisible(_temperamentBox);
    _temperamentAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        _processor.getParameterState(), "temperament", _temperamentBox));
    id = 1;
    for (const char * key : { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" })
        _temperamentKeyBox.addItem(key, id++);
    _temperamentKeyBox.setTooltip("Key of reference of the temperament");
    addAndMakeVisible(_temperamentKeyBox);
    _temperamentKeyAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        _processor.getParameterState(), "temperamentKey", _temperamentKeyBox));

    // Keyboard and what is played
    _keyboard.setKeyWidth(14.0f);
    _keyboard.setOctaveForMiddleC(4); // C4 = 60
    _keyboard.setLowestVisibleKey(36);
    addAndMakeVisible(_keyboard);
    _statusLabel.setColour(juce::Label::textColourId, DIM_TEXT_COLOR);
    addAndMakeVisible(_statusLabel);
    _panicButton.setButtonText("Panic");
    _panicButton.setTooltip("Stop all sounds and reset the controllers");
    _panicButton.onClick = [this] { _processor.getEngine().panic(); };
    addAndMakeVisible(_panicButton);
    _settingsButton.setButtonText("Settings");
    _settingsButton.onClick = [this] { showSettings(); };
    addAndMakeVisible(_settingsButton);

    setResizable(true, true);
    setResizeLimits(760, 520, 1800, 1400);
    setSize(900, 600);

    _processor.addChangeListener(this);
    refresh();
    timerCallback();
    startTimerHz(20);
}

PluginEditor::~PluginEditor()
{
    _processor.removeChangeListener(this);
    setLookAndFeel(nullptr);
}

PluginEditor::Knob * PluginEditor::addKnob(const juce::String &parameterId, const juce::String &name)
{
    Knob * knob = new Knob();
    _knobs.emplace_back(knob);
    knob->slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 72, 18);
    knob->slider.setPopupDisplayEnabled(false, false, this);
    addAndMakeVisible(knob->slider);
    knob->label.setText(name, juce::dontSendNotification);
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
    return text + " (no preset)";
}

void PluginEditor::refresh()
{
    PluginEngine &engine = _processor.getEngine();

    // Soundfont: name, then loading, error or file
    juce::File file = _processor.getSoundfontFile();
    juce::String name = juce::String::fromUTF8(engine.getSoundfontInfo("INAM").c_str()).trim();
    if (!engine.hasSoundfont())
        name = file == juce::File() ? juce::String("No soundfont") : file.getFileName();
    else if (name.isEmpty())
        name = file.getFileNameWithoutExtension();
    _soundfontLabel.setText(name, juce::dontSendNotification);

    juce::String error = _processor.getError();
    if (_processor.isLoading())
    {
        _messageLabel.setText("Loading...", juce::dontSendNotification);
        _messageLabel.setColour(juce::Label::textColourId, HIGHLIGHT_COLOR);
    }
    else if (error.isNotEmpty())
    {
        _messageLabel.setText("Cannot read the soundfont: " + error +
                                  (file == juce::File() ? juce::String() : " (" + file.getFullPathName() + ")"),
                              juce::dontSendNotification);
        _messageLabel.setColour(juce::Label::textColourId, ERROR_COLOR);
    }
    else
    {
        _messageLabel.setText(engine.hasSoundfont() ? file.getFullPathName() :
                                  juce::String("Open a soundfont or drop it here"), juce::dontSendNotification);
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
    PluginEngine &engine = _processor.getEngine();
    _statusLabel.setText("Voices: " + juce::String(engine.getVoiceCount()) + "    CPU: " +
                             juce::String(juce::roundToInt(100.0f * engine.getLoad())) + " %",
                         juce::dontSendNotification);
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
    _fileChooser.reset(new juce::FileChooser("Open a soundfont", directory, "*.sf2;*.sf3;*.sfz;*.dls"));
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
        menu.addItem(-1, "No recent soundfont", false);
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
        polyphonyMenu.addItem(polyphony, juce::String(polyphony) + " voices", true, _processor.getPolyphony() == polyphony);

    juce::PopupMenu channelMenu;
    for (int channel = 1; channel <= 16; channel++)
        channelMenu.addItem(10000 + channel, "Channel " + juce::String(channel), true,
                            _keyboard.getMidiChannel() == channel);

    juce::PopupMenu menu;
    menu.addSubMenu("Polyphony", polyphonyMenu);
    menu.addSubMenu("Keyboard", channelMenu);
    // An sfz file is not stored: its samples are other files
    bool isSfz = _processor.getSoundfontFile().hasFileExtension("sfz");
    menu.addItem(20000, isSfz ? "Store the soundfont in the project (not possible with an sfz file)" :
                                "Store the soundfont in the project", !isSfz, _processor.isSoundfontStored() && !isSfz);
    menu.addItem(20001, "Channels and effects on their own outputs", true, _processor.hasMultipleOutputs());
    menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&_settingsButton), [this](int result) {
        if (result == 20000)
            _processor.setSoundfontStored(!_processor.isSoundfontStored());
        else if (result == 20001)
            _processor.setMultipleOutputs(!_processor.hasMultipleOutputs());
        else if (result > 10000 && result <= 10016)
        {
            _keyboard.setMidiChannel(result - 10000);
            _keyboard.setMidiChannelsToDisplay(1 << (result - 10001));
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
        const char * title;
    };
    for (const Group &group : { Group { _synthArea, "Synth" }, Group { _reverbArea, "Reverb" }, Group { _chorusArea, "Chorus" } })
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

    // Header: logo, soundfont and buttons
    juce::Rectangle<int> header = area.removeFromTop(60).reduced(8, 8);
    header.removeFromLeft(56);
    _recentButton.setBounds(header.removeFromRight(28).withSizeKeepingCentre(28, 28));
    header.removeFromRight(2);
    _openButton.setBounds(header.removeFromRight(90).withSizeKeepingCentre(90, 28));
    header.removeFromRight(8);
    _soundfontLabel.setBounds(header.removeFromTop(24));
    _messageLabel.setBounds(header);

    // Footer: what is played and buttons, then the keyboard
    juce::Rectangle<int> footer = area.removeFromBottom(36).reduced(8, 4);
    footer.removeFromRight(10); // Corner for resizing the window
    _settingsButton.setBounds(footer.removeFromRight(90));
    footer.removeFromRight(6);
    _panicButton.setBounds(footer.removeFromRight(80));
    _statusLabel.setBounds(footer);
    _keyboard.setBounds(area.removeFromBottom(90).reduced(8, 2));

    // Presets on the left
    juce::Rectangle<int> body = area.reduced(8, 8);
    juce::Rectangle<int> left = body.removeFromLeft(body.getWidth() * 45 / 100);
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

    // Parameters on the right, by group
    body.removeFromLeft(10);
    int groupHeight = (body.getHeight() - 16) / 3;
    _synthArea = body.removeFromTop(groupHeight);
    body.removeFromTop(8);
    _reverbArea = body.removeFromTop(groupHeight);
    body.removeFromTop(8);
    _chorusArea = body;

    auto layoutKnobs = [this](juce::Rectangle<int> groupArea, size_t first, size_t count, int slots) {
        juce::Rectangle<int> inside = groupArea.reduced(6, 4);
        inside.removeFromTop(18);
        int width = inside.getWidth() / slots;
        for (size_t i = first; i < first + count; i++)
        {
            juce::Rectangle<int> cell = inside.removeFromLeft(width);
            _knobs[i]->label.setBounds(cell.removeFromBottom(16));
            _knobs[i]->slider.setBounds(cell.reduced(2, 0));
        }
        return inside;
    };
    juce::Rectangle<int> temperamentArea = layoutKnobs(_synthArea, 0, 3, 4).reduced(4, 6);
    _temperamentLabel.setBounds(temperamentArea.removeFromTop(20));
    _temperamentBox.setBounds(temperamentArea.removeFromTop(26));
    temperamentArea.removeFromTop(6);
    _temperamentKeyBox.setBounds(temperamentArea.removeFromTop(26).withSizeKeepingCentre(70, 26));
    layoutKnobs(_reverbArea, 3, 4, 4);
    layoutKnobs(_chorusArea, 7, 3, 4);
}
