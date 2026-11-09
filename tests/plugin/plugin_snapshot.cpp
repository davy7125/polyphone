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


// Interface of the plugin drawn in images without a display, the sources of the plugin being compiled here (see
// README.md):
//   plugin_snapshot <soundfont> <output png prefix>

#include <juce_audio_processors/juce_audio_processors.h>
#include <cstdio>
#include "pluginprocessor.h"

static void setState(PluginProcessor &processor, const juce::String &soundfont, int mode)
{
    juce::ValueTree state("PolyphonePlugin");
    state.setProperty("version", 1, nullptr);
    state.setProperty("mode", mode, nullptr);
    state.setProperty("channelPrograms", "-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1", nullptr);
    juce::ValueTree soundfontTree("Soundfont");
    soundfontTree.setProperty("path", soundfont, nullptr);
    state.appendChild(soundfontTree, nullptr);
    juce::MemoryBlock data;
    juce::MemoryOutputStream stream(data, false);
    state.writeToStream(stream);
    processor.setStateInformation(data.getData(), static_cast<int>(data.getSize()));
}

int main(int argc, char ** argv)
{
    juce::ScopedJuceInitialiser_GUI juce;
    if (argc != 3)
    {
        printf("Usage: %s <soundfont> <output png prefix>\n", argv[0]);
        return 2;
    }

    PluginProcessor processor;
    processor.prepareToPlay(48000, 512);
    for (int mode : { 0, 1 })
    {
        setState(processor, juce::String::fromUTF8(argv[1]), mode);

        // A few notes played, shown by the keyboard
        juce::AudioBuffer<float> buffer(2, 512);
        juce::MidiBuffer midi;
        for (int key : { 60, 64, 67 })
            midi.addEvent(juce::MidiMessage::noteOn(1, key, static_cast<juce::uint8>(100)), 0);
        processor.processBlock(buffer, midi);

        std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());
        juce::Image image = editor->createComponentSnapshot(editor->getLocalBounds());
        juce::File file(juce::String::fromUTF8(argv[2]) + (mode == 0 ? "_one_preset.png" : "_channels.png"));
        file.deleteFile();
        juce::FileOutputStream output(file);
        juce::PNGImageFormat().writeImageToStream(image, output);
        printf("WROTE %s (%dx%d)\n", file.getFullPathName().toRawUTF8(), image.getWidth(), image.getHeight());
    }
    processor.releaseResources();
    return 0;
}
