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
#include <cmath>
#include <new>
#include "synthconstants.h"
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
        s_pow10_table[i] = std::pow(10.0, 0.1 * (double)(i - 1024));
}

float * FastMaths::newAlignedBuffer(uint32_t size)
{
    return static_cast<float *>(::operator new[](size * sizeof(float), std::align_val_t(BUFFER_ALIGNMENT)));
}

void FastMaths::deleteAlignedBuffer(float * buffer)
{
    ::operator delete[](buffer, std::align_val_t(BUFFER_ALIGNMENT));
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

#ifdef __aarch64__
    unsigned int i = 0;
    const unsigned int simdSize = size & ~15u; // 16 floats per loop

    for (; i < simdSize; i += 16)
    {
        float32x4_t a0 = vld1q_f32(a + i + 0);
        float32x4_t a1 = vld1q_f32(a + i + 4);
        float32x4_t a2 = vld1q_f32(a + i + 8);
        float32x4_t a3 = vld1q_f32(a + i + 12);

        float32x4_t b0 = vld1q_f32(b + i + 0);
        float32x4_t b1 = vld1q_f32(b + i + 4);
        float32x4_t b2 = vld1q_f32(b + i + 8);
        float32x4_t b3 = vld1q_f32(b + i + 12);

        a0 = vaddq_f32(a0, b0);
        a1 = vaddq_f32(a1, b1);
        a2 = vaddq_f32(a2, b2);
        a3 = vaddq_f32(a3, b3);

        vst1q_f32(a + i + 0,  a0);
        vst1q_f32(a + i + 4,  a1);
        vst1q_f32(a + i + 8,  a2);
        vst1q_f32(a + i + 12, a3);
    }
#else
    for (unsigned int i = 0; i < size; ++i)
        a[i] += b[i];
#endif
}

void FastMaths::clamp(float * values, unsigned int size)
{
    values = (float*)__builtin_assume_aligned(values, 32);

#ifdef __aarch64__

    const float32x4_t vmaxv = vdupq_n_f32(1.0f);
    const float32x4_t vminv = vdupq_n_f32(-1.0f);

    unsigned int i = 0;
    const unsigned int simdSize = size & ~15u; // 16 floats

    for (; i < simdSize; i += 16)
    {
        float32x4_t v0 = vld1q_f32(values + i + 0);
        float32x4_t v1 = vld1q_f32(values + i + 4);
        float32x4_t v2 = vld1q_f32(values + i + 8);
        float32x4_t v3 = vld1q_f32(values + i + 12);

        v0 = vmaxq_f32(vminq_f32(v0, vmaxv), vminv);
        v1 = vmaxq_f32(vminq_f32(v1, vmaxv), vminv);
        v2 = vmaxq_f32(vminq_f32(v2, vmaxv), vminv);
        v3 = vmaxq_f32(vminq_f32(v3, vmaxv), vminv);

        vst1q_f32(values + i + 0,  v0);
        vst1q_f32(values + i + 4,  v1);
        vst1q_f32(values + i + 8,  v2);
        vst1q_f32(values + i + 12, v3);
    }
#else
    for (unsigned int i = 0; i < size; ++i)
    {
        if (values[i] > 1.0f)
            values[i] = 1.0f;
        else if (values[i] < -1.0f)
            values[i] = -1.0f;
    }
#endif
}

void FastMaths::multiplyAdd(float * __restrict data, const float * __restrict dataToMultiplyAndAdd,
                            unsigned int size, float coeff)
{
    data = (float*)__builtin_assume_aligned(data, 32);
    dataToMultiplyAndAdd = (const float*)__builtin_assume_aligned(dataToMultiplyAndAdd, 32);

#ifdef __aarch64__

    const float32x4_t vCoeff = vdupq_n_f32(coeff);

    unsigned int i = 0;
    const unsigned int simdSize = size & ~15u; // 16 floats

    for (; i < simdSize; i += 16)
    {
        // Load 4x4 floats from both arrays (128B total working set)

        float32x4_t d0 = vld1q_f32(data + i + 0);
        float32x4_t d1 = vld1q_f32(data + i + 4);
        float32x4_t d2 = vld1q_f32(data + i + 8);
        float32x4_t d3 = vld1q_f32(data + i + 12);

        float32x4_t s0 = vld1q_f32(dataToMultiplyAndAdd + i + 0);
        float32x4_t s1 = vld1q_f32(dataToMultiplyAndAdd + i + 4);
        float32x4_t s2 = vld1q_f32(dataToMultiplyAndAdd + i + 8);
        float32x4_t s3 = vld1q_f32(dataToMultiplyAndAdd + i + 12);

        // FMA: d += s * coeff
        d0 = vfmaq_f32(d0, s0, vCoeff);
        d1 = vfmaq_f32(d1, s1, vCoeff);
        d2 = vfmaq_f32(d2, s2, vCoeff);
        d3 = vfmaq_f32(d3, s3, vCoeff);

        vst1q_f32(data + i + 0,  d0);
        vst1q_f32(data + i + 4,  d1);
        vst1q_f32(data + i + 8,  d2);
        vst1q_f32(data + i + 12, d3);
    }
#else
    for (unsigned int i = 0; i < size; ++i)
        data[i] += coeff * dataToMultiplyAndAdd[i];
#endif
}

void FastMaths::multiply(float * __restrict data, const float * __restrict dataToMultiply, unsigned int size, float coeff)
{
    data = (float*)__builtin_assume_aligned(data, 32);
    dataToMultiply = (const float*)__builtin_assume_aligned(dataToMultiply, 32);

#ifdef __aarch64__
    const float32x4_t vCoeff = vdupq_n_f32(coeff);

    unsigned int i = 0;
    const unsigned int simdSize = size & ~15u; // 16 floats = 64B chunk

    for (; i < simdSize; i += 16)
    {
        // Unrolled 4x NEON (128 bytes total)

        float32x4_t v0 = vld1q_f32(dataToMultiply + i + 0);
        float32x4_t v1 = vld1q_f32(dataToMultiply + i + 4);
        float32x4_t v2 = vld1q_f32(dataToMultiply + i + 8);
        float32x4_t v3 = vld1q_f32(dataToMultiply + i + 12);

        v0 = vmulq_f32(v0, vCoeff);
        v1 = vmulq_f32(v1, vCoeff);
        v2 = vmulq_f32(v2, vCoeff);
        v3 = vmulq_f32(v3, vCoeff);

        vst1q_f32(data + i + 0,  v0);
        vst1q_f32(data + i + 4,  v1);
        vst1q_f32(data + i + 8,  v2);
        vst1q_f32(data + i + 12, v3);
    }
#else
    for (unsigned int i = 0; i < size; ++i)
        data[i] = coeff * dataToMultiply[i];
#endif
}

bool FastMaths::isZero(const float * values, unsigned int size)
{
    for (unsigned int i = 0; i < size; ++i)
        if (values[i] != 0.0f)
            return false;
    return true;
}

bool FastMaths::isUnder(const float * values, unsigned int size, float level)
{
    for (unsigned int i = 0; i < size; ++i)
        if (values[i] >= level || values[i] <= -level)
            return false;
    return true;
}

float FastMaths::multiply4(const float * __restrict coeffs, const float * __restrict srcDataF)
{
    coeffs = (const float*)__builtin_assume_aligned(coeffs, 32);
    srcDataF = (const float*)__builtin_assume_aligned(srcDataF, 32);

#ifdef __aarch64__
    // Load all values
    float32x4_t a = vld1q_f32(coeffs);
    float32x4_t b = vld1q_f32(srcDataF);

    // Multiply everything
    float32x4_t prod = vmulq_f32(a, b);

    // Return the sum
    return vaddvq_f32(prod);
#else
    return coeffs[0] * srcDataF[0] +
           coeffs[1] * srcDataF[1] +
           coeffs[2] * srcDataF[2] +
           coeffs[3] * srcDataF[3];
#endif
}
