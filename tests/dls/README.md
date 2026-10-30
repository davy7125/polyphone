# Tests of the DLS import and export

These tests check that a DLS file imported by Polyphone and a soundfont exported to the DLS format
sound as expected. The files are rendered with the command line of Polyphone (see `tests/render`) and
the renderings are compared.

## Usage

```bash
tests/dls/run_tests.sh <polyphone executable> <output directory> [threshold in dBFS]
```

Polyphone built with a Qt that is not the one of the system (Qt Creator, `~/Qt`) needs its libraries:
`LD_LIBRARY_PATH=~/Qt/<version>/gcc_64/lib tests/dls/run_tests.sh …`.

The script:

1. writes the test files with `make_dls_test_files.py` in `<output directory>/files`,
2. exports `dls-features.sf2` to `dls-features.dls`,
3. converts each DLS file to sf2 and exports it again to DLS (round trip, in `files/roundtrip`),
4. renders each case of `cases/` with its reference file, its tested file and the round trip of the
   tested file, then compares the renderings with `tests/render/compare.py`.

A comparison fails when two samples differ by more than -90 dBFS (default threshold). The renderings
are identical, except for the round trip of `dls2-test.dls`: its 24-bit wave is written in 16 bits
by the export (-98 dBFS).

## Files

They are computed, so that they are the same everywhere and have no license.

| File | Content |
|---|---|
| `dls-features.sf2` | everything that can be exported: filter, LFOs, envelopes and their key scaling, velocity to attack, tuning (scale tuning, root key, sample correction), parameters and modulator at the preset level, loops, loop and sample offsets, modulators (curves, bipolar sources, controllers, pressures, pitch wheel), stereo pairs, effects, drums with exclusive classes |
| `dls1-test.dls` | DLS level 1: art1 chunks, 8-bit wave, wave without wave sample, pool table in another order, regions without velocity range, global articulation of the melodic instruments, local articulation of the drums, gain and tuning connections, absolute zero not modulated, output transform without invert flag |
| `dls2-test.dls` | DLS level 2: art2 chunks, filter, vibrato LFO, controls of the LFOs (modulation wheel, pressures), velocity and key to the filter, conditional chunks (art1 for level 1 devices only, top level), stereo wave, phase locked waves, A-law and 24-bit waves, local articulation replacing the global one, loop and release, default frequency and delay of the LFO, key groups |
| `dls1-expected.sf2`, `dls2-expected.sf2` | soundfonts written from the formulas of the DLS specifications, that must sound like the DLS files once imported |

The modulators that make a sf2 soundfont sound like a DLS file (the default connections of the DLS
format are not the default modulators of the sf2 format) are in the global division of each
instrument of the expected soundfonts.

## Cases

The format is the one of `tests/render`, with two lines at the beginning:

- `# soundfont: <file>`: file tested,
- `# reference: <file>`: file that must sound the same.
