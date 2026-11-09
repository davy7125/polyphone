# Polyphone plugin

Soundfont player (sf2, sf3, sfz and dls) for the hosts of audio plugins, made with the synth of the
editor (`sources/synth_core`): a soundfont sounds the same in a host and in Polyphone. Formats: VST3, CLAP,
LV2, AU (macOS) and a standalone application ("Polyphone Player", not distributed), made with [JUCE](https://juce.com) and
[clap-juce-extensions](https://github.com/free-audio/clap-juce-extensions).

## Features

- A soundfont opened with the button, the recent files or a drop on the window: sf2, sf3, sfz (its
  samples being the files it references) or dls. The sfz and dls files are converted as the editor
  imports them, by the same code. The file is read at once, the samples of an sf3 file being
  decompressed: nothing is read while playing. The instances
  of the plugin playing the same file in a host (one per track for instance) share it: it is read and
  kept in memory once (it is read again if the file changed).
- Presets played in three modes:
  - **One preset**: the same preset on the 16 MIDI channels;
  - **16 channels**: each channel plays the preset chosen for it, or the one chosen by its program
    changes (bank select then program change, bank 128 on the channel 10), as a General MIDI synth;
  - **MPE**: the same preset played by an MPE controller (MIDI Polyphonic Expression), each note
    having its own channel, thus its own pitch bend, pressure and controllers. A lower zone of 15
    channels is used until the controller sends its configuration (RPN 6). The master channel of a
    zone sends the messages of all its notes: its pitch bend is added to the one of each note, its
    controllers apply to all of them (except the timbre, CC 74). The pitch bend range is 48
    semitones for the notes and 2 for the master channel, changed by the RPN 0.
- Notes at the frame: a note starts at the frame of its MIDI message, whatever the size of the
  blocks given by the host. The rendering does not depend on the size of the blocks, except
  rounding errors (about -140 dBFS) and the parameters automated, applied at the start of a block.
  The other messages (release, controllers) change a note from the next chunk of 64 frames it
  computes from its start.
- Multiple outputs (Settings, disabled by default): the outputs "Channel 1" to "Channel 16" receive
  the part of the notes of their channel without effect, and the output "Effects" the part sent to
  the reverb and the chorus, after them. A channel goes to the main output if its output is not
  enabled by the host. Without this setting, all is mixed in the main output and the other outputs
  are silent: some formats (LV2) or hosts always enable all outputs.
- Parameters that the host can automate, with the ranges of the settings of the editor: gain,
  reverb (level, size, width, damping), chorus (level, depth, frequency), transposition (except the
  percussions), reference pitch and temperament (the ones of the editor).
- A keyboard showing the notes received and played with the mouse, the number of voices, the share of
  the processor taken and a panic button (all sounds stopped, controllers reset).
- Settings: polyphony, channel of the keyboard, and the soundfont stored in the project of the host
  (the project can then be opened without the file; not possible with an sfz file, whose samples are
  other files).
- Offline rendering of the host: the computation time never limits the voices.

The state saved by the host contains the parameters, the presets, the polyphony and the path of the
soundfont (and its content if it is stored). When a project is opened, the soundfont is read before
the state is restored, so that an offline rendering can start at once.

The controllers received from the host are always absolute values (the editor detects the relative
values of some MIDI devices).

## Build

```bash
cmake -S sources/plugin -B build/plugin
cmake --build build/plugin
```

The plugins are in `build/plugin/PolyphonePlugin_artefacts/Release`: `VST3/Polyphone.vst3`,
`CLAP/Polyphone.clap`, `LV2/Polyphone.lv2`, `Standalone/polyphone-player` (`Polyphone Player` on macOS and
Windows, another name than the editor) (and `AU/Polyphone.component` on
macOS). Copy them in the folders of the hosts, for instance `~/.vst3`, `~/.clap` and `~/.lv2` on Linux.

Requirements:
- CMake 3.22 or more, a C++17 compiler;
- libvorbisfile (sf3 files), found by CMake or pkg-config, or downloaded and built with
  `-DPOLYPHONE_FETCH_VORBIS=ON` (Windows, macOS);
- libsndfile (samples of the sfz files, waves of the dls files), found by CMake or pkg-config, or
  downloaded and built with `-DPOLYPHONE_FETCH_SNDFILE=ON` (Windows, macOS): built this way, it has no
  external library and cannot read the samples in flac, ogg or opus (wav and aiff can be read);
- on Windows and macOS, both libraries can also come from [vcpkg](https://vcpkg.io), static and complete
  (flac, ogg, opus and mp3 read), as the official builds do (`.github/workflows/build-plugin.yml`):
  `vcpkg install --triplet <triplet> --overlay-triplets=packaging/vcpkg-triplets libsndfile libvorbis`,
  then `-DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=<triplet>
  -DVCPKG_OVERLAY_TRIPLETS=packaging/vcpkg-triplets -DVCPKG_MANIFEST_MODE=OFF`, the triplets being
  `x64-windows-polyphone` (with `-T ClangCL`), `arm64-osx-polyphone` and `x64-osx-polyphone`;
- on Linux, the packages needed by JUCE: ALSA, freetype, fontconfig and the X11 libraries
  (Fedora: `alsa-lib-devel freetype-devel fontconfig-devel libX11-devel libXcomposite-devel
  libXcursor-devel libXext-devel libXinerama-devel libXi-devel libXrandr-devel libXrender-devel
  mesa-libGL-devel`; Debian / Ubuntu: `libasound2-dev libfreetype-dev libfontconfig1-dev libx11-dev
  libxcomposite-dev libxcursor-dev libxext-dev libxinerama-dev libxi-dev libxrandr-dev libxrender-dev
  libgl-dev`).

JUCE and clap-juce-extensions are downloaded at the configuration (versions fixed in
`CMakeLists.txt`), unless local copies are given with `-DPOLYPHONE_JUCE_DIR=<path>` and
`-DPOLYPHONE_CLAP_JUCE_EXTENSIONS_DIR=<path>`.

The version is the one of the editor, read in `sources/VERSION` (its numbers only: `2.7.0` for
`2.7.0-BETA`).

## Sources

- `pluginengine`: the synth of the plugin without JUCE. The soundfont, read by `SynthSoundfontFile`,
  is played on the 16 channels by `SynthCore`, `MidiProcessor` and `SynthChannels`. The requests of
  the other threads (soundfont, presets, panic) go through a `SynthQueue` and are applied by the audio
  thread, which never waits: a soundfont replaced is released once no voice reads it anymore
  (`SynthSoundfontSet`). The MIDI messages are processed at their position in the blocks, the sound
  being computed at the frame (`SynthCore::renderExact`).
- `pluginsoundfont`: the soundfonts read, shared by the instances of the plugin in the process
  (without JUCE).

The files are read by `SynthSoundfontFile` (`sources/synth_core`) with the readers of the editor:
`Sf2Reader` for sf2 and sf3, `SfzReader` and `DlsReader` for sfz and dls. These two convert a file
into a soundfont through `ISoundfontBuilder`, written by the editor into its model and by the plugin
into its own (`SampleFileReader` reading the samples with libsndfile).
- `pluginprocessor`: the `juce::AudioProcessor` (parameters, state, soundfont read in another thread
  when it is opened by the interface, in the calling thread when a state is restored).
- `plugineditor`: the interface, with the colors of the theme "Dark, orange" of the editor.

## Identifiers

The hosts know the plugin by its codes (`PLUGIN_MANUFACTURER_CODE` and `PLUGIN_CODE`), its bundle
identifier, its LV2 URI and its CLAP identifier, all in `CMakeLists.txt`. They must not change once a
version is published, otherwise the projects using the plugin could not find it anymore.

## Licenses

Polyphone is under the GPL v3. JUCE is used under the AGPL v3, clap-juce-extensions under the MIT
license; the VST3 SDK (MIT license since version 3.8) and the LV2 headers (ISC license) come with
JUCE.

## Tests

See `tests/plugin/README.md`: the plugins built (VST3, LV2 and CLAP) are loaded as a host does, the
rendering cases of `tests/render` played through them must be the ones of the editor, bit for bit.
