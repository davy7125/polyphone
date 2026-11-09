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


#ifndef TEXTCONVERSIONS_H
#define TEXTCONVERSIONS_H

#include <cmath>
#include <string>
#include <vector>

// Text and number conversions of the readers shared by the editor and the plugin (sfz, dls). They behave as the
// functions of Qt that the editor used, so that a file is read the same way: UTF-8 texts whose lengths are counted
// in UTF-16 code units, white spaces of Unicode, numbers in the C locale.
namespace TextConversions
{
// Lower case of the ASCII letters
std::string toLower(std::string text);

// White spaces removed at both ends, as QString::trimmed
std::string trimmed(const std::string &text);

// All occurrences of a text replaced, as QString::replace
std::string replaceAll(std::string text, const std::string &before, const std::string &after);

// Parts separated by a text, possibly without the empty ones, as QString::split
std::vector<std::string> split(const std::string &text, const std::string &separator, bool keepEmptyParts);

// Number of UTF-16 code units, and the first ones (QString::size and QString::left)
int utf16Length(const std::string &text);
std::string utf16Left(const std::string &text, int count);

// Integer or real number of a text in the C locale, possibly surrounded by white spaces, 0 if it is not valid (as
// QString::toInt, QString::toUInt and QString::toDouble)
int toInt(const std::string &text, bool * ok = nullptr);
unsigned int toUInt(const std::string &text, bool * ok = nullptr);
double toDouble(const std::string &text, bool * ok = nullptr);

// Rounding of Qt (qRound)
inline int qtRound(double value)
{
#if defined(__aarch64__) && defined(__GNUC__) && !defined(__clang__)
    return static_cast<int>(__builtin_round(value));
#elif defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
    return static_cast<int>(value + std::copysign(0.5, value));
#else
    return static_cast<int>(value >= 0.0 ? value + 0.5 : value - 0.5);
#endif
}

inline int qtRound(float value)
{
#if defined(__aarch64__) && defined(__GNUC__) && !defined(__clang__)
    return static_cast<int>(__builtin_roundf(value));
#elif defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
    return static_cast<int>(value + std::copysign(0.5f, value));
#else
    return static_cast<int>(value >= 0.0f ? value + 0.5f : value - 0.5f);
#endif
}
} // namespace TextConversions

#endif // TEXTCONVERSIONS_H
