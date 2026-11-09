# Tests of the synth core without the editor

The synth core (`sources/synth_core`) plays soundfonts without Qt, for a plugin. These tests check
how its classes behave between the threads of a plugin, where the audio thread must never wait.

## stress_threads

An "interface" thread reads soundfonts with `SynthSoundfontFile` (from the file, or from its content
in memory every other time) and replaces the soundfont played, while the audio thread plays notes
on the 16 channels (`MidiProcessor`, `SynthChannels`, `SynthCore`). The interface also sends notes
through a `SynthQueue`, as a virtual keyboard would. A soundfont replaced is removed by the audio
thread (`SynthSoundfontSet::remove`, its voices released quickly), marked as unused once it has no
voice anymore (`checkRemoved`) and deleted by the interface (`deleteUnused`).

Built with ThreadSanitizer, it shows the data races between both threads. Built with
AddressSanitizer, it shows the memory read after being freed, for instance the sample data of a
soundfont deleted while voices still read it.

```bash
cmake -S tests/synth_core -B build/synth_core_tsan -DSANITIZER=thread
cmake --build build/synth_core_tsan
build/synth_core_tsan/stress_threads 20 <soundfont> [<soundfont>...]

cmake -S tests/synth_core -B build/synth_core_asan -DSANITIZER=address
cmake --build build/synth_core_asan
build/synth_core_asan/stress_threads 20 <soundfont> [<soundfont>...]
```

The first argument is the duration in seconds. Give several soundfonts of different sizes, an sf3
file among them (for instance the soundfonts of `tests/render`: `polyphone-test.sf2`,
`VintageDreamsWaves-v2.sf3`, `FluidR3_GM.sf2`). The test prints what happened and `OK`, or `FAILED`
if a soundfont cannot be read, if the synth computes an invalid value or nothing, or if no soundfont
could be deleted while playing. The sanitizer reports are printed before.

ThreadSanitizer may refuse to start on recent kernels ("unexpected memory mapping"): run it with
`setarch -R` (no address randomization).
