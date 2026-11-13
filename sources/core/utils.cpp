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
**             Date: 19.09.2016                                           **
***************************************************************************/

#include "utils.h"
#include "samplefilereader.h"
#include "synthmath.h"
#include "sf2reader.h"
#include <QStringList>
#include <cmath>
#include <QFile>
#include <QRegularExpression>
#include <QMimeData>
#include <QUrl>
#include <QDir>
#include <QLocale>
#include <QVarLengthArray>
#include <QHash>
#include <QDebug>

#ifdef USE_WOLFSSL
#include <wolfssl/options.h>
#include <wolfssl/openssl/pem.h>

// Debian #962149, fixed in libwolfssl-dev (>= 4.4.0+dfsg-4~)
#ifndef BIO_FLAGS_BASE64_NO_NL
#define BIO_FLAGS_BASE64_NO_NL WOLFSSL_BIO_FLAG_BASE64_NO_NL
#endif
#else
#include "openssl/pem.h"
#endif

QString Utils::s_diacriticLetters;
QStringList Utils::s_noDiacriticLetters;
float Utils::s_floatConversionCoef;

void Utils::prepareStaticVariables()
{

    s_diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
    s_noDiacriticLetters << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"
                         <<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"
                        <<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"
                       <<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
    s_floatConversionCoef = 1.0f / 8388607.5f; // (0.5f + 0x7FFFFF);
}

namespace
{
// Natural order: Unicode Collation Algorithm (UTS #10) with the root order of CLDR and its "numeric" option, as
// implemented by ICU (see Utils::naturalOrder). Each character gives collation elements with 3 weights:
// - primary: group (whitespace < punctuation < symbol < currency < number < letter) then the character in its group,
//   a letter being compared without its accents and its case and a sequence of digits by its numerical value,
// - secondary: accent,
// - tertiary: case,
// then the leading zeros of the numbers are compared.
enum : quint32
{
    GROUP_SPACE = 1u << 24,
    GROUP_PUNCTUATION = 2u << 24,
    GROUP_SYMBOL = 3u << 24,
    GROUP_CURRENCY = 4u << 24,
    GROUP_NUMBER = 5u << 24,
    GROUP_LETTER = 6u << 24
};

const quint32 SECONDARY_COMMON = 1; // Character without accent
const quint32 SECONDARY_LIGATURE = 0x200; // Letters such as "æ", "ß", "ð" after their base letters
const quint8 TERTIARY_UPPER = 0; // Uppercase first, as the option "caseFirst=upper" of CLDR
const quint8 TERTIARY_OTHER = 1;

struct CollationElement
{
    quint32 primary; // 0 for an accent, which is then ignored at the first level
    quint32 secondary;
    quint8 tertiary;
    int digitStart; // For a number: position and count of its significant digits in CollationElements::digits
    int digitCount;
    int leadingZeros;
};

struct CollationElements
{
    QVarLengthArray<CollationElement, 64> elements;
    QVarLengthArray<quint8, 64> digits;

    void add(quint32 primary, quint32 secondary, quint8 tertiary)
    {
        CollationElement element = {primary, secondary, tertiary, 0, 0, 0};
        elements.append(element);
    }
};

// Rank of a whitespace, punctuation mark, symbol or currency sign: most frequent ones in the root order of CLDR
// (generated with ICU), then the other ones by code point
quint32 symbolRank(uint c)
{
    static const ushort ORDER[] = {
        0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x2028, 0x2029, 0x0020, 0x203E, 0x005F, 0x2017, 0x002D,
        0x2010, 0x2012, 0x2013, 0x2014, 0x2015, 0x2053, 0x301C, 0x3030, 0x002C, 0x3001, 0x003B, 0x204F,
        0x003A, 0x0021, 0x00A1, 0x003F, 0x00BF, 0x203D, 0x002E, 0x3002, 0x00B7, 0x2055, 0x2056, 0x2058,
        0x2059, 0x205A, 0x205B, 0x205C, 0x205D, 0x205E, 0x0027, 0x2018, 0x2019, 0x201A, 0x201B, 0x2039,
        0x203A, 0x0022, 0x201C, 0x201D, 0x201E, 0x201F, 0x301D, 0x301E, 0x301F, 0x00AB, 0x00BB, 0x0028,
        0x0029, 0x005B, 0x005D, 0x007B, 0x007D, 0x2045, 0x2046, 0x3008, 0x3009, 0x300A, 0x300B, 0x300C,
        0x300D, 0x300E, 0x300F, 0x3010, 0x3011, 0x3014, 0x3015, 0x3016, 0x3017, 0x3018, 0x3019, 0x301A,
        0x301B, 0x2016, 0x00A7, 0x00B6, 0x204B, 0x0040, 0x002A, 0x204E, 0x2051, 0x002F, 0x005C, 0x0026,
        0x204A, 0x0023, 0x0025, 0x2030, 0x2031, 0x2020, 0x2021, 0x2022, 0x2023, 0x2027, 0x2043, 0x204C,
        0x204D, 0x2032, 0x2035, 0x3003, 0x303D, 0x2038, 0x203B, 0x203F, 0x2054, 0x2040, 0x2050, 0x2041,
        0x2042, 0x0060, 0x00B4, 0x02DC, 0x005E, 0x00AF, 0x02D8, 0x02D9, 0x00A8, 0x02DA, 0x02DD, 0x00B8,
        0x02DB, 0x02C2, 0x02C3, 0x02C4, 0x02C5, 0x02D2, 0x02D3, 0x02D4, 0x02D5, 0x02D6, 0x02D7, 0x02DE,
        0x02DF, 0x00B0, 0x00A9, 0x00AE, 0x002B, 0x00B1, 0x00F7, 0x00D7, 0x003C, 0x003D, 0x003E, 0x00AC,
        0x007C, 0x00A6, 0x007E, 0x2052, 0x2044, 0x3004, 0x3012, 0x3013, 0x3020, 0x3037, 0x303E, 0x303F,
        0x00A4, 0x00A2, 0x0024, 0x00A3, 0x00A5, 0x20A0, 0x20A1, 0x20A2, 0x20A3, 0x20A4, 0x20A5, 0x20A6,
        0x20A9, 0x20AA, 0x20AB, 0x20AC, 0x20AD, 0x20AE, 0x20AF, 0x20B0, 0x20B1, 0x20B2, 0x20B3, 0x20B4,
        0x20B5, 0x20B6, 0x20B7, 0x20B8, 0x20B9, 0x20BA, 0x20BB, 0x20BC, 0x20BD, 0x20BE, 0x20BF, 0x20C0,
        0x20A7
    };
    static const QHash<uint, quint32> RANKS = [] {
        QHash<uint, quint32> ranks;
        for (uint i = 0; i < sizeof(ORDER) / sizeof(ORDER[0]); i++)
            ranks[ORDER[i]] = i + 1;
        return ranks;
    }();
    return RANKS.value(c, 0x1000 + c);
}

// Secondary weight of a combining mark, the most frequent ones being in the root order of CLDR
quint32 accentWeight(uint mark)
{
    static const ushort ORDER[] = {
        0x0301, // acute
        0x0300, // grave
        0x0306, // breve
        0x0302, // circumflex
        0x030C, // caron
        0x030A, // ring
        0x0308, // diaeresis
        0x030B, // double acute
        0x0303, // tilde
        0x0307, // dot
        0x0338, // slash (ø)
        0x0327, // cedilla
        0x0328, // ogonek
        0x0304, // macron
        0x0336, // long stroke
        0x0335 // short stroke (đ, ħ, ł)
    };
    for (uint i = 0; i < sizeof(ORDER) / sizeof(ORDER[0]); i++)
        if (ORDER[i] == mark)
            return SECONDARY_COMMON + 1 + i;
    return SECONDARY_LIGATURE + mark;
}

uint nextCodePoint(const QString &str, int &pos)
{
    QChar c = str[pos++];
    if (c.isHighSurrogate() && pos < str.length() && str[pos].isLowSurrogate())
        return QChar::surrogateToUcs4(c, str[pos++]);
    return c.unicode();
}

void addLetter(CollationElements &result, uint c)
{
    uint folded = QChar::toCaseFolded(c);
    quint8 tertiary = (QChar::isUpper(c) || QChar::isTitleCase(c)) ? TERTIARY_UPPER : TERTIARY_OTHER;

    // Letters without decomposition, sorted as in the root order of CLDR
    const char * base = nullptr; // Base letters, followed by a secondary difference
    quint32 secondary = 0;
    switch (folded)
    {
    case 0x00DF: base = "ss"; secondary = SECONDARY_LIGATURE; break; // ß
    case 0x00E6: base = "ae"; secondary = SECONDARY_LIGATURE; break; // æ
    case 0x0153: base = "oe"; secondary = SECONDARY_LIGATURE; break; // œ
    case 0x00F0: base = "d"; secondary = SECONDARY_LIGATURE; break; // ð
    case 0x00F8: base = "o"; secondary = accentWeight(0x0338); break; // ø
    case 0x0111: base = "d"; secondary = accentWeight(0x0335); break; // đ
    case 0x0127: base = "h"; secondary = accentWeight(0x0335); break; // ħ
    case 0x0142: base = "l"; secondary = accentWeight(0x0335); break; // ł
    case 0x0131: folded = ('i' << 1) | 1; break; // ı, just after i (letters have even primary weights)
    case 0x014B: folded = ('n' << 1) | 1; break; // ŋ, just after n
    default: folded <<= 1; break;
    }

    if (base == nullptr)
        result.add(GROUP_LETTER | folded, SECONDARY_COMMON, tertiary);
    else
    {
        for (const char * p = base; *p != 0; p++)
            result.add(GROUP_LETTER | (static_cast<uchar>(*p) << 1), SECONDARY_COMMON, tertiary);
        result.add(0, secondary, TERTIARY_OTHER);
    }
}

void decompose(const QString &str, QVarLengthArray<uint, 128> &codePoints)
{
    const int length = str.length();
    int pos = 0;
    while (pos < length)
    {
        int start = pos;
        uint c = nextCodePoint(str, pos);
        if (c >= 0x80 && QChar::decompositionTag(c) != QChar::NoDecomposition)
        {
            // Compatibility decomposition: accents separated from their letters, variants such as "ﬁ", "²", "Ｐ"
            // replaced by their base characters, except the spacing accents such as "´" (symbols)
            const QString decomposed = str.mid(start, pos - start).normalized(QString::NormalizationForm_KD);
            if (decomposed[0] != QChar(' ') || QChar::isSpace(c))
            {
                int decomposedPos = 0;
                while (decomposedPos < decomposed.length())
                    codePoints.append(nextCodePoint(decomposed, decomposedPos));
                continue;
            }
        }
        codePoints.append(c);
    }
}

void computeCollationElements(const QString &str, CollationElements &result)
{
    QVarLengthArray<uint, 128> codePoints;
    decompose(str, codePoints);

    const int count = codePoints.size();
    for (int pos = 0; pos < count; pos++)
    {
        uint c = codePoints[pos];
        if (c >= 'a' && c <= 'z') // Fast path for the most frequent characters
        {
            result.add(GROUP_LETTER | (c << 1), SECONDARY_COMMON, TERTIARY_OTHER);
            continue;
        }
        if (c >= 'A' && c <= 'Z')
        {
            result.add(GROUP_LETTER | ((c + 'a' - 'A') << 1), SECONDARY_COMMON, TERTIARY_UPPER);
            continue;
        }
        if (QChar::isSpace(c))
        {
            result.add(GROUP_SPACE | symbolRank(c), SECONDARY_COMMON, TERTIARY_OTHER);
            continue;
        }

        quint32 group = 0;
        switch (QChar::category(c))
        {
        case QChar::Other_Control: case QChar::Other_Format:
            continue; // Ignored
        case QChar::Mark_NonSpacing: case QChar::Mark_SpacingCombining: case QChar::Mark_Enclosing:
            result.add(0, accentWeight(c), TERTIARY_OTHER);
            continue;
        case QChar::Number_DecimalDigit: {
            // Sequence of digits, compared by its value
            CollationElement number = {GROUP_NUMBER, SECONDARY_COMMON, TERTIARY_OTHER, static_cast<int>(result.digits.size()), 0, 0};
            while (true)
            {
                int value = QChar::digitValue(codePoints[pos]);
                if (value == 0 && number.digitCount == 0)
                    number.leadingZeros++;
                else
                {
                    result.digits.append(static_cast<quint8>(value));
                    number.digitCount++;
                }
                if (pos + 1 >= count || QChar::category(codePoints[pos + 1]) != QChar::Number_DecimalDigit)
                    break;
                pos++;
            }
            result.elements.append(number);
        } continue;
        case QChar::Punctuation_Connector: case QChar::Punctuation_Dash: case QChar::Punctuation_Open:
        case QChar::Punctuation_Close: case QChar::Punctuation_InitialQuote: case QChar::Punctuation_FinalQuote:
        case QChar::Punctuation_Other:
            group = GROUP_PUNCTUATION;
            break;
        case QChar::Symbol_Math: case QChar::Symbol_Modifier: case QChar::Symbol_Other:
            group = GROUP_SYMBOL;
            break;
        case QChar::Symbol_Currency:
            group = GROUP_CURRENCY;
            break;
        default:
            addLetter(result, c);
            continue;
        }

        result.add(group | symbolRank(c), SECONDARY_COMMON, TERTIARY_OTHER);
    }
}

int comparePrimaryWeights(const CollationElements &ce1, const CollationElements &ce2)
{
    const int count1 = ce1.elements.size();
    const int count2 = ce2.elements.size();
    int i1 = 0, i2 = 0;
    while (true)
    {
        // Skip accents
        while (i1 < count1 && ce1.elements[i1].primary == 0)
            i1++;
        while (i2 < count2 && ce2.elements[i2].primary == 0)
            i2++;
        if (i1 == count1 || i2 == count2)
            return (i1 < count1) - (i2 < count2);

        const CollationElement &e1 = ce1.elements[i1++];
        const CollationElement &e2 = ce2.elements[i2++];
        if (e1.primary != e2.primary)
            return e1.primary < e2.primary ? -1 : 1;
        if (e1.primary == GROUP_NUMBER)
        {
            // More significant digits => greater number
            if (e1.digitCount != e2.digitCount)
                return e1.digitCount < e2.digitCount ? -1 : 1;
            for (int i = 0; i < e1.digitCount; i++)
                if (ce1.digits[e1.digitStart + i] != ce2.digits[e2.digitStart + i])
                    return ce1.digits[e1.digitStart + i] < ce2.digits[e2.digitStart + i] ? -1 : 1;
        }
    }
}

template<typename T>
int compareWeights(const CollationElements &ce1, const CollationElements &ce2, T CollationElement::*weight)
{
    const int count = qMin(ce1.elements.size(), ce2.elements.size());
    for (int i = 0; i < count; i++)
        if (ce1.elements[i].*weight != ce2.elements[i].*weight)
            return ce1.elements[i].*weight < ce2.elements[i].*weight ? -1 : 1;
    return (ce1.elements.size() > count) - (ce2.elements.size() > count);
}
}

int Utils::naturalOrder(const QString &str1, const QString &str2)
{
    if (str1 == str2)
        return 0;

    CollationElements ce1, ce2;
    computeCollationElements(str1, ce1);
    computeCollationElements(str2, ce2);

    int result = comparePrimaryWeights(ce1, ce2); // Letters, numbers, symbols
    if (result == 0)
        result = compareWeights(ce1, ce2, &CollationElement::secondary); // Accents
    if (result == 0)
        result = compareWeights(ce1, ce2, &CollationElement::tertiary); // Case
    if (result == 0)
        result = compareWeights(ce2, ce1, &CollationElement::leadingZeros); // More leading zeros first ("01" < "1")
    if (result == 0)
        result = str1.compare(str2); // Only identical strings are equal
    return result < 0 ? -1 : 1;
}

QString Utils::removeAccents(QString s)
{
    QString output = "";
    for (int i = 0; i < s.length(); i++)
    {
        QChar c = s[i];
        int dIndex = s_diacriticLetters.indexOf(c);
        if (dIndex < 0) {
            output.append(c);
        } else {
            QString replacement = s_noDiacriticLetters[dIndex];
            output.append(replacement);
        }
    }

    return output;
}

QString Utils::simplifySpaces(QString s)
{
    // Nothing to do in most cases (no copy)
    bool found = false;
    for (int i = 0; i < s.length() && !found; i++)
        found = s[i].isSpace() && (s[i] != QChar(' ') || (i > 0 && s[i - 1].isSpace()));
    if (!found)
        return s;

    QString output = "";
    for (int i = 0; i < s.length(); i++)
        if (!s[i].isSpace() || i == 0 || !s[i - 1].isSpace())
            output.append(s[i].isSpace() ? QChar(' ') : s[i]);

    return output;
}

QString Utils::commonPart(QStringList list)
{
    if (list.empty())
        return "";

    QString commonPart = removeSuffix(list.takeFirst());
    while (!list.empty())
    {
        QString otherStr = removeSuffix(list.takeFirst());

        int sameCharNumber = 0;
        while (sameCharNumber < commonPart.length() && sameCharNumber < otherStr.length() && commonPart[sameCharNumber] == otherStr[sameCharNumber])
            sameCharNumber++;
        commonPart = commonPart.left(sameCharNumber);
    }

    // Remove the extra "-" or spaces at the end and return the result
    commonPart = commonPart.trimmed();
    if (commonPart.endsWith("-"))
        commonPart = commonPart.left(commonPart.length() - 1);
    return commonPart.trimmed();
}

QString Utils::rsaEncrypt(QString input)
{
    // Load the public key from the resources
    QFile file(":/misc/id_rsa.pub");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Error when opening the public key file";
        return "";
    }
    QByteArray publicKeyData = file.readAll();
    file.close();
    QByteArray inputData = input.toLatin1();

    BIO* bo = BIO_new(BIO_s_mem());
    BIO_write(bo, publicKeyData.data(), publicKeyData.length());

    EVP_PKEY* pkey = 0;
    //PEM_read_bio_PrivateKey(bo, &pkey, 0, 0 );
    PEM_read_bio_PUBKEY(bo, &pkey, 0, 0 );
    BIO_free(bo);

    // Create/initialize context
    EVP_PKEY_CTX* ctx;
    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    EVP_PKEY_encrypt_init(ctx);

    // Specify padding: default is PKCS#1 v1.5
    // EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING); // for OAEP with SHA1 for both digests

    // Encryption
    size_t ciphertextLen;
    EVP_PKEY_encrypt(ctx, NULL, &ciphertextLen, (const unsigned char*)inputData.constData(), inputData.length());
    unsigned char* ciphertext = (unsigned char*)OPENSSL_malloc(ciphertextLen);
    EVP_PKEY_encrypt(ctx, ciphertext, &ciphertextLen, (const unsigned char*)inputData.constData(), inputData.length());
    QString result = QByteArray((char*)ciphertext, ciphertextLen).toBase64();

    // Release memory
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    OPENSSL_free(ciphertext);

    return result;
}

QString Utils::removeSuffix(QString txt)
{
    // Detect a possible suffix like "-01" or " 061R" at the end and remove it
    QRegularExpression regExp1("-[0-9]{2}");
    QRegularExpression regExp2(" [0-9]{3}");
    QRegularExpression regExp3(" [0-9]{3}[LR]");
    if (txt.size() >= 3 && txt.lastIndexOf(regExp1) == txt.size() - 3)
        return txt.left(txt.size() - 3);
    if (txt.size() >= 4 && txt.lastIndexOf(regExp2) == txt.size() - 4)
        return txt.left(txt.size() - 4);
    if (txt.size() >= 5 && txt.lastIndexOf(regExp3) == txt.size() - 5)
        return txt.left(txt.size() - 5);
    return txt;
}

qint8 Utils::round8(double value)
{
    // Shared with the synth
    return SynthMath::round8(value);
}

qint16 Utils::round16(double value)
{
    return SynthMath::round16(value);
}

qint32 Utils::round32(double value)
{
    return SynthMath::round32(value);
}

double Utils::roundSignificant(double value, int significantDigits)
{
    if (value == 0 || !std::isfinite(value))
        return 0;
    double magnitude = std::pow(10.0, significantDigits - 1 - static_cast<int>(std::floor(std::log10(std::fabs(value)))));
    return std::round(value * magnitude) / magnitude;
}

qint32 Utils::floatToInt24(float f)
{
    return SampleFileReader::floatToInt24(f);
}

float Utils::int24ToFloat(qint32 i)
{
    float result = (0.5f + static_cast<float>(i)) * s_floatConversionCoef;

    // Limit the output
    if (result > 1.0f)
        return 1.0f;
    else if (result < -1.0f)
        return -1.0f;

    return result;
}

QString Utils::fixFilePath(QString filePath)
{
    filePath = filePath.replace('\\', '/');
    if (filePath.left(7).compare("file://") == 0)
        filePath = filePath.right(filePath.length() - 7);
#ifdef Q_OS_WIN
    if (filePath.size() > 2 && filePath[0] == '/' && filePath[1] != '/')
        filePath = filePath.remove(0, 1);
#endif
    return filePath;
}

QStringList Utils::getDroppedPaths(const QMimeData * mimeData)
{
    // Without a trailing slash (macOS Finder), UNC paths being kept
    QStringList paths;
    foreach (QUrl url, mimeData->urls())
        if (url.isLocalFile())
            paths << QDir::cleanPath(url.toLocalFile());
    return paths;
}

QString Utils::removeForbiddenFilePathCharacters(QString str)
{
    QRegularExpression reg = QRegularExpression(QString::fromUtf8("[`~*|:<>«»?/{}\"\\\\]"));
    return str.replace(reg, "_");
}

bool Utils::isValidUtf8(QByteArray data)
{
    // Same test as for the comments of the soundfonts
    return Sf2Reader::isValidUtf8(data.constData(), static_cast<size_t>(data.size()));
}

QString Utils::getFormattedSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString::number(bytes) + " " + QObject::tr("bytes");
    if (bytes < 1024 * 1024)
        return QLocale::system().toString(static_cast<double>(bytes) / 1024, 'f', 2) + " " + QObject::tr("kB", "kilobytes");
    if (bytes < 1024 * 1024 * 1024)
        return QLocale::system().toString(static_cast<double>(bytes) / 1024 / 1024, 'f', 2) + " " + QObject::tr("MB", "megabytes");
    if (bytes / 1024 < 1024 * 1024 * 1024)
        return QLocale::system().toString(static_cast<double>(bytes) / 1024 / 1024 / 1024, 'f', 2) + " " + QObject::tr("GB", "gigabytes");
    return QLocale::system().toString(static_cast<double>(bytes) / 1024 / 1024 / 1024 / 1024, 'f', 2) + " " + QObject::tr("TB", "terabytes");
}