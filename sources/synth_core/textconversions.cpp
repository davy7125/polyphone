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


#include "textconversions.h"
#include <climits>
#include <cstdint>
#include <locale>
#include <sstream>

namespace TextConversions
{
// Length of the UTF-8 sequence starting with a byte, and its code point
static size_t decode(const std::string &text, size_t position, uint32_t &codePoint)
{
    unsigned char c = static_cast<unsigned char>(text[position]);
    size_t length = c < 0x80 ? 1 : (c >= 0xF0 ? 4 : (c >= 0xE0 ? 3 : (c >= 0xC0 ? 2 : 1)));
    if (position + length > text.size())
        length = 1;
    codePoint = (length == 1) ? c : (c & (0xFF >> (length + 1)));
    for (size_t i = 1; i < length; i++)
        codePoint = (codePoint << 6) | (static_cast<unsigned char>(text[position + i]) & 0x3F);
    return length;
}

// White space of Unicode, as QChar::isSpace
static bool isSpace(uint32_t c)
{
    return (c >= 0x09 && c <= 0x0D) || c == 0x20 || c == 0x85 || c == 0xA0 || c == 0x1680 ||
           (c >= 0x2000 && c <= 0x200A) || c == 0x2028 || c == 0x2029 || c == 0x202F || c == 0x205F || c == 0x3000;
}

std::string toLower(std::string text)
{
    for (char &c : text)
        if (c >= 'A' && c <= 'Z')
            c = static_cast<char>(c - 'A' + 'a');
    return text;
}

std::string trimmed(const std::string &text)
{
    // First character that is not a space
    size_t start = 0;
    uint32_t codePoint;
    while (start < text.size())
    {
        size_t length = decode(text, start, codePoint);
        if (!isSpace(codePoint))
            break;
        start += length;
    }

    // Character following the last one that is not a space
    size_t end = start;
    for (size_t position = start; position < text.size(); )
    {
        size_t length = decode(text, position, codePoint);
        position += length;
        if (!isSpace(codePoint))
            end = position;
    }
    return text.substr(start, end - start);
}

std::string replaceAll(std::string text, const std::string &before, const std::string &after)
{
    if (before.empty())
        return text;
    for (size_t position = text.find(before); position != std::string::npos;
         position = text.find(before, position + after.size()))
        text.replace(position, before.size(), after);
    return text;
}

std::vector<std::string> split(const std::string &text, const std::string &separator, bool keepEmptyParts)
{
    std::vector<std::string> parts;
    size_t start = 0;
    while (true)
    {
        size_t position = separator.empty() ? std::string::npos : text.find(separator, start);
        std::string part = text.substr(start, position == std::string::npos ? std::string::npos : position - start);
        if (keepEmptyParts || !part.empty())
            parts.push_back(part);
        if (position == std::string::npos)
            break;
        start = position + separator.size();
    }
    return parts;
}

int utf16Length(const std::string &text)
{
    int length = 0;
    uint32_t codePoint;
    for (size_t position = 0; position < text.size(); )
    {
        position += decode(text, position, codePoint);
        length += codePoint >= 0x10000 ? 2 : 1;
    }
    return length;
}

std::string utf16Left(const std::string &text, int count)
{
    int length = 0;
    uint32_t codePoint;
    size_t position = 0;
    while (position < text.size())
    {
        size_t byteCount = decode(text, position, codePoint);
        int units = codePoint >= 0x10000 ? 2 : 1;
        if (length + units > count)
            break; // A surrogate pair is not cut (Qt would keep its first half, not valid in UTF-8)
        length += units;
        position += byteCount;
    }
    return text.substr(0, position);
}

// Number of the C locale: optional sign then digits (and for a real number a decimal point and an exponent), without
// spaces inside, possibly surrounded by spaces
static std::string getNumberText(const std::string &text, bool isReal, bool &valid)
{
    std::string number = trimmed(text);
    valid = !number.empty();
    for (size_t i = 0; i < number.size() && valid; i++)
    {
        char c = number[i];
        if (c >= '0' && c <= '9')
            continue;
        if ((c == '+' || c == '-') && (i == 0 || (isReal && (number[i - 1] == 'e' || number[i - 1] == 'E'))))
            continue;
        if (isReal && (c == '.' || c == 'e' || c == 'E'))
            continue;
        valid = false;
    }
    return number;
}

int toInt(const std::string &text, bool * ok)
{
    bool valid;
    std::string number = getNumberText(text, false, valid);
    long long value = 0;
    if (valid)
    {
        size_t position = (number[0] == '+' || number[0] == '-') ? 1 : 0;
        valid = position < number.size();
        for (; position < number.size() && valid; position++)
        {
            value = 10 * value + (number[position] - '0');
            if (value > static_cast<long long>(INT_MAX) + 1)
                valid = false;
        }
        if (number[0] == '-')
            value = -value;
        valid = valid && value >= INT_MIN && value <= INT_MAX;
    }
    if (ok != nullptr)
        *ok = valid;
    return valid ? static_cast<int>(value) : 0;
}

unsigned int toUInt(const std::string &text, bool * ok)
{
    bool valid;
    std::string number = getNumberText(text, false, valid);
    unsigned long long value = 0;
    if (valid)
    {
        // A minus sign is only possible for 0
        size_t position = (number[0] == '+' || number[0] == '-') ? 1 : 0;
        valid = position < number.size();
        for (; position < number.size() && valid; position++)
        {
            value = 10 * value + static_cast<unsigned long long>(number[position] - '0');
            if (value > UINT_MAX)
                valid = false;
        }
        if (number[0] == '-' && value != 0)
            valid = false;
    }
    if (ok != nullptr)
        *ok = valid;
    return valid ? static_cast<unsigned int>(value) : 0;
}

double toDouble(const std::string &text, bool * ok)
{
    bool valid;
    std::string number = getNumberText(text, true, valid);
    double value = 0;
    if (valid)
    {
        std::istringstream stream(number);
        stream.imbue(std::locale::classic());
        stream >> value;
        valid = !stream.fail() && stream.peek() == std::char_traits<char>::eof() && std::isfinite(value);
    }
    if (ok != nullptr)
        *ok = valid;
    return valid ? value : 0;
}
} // namespace TextConversions
