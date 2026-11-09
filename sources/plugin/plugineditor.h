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


#ifndef PLUGINEDITOR_H
#define PLUGINEDITOR_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "pluginprocessor.h"

// Interface of the plugin: the soundfont (open, recent files, drag and drop), the presets played (one for all channels
// or one for each channel), the parameters of the synth, a keyboard and what is played
class PluginEditor: public juce::AudioProcessorEditor,
                    public juce::FileDragAndDropTarget,
                    private juce::ChangeListener,
                    private juce::Timer
{
public:
    explicit PluginEditor(PluginProcessor &pluginProcessor);
    ~PluginEditor() override;

    void paint(juce::Graphics &g) override;
    void paintOverChildren(juce::Graphics &g) override;
    void resized() override;

    // FileDragAndDropTarget implementation
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

private:
    class LookAndFeel;
    class PresetList;
    class ChannelRow;
    struct Knob;

    void changeListenerCallback(juce::ChangeBroadcaster * source) override;
    void timerCallback() override;
    void refresh(); // Soundfont, presets and settings
    void openFile();
    void showRecentFiles();
    void showSettings();
    void setMode(PluginEngine::Mode mode);
    juce::String getPresetText(int program) const; // "bank:number name"
    Knob * addKnob(const juce::String &parameterId, const juce::String &name);

    PluginProcessor &_processor;
    std::unique_ptr<LookAndFeel> _lookAndFeel;
    juce::TooltipWindow _tooltipWindow;
    juce::Image _logo;

    // Soundfont
    juce::Label _soundfontLabel;
    juce::Label _messageLabel;
    juce::TextButton _openButton;
    juce::TextButton _recentButton;
    std::unique_ptr<juce::FileChooser> _fileChooser;
    std::vector<PluginEngine::Preset> _presets;
    bool _isDragging;

    // Presets played
    juce::TextButton _onePresetButton;
    juce::TextButton _channelsButton;
    juce::TextButton _mpeButton;
    std::unique_ptr<PresetList> _presetList;
    juce::Viewport _channelViewport;
    juce::Component _channelContainer;
    std::vector<std::unique_ptr<ChannelRow>> _channelRows;

    // Parameters
    std::vector<std::unique_ptr<Knob>> _knobs;
    juce::Label _temperamentLabel;
    juce::ComboBox _temperamentBox;
    juce::ComboBox _temperamentKeyBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _temperamentAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _temperamentKeyAttachment;
    juce::Rectangle<int> _synthArea, _reverbArea, _chorusArea;

    // Keyboard and what is played
    juce::MidiKeyboardComponent _keyboard;
    juce::Label _statusLabel;
    juce::TextButton _panicButton;
    juce::TextButton _settingsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

#endif // PLUGINEDITOR_H
