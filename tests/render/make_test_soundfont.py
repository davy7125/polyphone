#!/usr/bin/env python3
"""Write polyphone-test.sf2, a small soundfont for the rendering tests.

Usage: make_test_soundfont.py <output file>

It covers what the other soundfonts of the tests do not: 24-bit samples (sm24 chunk, including a
sample whose signal is almost only in the 8 least significant bits), a linked stereo pair, the loop
modes, an exclusive class, velocity layers, a modulator and generators at the preset level. The
samples are computed, so that the file is the same everywhere and has no license.
"""

import math
import struct
import sys

# Generators
START_LOOP_OFFSET, PAN, INITIAL_FILTER_FC, INITIAL_FILTER_Q = 2, 17, 8, 9
ATTACK_VOL_ENV, DECAY_VOL_ENV, SUSTAIN_VOL_ENV, RELEASE_VOL_ENV = 34, 36, 37, 38
INSTRUMENT, KEY_RANGE, VEL_RANGE, INITIAL_ATTENUATION = 41, 43, 44, 48
COARSE_TUNE, SAMPLE_ID, SAMPLE_MODES, EXCLUSIVE_CLASS = 51, 53, 54, 57

# Sample types
MONO, RIGHT, LEFT = 1, 2, 4


def timecents(seconds):
    return int(round(1200 * math.log2(seconds)))


def absolute_cents(frequency):
    return int(round(1200 * math.log2(frequency / 8.176)))


class Sample:
    def __init__(self, name, rate, values, loop, root_key, sample_type=MONO, link=0):
        self.name, self.rate, self.values, self.loop = name, rate, values, loop
        self.root_key, self.sample_type, self.link = root_key, sample_type, link


def periodic(rate, length, frequency, amplitude, harmonics):
    """24-bit values of a sum of harmonics (rank, relative level)."""
    values = []
    for i in range(length):
        x = sum(level * math.sin(2 * math.pi * rank * frequency * i / rate) for rank, level in harmonics)
        values.append(int(round(amplitude * x * 8388607)))
    return values


def make_samples():
    samples = []

    # 0: sine at -6 dBFS, 400 Hz at 32 kHz (80 points per period)
    samples.append(Sample('sine24', 32000, periodic(32000, 32000, 400, 0.5, [(1, 1)]), (16000, 31920), 69))

    # 1: sine around -98 dBFS: its signal is almost only in the 8 bits of the sm24 chunk
    samples.append(Sample('quiet24', 32000, periodic(32000, 32000, 250, 100 / 8388607, [(1, 1)]), (16000, 31872), 69))

    # 2 and 3: linked stereo pair at 44.1 kHz
    samples.append(Sample('stereoL', 44100, periodic(44100, 22050, 441, 0.4, [(1, 1), (2, 0.5), (3, 0.33)]),
                          (11000, 21000), 69, LEFT, 3))
    samples.append(Sample('stereoR', 44100, periodic(44100, 22050, 294, 0.4, [(1, 1), (3, 0.33), (5, 0.2)]),
                          (10950, 20850), 69, RIGHT, 2))

    # 4: decaying noise at 22.05 kHz, from a linear congruential generator
    values, state = [], 12345
    for i in range(6615):
        state = (1103515245 * state + 12345) % 2147483648
        noise = state / 1073741824 - 1
        values.append(int(round(0.6 * noise * math.exp(-i / 1500) * 8388607)))
    samples.append(Sample('noise', 22050, values, (0, 0), 60))

    # 5: decaying plucked tone, looped at its end (played with the mode "loop until release")
    values = periodic(32000, 32000, 320, 0.45, [(1, 1), (2, 0.6), (3, 0.3), (4, 0.15)])
    values = [int(round(v * (0.15 + 0.85 * math.exp(-i / 6000)))) for i, v in enumerate(values)]
    samples.append(Sample('pluck', 32000, values, (24000, 31900), 67))
    return samples


def gen(oper, amount):
    return struct.pack('<Hh', oper, amount)


def gen_range(oper, low, high):
    return struct.pack('<HBB', oper, low, high)


def name20(name):
    return name.encode('ascii')[:20].ljust(20, b'\0')


def chunk(chunk_id, data):
    # Even sizes, as the texts of a soundfont are
    data += b'\0' if len(data) % 2 else b''
    return chunk_id + struct.pack('<I', len(data)) + data


def list_chunk(list_id, data):
    return chunk(b'LIST', list_id + data)


def build_instruments(instruments):
    """Records, bags, modulators and generators of the instruments.

    instruments: list of (name, zones), a zone being (generators, modulators).
    """
    records, bags, mods, gens = b'', b'', b'', b''
    bag_index = gen_index = mod_index = 0
    for name, zones in instruments:
        records += name20(name) + struct.pack('<H', bag_index)
        for generators, modulators in zones:
            bags += struct.pack('<HH', gen_index, mod_index)
            bag_index += 1
            for g in generators:
                gens += g
                gen_index += 1
            for m in modulators:
                mods += m
                mod_index += 1
    records += name20('EOI') + struct.pack('<H', bag_index)
    bags += struct.pack('<HH', gen_index, mod_index)
    mods += b'\0' * 10
    gens += b'\0' * 4
    return records, bags, mods, gens


def make_soundfont(path):
    samples = make_samples()

    # Sample data: 16 bits in smpl, the 8 least significant bits in sm24, 46 zero points after each sample
    smpl, sm24, headers, position = bytearray(), bytearray(), b'', 0
    for sample in samples:
        start = position
        for v in sample.values + [0] * 46:
            v = max(-8388608, min(8388607, v)) & 0xFFFFFF
            smpl += struct.pack('<H', v >> 8)
            sm24.append(v & 0xFF)
            position += 1
        end = start + len(sample.values)
        headers += name20(sample.name) + struct.pack('<IIIIIBbHH', start, end, start + sample.loop[0],
                                                      start + sample.loop[1], sample.rate, sample.root_key, 0,
                                                      sample.link, sample.sample_type)
    headers += name20('EOS') + b'\0' * 26

    # Instruments: zones of generators (key and velocity ranges first, sample last) and modulators
    release = gen(RELEASE_VOL_ENV, timecents(0.3))
    attack = gen(ATTACK_VOL_ENV, timecents(0.01))
    # CC 1 (unipolar, linear) to the filter cutoff, -3000 cents
    mod_wheel_to_filter = struct.pack('<HHhHH', 0x0081, INITIAL_FILTER_FC, -3000, 0, 0)
    instruments = [
        ('loop modes', [
            ([attack, release], [mod_wheel_to_filter]),
            ([gen_range(KEY_RANGE, 0, 59), gen(SAMPLE_MODES, 1), gen(SAMPLE_ID, 0)], []),
            ([gen_range(KEY_RANGE, 60, 71), gen(SAMPLE_MODES, 3), gen(SAMPLE_ID, 5)], []),
            ([gen_range(KEY_RANGE, 72, 127), gen(INITIAL_FILTER_FC, absolute_cents(2000)), gen(INITIAL_FILTER_Q, 60),
              gen(SAMPLE_MODES, 0), gen(SAMPLE_ID, 0)], []),
        ]),
        ('stereo', [
            ([gen(PAN, -500), gen(SAMPLE_MODES, 1), release, gen(SAMPLE_ID, 2)], []),
            ([gen(PAN, 500), gen(SAMPLE_MODES, 1), release, gen(SAMPLE_ID, 3)], []),
        ]),
        ('exclusive', [
            ([gen_range(KEY_RANGE, 60, 60), gen(EXCLUSIVE_CLASS, 1), gen(SAMPLE_ID, 4)], []),
            ([gen_range(KEY_RANGE, 62, 62), gen(EXCLUSIVE_CLASS, 1), gen(SAMPLE_MODES, 1), gen(SAMPLE_ID, 0)], []),
            ([gen_range(KEY_RANGE, 64, 64), gen(SAMPLE_ID, 4)], []),
        ]),
        ('quiet 24 bits', [
            ([gen(SAMPLE_MODES, 1), release, gen(SAMPLE_ID, 1)], []),
        ]),
        ('velocity layers', [
            ([gen(DECAY_VOL_ENV, timecents(0.5)), gen(SUSTAIN_VOL_ENV, 100), release], []),
            ([gen_range(VEL_RANGE, 0, 63), gen(SAMPLE_MODES, 1), gen(SAMPLE_ID, 0)], []),
            ([gen_range(VEL_RANGE, 64, 127), gen(SAMPLE_MODES, 1), gen(SAMPLE_ID, 2)], []),
        ]),
    ]
    inst, ibag, imod, igen = build_instruments(instruments)

    # Presets: bank 0 programs 0 to 4, bank 128 program 0 (the last one layers two instruments)
    def preset_header(program, bank):
        return struct.pack('<HH', program, bank)

    def preset_extra():
        return struct.pack('<III', 0, 0, 0)

    presets = [
        ('Loop modes', 0, 0, [([gen(INSTRUMENT, 0)], [])]),
        ('Stereo', 1, 0, [([gen(INSTRUMENT, 1)], [])]),
        ('Velocity layers', 2, 0, [([gen(INSTRUMENT, 4)], [])]),
        ('Quiet 24 bits', 3, 0, [([gen(INSTRUMENT, 3)], [])]),
        ('Layers', 4, 0, [
            ([gen(INITIAL_ATTENUATION, 30)], []),
            ([gen(INSTRUMENT, 0)], []),
            ([gen(COARSE_TUNE, 12), gen(INITIAL_ATTENUATION, 60), gen(INSTRUMENT, 1)], []),
        ]),
        ('Exclusive', 0, 128, [([gen(INSTRUMENT, 2)], [])]),
    ]
    phdr, pbag, pmod, pgen = b'', b'', b'', b''
    bag_index = gen_index = mod_index = 0
    for name, program, bank, zones in presets:
        phdr += name20(name) + preset_header(program, bank) + struct.pack('<H', bag_index) + preset_extra()
        for generators, modulators in zones:
            pbag += struct.pack('<HH', gen_index, mod_index)
            bag_index += 1
            for g in generators:
                pgen += g
                gen_index += 1
            for m in modulators:
                pmod += m
                mod_index += 1
    phdr += name20('EOP') + struct.pack('<HH', 0, 0) + struct.pack('<H', bag_index) + preset_extra()
    pbag += struct.pack('<HH', gen_index, mod_index)
    pmod += b'\0' * 10
    pgen += b'\0' * 4

    info = (chunk(b'ifil', struct.pack('<HH', 2, 4)) + chunk(b'isng', b'EMU8000\0') +
            chunk(b'INAM', b'Polyphone rendering tests\0') + chunk(b'ISFT', b'make_test_soundfont.py\0'))
    sdta = chunk(b'smpl', bytes(smpl)) + chunk(b'sm24', bytes(sm24))
    pdta = (chunk(b'phdr', phdr) + chunk(b'pbag', pbag) + chunk(b'pmod', pmod) + chunk(b'pgen', pgen) +
            chunk(b'inst', inst) + chunk(b'ibag', ibag) + chunk(b'imod', imod) + chunk(b'igen', igen) +
            chunk(b'shdr', headers))
    riff = chunk(b'RIFF', b'sfbk' + list_chunk(b'INFO', info) + list_chunk(b'sdta', sdta) + list_chunk(b'pdta', pdta))
    with open(path, 'wb') as f:
        f.write(riff)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: make_test_soundfont.py <output file>')
        sys.exit(1)
    make_soundfont(sys.argv[1])
