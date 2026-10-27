#!/usr/bin/env python3
"""Compare the wav files rendered by render.sh with reference renderings.

Usage: compare.py <reference directory> <output directory> [--threshold dB]

For each reference file, the rendering of the same name must have the same format and length,
and the largest difference between their samples must stay under the threshold (dBFS).
"""

import argparse
import array
import math
import os
import struct
import sys


def read_wav(path):
    """Return (sample rate, channel count, samples) of a 32-bit float wav file."""
    with open(path, 'rb') as f:
        data = f.read()
    if data[0:4] != b'RIFF' or data[8:12] != b'WAVE':
        raise ValueError('not a wav file')
    position = 12
    sample_rate = channels = None
    samples = None
    while position + 8 <= len(data):
        chunk_id = data[position:position + 4]
        chunk_size = struct.unpack('<I', data[position + 4:position + 8])[0]
        body = data[position + 8:position + 8 + chunk_size]
        if chunk_id == b'fmt ':
            audio_format, channels, sample_rate = struct.unpack('<HHI', body[0:8])
            bits = struct.unpack('<H', body[14:16])[0]
            if audio_format != 3 or bits != 32:
                raise ValueError('not a 32-bit float wav file')
        elif chunk_id == b'data':
            samples = array.array('f')
            samples.frombytes(body)
            if sys.byteorder == 'big':
                samples.byteswap()
        position += 8 + chunk_size + (chunk_size & 1)
    if samples is None or sample_rate is None:
        raise ValueError('incomplete wav file')
    return sample_rate, channels, samples


def to_db(value):
    return -math.inf if value == 0 else 20 * math.log10(value)


def compare(reference_path, output_path, threshold_db):
    """Return (passed, message) for one rendering."""
    if not os.path.exists(output_path):
        return False, 'missing'
    reference_rate, reference_channels, reference = read_wav(reference_path)
    output_rate, output_channels, output = read_wav(output_path)
    if (reference_rate, reference_channels) != (output_rate, output_channels):
        return False, 'format %d Hz / %d channels instead of %d Hz / %d channels' % (
            output_rate, output_channels, reference_rate, reference_channels)
    if len(reference) != len(output):
        return False, 'length %d instead of %d samples' % (len(output), len(reference))

    max_difference = 0.0
    square_sum = 0.0
    for a, b in zip(reference, output):
        difference = abs(a - b)
        if difference > max_difference:
            max_difference = difference
        square_sum += difference * difference
    peak = max(max(reference), -min(reference)) if len(reference) else 0.0
    rms_difference = math.sqrt(square_sum / len(reference)) if len(reference) else 0.0

    if max_difference == 0:
        return True, 'identical (peak %.1f dBFS)' % to_db(peak)
    message = 'max difference %.1f dBFS, rms difference %.1f dBFS (peak %.1f dBFS)' % (
        to_db(max_difference), to_db(rms_difference), to_db(peak))
    return to_db(max_difference) <= threshold_db, message


def main():
    parser = argparse.ArgumentParser(description='Compare renderings with reference renderings.')
    parser.add_argument('reference_dir')
    parser.add_argument('output_dir')
    parser.add_argument('--threshold', type=float, default=-100.0,
                        help='largest difference allowed between two samples, in dBFS (default: -100)')
    arguments = parser.parse_args()

    names = sorted(name for name in os.listdir(arguments.reference_dir) if name.endswith('.wav'))
    if not names:
        print('No reference rendering in %s' % arguments.reference_dir)
        return 1

    failures = 0
    for name in names:
        try:
            passed, message = compare(os.path.join(arguments.reference_dir, name),
                                      os.path.join(arguments.output_dir, name), arguments.threshold)
        except ValueError as error:
            passed, message = False, str(error)
        print('%-8s%-36s%s' % ('OK' if passed else 'FAILED', name, message))
        failures += 0 if passed else 1

    print('%d of %d renderings differ' % (failures, len(names)) if failures else 'All %d renderings match' % len(names))
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
