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
class PluginTemperamentKeyboard;

// Interface of the plugin: the soundfont (open, recent files, drag and drop) and the gain, the presets played (one for
// all channels or one for each channel) on the left, the note (play mode, tuning) and the velocity, the temperaments
// and scales and the effects on the right, a keyboard and what is played
class PluginEditor: public juce::AudioProcessorEditor,
                    public juce::FileDragAndDropTarget,
                    private juce::ChangeListener,
                    private juce::Timer
{
    Q_DECLARE_TR_FUNCTIONS(PluginEditor)

public:
    explicit PluginEditor(PluginProcessor &pluginProcessor);
    ~PluginEditor() override;

    void paint(juce::Graphics &g) override;
    void paintOverChildren(juce::Graphics &g) override;
    void resized() override;
    void parentHierarchyChanged() override;

    // FileDragAndDropTarget implementation
    bool isInterestedInFileDrag(const juce::StringArray &files) override;
    void fileDragEnter(const juce::StringArray &files, int x, int y) override;
    void fileDragExit(const juce::StringArray &files) override;
    void filesDropped(const juce::StringArray &files, int x, int y) override;

private:
    class LookAndFeel;
    class PresetList;
    class ChannelRow;
    class VelocityGraph;
    class SwitchButton;
    class FieldSlider;
    class BarSlider;
    class InfoIcon;
    class LoadingSpinner;
    struct RightWidths;
    struct Knob;

    // List whose item is changed by the mouse wheel, one item per notch
    class WheelComboBox: public juce::ComboBox
    {
    public:
        void mouseWheelMove(const juce::MouseEvent &event, const juce::MouseWheelDetails &wheel) override;

    private:
        float _wheelDelta = 0.0f; // Remainder of the movements of a smooth wheel (trackpad)
    };

    void changeListenerCallback(juce::ChangeBroadcaster * source) override;
    void timerCallback() override;
    void updateTexts(); // In the current language
    void fillTemperamentBox(); // Temperaments in the submenus of their categories
    void updateTuning(); // Temperament shown, tonality, note of the reference pitch, temperaments shown or hidden
    void updatePlayMode(); // Glide shown with the legato, play mode enabled except with the MPE
    int getPlayModeBoxWidth(); // Longest play mode
    int getGlideTextWidth(); // Longest text of the glide
    void onTemperamentEdited(); // The fine tunes shown become the custom temperament
    void updateGroups(); // Groups expanded if they are switched on, the window growing or shrinking
    void updateResizeLimits();
    int getGroupHeight(int group) const; // Group with a switch, expanded or not
    bool areEffectsSideBySide() const; // Both effects expanded
    int getRequiredHeight() const; // Height of the window needed by the groups on the right side
    RightWidths getRightWidths(); // Width needed by the groups on the right side (texts in the current language)
    void updateStandaloneTexts();
    void refresh(); // Soundfont, presets and settings
    void updateLoading(); // Spinner shown while the soundfont is read
    void openFile();
    void showRecentFiles();
    void showSettings();
    void setMode(PluginEngine::Mode mode);
    juce::String getPresetText(int program) const; // "bank:number name"
    static juce::String translateError(const juce::String &error);
    Knob * addKnob(const juce::String &parameterId);

    PluginProcessor &_processor;
    std::unique_ptr<LookAndFeel> _lookAndFeel;
    juce::TooltipWindow _tooltipWindow;
    juce::Image _logo;
    int _languageVersion;

    // Soundfont
    std::unique_ptr<LoadingSpinner> _loadingSpinner;
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

    // Knobs: gain in the header, effects
    std::vector<std::unique_ptr<Knob>> _knobs;
    juce::Rectangle<int> _reverbArea, _chorusArea;

    // Groups with a switch at the right side of their title (temperaments and scales, reverb, chorus), expanded if it
    // is on (the temperament or the effect being then applied)
    enum SwitchedGroup
    {
        GROUP_TEMPERAMENTS,
        GROUP_REVERB,
        GROUP_CHORUS,
        GROUP_COUNT
    };
    std::unique_ptr<SwitchButton> _switches[GROUP_COUNT];
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> _switchAttachments[GROUP_COUNT];
    bool _isGroupExpanded[GROUP_COUNT];

    // Note: play mode (polyphonic, monophonic or legato with a priority) with the glide of the legato on its right,
    // reference pitch (note = its frequency, the parameter being the frequency of A4) and transposition, then the
    // temperaments and scales (temperament, tonality, fine tunes of the 12 notes, notes not used)
    WheelComboBox _playModeBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _playModeAttachment;
    std::unique_ptr<BarSlider> _glideSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _glideAttachment;
    juce::Label _referencePitchLabel;
    WheelComboBox _referencePitchKeyBox;
    juce::Label _equalLabel;
    std::unique_ptr<FieldSlider> _referencePitchSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _referencePitchAttachment;
    juce::Label _transpositionLabel;
    WheelComboBox _transpositionBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _transpositionAttachment;
    WheelComboBox _temperamentBox;
    std::unique_ptr<juce::ParameterAttachment> _temperamentAttachment; // By identifier: items in submenus
    juce::Label _tonalityLabel;
    WheelComboBox _temperamentKeyBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _temperamentKeyAttachment;
    juce::ToggleButton _playUnusedNotesButton;
    std::unique_ptr<PluginTemperamentKeyboard> _temperamentKeyboard;
    juce::Rectangle<int> _tuningArea, _temperamentsArea;

    // Velocity: default modulator to the filter cutoff and curve, with their graphs
    juce::Label _velocityToFilterLabel;
    std::unique_ptr<InfoIcon> _velocityToFilterInfo;
    WheelComboBox _velocityToFilterBox;
    std::unique_ptr<VelocityGraph> _velocityToFilterGraph;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> _velocityToFilterAttachment;
    juce::Label _velocityCurveLabel;
    juce::Slider _velocityCurveSlider;
    std::unique_ptr<VelocityGraph> _velocityCurveGraph;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> _velocityCurveAttachment;
    juce::Rectangle<int> _velocityArea;

    // Keyboard and what is played
    juce::MidiKeyboardComponent _keyboard;
    juce::Label _statusLabel;
    juce::TextButton _panicButton;
    juce::TextButton _settingsButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};

#endif // PLUGINEDITOR_H
