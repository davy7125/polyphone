# Tests of the plugin

The plugins built from `sources/plugin` are loaded as a host does, without a DAW. The tools are built
with the same JUCE as the plugin:

```bash
cmake -S tests/plugin -B build/plugin_tests -DPOLYPHONE_JUCE_DIR=<JUCE of build/plugin/_deps/juce-src>
cmake --build build/plugin_tests
```

(without `POLYPHONE_JUCE_DIR`, JUCE is downloaded). The headers of the CLAP format are downloaded.

## Same sound as the editor

`run_cases.sh` plays the cases of `tests/render` through a plugin, as `OfflineRenderer` does: a first
block of silence, then the MIDI messages of each block at its beginning (the channels choosing their
presets with the program changes), offline. The renderings are compared with the references rendered
by the editor (see `tests/render/README.md`):

```bash
tests/render/render.sh <polyphone> <soundfont directory> ~/polyphone-render/reference
host=build/plugin_tests/plugin_host_artefacts/Release/plugin_host
plugins=build/plugin/PolyphonePlugin_artefacts/Release
tests/plugin/run_cases.sh $host $plugins/VST3/Polyphone.vst3 <soundfont directory> ~/polyphone-render/reference ~/polyphone-render/vst3
tests/plugin/run_cases.sh $host $plugins/LV2/Polyphone.lv2 <soundfont directory> ~/polyphone-render/reference ~/polyphone-render/lv2
tests/plugin/run_cases.sh $host $plugins/CLAP/Polyphone.clap <soundfont directory> ~/polyphone-render/reference ~/polyphone-render/clap
```

The renderings are the ones of the editor bit for bit, built by the same compiler, except the cases
whose blocks are not a multiple of 64 frames (`block_size_37`, `_100` and `_7400`): the plugin
computes at the frame, its voices being summed in another order (rounding errors, about -140 dBFS). The cases that
only the editor can play (instruments and samples played alone, muted zones) are skipped. A pitch bend
of 0.3 or 1 cannot be sent exactly with a MIDI message: the comparison stops at its block.

## Checks of a host

```bash
$host checks $plugins/VST3/Polyphone.vst3 <soundfont> <other soundfont>   # or the LV2 plugin
$host clapchecks $plugins/CLAP/Polyphone.clap <soundfont>
$host formatchecks $plugins/VST3/Polyphone.vst3 <sfz file> <dls file>   # or the LV2 plugin
```

Instances created and deleted, a soundfont shared by 8 instances (memory used, Linux only), all
outputs enabled as an LV2 host does (main, 16 channels, effects), blocks of 1 to 4096 frames giving
the same rendering (rounding errors only), a note starting at the frame of its message (played
later, the rendering is the same shifted, with any block size), multiple outputs (channels playing,
silent ones, sum of the outputs equal to the main output), MPE (pitch bend of a note and of its zone,
configuration of the zones, controllers of the master channel, compared with the same notes in the
mode "One preset"), sample rates from 22050 to 192000 Hz, state restored by another instance
(parameters, soundfont stored in the state and played without the file, missing file), automation
of all parameters at each block, instances computed by several threads, soundfonts set by the
message thread while another thread computes the audio. `formatchecks`: an sfz and a dls file played,
the dls file stored in the state and played without the file, the sfz file not stored.

## Threads

`plugin_threads` compiles the sources of the plugin: an audio thread computes the sound on all the
outputs while the message thread does what a host and the interface can do (soundfonts read, states
restored and saved, modes and presets including MPE, multiple outputs, parameters, polyphony,
keyboard, panic). Built with ThreadSanitizer or
AddressSanitizer (applied to the synth core and to JUCE too), it shows the data races or the memory
errors:

```bash
cmake -S tests/plugin -B build/plugin_tests_tsan -DSANITIZER=thread -DPOLYPHONE_JUCE_DIR=<JUCE>
cmake --build build/plugin_tests_tsan --target plugin_threads
setarch -R build/plugin_tests_tsan/plugin_threads_artefacts/Release/plugin_threads 20 <soundfont> <other soundfont>
```

## Interface

`plugin_snapshot <soundfont> <png prefix>` draws the interface of both modes in images, without a
display.

## External validators

[pluginval](https://github.com/Tracktion/pluginval) (VST3, LV2, AU) and
[clap-validator](https://github.com/free-audio/clap-validator) (CLAP) check the plugin against the
formats, without a soundfont:

```bash
pluginval --strictness-level 10 --skip-gui-tests --validate $plugins/VST3/Polyphone.vst3   # or the LV2 plugin
clap-validator validate $plugins/CLAP/Polyphone.clap
```

pluginval 1.0.4 passes at the level 10. clap-validator 0.4.1 passes, with a warning: random bytes
given as a state are "loaded", the wrapper of JUCE having no way to report that `setStateInformation`
ignored them.
