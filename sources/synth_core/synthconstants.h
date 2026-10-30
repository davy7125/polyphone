/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2026 Davy Triponney                                **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program. If not, see http://www.gnu.org/licenses/.    **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: https://www.polyphone.io                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#ifndef SYNTHCONSTANTS_H
#define SYNTHCONSTANTS_H

#define DB_SF2_TO_REAL_DB 0.4

// In voice.h
#define COMPUTATION_CHUNK 64
#define COMPUTATION_CHUNK_SHIFT 6
#define COMPUTATION_CHUNK_INVERT 0.015625
#define RESAMPLING_ORDER 8
#define RESAMPLING_SUBDIVISION 256
#define NOISE_FLOOR 0.000001f // -120 dBFS: a voice in its release is stopped under this amplitude
#define FADE_OUT_DURATION 0.01f // Seconds: fade of a voice stopped to free the polyphony or the processor
#define YOUNG_VOICE_DURATION 0.05f // Seconds: a voice starting is not stopped to free the polyphony or the processor

// In synthcore.h: frames computed at once if no other maximum is given (setMaxBlockSize)
#define DEFAULT_MAX_BLOCK_SIZE 4096

// In synthcore.h and voicelist.h: channels of the voices, from -1 (editor) to MAX_CHANNEL (the editor plays the MIDI
// files on the channels 16 to 31)
#define MAX_CHANNEL 31

// In syntheffects.h: the reverb stops after a silent input and an output under -140 dBFS during more frames than its
// longest delays (also at 192 kHz)
#define REVERB_IDLE_LEVEL 0.0000001f
#define REVERB_IDLE_LENGTH 16384

// In fastmaths.h: alignment of the buffers, in bytes
#define BUFFER_ALIGNMENT 64

// In voicelist.h: maximum number of voices that are not fading out, and default share of the duration of the audio
// that the computation can take in real time
#define MIN_POLYPHONY 16
#define MAX_POLYPHONY 4096
#define DEFAULT_POLYPHONY 1024
#define DEFAULT_CPU_BUDGET 60 // %

// In modulatorgroup.h
#define MAX_NUMBER_OF_PARAMETER_MODULATORS 64

#endif // SYNTHCONSTANTS_H
