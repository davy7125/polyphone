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
3. After a change, render again and compare:
   ```bash
   tests/render/render.sh <polyphone executable> <soundfont directory> ~/polyphone-render/current
   tests/render/compare.py ~/polyphone-render/reference ~/polyphone-render/current
   ```

`compare.py` fails when a rendering is missing, has another format or length, or when two samples
differ by more than -100 dBFS (`--threshold` to change it). Two renderings of the same build are
identical bit for bit: `render.sh` runs Polyphone on two CPUs (`taskset`), which gives a single sound
engine thread. With several threads, the voices are summed in a varying order and the results differ
by rounding errors (-136 to -150 dBFS, measured on the cases). A real change is far above the
threshold: a master gain lowered by 0.1 dB gives -42 dBFS. Listen to both files when a difference is
expected. Render the references and the comparisons with the same compiler and build configuration.

Polyphone built with a Qt that is not the one of the system (Qt Creator, `~/Qt`) needs its libraries:
`LD_LIBRARY_PATH=~/Qt/<version>/gcc_64/lib tests/render/render.sh …`.

## Soundfonts

They are not in the repository. Put them (or links to them) in one directory:

| File | License | Where to find it |
|---|---|---|
| `FluidR3_GM.sf2` | MIT | package `fluid-soundfont-gm` (`/usr/share/sounds/sf2`) |
| `TimGM6mb.sf2` | GPL v2 | package `timgm6mb-soundfont` (`/usr/share/sounds/sf2`) |
| `VintageDreamsWaves-v2.sf2` and `.sf3` | freeware, see its COPYRIGHT.txt | `sf2/` of the FluidSynth sources |

## The command

```bash
polyphone -5 -i <soundfont> -c <event file> [-d <output directory>] [-o <output name>]
```

The output is a stereo 32-bit float wav file, next to the event file by default. An existing file
is replaced. Exit codes: 1 for a missing input file or output directory, 2 for a wrong event file,
4 when a preset or a program is missing or the output cannot be written.

## Event files

The cases are in `cases/`. The first line names the soundfont used by `render.sh`. Then each line is
a setting or an event, `#` starts a comment.

| Setting | Meaning | Default |
|---|---|---|
| `samplerate <Hz>` | sample rate of the rendering | 48000 |
| `block <frames>` | size of the blocks computed by the synth, a multiple of 256 | 256 |
| `duration <s>` | length of the rendering | mandatory |
| `gain <dB>` | master gain | 0 |
| `tuning <Hz>` | reference pitch | 440 |
| `reverb <level> <size> <width> <damping>` | reverb, values from 0 to 100 | off |
| `chorus <level> <depth> <frequency>` | chorus, values from 0 to 100 | off |

An event starts with its time in seconds. Channels go from 0 to 15.

| Event | Meaning |
|---|---|
| `program <channel> <bank> <preset>` | preset played by a channel (mandatory before its notes) |
| `on <channel> <key> <velocity>` | note on (a velocity of 0 is a note off) |
| `off <channel> <key>` | note off |
| `cc <channel> <controller> <value>` | controller change |
| `bend <channel> <value>` | pitch bend, from -1 to 1 |
| `bendrange <channel> <semitones>` | pitch bend range (RPN 0) |
| `pressure <channel> <value>` | channel aftertouch |
| `polypressure <channel> <key> <value>` | key aftertouch |

## How it works

- `OfflineRenderer` (`sources/core/render`) drives the `Synth` class like the application does: same
  default values of the controllers as `MidiDevice` (including the ones set by the modulators of the
  soundfont), the values updated before the synth is notified, and the configuration given before
  the sample rate.
- An event is applied at the start of the block that contains its time (blocks of 256 frames, 5.3 ms
  at 48 kHz). The voices compute their envelopes and LFOs by chunks of 256 frames
  (`COMPUTATION_CHUNK`, `Voice::generateData`) and only write whole chunks: the rest of a buffer
  whose size is not a multiple of 256 keeps the data of the previous voice computed by the same
  thread (a buffer under 256 frames is never written). The application asks 256 or 512 frames, but
  a plugin host can ask any size, which the extracted engine will have to handle.
- In real time, the synth returns the data computed since its previous call and skips the voices
  that were not computed in time. Offline, `Synth::waitForComputation()` waits for every voice
  before the data is read, and the first block is skipped so that an event at 0 s is heard at 0 s.
- Not covered: the sustain and sostenuto pedals (handled by `MidiDevice`, not by the synth), the
  playback of an instrument or a sample alone (editor), 24-bit samples (none in these soundfonts).
