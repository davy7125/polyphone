/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2024 Davy Triponney                                **
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

#ifndef FASTMATHS_H
#define FASTMATHS_H

#include <cstdint>
#if defined(__aarch64__)
#include <arm_neon.h>
#elif defined(__SSE2__) || defined(_M_X64)
#include <emmintrin.h>
#endif

// While an instance exists, the denormal numbers (much slower to compute, in the tails of the filters and of the
// reverb) are flushed to zero in the current thread, as the inputs and as the results
class DenormalsDisabled
{
public:
    DenormalsDisabled()
    {
#if defined(__SSE2__) || defined(_M_X64)
        _previousState = _mm_getcsr();
        _mm_setcsr(_previousState | 0x8040); // Flush to zero and denormals are zero
#elif defined(__aarch64__)
        uint64_t fpcr;
        __asm__ __volatile__("mrs %0, fpcr" : "=r"(fpcr));
        _previousState = fpcr;
        fpcr |= (1 << 24); // Flush to zero
        __asm__ __volatile__("msr fpcr, %0" : : "r"(fpcr));
#endif
    }

    ~DenormalsDisabled()
    {
#if defined(__SSE2__) || defined(_M_X64)
        _mm_setcsr(static_cast<unsigned int>(_previousState));
#elif defined(__aarch64__)
        uint64_t fpcr = _previousState;
        __asm__ __volatile__("msr fpcr, %0" : : "r"(fpcr));
#endif
    }

private:
    uint64_t _previousState;
};

class FastMaths
{
public:
    static void initialize();

    static float fastSin(float value); // Range [0; 1] for [0; pi]
    static float fastCos(float value); // Range [0; 1] for [0; pi]

    static float fastPow10(float value); // Range [-102.4; 102.3]
    static float fastPow2(float p)
    {
        float offset = (p < 0) ? 1.0f : 0.0f;
        float clipp = (p < -126) ? -126.0f : p;
        int w = static_cast<int>(clipp);
        float z = clipp - static_cast<float>(w) + offset;
        union { uint32_t i; float f; } v =
            { static_cast<uint32_t> ( (1 << 23) * (clipp + 121.2740575f + 27.7280233f / (4.84252568f - z) - 1.49012907f * z) ) };
        return v.f;
    }

    // Buffers aligned for the functions below, which assume it (BUFFER_ALIGNMENT bytes)
    static float * newAlignedBuffer(uint32_t size);
    static void deleteAlignedBuffer(float * buffer);

    static void addVectors(float * __restrict a, const float * __restrict b, unsigned int size);
    static void clamp(float * values, unsigned int size);
    static void multiplyAdd(float* __restrict data, const float* __restrict dataToMultiplyAndAdd, unsigned int size, float coeff);
    static void multiply(float* __restrict data, const float* __restrict dataToMultiply, unsigned int size, float coeff);

    // True if all values are 0, or if their absolute values are all under a level
    static bool isZero(const float * values, unsigned int size);
    static bool isUnder(const float * values, unsigned int size, float level);

    // Sum of 8 coefficients (aligned on 32 bytes) multiplied by 8 points of 24-bit data, made of 16-bit values and of
    // their 8 least significant bits
    // Inline: called for each point computed by the voices
    static float multiply8(const float * __restrict coeffs, const int16_t * __restrict srcData16, const uint8_t * __restrict srcData24)
    {
        coeffs  = (const float*)__builtin_assume_aligned(coeffs, 32);

#if defined(__aarch64__)
        // Load 8 int16 and convert to int32
        int16x8_t s16 = vld1q_s16(srcData16);
        int32x4_t s16_lo = vmovl_s16(vget_low_s16(s16));
        int32x4_t s16_hi = vmovl_s16(vget_high_s16(s16));

        // Load the 8 LSB uint8, convert to 16-bit
        uint8x8_t lsb_u8 = vld1_u8(srcData24);
        uint16x8_t lsb_u16 = vmovl_u8(lsb_u8);
        uint32x4_t lsb_lo = vmovl_u16(vget_low_u16(lsb_u16));
        uint32x4_t lsb_hi = vmovl_u16(vget_high_u16(lsb_u16));

        // Put data together int24 = (s16 << 8) | lsb
        int32x4_t val_lo = vorrq_s32(vshlq_n_s32(s16_lo, 8), vreinterpretq_s32_u32(lsb_lo));
        int32x4_t val_hi = vorrq_s32(vshlq_n_s32(s16_hi, 8), vreinterpretq_s32_u32(lsb_hi));

        // Convert to float
        float32x4_t f_lo = vcvtq_f32_s32(val_lo);
        float32x4_t f_hi = vcvtq_f32_s32(val_hi);

        // Load the coeffs
        float32x4_t c_lo = vld1q_f32(coeffs);
        float32x4_t c_hi = vld1q_f32(coeffs + 4);

        // Multiplication / addition
        float32x4_t mul_lo = vmulq_f32(c_lo, f_lo);
        float32x4_t mul_hi_and_lo = vfmaq_f32(mul_lo, c_hi, f_hi);
        float32x2_t sum_pair = vadd_f32(vget_low_f32(mul_hi_and_lo), vget_high_f32(mul_hi_and_lo));
        return vget_lane_f32(sum_pair, 0) + vget_lane_f32(sum_pair, 1);
#elif defined(__SSE2__) || defined(_M_X64)
        // Load 8 int16 and the 8 LSB uint8 (unaligned)
        __m128i s16 = _mm_loadu_si128(reinterpret_cast<const __m128i *>(srcData16));
        __m128i lsb = _mm_loadl_epi64(reinterpret_cast<const __m128i *>(srcData24));

        // Put data together int24 = (s16 << 8) | lsb: 32-bit values (s16 << 16) | (lsb << 8), shifted with the sign
        __m128i lsbShifted = _mm_unpacklo_epi8(_mm_setzero_si128(), lsb);
        __m128i val_lo = _mm_srai_epi32(_mm_unpacklo_epi16(lsbShifted, s16), 8);
        __m128i val_hi = _mm_srai_epi32(_mm_unpackhi_epi16(lsbShifted, s16), 8);

        // Convert to float, multiply by the coeffs and add
        __m128 sum = _mm_add_ps(_mm_mul_ps(_mm_load_ps(coeffs), _mm_cvtepi32_ps(val_lo)),
                                _mm_mul_ps(_mm_load_ps(coeffs + 4), _mm_cvtepi32_ps(val_hi)));

        // Sum of the 4 values
        __m128 shuffled = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(2, 3, 0, 1));
        sum = _mm_add_ps(sum, shuffled);
        shuffled = _mm_movehl_ps(shuffled, sum);
        return _mm_cvtss_f32(_mm_add_ss(sum, shuffled));
#else
        return coeffs[0] * static_cast<float>(toInt32(srcData16[0], srcData24[0])) +
               coeffs[1] * static_cast<float>(toInt32(srcData16[1], srcData24[1])) +
               coeffs[2] * static_cast<float>(toInt32(srcData16[2], srcData24[2])) +
               coeffs[3] * static_cast<float>(toInt32(srcData16[3], srcData24[3])) +
               coeffs[4] * static_cast<float>(toInt32(srcData16[4], srcData24[4])) +
               coeffs[5] * static_cast<float>(toInt32(srcData16[5], srcData24[5])) +
               coeffs[6] * static_cast<float>(toInt32(srcData16[6], srcData24[6])) +
               coeffs[7] * static_cast<float>(toInt32(srcData16[7], srcData24[7]));
#endif
    }
    static float multiply4(const float * __restrict coeffs, const float * __restrict srcDataF);

private:
    static float getSinValue(float value); // Range [0; 0.5] for [0; pi / 2]
    static float s_sin_table[2048];
    static float s_pow10_table[2048];
    static inline int32_t toInt32(int16_t val16, uint8_t val8) noexcept {
        return static_cast<int32_t>(static_cast<uint32_t>(val16) << 8) | val8;
    }
};

#endif // FASTMATHS_H
