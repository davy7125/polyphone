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

#include "fastmaths.h"
#include "qmath.h"
#ifdef __aarch64__
#include <arm_neon.h>
#endif

float FastMaths::s_sin_table[2048];
float FastMaths::s_pow10_table[2048];

void FastMaths::initialize()
{
    // Lookup table for sinus, values in [0; pi/2[
    for (int i = 0; i < 2048; i++)
        s_sin_table[i] = (float)sin(static_cast<double>(i) * M_PI_2 / 2048.0);

    // Lookup table for pow10, values in [-102.4; 102.3]
    for (int i = 0; i < 2048; i++)
        s_pow10_table[i] = qPow(10.0, 0.1 * (double)(i - 1024));
}

float FastMaths::fastSin(float value)
{
    return value < 0.5f ? getSinValue(value) : getSinValue(1.f - value);
}

float FastMaths::fastCos(float value)
{
    return value < 0.5f ? getSinValue(0.5f - value) : -getSinValue(value - 0.5f);
}

float FastMaths::getSinValue(float value)
{
    value *= 4096.f;
    int indexBefore = static_cast<int>(value);
    float diff = value - static_cast<float>(indexBefore);

    if (indexBefore < 0)
        return 0.f;
    if (indexBefore > 2047)
        return 1.f;

    // Linear interpolation
    return indexBefore >= 2047 ?
               s_sin_table[2047] + (1.f - s_sin_table[2047]) * diff :
               s_sin_table[indexBefore] + (s_sin_table[indexBefore + 1] - s_sin_table[indexBefore]) * diff;
}

float FastMaths::fastPow10(float value)
{
    value = 10.0f * (value + 102.4f);
    int indexBefore = static_cast<int>(value);
    float diff = value - static_cast<float>(indexBefore);

    if (indexBefore < 0)
        return s_pow10_table[0];
    if (indexBefore > 2047)
        return s_pow10_table[2047];

    // Linear interpolation
    return s_pow10_table[indexBefore] + (s_pow10_table[indexBefore + 1] - s_pow10_table[indexBefore]) * diff;
}

void FastMaths::addVectors(float * __restrict a, const float * __restrict b, unsigned int size)
{
    a = (float*)__builtin_assume_aligned(a, 32);
    b = (const float*)__builtin_assume_aligned(b, 32);
    unsigned int i = 0;

#ifdef __aarch64__
    // Vectorization
    float32x4_t va, vb;
    for (; i + 4 <= size; i += 4)
    {
        // Load values
        va = vld1q_f32(a + i);
        vb = vld1q_f32(b + i);

        // Add
        va = vaddq_f32(va, vb);

        // Store the result
        vst1q_f32(a + i, va);
    }
#else
    for (; i < size; ++i)
        a[i] += b[i];
#endif
}

void FastMaths::clamp(float * values, unsigned int size)
{
    values = (float*)__builtin_assume_aligned(values, 32);
    unsigned int i = 0;

#ifdef __aarch64__
    // Vectorization
    float32x4_t max_val = vdupq_n_f32(1.0f);
    float32x4_t min_val = vdupq_n_f32(-1.0f);
    float32x4_t v;
    for (; i + 4 <= size; i += 4)
    {
        v = vld1q_f32(values + i);  // Load
        v = vminq_f32(v, max_val); // Max value is 1.0
        v = vmaxq_f32(v, min_val); // Min value is -1.0
        vst1q_f32(values + i, v);   // Store
    }
#else
    for (; i < size; ++i)
    {
        if (values[i] > 1.0f)
            values[i] = 1.0f;
        else if (values[i] < -1.0f)
            values[i] = -1.0f;
    }
#endif
}

void FastMaths::multiplyAdd(float * __restrict data, const float * __restrict dataToMultiplyAndAdd, unsigned int size, float coeff)
{
    data = (float*)__builtin_assume_aligned(data, 32);
    dataToMultiplyAndAdd = (float*)__builtin_assume_aligned(dataToMultiplyAndAdd, 32);
    unsigned int i = 0;

#ifdef __aarch64__
    // Vectorization
    float32x4_t vCoeff = vdupq_n_f32(coeff);
    float32x4_t v1, v2;
    for (; i + 4 <= size; i += 4)
    {
        // Load data
        v1 = vld1q_f32(data + i);
        v2 = vld1q_f32(dataToMultiplyAndAdd + i);

        // Multiply and add
        v1 = vfmaq_f32(v1, v2, vCoeff);

        // Store the result
        vst1q_f32(data + i, v1);
    }
#else
    for (; i < size; ++i)
        data[i] += coeff * dataToMultiplyAndAdd[i];
#endif
}

void FastMaths::multiply(float * __restrict data, const float * __restrict dataToMultiply, unsigned int size, float coeff)
{
    data = (float*)__builtin_assume_aligned(data, 32);
    dataToMultiply = (const float*)__builtin_assume_aligned(dataToMultiply, 32);
    unsigned int i = 0;

#ifdef __aarch64__
    // Vectorization
    float32x4_t vCoeff = vdupq_n_f32(coeff);
    float32x4_t v2;
    for (; i + 4 <= size; i += 4)
    {
        // Load data
        v2 = vld1q_f32(dataToMultiply + i);

        // Multiply
        v2 = vmulq_f32(v2, vCoeff);

        // Store the result
        vst1q_f32(data + i, v2);
    }
#else
    for (; i < size; ++i)
        data[i] = coeff * dataToMultiply[i];
#endif
}

float FastMaths::multiply8(const float * __restrict coeffs, const qint16 * __restrict srcData16, const quint8 * __restrict srcData24)
{
    coeffs  = (const float*)__builtin_assume_aligned(coeffs, 32);

#ifdef __aarch64__
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
#else
    return coeffs[0] * static_cast<float>(toInt32(srcData16[0], srcData24[0])) +
           coeffs[1] * static_cast<float>(toInt32(srcData16[1], srcData24[1])) +
           coeffs[2] * static_cast<float>(toInt32(srcData16[2], srcData24[2])) +
           coeffs[3] * static_cast<float>(toInt32(srcData16[3], srcData24[3])) +
           coeffs[4] * static_cast<float>(toInt32(srcData16[4], srcData24[4])) +
           coeffs[5] * static_cast<float>(toInt32(srcData16[5], srcData24[5])) +
           coeffs[6] * static_cast<float>(toInt32(srcData16[6], srcData24[6]));
#endif
}
