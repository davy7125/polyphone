#!/usr/bin/env python3
"""Write the files of the DLS import and export tests.

Usage: make_dls_test_files.py <output directory>

- dls-features.sf2: a soundfont using everything that can be exported to the DLS format (filter, LFOs,
  envelopes, key scaling, tuning, preset level, loops, offsets, modulators, stereo, effects, exclusive
  classes). Its DLS export must sound the same.
- dls1-test.dls: a DLS level 1 file (art1 chunks, 8-bit wave, pool table in another order, regions without
  velocity range, global articulation of the melodic instruments, local articulation of the drums).
- dls2-test.dls: a DLS level 2 file (art2 chunks, filter, vibrato LFO, controls of the LFOs, key and velocity
  scalings, conditional chunks, stereo wave, phase locked waves, A-law and 24-bit waves, local articulation
  replacing the global one, loop and release).
- dls1-expected.sf2 and dls2-expected.sf2: soundfonts written from the formulas of the DLS specifications,
  that must sound like dls1-test.dls and dls2-test.dls once imported.

The samples are computed, so that the files are the same everywhere and have no license.
"""

import math
import os
import struct
import sys

# Generators
GEN = {name: index for index, name in enumerate([
    'startAddrsOffset', 'endAddrsOffset', 'startloopAddrsOffset', 'endloopAddrsOffset', 'startAddrsCoarseOffset',
    'modLfoToPitch', 'vibLfoToPitch', 'modEnvToPitch', 'initialFilterFc', 'initialFilterQ', 'modLfoToFilterFc',
    'modEnvToFilterFc', 'endAddrsCoarseOffset', 'modLfoToVolume', 'unused1', 'chorusEffectsSend',
    'reverbEffectsSend', 'pan', 'unused2', 'unused3', 'unused4', 'delayModLFO', 'freqModLFO', 'delayVibLFO',
    'freqVibLFO', 'delayModEnv', 'attackModEnv', 'holdModEnv', 'decayModEnv', 'sustainModEnv', 'releaseModEnv',
    'keynumToModEnvHold', 'keynumToModEnvDecay', 'delayVolEnv', 'attackVolEnv', 'holdVolEnv', 'decayVolEnv',
    'sustainVolEnv', 'releaseVolEnv', 'keynumToVolEnvHold', 'keynumToVolEnvDecay', 'instrument', 'reserved1',
    'keyRange', 'velRange', 'startloopAddrsCoarseOffset', 'keynum', 'velocity', 'initialAttenuation', 'reserved2',
    'endloopAddrsCoarseOffset', 'coarseTune', 'fineTune', 'sampleID', 'sampleModes', 'reserved3', 'scaleTuning',
    'exclusiveClass', 'overridingRootKey'])}

# Sample types
MONO, RIGHT, LEFT = 1, 2, 4

# Modulator sources (sf2)
NO_CONTROLLER, VELOCITY, KEY, POLY_PRESSURE, CHANNEL_PRESSURE, PITCH_WHEEL = 0, 2, 3, 10, 13, 14
LINEAR, CONCAVE, CONVEX, SWITCH = 0, 1, 2, 3

# DLS sources, destinations, transforms
SRC_NONE, SRC_LFO, SRC_VELOCITY, SRC_KEY, SRC_EG1, SRC_EG2, SRC_PITCHWHEEL = 0, 1, 2, 3, 4, 5, 6
SRC_POLYPRESSURE, SRC_CHANNELPRESSURE, SRC_VIBRATO, SRC_RPN0 = 7, 8, 9, 0x100


def SRC_CC(cc):
    return 0x80 + cc


DST_GAIN, DST_PITCH, DST_PAN, DST_CHORUS, DST_REVERB = 0x1, 0x3, 0x4, 0x80, 0x81
DST_LFO_FREQ, DST_LFO_DELAY, DST_VIB_FREQ, DST_VIB_DELAY = 0x104, 0x105, 0x114, 0x115
DST_EG1_ATTACK, DST_EG1_DECAY, DST_EG1_RELEASE, DST_EG1_SUSTAIN = 0x206, 0x207, 0x209, 0x20A
DST_EG1_DELAY, DST_EG1_HOLD, DST_EG1_SHUTDOWN = 0x20B, 0x20C, 0x20D
DST_EG2_ATTACK, DST_EG2_DECAY, DST_EG2_RELEASE, DST_EG2_SUSTAIN = 0x30A, 0x30B, 0x30D, 0x30E
DST_EG2_DELAY, DST_EG2_HOLD = 0x30F, 0x310
DST_FILTER_CUTOFF, DST_FILTER_Q = 0x500, 0x501
ABSOLUTE_ZERO = -0x80000000
F_INSTRUMENT_DRUMS = 0x80000000
F_RGN_OPTION_SELFNONEXCLUSIVE = 0x0001
F_WAVELINK_PHASE_MASTER, F_WAVELINK_MULTICHANNEL = 0x1, 0x2
CHANNEL_LEFT, CHANNEL_RIGHT = 1, 2
LOOP_FORWARD, LOOP_RELEASE = 0, 1


############
## Values ##
############

def timecents(seconds):
    return int(round(1200 * math.log2(seconds)))


def absolute_cents(frequency):
    return int(round(1200 * math.log2(frequency / 8.17579891564)))


def dls_time(seconds):
    return int(round(1200 * math.log2(seconds) * 65536))


def dls_pitch(frequency):
    return int(round((1200 * math.log2(frequency / 440) + 6900) * 65536))


def dls(value):
    """Gain in cB, relative pitch in cents, time in timecents, percentage in 0.1%..."""
    return int(round(value * 65536))


def dls_transform(src_curve=0, src_bipolar=False, src_invert=False, ctl_curve=0, ctl_bipolar=False,
                  ctl_invert=False, output=0):
    return ((src_invert << 15) | (src_bipolar << 14) | (src_curve << 10) | (ctl_invert << 9) |
            (ctl_bipolar << 8) | (ctl_curve << 4) | output)


def sf_source(index, cc=False, descending=False, bipolar=False, curve=LINEAR):
    return index | (cc << 7) | (descending << 8) | (bipolar << 9) | (curve << 10)


def sf_modulator(source, destination, amount, amount_source=0):
    return (source, GEN[destination], amount, amount_source)


#############
## Samples ##
#############

class Sample:
    def __init__(self, name, rate, values, loop=(0, 0), root_key=60, correction=0, sample_type=MONO, link=0):
        """values: 24-bit integers"""
        self.name, self.rate, self.values, self.loop = name, rate, values, loop
        self.root_key, self.correction, self.sample_type, self.link = root_key, correction, sample_type, link


def periodic(rate, length, frequency, amplitude, harmonics, decay=None):
    values = []
    for i in range(length):
        x = sum(level * math.sin(2 * math.pi * rank * frequency * i / rate) for rank, level in harmonics)
        if decay is not None:
            x *= 0.2 + 0.8 * math.exp(-i / decay)
        values.append(int(round(amplitude * x * 8388607)))
    return values


def noise(length, amplitude, decay):
    values, state = [], 12345
    for i in range(length):
        state = (1103515245 * state + 12345) % 2147483648
        values.append(int(round(amplitude * (state / 1073741824 - 1) * math.exp(-i / decay) * 8388607)))
    return values


def to_16_bits(values):
    """24-bit values whose 8 least significant bits are 0"""
    return [max(-32768, min(32767, int(round(v / 256)))) * 256 for v in values]


def to_8_bits(values):
    """Unsigned 8-bit values, and the corresponding 24-bit values"""
    bytes8 = [max(0, min(255, int(round(v / 65536)) + 128)) for v in values]
    return bytes8, [(b - 128) * 65536 for b in bytes8]


def alaw_encode(value16):
    # ITU-T G.711
    if value16 >= 0:
        mask, pcm = 0xD5, value16 >> 3
    else:
        mask, pcm = 0x55, -(value16 >> 3) - 1
    for segment, end in enumerate([0x1F, 0x3F, 0x7F, 0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF]):
        if pcm <= end:
            break
    else:
        return 0x7F ^ mask
    code = segment << 4
    code |= ((pcm >> 1) if segment < 2 else (pcm >> segment)) & 0x0F
    return code ^ mask


def alaw_decode(code):
    code ^= 0x55
    t = (code & 0x0F) << 4
    segment = (code & 0x70) >> 4
    if segment == 0:
        t += 8
    elif segment == 1:
        t += 0x108
    else:
        t = (t + 0x108) << (segment - 1)
    return t if code & 0x80 else -t


def common_samples():
    tone = periodic(32000, 64000, 400, 0.4, [(1, 1), (2, 0.5), (3, 0.3), (5, 0.1)])
    pluck = periodic(32000, 32000, 320, 0.45, [(1, 1), (2, 0.6), (3, 0.3), (4, 0.15)], decay=6000)
    return {
        'tone': to_16_bits(tone),
        'pluck': to_16_bits(pluck),
        'noise': to_16_bits(noise(6615, 0.6, 1500)),
        'left': to_16_bits(periodic(44100, 22050, 441, 0.4, [(1, 1), (2, 0.5), (3, 0.33)])),
        'right': to_16_bits(periodic(44100, 22050, 294, 0.4, [(1, 1), (3, 0.33), (5, 0.2)])),
        'tone24': periodic(32000, 32000, 400, 0.4, [(1, 1), (2, 0.3)]),
    }


###############
## Sf2 files ##
###############

def chunk(chunk_id, data):
    data += b'\0' if len(data) % 2 else b''
    return chunk_id + struct.pack('<I', len(data)) + data


def list_chunk(list_id, data):
    return chunk(b'LIST', list_id + data)


def name20(name):
    return name.encode('ascii')[:20].ljust(20, b'\0')


def sf2_zone(generators, modulators):
    """Generators sorted as required (ranges first, link last)"""
    order = ['keyRange', 'velRange']
    data = b''
    for name in order:
        if name in generators:
            low, high = generators[name]
            data += struct.pack('<HBB', GEN[name], low, high)
    for name, value in generators.items():
        if name not in order and name not in ('sampleID', 'instrument'):
            data += struct.pack('<Hh', GEN[name], value)
    for name in ('sampleID', 'instrument'):
        if name in generators:
            data += struct.pack('<HH', GEN[name], generators[name])
    mods = b''.join(struct.pack('<HHhHH', src, dst, amount, amt_src, 0) for src, dst, amount, amt_src in modulators)
    return data, mods


def sf2_hydra(elements, header):
    """elements: list of (header fields, zones), a zone being (generators, modulators)"""
    records, bags, mods, gens = b'', b'', b'', b''
    gen_count = mod_count = bag_count = 0
    for fields, zones in elements:
        records += header(fields, bag_count)
        for generators, modulators in zones:
            g, m = sf2_zone(generators, modulators)
            bags += struct.pack('<HH', gen_count, mod_count)
            bag_count += 1
            gens += g
            mods += m
            gen_count += len(g) // 4
            mod_count += len(m) // 10
    bags += struct.pack('<HH', gen_count, mod_count)
    return records, bags, mods + b'\0' * 10, gens + b'\0' * 4


def write_sf2(path, name, samples, instruments, presets):
    """samples: list of Sample, instruments: list of (name, zones), presets: list of ((name, bank, program), zones)"""
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
                                                      start + sample.loop[1], sample.rate, sample.root_key,
                                                      sample.correction, sample.link, sample.sample_type)
    headers += name20('EOS') + b'\0' * 26

    inst, ibag, imod, igen = sf2_hydra(instruments + [('EOI', [])],
                                       lambda name, bag: name20(name) + struct.pack('<H', bag))
    phdr, pbag, pmod, pgen = sf2_hydra(presets + [(('EOP', 0, 0), [])],
                                       lambda f, bag: name20(f[0]) + struct.pack('<HHHIII', f[2], f[1], bag, 0, 0, 0))

    info = (chunk(b'ifil', struct.pack('<HH', 2, 4)) + chunk(b'isng', b'EMU8000\0') +
            chunk(b'INAM', name.encode('ascii') + b'\0') + chunk(b'ISFT', b'make_dls_test_files.py\0'))
    sdta = chunk(b'smpl', bytes(smpl)) + chunk(b'sm24', bytes(sm24))
    pdta = (chunk(b'phdr', phdr) + chunk(b'pbag', pbag) + chunk(b'pmod', pmod) + chunk(b'pgen', pgen) +
            chunk(b'inst', inst) + chunk(b'ibag', ibag) + chunk(b'imod', imod) + chunk(b'igen', igen) +
            chunk(b'shdr', headers))
    with open(path, 'wb') as f:
        f.write(chunk(b'RIFF', b'sfbk' + list_chunk(b'INFO', info) + list_chunk(b'sdta', sdta) +
                      list_chunk(b'pdta', pdta)))


# Modulators that make a sf2 soundfont sound like a DLS file (the DLS format has other default connections)
DLS_COMPENSATION = [
    sf_modulator(sf_source(VELOCITY, descending=True), 'initialFilterFc', 0),
    sf_modulator(sf_source(1, cc=True), 'vibLfoToPitch', 0),
    sf_modulator(sf_source(CHANNEL_PRESSURE), 'vibLfoToPitch', 0),
    sf_modulator(sf_source(91, cc=True), 'reverbEffectsSend', 1000),
    sf_modulator(sf_source(93, cc=True), 'chorusEffectsSend', 1000),
]


###############
## DLS files ##
###############

def info_list(texts):
    return list_chunk(b'INFO', b''.join(chunk(key, value.encode('latin1') + b'\0') for key, value in texts))


def dls_wave(name, rate, channels, bits, data, format_tag=1, wsmp=None):
    block_align = channels * bits // 8
    fmt = struct.pack('<HHIIHH', format_tag, channels, rate, rate * block_align, block_align, bits)
    content = chunk(b'fmt ', fmt)
    if wsmp is not None:
        content += chunk(b'wsmp', wsmp)
    content += chunk(b'data', data)
    content += info_list([(b'INAM', name)])
    return list_chunk(b'wave', content)


def wsmp(unity, fine_tune=0, gain=0, loop=None):
    """loop: (type, start, length)"""
    data = struct.pack('<IHhiII', 20, unity, fine_tune, gain, 0, 1 if loop else 0)
    if loop:
        data += struct.pack('<IIII', 16, *loop)
    return data


def articulation(level, connections, condition=None):
    """level: 1 or 2, connections: list of (source, control, destination, transform, scale)"""
    data = struct.pack('<II', 8, len(connections))
    for source, control, destination, transform, scale in connections:
        data += struct.pack('<HHHHi', source, control, destination, transform, scale)
    content = (chunk(b'cdl ', condition) if condition else b'') + chunk(b'art1' if level == 1 else b'art2', data)
    return list_chunk(b'lart' if level == 1 else b'lar2', content)


def region(keys, velocities, table_index, level=2, options=0, key_group=0, wave_sample=None, art=b'',
           link_options=0, phase_group=0, channel=CHANNEL_LEFT):
    header = struct.pack('<HHHHHH', keys[0], keys[1], velocities[0], velocities[1], options, key_group)
    if level == 2:
        header += struct.pack('<H', 0)
    content = chunk(b'rgnh', header)
    if wave_sample is not None:
        content += chunk(b'wsmp', wave_sample)
    content += chunk(b'wlnk', struct.pack('<HHII', link_options, phase_group, channel, table_index))
    content += art
    return list_chunk(b'rgn ' if level == 1 else b'rgn2', content)


def instrument(name, bank, program, regions, art=b''):
    content = chunk(b'insh', struct.pack('<III', len(regions), bank, program))
    content += list_chunk(b'lrgn', b''.join(regions)) + art + info_list([(b'INAM', name)])
    return list_chunk(b'ins ', content)


def write_dls(path, name, instruments, waves, table, top_condition=None):
    """table: index of the wave for each cue of the pool table"""
    offsets, pool, position = [], b'', 0
    for wave in waves:
        offsets.append(position)
        pool += wave
        position += len(wave)
    ptbl = struct.pack('<II', 8, len(table)) + b''.join(struct.pack('<I', offsets[i]) for i in table)
    content = b'DLS '
    if top_condition:
        content += chunk(b'cdl ', top_condition)
    content += (chunk(b'colh', struct.pack('<I', len(instruments))) + list_chunk(b'lins', b''.join(instruments)) +
                chunk(b'ptbl', ptbl) + list_chunk(b'wvpl', pool) +
                info_list([(b'INAM', name), (b'ICOP', 'No copyright'), (b'IENG', 'make_dls_test_files.py')]))
    with open(path, 'wb') as f:
        f.write(chunk(b'RIFF', content))


def pcm16(values24, channels=1):
    """values24: list of 24-bit values, or a list of channels"""
    if channels == 1:
        return b''.join(struct.pack('<h', v // 256) for v in values24)
    return b''.join(struct.pack('<h', values24[c][i] // 256) for i in range(len(values24[0])) for c in range(channels))


def pcm24(values24):
    return b''.join(struct.pack('<i', v)[0:3] for v in values24)


############################
## Soundfont for the export ##
############################

def make_features(directory, s):
    tone_loop = (32000, 63920)
    pluck_loop = (24000, 31840)
    samples = [
        Sample('tone', 32000, s['tone'], tone_loop, 67),
        Sample('toneCorrection', 32000, s['tone'], tone_loop, 67, correction=-20),
        Sample('pluck', 32000, s['pluck'], pluck_loop, 64),
        Sample('noise', 22050, s['noise'], (0, 0), 60),
        Sample('stereoL', 44100, s['left'], (11025, 22050), 69, sample_type=LEFT, link=5),
        Sample('stereoR', 44100, s['right'], (10950, 21900), 69, sample_type=RIGHT, link=4),
    ]
    TONE, TONE_CORRECTION, PLUCK, NOISE, LEFT_SAMPLE, RIGHT_SAMPLE = range(6)
    release = timecents(0.3)
    instruments = [
        ('filter lfo', [
            ({'initialFilterFc': absolute_cents(3000), 'initialFilterQ': 100, 'modLfoToFilterFc': 1200,
              'freqModLFO': absolute_cents(2), 'delayModLFO': timecents(0.2), 'modLfoToVolume': 60,
              'modLfoToPitch': 30, 'vibLfoToPitch': 40, 'freqVibLFO': absolute_cents(6),
              'delayVibLFO': timecents(0.1), 'releaseVolEnv': release}, []),
            ({'sampleModes': 1, 'sampleID': TONE}, []),
        ]),
        ('mod envelope', [
            ({'delayModEnv': timecents(0.05), 'attackModEnv': timecents(0.1), 'holdModEnv': timecents(0.05),
              'decayModEnv': timecents(0.4), 'sustainModEnv': 300, 'releaseModEnv': timecents(0.3),
              'modEnvToPitch': 200, 'modEnvToFilterFc': -2400, 'initialFilterFc': absolute_cents(6000),
              'keynumToModEnvDecay': 20, 'keynumToModEnvHold': 10, 'releaseVolEnv': release}, []),
            ({'sampleModes': 1, 'sampleID': TONE}, []),
        ]),
        ('volume envelope', [
            ({'delayVolEnv': timecents(0.02), 'attackVolEnv': timecents(0.05), 'holdVolEnv': timecents(0.1),
              'decayVolEnv': timecents(0.5), 'sustainVolEnv': 200, 'releaseVolEnv': timecents(0.4),
              'keynumToVolEnvDecay': 30, 'keynumToVolEnvHold': -15},
             [sf_modulator(sf_source(VELOCITY, descending=True), 'attackVolEnv', 1200)]),
            ({'sampleModes': 1, 'sampleID': TONE}, []),
        ]),
        ('tuning', [
            ({'releaseVolEnv': release}, []),
            ({'keyRange': (0, 63), 'scaleTuning': 50, 'coarseTune': -3, 'fineTune': 25, 'overridingRootKey': 64,
              'sampleModes': 1, 'sampleID': TONE}, []),
            ({'keyRange': (64, 64), 'scaleTuning': 0, 'sampleID': NOISE}, []),
            ({'keyRange': (65, 127), 'sampleModes': 1, 'sampleID': TONE_CORRECTION}, []),
        ]),
        ('preset level', [
            ({'attackVolEnv': timecents(0.01), 'releaseVolEnv': release}, []),
            ({'keyRange': (30, 60), 'initialAttenuation': 30, 'sampleModes': 1, 'sampleID': TONE}, []),
            ({'keyRange': (61, 100), 'sampleModes': 3, 'sampleID': PLUCK}, []),
        ]),
        ('loops', [
            ({'releaseVolEnv': release}, []),
            ({'keyRange': (0, 40), 'sampleModes': 1, 'startloopAddrsOffset': 800, 'endloopAddrsOffset': -800,
              'sampleID': TONE}, []),
            ({'keyRange': (41, 60), 'sampleModes': 3, 'sampleID': PLUCK}, []),
            ({'keyRange': (61, 80), 'sampleModes': 1, 'startAddrsOffset': 2000, 'endAddrsOffset': -40,
              'sampleID': TONE}, []),
            ({'keyRange': (81, 127), 'startAddrsCoarseOffset': 1, 'sampleID': TONE}, []),
        ]),
        ('modulators', [
            ({'initialFilterFc': absolute_cents(8000), 'freqModLFO': absolute_cents(5),
              'freqVibLFO': absolute_cents(6), 'releaseVolEnv': release}, [
                sf_modulator(sf_source(1, cc=True), 'modLfoToPitch', 50),
                sf_modulator(sf_source(CHANNEL_PRESSURE), 'initialFilterFc', -2400),
                sf_modulator(sf_source(74, cc=True, bipolar=True), 'initialFilterFc', 2400),
                sf_modulator(sf_source(PITCH_WHEEL, bipolar=True), 'fineTune', 1200),
                sf_modulator(sf_source(KEY), 'initialFilterFc', 2400),
                sf_modulator(sf_source(11, cc=True, descending=True, curve=CONCAVE), 'initialAttenuation', 480),
                sf_modulator(sf_source(2, cc=True, curve=CONVEX), 'reverbEffectsSend', 500),
                sf_modulator(sf_source(POLY_PRESSURE), 'vibLfoToPitch', 100),
                sf_modulator(sf_source(80, cc=True, curve=SWITCH), 'initialAttenuation', 200),
                sf_modulator(sf_source(VELOCITY, descending=True), 'initialFilterFc', -1200),
            ]),
            ({'sampleModes': 1, 'sampleID': TONE}, []),
        ]),
        ('stereo', [
            ({'sampleModes': 1, 'releaseVolEnv': release}, []),
            ({'keyRange': (0, 71), 'pan': -500, 'sampleID': LEFT_SAMPLE}, []),
            ({'keyRange': (0, 71), 'pan': 500, 'sampleID': RIGHT_SAMPLE}, []),
            ({'keyRange': (72, 127), 'pan': -250, 'sampleID': LEFT_SAMPLE}, []),
            ({'keyRange': (72, 127), 'pan': 250, 'sampleID': RIGHT_SAMPLE}, []),
        ]),
        ('effects', [
            ({'reverbEffectsSend': 300, 'chorusEffectsSend': 200, 'releaseVolEnv': release},
             [sf_modulator(sf_source(91, cc=True), 'reverbEffectsSend', 500)]),
            ({'sampleModes': 1, 'sampleID': TONE}, []),
        ]),
        ('drums', [
            ({'keyRange': (36, 36), 'exclusiveClass': 20, 'sampleID': NOISE}, []),
            ({'keyRange': (38, 38), 'exclusiveClass': 20, 'coarseTune': 5, 'sampleID': NOISE}, []),
            ({'keyRange': (42, 42), 'exclusiveClass': 100, 'sampleID': PLUCK}, []),
            ({'keyRange': (44, 44), 'exclusiveClass': 100, 'sampleID': TONE}, []),
            ({'keyRange': (46, 46), 'exclusiveClass': 2, 'pan': -300, 'sampleID': NOISE}, []),
            ({'keyRange': (49, 49), 'exclusiveClass': 1, 'pan': 300, 'sampleID': NOISE}, []),
        ]),
    ]
    presets = [((name.capitalize(), 0, program), [({'instrument': program}, [])]) for program, (name, _) in
               enumerate(instruments[:-1])]
    presets[4] = (('Preset level', 0, 4), [
        ({'reverbEffectsSend': 100}, []),
        ({'keyRange': (40, 90), 'initialAttenuation': 60, 'coarseTune': 2, 'fineTune': -30, 'pan': 200,
          'initialFilterFc': -1200, 'instrument': 4}, [sf_modulator(sf_source(1, cc=True), 'pan', 300)]),
    ])
    presets.append((('Drums', 128, 0), [({'instrument': len(instruments) - 1}, [])]))
    write_sf2(os.path.join(directory, 'dls-features.sf2'), 'DLS features', samples, instruments, presets)


#####################
## DLS level 1 test ##
#####################

def make_dls1(directory, s):
    tone8_bytes, tone8_values = to_8_bits(s['tone'][0:32000])

    # Waves, the pool table referencing them in another order: noise, tone, tone8
    waves = [
        dls_wave('tone16', 32000, 1, 16, pcm16(s['tone']), wsmp=wsmp(67, loop=(LOOP_FORWARD, 32000, 31920))),
        dls_wave('tone8', 32000, 1, 8, bytes(tone8_bytes), wsmp=wsmp(79, fine_tune=-30)),
        dls_wave('noise16', 22050, 1, 16, pcm16(s['noise'])),
    ]
    table = [2, 0, 1]
    NOISE, TONE, TONE8 = 0, 1, 2

    melodic_art = articulation(1, [
        (SRC_NONE, SRC_NONE, DST_LFO_FREQ, 0, dls_pitch(4)),
        (SRC_NONE, SRC_NONE, DST_LFO_DELAY, 0, dls_time(0.3)),
        (SRC_LFO, SRC_CC(1), DST_PITCH, 0, dls(50)),
        (SRC_LFO, SRC_NONE, DST_GAIN, 0, dls(30)),
        (SRC_NONE, SRC_NONE, DST_EG1_ATTACK, 0, dls_time(0.05)),
        (SRC_NONE, SRC_NONE, DST_EG1_DECAY, 0, dls_time(1)),
        (SRC_NONE, SRC_NONE, DST_EG1_SUSTAIN, 0, dls(500)),
        (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.5)),
        (SRC_VELOCITY, SRC_NONE, DST_EG1_ATTACK, 0, dls(-1200)),
        (SRC_KEY, SRC_NONE, DST_EG1_DECAY, 0, dls(-1280)),
        (SRC_NONE, SRC_NONE, DST_EG2_ATTACK, 0, dls_time(0.1)),
        (SRC_NONE, SRC_NONE, DST_EG2_DECAY, 0, dls_time(0.5)),
        (SRC_NONE, SRC_NONE, DST_EG2_SUSTAIN, 0, 0),
        (SRC_NONE, SRC_NONE, DST_EG2_RELEASE, 0, dls_time(0.2)),
        (SRC_EG2, SRC_NONE, DST_PITCH, 0, dls(100)),
        # Only the output transform, as in some banks: same as the default connection
        (SRC_VELOCITY, SRC_NONE, DST_GAIN, 1, dls(-960)),
    ])
    instruments = [
        instrument('melodic', 0, 0, [
            region((0, 59), (0, 127), TONE, level=1,
                   wave_sample=wsmp(62, fine_tune=10, gain=dls(-60), loop=(LOOP_FORWARD, 32000, 31920))),
            # Velocity range not used in DLS level 1
            region((60, 127), (0, 0), TONE8, level=1),
        ], art=melodic_art),
        instrument('drums', F_INSTRUMENT_DRUMS, 0, [
            region((36, 36), (0, 127), NOISE, level=1, art=articulation(1, [
                (SRC_NONE, SRC_NONE, DST_PAN, 0, dls(-300)),
                (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.2))])),
            region((38, 38), (0, 127), NOISE, level=1, key_group=1, art=articulation(1, [
                (SRC_NONE, SRC_NONE, DST_PAN, 0, dls(300)),
                (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.2)),
                # The attack is 0 second: it cannot be modulated
                (SRC_NONE, SRC_NONE, DST_EG1_ATTACK, 0, ABSOLUTE_ZERO),
                (SRC_VELOCITY, SRC_NONE, DST_EG1_ATTACK, 0, dls(2400))])),
            region((42, 42), (0, 127), TONE, level=1, key_group=1, wave_sample=wsmp(42), art=articulation(1, [
                (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.1))])),
        ]),
        instrument('variation', 1 << 8, 5, [
            region((0, 127), (0, 127), TONE8, level=1, wave_sample=wsmp(72, loop=(LOOP_FORWARD, 1000, 19000))),
        ], art=articulation(1, [
            (SRC_NONE, SRC_NONE, DST_GAIN, 0, dls(-60)),
            (SRC_NONE, SRC_NONE, DST_PITCH, 0, dls(50)),
            (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.3))])),
    ]
    write_dls(os.path.join(directory, 'dls1-test.dls'), 'DLS level 1 test', instruments, waves, table)

    # Expected soundfont
    samples = [
        Sample('tone16', 32000, s['tone'], (32000, 63920), 67),
        Sample('tone8', 32000, tone8_values, (0, 0), 79, correction=-30),
        Sample('noise16', 22050, s['noise'], (0, 0), 60),
    ]
    sf_instruments = [
        ('melodic', [
            ({'freqModLFO': absolute_cents(4), 'delayModLFO': timecents(0.3), 'modLfoToVolume': 30,
              'attackVolEnv': timecents(0.05), 'decayVolEnv': -600, 'keynumToVolEnvDecay': 10, 'sustainVolEnv': 480,
              'releaseVolEnv': timecents(0.5), 'attackModEnv': timecents(0.1), 'decayModEnv': timecents(0.5),
              'sustainModEnv': 1000, 'releaseModEnv': timecents(0.2), 'modEnvToPitch': 100},
             DLS_COMPENSATION + [sf_modulator(sf_source(1, cc=True), 'modLfoToPitch', 50),
                                 sf_modulator(sf_source(VELOCITY), 'attackVolEnv', -1200)]),
            ({'keyRange': (0, 59), 'overridingRootKey': 62, 'fineTune': 10, 'initialAttenuation': 150,
              'sampleModes': 1, 'sampleID': 0}, []),
            ({'keyRange': (60, 127), 'sampleID': 1}, []),
        ]),
        ('drums', [
            ({}, DLS_COMPENSATION),
            ({'keyRange': (36, 36), 'pan': -300, 'releaseVolEnv': timecents(0.2), 'sampleID': 2}, []),
            ({'keyRange': (38, 38), 'exclusiveClass': 1, 'pan': 300, 'releaseVolEnv': timecents(0.2),
              'sampleID': 2}, []),
            ({'keyRange': (42, 42), 'exclusiveClass': 1, 'overridingRootKey': 42, 'releaseVolEnv': timecents(0.1),
              'sampleID': 0}, []),
        ]),
        ('variation', [
            ({'initialAttenuation': 150, 'releaseVolEnv': timecents(0.3)}, DLS_COMPENSATION),
            ({'overridingRootKey': 72, 'fineTune': 80, 'sampleModes': 1, 'startloopAddrsOffset': 1000,
              'endloopAddrsOffset': 20000, 'sampleID': 1}, []),
        ]),
    ]
    sf_presets = [(('melodic', 0, 0), [({'instrument': 0}, [])]),
                  (('drums', 128, 0), [({'instrument': 1}, [])]),
                  (('variation', 1, 5), [({'instrument': 2}, [])])]
    write_sf2(os.path.join(directory, 'dls1-expected.sf2'), 'DLS level 1 expected', samples, sf_instruments,
              sf_presets)


#####################
## DLS level 2 test ##
#####################

def make_dls2(directory, s):
    alaw_bytes = bytes(alaw_encode(v // 256) for v in s['tone'][0:32000])
    alaw_values = [alaw_decode(b) * 256 for b in alaw_bytes]

    waves = [
        dls_wave('tone16', 32000, 1, 16, pcm16(s['tone']), wsmp=wsmp(67, loop=(LOOP_FORWARD, 32000, 31920))),
        dls_wave('stereo', 44100, 2, 16, pcm16([s['left'], s['right']], channels=2)),
        dls_wave('left', 44100, 1, 16, pcm16(s['left']), wsmp=wsmp(69, loop=(LOOP_FORWARD, 11025, 11025))),
        dls_wave('right', 44100, 1, 16, pcm16(s['right']), wsmp=wsmp(69, loop=(LOOP_FORWARD, 10950, 10950))),
        dls_wave('alaw', 32000, 1, 8, alaw_bytes, format_tag=6, wsmp=wsmp(67, loop=(LOOP_FORWARD, 16000, 15920))),
        dls_wave('tone24', 32000, 1, 24, pcm24(s['tone24']), wsmp=wsmp(67, loop=(LOOP_FORWARD, 16000, 15920))),
    ]
    table = list(range(6))
    TONE, STEREO, LEFT_WAVE, RIGHT_WAVE, ALAW, TONE24 = range(6)

    query_dls2 = struct.pack('<HIHH8B', 0x11, 0xf14599e5, 0x4689, 0x11d2, 0xaf, 0xa6, 0x0, 0xaa, 0x0, 0x24, 0xd8,
                             0xb6)
    query_dls1 = struct.pack('<HIHH8B', 0x11, 0x178f2f27, 0xc364, 0x11d1, 0xa7, 0x60, 0x00, 0x00, 0xf8, 0x75, 0xac,
                             0x12)
    not_dls2 = query_dls2 + struct.pack('<H', 0x0F)
    dls1_or_dls2 = query_dls1 + query_dls2 + struct.pack('<H', 0x09)

    release = (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(0.3))
    instruments = [
        instrument('filter vibrato', 0, 0, [
            region((0, 127), (0, 63), TONE, wave_sample=wsmp(67, loop=(LOOP_RELEASE, 32000, 31920))),
            region((0, 127), (64, 127), ALAW),
        ], art=articulation(1, [(SRC_NONE, SRC_NONE, DST_PAN, 0, dls(500))], condition=not_dls2) +
            articulation(2, [
                (SRC_NONE, SRC_NONE, DST_FILTER_CUTOFF, 0, dls_pitch(3000)),
                (SRC_NONE, SRC_NONE, DST_FILTER_Q, 0, dls(60)),
                (SRC_LFO, SRC_NONE, DST_FILTER_CUTOFF, 0, dls(1200)),
                (SRC_NONE, SRC_NONE, DST_LFO_FREQ, 0, dls_pitch(2)),
                (SRC_NONE, SRC_NONE, DST_LFO_DELAY, 0, dls_time(0.2)),
                (SRC_VIBRATO, SRC_NONE, DST_PITCH, 0, dls(30)),
                (SRC_NONE, SRC_NONE, DST_VIB_FREQ, 0, dls_pitch(6)),
                (SRC_NONE, SRC_NONE, DST_VIB_DELAY, 0, dls_time(0.1)),
                (SRC_VIBRATO, SRC_CC(1), DST_PITCH, 0, dls(50)),
                (SRC_LFO, SRC_CHANNELPRESSURE, DST_FILTER_CUTOFF, 0, dls(600)),
                (SRC_VELOCITY, SRC_NONE, DST_FILTER_CUTOFF, 0, dls(-1200)),
                (SRC_KEY, SRC_NONE, DST_FILTER_CUTOFF, 0, dls(2400)),
                (SRC_NONE, SRC_NONE, DST_EG1_DELAY, 0, dls_time(0.02)),
                (SRC_NONE, SRC_NONE, DST_EG1_ATTACK, 0, dls_time(0.01)),
                (SRC_NONE, SRC_NONE, DST_EG1_HOLD, 0, dls_time(0.1)),
                (SRC_NONE, SRC_NONE, DST_EG1_DECAY, 0, dls_time(0.3)),
                (SRC_NONE, SRC_NONE, DST_EG1_SUSTAIN, 0, dls(700)),
                (SRC_NONE, SRC_NONE, DST_EG1_SHUTDOWN, 0, dls_time(0.01)),
                release,
                (SRC_CC(91), SRC_NONE, DST_REVERB, 0, dls(500)),
                (SRC_NONE, SRC_NONE, DST_REVERB, 0, dls(100)),
                (SRC_NONE, SRC_NONE, DST_CHORUS, 0, dls(50)),
            ])),
        instrument('stereo', 0, 1, [
            # Both channels of a stereo wave
            region((0, 59), (0, 127), STEREO),
            # Phase locked waves
            region((60, 127), (0, 127), LEFT_WAVE, link_options=F_WAVELINK_PHASE_MASTER | F_WAVELINK_MULTICHANNEL,
                   phase_group=1, channel=CHANNEL_LEFT),
            region((60, 127), (0, 127), RIGHT_WAVE, link_options=F_WAVELINK_MULTICHANNEL, phase_group=1,
                   channel=CHANNEL_RIGHT),
        ], art=articulation(2, [release])),
        instrument('mod envelope', 0, 2, [
            region((0, 127), (0, 127), TONE24),
        ], art=articulation(2, [
            (SRC_NONE, SRC_NONE, DST_EG2_DELAY, 0, dls_time(0.05)),
            (SRC_NONE, SRC_NONE, DST_EG2_ATTACK, 0, dls_time(0.1)),
            (SRC_NONE, SRC_NONE, DST_EG2_HOLD, 0, dls_time(0.05)),
            (SRC_NONE, SRC_NONE, DST_EG2_DECAY, 0, dls_time(0.4)),
            (SRC_NONE, SRC_NONE, DST_EG2_SUSTAIN, 0, dls(700)),
            (SRC_NONE, SRC_NONE, DST_EG2_RELEASE, 0, dls_time(0.3)),
            (SRC_EG2, SRC_NONE, DST_PITCH, 0, dls(200)),
            (SRC_EG2, SRC_NONE, DST_FILTER_CUTOFF, 0, dls(-2400)),
            (SRC_NONE, SRC_NONE, DST_FILTER_CUTOFF, 0, dls_pitch(6000)),
            (SRC_KEY, SRC_NONE, DST_EG2_DECAY, 0, dls(-2560)),
            (SRC_KEY, SRC_NONE, DST_EG2_HOLD, 0, dls(-1280)),
            release])),
        instrument('local articulation', 0, 3, [
            region((0, 59), (0, 127), TONE),
            # The local articulation replaces the global one: no pan and release of 0 second
            region((60, 127), (0, 127), TONE, art=articulation(2, [
                (SRC_NONE, SRC_NONE, DST_EG1_ATTACK, 0, dls_time(0.2))])),
        ], art=articulation(2, [
            (SRC_NONE, SRC_NONE, DST_EG1_RELEASE, 0, dls_time(1)),
            (SRC_NONE, SRC_NONE, DST_PAN, 0, dls(-200))])),
        instrument('modulators', 0, 4, [
            region((0, 127), (0, 127), TONE),
        ], art=articulation(2, [
            (SRC_NONE, SRC_NONE, DST_FILTER_CUTOFF, 0, dls_pitch(8000)),
            (SRC_CHANNELPRESSURE, SRC_NONE, DST_FILTER_CUTOFF, 0, dls(-2400)),
            (SRC_CC(74), SRC_NONE, DST_FILTER_CUTOFF, dls_transform(src_bipolar=True), dls(2400)),
            (SRC_PITCHWHEEL, SRC_NONE, DST_PITCH, dls_transform(src_bipolar=True), dls(1200)),
            (SRC_CC(11), SRC_NONE, DST_GAIN, dls_transform(src_curve=CONCAVE, src_invert=True), dls(-480)),
            (SRC_CC(2), SRC_NONE, DST_REVERB, dls_transform(src_curve=CONVEX), dls(500)),
            (SRC_VIBRATO, SRC_POLYPRESSURE, DST_PITCH, 0, dls(100)),
            (SRC_NONE, SRC_NONE, DST_VIB_FREQ, 0, dls_pitch(6)),
            (SRC_CC(80), SRC_NONE, DST_GAIN, dls_transform(src_curve=SWITCH), dls(-200)),
            # LFO with its default frequency and delay
            (SRC_LFO, SRC_CC(1), DST_PITCH, 0, dls(50)),
            release])),
        instrument('drums', F_INSTRUMENT_DRUMS, 0, [
            region((36, 36), (0, 127), TONE, key_group=1, options=F_RGN_OPTION_SELFNONEXCLUSIVE,
                   art=articulation(2, [(SRC_NONE, SRC_NONE, DST_PAN, 0, dls(-300)), release])),
            region((38, 38), (0, 127), TONE, key_group=1,
                   art=articulation(2, [(SRC_NONE, SRC_NONE, DST_PAN, 0, dls(300)), release])),
            region((42, 42), (0, 127), TONE, key_group=2, wave_sample=wsmp(54, gain=dls(-120))),
        ]),
    ]
    write_dls(os.path.join(directory, 'dls2-test.dls'), 'DLS level 2 test', instruments, waves, table,
              top_condition=dls1_or_dls2)

    # Expected soundfont
    samples = [
        Sample('tone16', 32000, s['tone'], (32000, 63920), 67),
        Sample('stereoL', 44100, s['left'], (0, 0), 60, sample_type=LEFT, link=2),
        Sample('stereoR', 44100, s['right'], (0, 0), 60, sample_type=RIGHT, link=1),
        Sample('left', 44100, s['left'], (11025, 22050), 69, sample_type=LEFT, link=4),
        Sample('right', 44100, s['right'], (10950, 21900), 69, sample_type=RIGHT, link=3),
        Sample('alaw', 32000, alaw_values, (16000, 31920), 67),
        Sample('tone24', 32000, s['tone24'], (16000, 31920), 67),
    ]
    release_gen = {'releaseVolEnv': timecents(0.3)}
    sf_instruments = [
        ('filter vibrato', [
            ({'initialFilterFc': absolute_cents(3000), 'initialFilterQ': 60, 'modLfoToFilterFc': 1200,
              'freqModLFO': absolute_cents(2), 'delayModLFO': timecents(0.2), 'vibLfoToPitch': 30,
              'freqVibLFO': absolute_cents(6), 'delayVibLFO': timecents(0.1), 'delayVolEnv': timecents(0.02),
              'attackVolEnv': timecents(0.01), 'holdVolEnv': timecents(0.1), 'decayVolEnv': timecents(0.3),
              'sustainVolEnv': 288, 'releaseVolEnv': timecents(0.3), 'reverbEffectsSend': 100,
              'chorusEffectsSend': 50},
             DLS_COMPENSATION[0:3] + [
                 sf_modulator(sf_source(91, cc=True), 'reverbEffectsSend', 500),
                 DLS_COMPENSATION[4],
                 sf_modulator(sf_source(1, cc=True), 'vibLfoToPitch', 50),
                 sf_modulator(sf_source(CHANNEL_PRESSURE), 'modLfoToFilterFc', 600),
                 sf_modulator(sf_source(VELOCITY), 'initialFilterFc', -1200),
                 sf_modulator(sf_source(KEY), 'initialFilterFc', 2400)]),
            ({'velRange': (0, 63), 'sampleModes': 3, 'sampleID': 0}, []),
            ({'velRange': (64, 127), 'sampleModes': 1, 'sampleID': 5}, []),
        ]),
        ('stereo', [
            (release_gen, DLS_COMPENSATION),
            ({'keyRange': (0, 59), 'pan': -500, 'sampleID': 1}, []),
            ({'keyRange': (0, 59), 'pan': 500, 'sampleID': 2}, []),
            ({'keyRange': (60, 127), 'pan': -500, 'sampleModes': 1, 'sampleID': 3}, []),
            ({'keyRange': (60, 127), 'pan': 500, 'sampleModes': 1, 'sampleID': 4}, []),
        ]),
        ('mod envelope', [
            ({'delayModEnv': timecents(0.05), 'attackModEnv': timecents(0.1),
              'holdModEnv': timecents(0.05) - 600, 'keynumToModEnvHold': 10,
              'decayModEnv': timecents(0.4) - 1200, 'keynumToModEnvDecay': 20, 'sustainModEnv': 300,
              'releaseModEnv': timecents(0.3), 'modEnvToPitch': 200, 'modEnvToFilterFc': -2400,
              'initialFilterFc': absolute_cents(6000), 'releaseVolEnv': timecents(0.3), 'sampleModes': 1},
             DLS_COMPENSATION),
            ({'sampleID': 6}, []),
        ]),
        ('local articulation', [
            ({'sampleModes': 1}, DLS_COMPENSATION),
            ({'keyRange': (0, 59), 'releaseVolEnv': timecents(1), 'pan': -200, 'sampleID': 0}, []),
            ({'keyRange': (60, 127), 'attackVolEnv': timecents(0.2), 'sampleID': 0}, []),
        ]),
        ('modulators', [
            ({'initialFilterFc': absolute_cents(8000), 'freqVibLFO': absolute_cents(6), 'delayVibLFO': -7973,
              'freqModLFO': -851, 'delayModLFO': -7973, 'releaseVolEnv': timecents(0.3), 'sampleModes': 1},
             DLS_COMPENSATION + [
                 sf_modulator(sf_source(CHANNEL_PRESSURE), 'initialFilterFc', -2400),
                 sf_modulator(sf_source(74, cc=True, bipolar=True), 'initialFilterFc', 2400),
                 sf_modulator(sf_source(PITCH_WHEEL, bipolar=True), 'fineTune', 1200),
                 sf_modulator(sf_source(11, cc=True, descending=True, curve=CONCAVE), 'initialAttenuation', 480),
                 sf_modulator(sf_source(2, cc=True, curve=CONVEX), 'reverbEffectsSend', 500),
                 sf_modulator(sf_source(POLY_PRESSURE), 'vibLfoToPitch', 100),
                 sf_modulator(sf_source(80, cc=True, curve=SWITCH), 'initialAttenuation', 200),
                 sf_modulator(sf_source(1, cc=True), 'modLfoToPitch', 50)]),
            ({'sampleID': 0}, []),
        ]),
        ('drums', [
            ({}, DLS_COMPENSATION),
            ({'keyRange': (36, 36), 'exclusiveClass': 1, 'pan': -300, 'releaseVolEnv': timecents(0.3),
              'sampleModes': 1, 'sampleID': 0}, []),
            ({'keyRange': (38, 38), 'exclusiveClass': 1, 'pan': 300, 'releaseVolEnv': timecents(0.3),
              'sampleModes': 1, 'sampleID': 0}, []),
            ({'keyRange': (42, 42), 'exclusiveClass': 2, 'overridingRootKey': 54, 'initialAttenuation': 300,
              'sampleID': 0}, []),
        ]),
    ]
    sf_presets = [((name, 0, program), [({'instrument': program}, [])]) for program, (name, _) in
                  enumerate(sf_instruments[:-1])]
    sf_presets.append((('drums', 128, 0), [({'instrument': len(sf_instruments) - 1}, [])]))
    write_sf2(os.path.join(directory, 'dls2-expected.sf2'), 'DLS level 2 expected', samples, sf_instruments,
              sf_presets)


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: make_dls_test_files.py <output directory>')
        sys.exit(1)
    os.makedirs(sys.argv[1], exist_ok=True)
    computed_samples = common_samples()
    make_features(sys.argv[1], computed_samples)
    make_dls1(sys.argv[1], computed_samples)
    make_dls2(sys.argv[1], computed_samples)
