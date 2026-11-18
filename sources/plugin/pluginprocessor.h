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


#ifndef PLUGINPROCESSOR_H
#define PLUGINPROCESSOR_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <atomic>
#include "pluginengine.h"
#include "plugintranslator.h"
#include "temperaments.h"
class PluginSoundfont;

// Polyphone as a plugin: a soundfont (sf2 or sf3) played by the synth of the editor on the 16 MIDI channels.
// The parameters of the synth can be automated, the soundfont and the presets are saved in the state of the plugin,
// possibly with the content of the file. The editor (PluginEditor) is notified of the changes as a ChangeListener.
class PluginProcessor: public juce::AudioProcessor,
                       public juce::ChangeBroadcaster,
                       private juce::Timer
{
    Q_DECLARE_TR_FUNCTIONS(PluginProcessor)

public:
    PluginProcessor();
    ~PluginProcessor() override;

    /// Soundfont

    // Read a file in another thread, the editor being notified when it is played (or when it failed)
    void loadSoundfont(const juce::File &file);
    bool isLoading() const { return _isLoading.load(); }

    // File played (possibly missing if its content was stored in the state), error of the last loading (in English, see
    // PluginEditor::translateError)
    juce::File getSoundfontFile() const;
    juce::String getError() const;

    // Files read recently, shared by all instances
    juce::StringArray getRecentFiles();

    // The content of the file can be stored in the state (the project of the host) so that it can be played without
    // the file
    bool isSoundfontStored() const { return _isSoundfontStored; }
    void setSoundfontStored(bool isStored);

    /// Settings

    // Maximum number of voices (the notes stop when it changes)
    int getPolyphony() const { return _polyphony; }
    void setPolyphony(int polyphony);

    // With multiple outputs, the channels and the effects go to their own outputs when the host enabled them (the part of
    // the voices of a channel without effect, the part sent to the effects after them). Otherwise, or if the output is
    // disabled, they are in the main output.
    bool hasMultipleOutputs() const { return _hasMultipleOutputs.load(); }
    void setMultipleOutputs(bool hasMultipleOutputs);

    // In real time, the MIDI messages of a block are moved together so that its first note starts at the beginning of
    // the block (default): the latency is lower on average but varies. With a fixed latency, each message is processed
    // at its frame, as offline.
    bool hasFixedLatency() const { return _hasFixedLatency.load(); }
    void setFixedLatency(bool hasFixedLatency);

    /// Tuning, completing the parameters "transpose", "tuning" (frequency of A4), "temperamentEnabled" (equal
    /// temperament otherwise), "temperament" and "temperamentKey" (tonality)

    // Note of the reference pitch as in the editor, from A#3 to A4 (0 = C4, 9 = A4, 10 = A#3, 11 = B3): it sounds at
    // the frequency given by the parameter "tuning" in the equal temperament, whatever the temperament
    int getReferencePitchKey() const { return _referencePitchKey.load(); }
    void setReferencePitchKey(int key);
    static double getRatioToA4(int key); // Frequency of the note divided by the frequency of A4

    // Custom temperament, last item of the parameter "temperament": fine tunes and use of the notes from its tonic
    static int getCustomTemperamentIndex();
    void getCustomTemperament(float fineTunes[12], bool used[12]) const;
    void setCustomTemperament(const float fineTunes[12], const bool used[12]);

    // Fine tunes and use of the notes from C to B of a temperament (index of the parameter "temperament") transposed to
    // a tonality
    void getTemperament(int index, int tonality, float fineTunes[12], bool used[12]) const;

    // Notes not used by the temperament (scale) played anyway
    bool playsUnusedNotes() const { return _playUnusedNotes.load(); }
    void setPlayUnusedNotes(bool playUnusedNotes);

    // Outputs clipped to [-1; 1] (default) or not
    bool isClipping() const { return _isClipping.load(); }
    void setClipping(bool isClipping);

    PluginEngine &getEngine() { return _engine; }
    juce::AudioProcessorValueTreeState &getParameterState() { return _parameters; }
    juce::MidiKeyboardState &getKeyboardState() { return _keyboardState; }
    // Equal temperament, then the temperaments and scales of the editor (resources/misc/temperaments.csv)
    static const std::vector<TemperamentData> &getTemperaments();
    static juce::String getTemperamentName(size_t index); // Translated, the custom temperament included
    static juce::StringArray getTemperamentCategory(size_t index); // Levels, translated (empty: top level)

    // Default modulators of the velocity to the filter cutoff (SynthCoreConfig::modulatorVelToFilterType): SoundFont
    // 2.01, SoundFont 2.04 and disabled (translated)
    static juce::StringArray getVelocityToFilterNames();

    // Settings shared by all instances (recent files, language)
    static juce::PropertiesFile::Options getSettingsOptions();

    /// AudioProcessor implementation

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;
    void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;
    using AudioProcessor::processBlock;

    juce::AudioProcessorEditor * createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return true; }
    bool supportsMPE() const override { return true; } // Mode MPE
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String &) override {}

    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void * data, int sizeInBytes) override;

private:
    static BusesProperties createBuses();
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    void timerCallback() override;
    void updateConfiguration();
    void setSoundfont(std::shared_ptr<PluginSoundfont> soundfont, const juce::File &file, const juce::String &error);

    PluginEngine _engine;
    juce::AudioProcessorValueTreeState _parameters;
    juce::MidiKeyboardState _keyboardState;
    juce::ThreadPool _loadingThread;
    juce::ApplicationProperties _settings;

    // Soundfont
    mutable juce::CriticalSection _soundfontLock;
    juce::File _soundfontFile;
    std::shared_ptr<PluginSoundfont> _soundfont; // Possibly read from the content stored in the state (missing file)
    juce::String _error;
    std::atomic<bool> _isLoading;
    std::atomic<int> _loadingCount; // Loadings started, the last one only being used
    bool _isSoundfontStored;

    // Settings of the synth
    static const int PARAMETER_COUNT = 17;
    int _polyphony;
    std::atomic<bool> _hasMultipleOutputs;
    std::atomic<bool> _hasFixedLatency;
    std::atomic<bool> _isClipping;
    std::atomic<bool> _playUnusedNotes;
    bool _appliedPlayUnusedNotes; // Audio thread
    std::atomic<int> _referencePitchKey;
    std::atomic<float> _customFineTunes[12];
    std::atomic<bool> _customUsed[12];
    std::atomic<int> _tuningVersion; // Reference pitch key or custom temperament changed: applied by the next block
    int _appliedTuningVersion; // Audio thread
    bool _isPrepared;
    double _sampleRate;
    int _maxBlockSize;
    std::atomic<float> * _parameterValues[PARAMETER_COUNT];
    float _appliedValues[PARAMETER_COUNT];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};

#endif // PLUGINPROCESSOR_H
