# Rendering tests of the sound engine

These tests check that a change of the sound engine does not change the sound. Each case plays timed
events with a soundfont, faster than real time, and writes a wav file. The renderings of the version
before the change are kept as references, and the renderings of the new version are compared to them.

## Usage

1. Gather the soundfonts of the cases in a directory (see below).
2. Before changing the engine, build Polyphone and render the references, outside the repository:
   ```bash
   tests/render/render.sh <polyphone executable> <soundfont directory> ~/polyphone-render/reference
   ```
3. After a change, render again and compare, with the synth core (`sources/synth_core`), with the
   synth core next to a second one, and with the synth of the editor, which computes the voices with
   several threads:
   ```bash
   tests/render/render.sh <polyphone executable> <soundfont directory> ~/polyphone-render/core
   tests/render/compare.py ~/polyphone-render/reference ~/polyphone-render/core
   tests/render/render.sh <polyphone executable> <soundfont directory> ~/polyphone-render/two two_cores
   tests/render/compare.py ~/polyphone-render/reference ~/polyphone-render/two
   tests/render/render.sh <polyphone executable> <soundfont directory> ~/polyphone-render/threaded threaded
   tests/render/compare.py ~/polyphone-render/reference ~/polyphone-render/threaded
   ```

`compare.py` fails when a rendering is missing, has another format or length, or when two samples
differ by more than -100 dBFS (`--threshold` to change it). It also checks the cases that must sound
like another one (line `# same as: <case>`, for the pedals): they need no reference. Both synths give the same renderings,
identical bit for bit from one run to another: `render.sh` runs Polyphone on two CPUs (`taskset`),
which gives a single sound engine thread to the synth of the editor. With several threads, the
voices are summed in a varying order and the results differ by rounding errors (-136 to -150 dBFS,
measured on the cases). A real change is far above the threshold: a master gain lowered by 0.1 dB
gives -42 dBFS. Listen to both files when a difference is expected. Render the references and the
comparisons with the same compiler and build configuration.

Polyphone built with a Qt that is not the one of the system (Qt Creator, `~/Qt`) needs its libraries:
`LD_LIBRARY_PATH=~/Qt/<version>/gcc_64/lib tests/render/render.sh …`.

## Soundfonts

They are not in the repository. Put them (or links to them) in one directory:

| File | License | Where to find it |
|---|---|---|
| `FluidR3_GM.sf2` | MIT | package `fluid-soundfont-gm` (`/usr/share/sounds/sf2`) |
| `TimGM6mb.sf2` | GPL v2 | package `timgm6mb-soundfont` (`/usr/share/sounds/sf2`) |
| `VintageDreamsWaves-v2.sf2` and `.sf3` | freeware, see its COPYRIGHT.txt | `sf2/` of the FluidSynth sources |
| `polyphone-test.sf2` | none (computed) | written by `render.sh` with `make_test_soundfont.py` if it is missing |

`polyphone-test.sf2` covers what the others do not: 24-bit samples (one of them almost only in the
8 least significant bits, played with a gain of 40 dB), a linked stereo pair, the 3 loop modes, an
exclusive class, velocity layers, a modulator (CC 1 to the filter) and generators at the preset
level. Its samples are computed, so the file is the same on every machine.

## The command

```bash
polyphone -5 -i <soundfont> -c <event file>[|two_cores|threaded] [-d <output directory>] [-o <output name>]
```

The synth core renders the events, next to a second synth core if `|two_cores` follows the event
file (see below), or the synth of the editor if `|threaded` follows it.
The output is a stereo 32-bit float wav file, next to the event file by default. An existing file
is replaced. Exit codes: 1 for a missing input file or output directory, 2 for a wrong event file,
4 when a preset or a program is missing, the output cannot be written or the second synth core gives
the same rendering as the first one.

## Event files

The cases are in `cases/`. The first line names the soundfont used by `render.sh`. Then each line is
a setting or an event, `#` starts a comment.

| Setting | Meaning | Default |
|---|---|---|
| `samplerate <Hz>` | sample rate of the rendering | 48000 |
| `block <frames>` | size of the blocks asked to the synth, from 1 to 8192 | 256 |
| `duration <s>` | length of the rendering | mandatory |
| `gain <dB>` | master gain | 0 |
| `tuning <Hz>` | reference pitch | 440 |
| `reverb <level> <size> <width> <damping>` | reverb, values from 0 to 100 | off |
| `chorus <level> <depth> <frequency>` | chorus, values from 0 to 100 | off |
| `sampleloop <0 or 1>` | loop of a sample played alone | 1 |
| `samplestereo <0 or 1>` | stereo for a sample played alone (otherwise its linked part is silent) | 0 |
| `samplegain <dB>` | gain of a sample played alone | 0 |
| `polyphony <voices>` | maximum number of voices, from 16 to 4096, the quietest notes fading out above it | 1024 |
| `cpubudget <percent>` | real time: the quietest notes fade out if the voices take more than this share of the duration of the audio (the rendering depends on the speed of the computer and is not reproducible, for manual tests only) | offline |
| `mute instrument\|preset <index> <zone>` | zone muted, as in the editor | |
| `alwaysplayed instrument\|preset <index>` | element played with any other one of its kind, as in the editor | |

An event starts with its time in seconds. Channels go from 0 to 15. The indexes of the instruments,
samples and zones are their order in the soundfont, from 0.

| Event | Meaning |
|---|---|
| `program <channel> <bank> <preset>` | preset played by a channel (mandatory before its notes; bank 128 on the channel 9 only) |
| `on <channel> <key> <velocity>` | note on (a velocity of 0 is a note off) |
| `off <channel> <key>` | note off |
| `cc <channel> <controller> <value>` | controller change |
| `bend <channel> <value>` | pitch bend, from -1 to 1 |
| `bendrange <channel> <semitones>` | pitch bend range (RPN 0) |
| `pressure <channel> <value>` | channel aftertouch |
| `polypressure <channel> <key> <value>` | key aftertouch |
| `instrument <index> <key> <velocity>` | instrument played alone on the channel of the editor (-1), a velocity of 0 releasing it |
| `sample <index> <key> <velocity>` | sample played alone on the channel of the editor, key -1 being its root key |

## How it works

- `OfflineRenderer` (`sources/core/render`) drives `SynthCore`, or the `Synth` class like the
  application does, with the configuration given before the sample rate.
- The MIDI events go through `MidiProcessor` (`sources/synth_core`), which `MidiDevice` uses in the
  application: default values of the controllers (including the ones set by the modulators of the
  soundfont), sustain and sostenuto pedals, (N)RPN (a bend range is sent as the controllers 101, 100,
  6 and 38), bank select (controller 0, then program change). `SynthMidi` then plays the preset of
  each channel with the core, a small listener does it with the synth of the editor. The instruments
  and samples played alone are actions of the editor and go directly to the synth.
- The synth computes chunks of 64 frames (`COMPUTATION_CHUNK`, 1.3 ms at 48 kHz): the voices compute
  their envelopes and LFOs once per chunk (`Voice::generateData`). A block of any size can be asked:
  when everything computed has been returned, the synth computes the chunks covering the rest of the
  block at once (as FluidSynth does), and the frames of the last chunk not asked are returned first
  by the next call (always less than a chunk). An event is processed at the start of the block that
  contains its time and heard from the next chunk computed: from the start of the block if its size
  is a multiple of 64.
- `SynthCore::render` computes in the calling thread. The synth of the editor returns the data
  computed by its threads since its previous call (the chunks that will be missing for a next call of
  the same length) and, in real time, skips the voices that were not computed in time. Offline,
  `Synth::waitForComputation()` waits for every voice before the data is read, and the first block is
  skipped so that an event at 0 s is heard at 0 s. The effects of the synth of the editor process this
  first block of silence: the core renders one too before the events, otherwise the modulation of the
  chorus would start with another phase. This first block is made of whole chunks.
- The `block_size_*` cases must sound like `block_size` (blocks of 64 frames) with blocks of 37, 100
  and 7400 frames: their events are at multiples of 59200 frames, the start of a chunk and of a block
  for each size. The voices are summed in another order when the synth computes several chunks at
  once (rounding errors, about -140 dBFS).
- With `two_cores`, a second synth core plays the same events in the same thread, with another
  reference pitch (415 Hz), a temperament, the velocity to filter cutoff of the version 2.01 and other
  values of the controllers (vibrato and volume). It is computed between the events and the
  computation of the first one, and only the first one is written: its rendering must stay the one of
  the reference. The settings read by the voices (tuning, values of the controllers, default
  modulators) belong to each synth, as for instances of a plugin in the same process: when they were
  shared, every case failed. The rendering fails (exit code 4) if the second synth gives the same data
  as the first one, which would test nothing.
- The pedals are checked without reference: `pedal_sustain` must sound exactly like
  `pedal_sustain_equivalent`, where each note is released when the pedal releases it (same for the
  sostenuto, and a key pressed again while it is held). Likewise `key_restarted` must sound like
  `key_restarted_equivalent`: a key pressed again before being released stops its note first.
- `polyphony_limit` plays more notes than a polyphony of 16 voices: the quietest notes fade out in 10 ms
  (`FADE_OUT_DURATION`), the lowest and the highest keys held on a channel and the notes just started being kept
  (`VoiceList::prepareComputation`). The same happens in real time when the voices take too long to compute: the
  renderer is offline (the computation time does not limit the voices) unless `cpubudget` is given.
- Not covered: what `MidiDevice` adds to `MidiProcessor` (the input of a device and the listeners of
  the editor), the relative values of the controllers of a device.
