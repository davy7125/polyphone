Polyphone plugin
================

Soundfont player (sf2, sf3, sfz and dls) for the hosts of audio plugins (DAW), with the synth of the
Polyphone editor: a soundfont sounds the same in a host and in Polyphone.
More information: https://www.polyphone.io

Installation
------------

Copy the formats used by your host in the folders where it looks for the plugins, then rescan the
plugins in the host.

Linux
  Polyphone.vst3       ->  ~/.vst3/
  Polyphone.clap       ->  ~/.clap/
  Polyphone.lv2        ->  ~/.lv2/

macOS
  Polyphone.vst3       ->  ~/Library/Audio/Plug-Ins/VST3/
  Polyphone.clap       ->  ~/Library/Audio/Plug-Ins/CLAP/
  Polyphone.component  ->  ~/Library/Audio/Plug-Ins/Components/   (Audio Unit: Logic Pro, GarageBand...)
  Polyphone.lv2        ->  ~/Library/Audio/Plug-Ins/LV2/
  The plugins are not notarized: if the host refuses to load them, remove the quarantine flag
  of the downloaded files, for instance:
    xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/Polyphone.vst3

Windows
  Polyphone.vst3       ->  C:\Program Files\Common Files\VST3\
  Polyphone.clap       ->  C:\Program Files\Common Files\CLAP\
  Polyphone.lv2        ->  %APPDATA%\LV2\

Licenses
--------

Polyphone is under the GNU General Public License v3. The plugin is made with JUCE, used under the
GNU Affero General Public License v3, and clap-juce-extensions (MIT license).
Sources: https://github.com/davy7125/polyphone
