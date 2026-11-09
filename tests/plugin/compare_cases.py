#!/usr/bin/env python3
# Compare the renderings of a plugin (plugin_host case) with the references of the editor, on the frames that MIDI
# can give exactly (a pitch bend of 0.3 or 1 cannot be sent exactly with MIDI messages).
# Usage: compare_cases.py <reference directory> <rendering directory> <list written by run_cases.sh> [threshold in dBFS]
import math
import struct
import sys

HEADER_SIZE = 58  # Stereo 32-bit float wav files written by Polyphone and plugin_host


def read(path):
    with open(path, 'rb') as f:
        data = f.read()[HEADER_SIZE:]
    return data


def main():
    reference_dir, output_dir, list_file = sys.argv[1:4]
    threshold = float(sys.argv[4]) if len(sys.argv) > 4 else -100.0
    failures = 0
    count = 0
    for line in open(list_file):
        words = line.split()
        if len(words) != 4 or words[0] != 'RENDERED':
            continue
        name = words[1][:-4]
        exact_frames = int(words[3])
        count += 1
        reference = read('%s/%s.wav' % (reference_dir, name))
        rendering = read('%s/%s.wav' % (output_dir, name))
        size = min(len(reference), len(rendering), exact_frames * 8)
        if reference[:size] == rendering[:size]:
            print('OK      %-32s identical (%d frames)' % (name, size // 8))
            continue
        values = size // 4
        a = struct.unpack('<%df' % values, reference[:size])
        b = struct.unpack('<%df' % values, rendering[:size])
        difference = max(abs(x - y) for x, y in zip(a, b))
        level = 20 * math.log10(difference)
        ok = level <= threshold
        print('%s %-32s max difference %.1f dBFS (%d frames)' % ('OK     ' if ok else 'FAILED ', name, level, size // 8))
        failures += 0 if ok else 1
    print('%d cases, %d failure(s)' % (count, failures))
    return 1 if failures > 0 or count == 0 else 0


if __name__ == '__main__':
    sys.exit(main())
