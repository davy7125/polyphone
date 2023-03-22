/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2020 Davy Triponney                                **
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
**  Website/Contact: https://www.polyphone-soundfonts.com                 **
**             Date: 19.09.2016                                           **
***************************************************************************/

#include "utils.h"
#include <QStringList>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>

#ifdef USE_WOLFSSL

#include <wolfssl/options.h>
#include <wolfssl/openssl/rsa.h>
#include <wolfssl/openssl/pem.h>
#include <wolfssl/openssl/engine.h>

/* Debian #962149, fixed in libwolfssl-dev (>= 4.4.0+dfsg-4~) */
#ifndef BIO_FLAGS_BASE64_NO_NL
#define BIO_FLAGS_BASE64_NO_NL WOLFSSL_BIO_FLAG_BASE64_NO_NL
#endif

#else
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/err.h"
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

int Utils::naturalOrder(QString str1, QString str2)
{
    while (!str1.isEmpty() && !str2.isEmpty())
    {
        // Find the number part
        int numberLength1 = 0, numberLength2 = 0;
        int number1 = getNumberPart(str1, numberLength1);
        int number2 = getNumberPart(str2, numberLength2);

        if (numberLength1 > 0 && numberLength2 > 0)
        {
            // Two numbers to compare
            if (number1 == number2)
            {
                // We skip the numbers and continue the reading
                str1 = str1.right(str1.length() - numberLength1);
                str2 = str2.right(str2.length() - numberLength2);
            }
            else
                return number1 - number2;
        }
        else if (numberLength1 == 0 && numberLength2 == 0)
        {
            // Two letters to compare
            if (str1[0] == str2[0])
            {
                // We skip the first letter and continue the reading
                str1 = str1.right(str1.length() - 1);
                str2 = str2.right(str2.length() - 1);
            }
            else
                return str1[0].toLatin1() - str2[0].toLatin1();
        }
        else
        {
            // One number for one string, one letter for the other
            return str1[0].toLatin1() - str2[0].toLatin1();
        }
    }

    if (!str1.isEmpty())
        return 1;
    if (!str2.isEmpty())
        return -1;

    return 0;
}

int Utils::getNumberPart(const QString &str, int &length)
{
    // Find all characters that are numbers
    length = 0;
    while (length < str.length() && str[length].isNumber())
        length++;
    if (length > 0)
        return str.left(length).toInt();
    return 0;
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

    // Create the RSA public key
    BIO* bio = BIO_new_mem_buf((void*)publicKeyData.data(), publicKeyData.size());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // NO NL
    RSA* publicKey = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (publicKey == nullptr)
    {
        qWarning() << "Error when reading the public key:" << ERR_error_string(ERR_get_error(), nullptr);
        return "";
    }

    // Encrypt data
    int rsaLen = RSA_size(publicKey);
    unsigned char* encryptedText = new unsigned char[rsaLen];
    int resultLen = RSA_public_encrypt(input.size(), (const unsigned char*)input.toLatin1().data(), encryptedText, publicKey, RSA_PKCS1_PADDING);
    RSA_free(publicKey);
    if (resultLen == -1)
    {
        qWarning() << "Error when encrypting with RSA:" << ERR_error_string(ERR_get_error(), nullptr);
        delete [] encryptedText;
        return "";
    }

    // Convert the encryptedText to QString (base 64) and return it
    QString result = QByteArray((char*)encryptedText, resultLen).toBase64();
    delete [] encryptedText;
    return result;
}

QString Utils::rsaDecrypt(QString input)
{
    // Load the public key from the resources
    QFile file(":/misc/id_rsa.pub");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Error when opening the public key file";
        return "";
    }
    QByteArray publicKeyData = file.readAll();
    file.close();

    // Create the RSA public key
    BIO *bio = BIO_new_mem_buf((void*)publicKeyData.data(), publicKeyData.size());
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // NO NL
    RSA* publicKey = PEM_read_bio_RSA_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio) ;
    if (publicKey == nullptr)
    {
        qWarning() << "Error when reading the public key:" << ERR_error_string(ERR_get_error(), nullptr);
        return "";
    }

    // Decrypt data
    int rsaLen = RSA_size(publicKey); // That's how many bytes the decrypted data would be
    unsigned char * decryptedText = new unsigned char[rsaLen];
    int resultLen = RSA_public_decrypt(RSA_size(publicKey), (unsigned char *)QByteArray::fromBase64(input.toUtf8()).data(), decryptedText, publicKey, RSA_PKCS1_PADDING);
    if (resultLen == -1)
    {
        qWarning() << "Error when decrypting with RSA:" << ERR_error_string(ERR_get_error(), nullptr);
        delete [] decryptedText;
        return "";
    }

    // Convert the decryptedText to QString (base 64) and return it
    QString result = QString::fromLatin1((char*)decryptedText, resultLen);
    delete [] decryptedText;
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
    return static_cast<qint8>(value > 0 ? (value + 0.5) : (value - 0.5));
}

qint16 Utils::round16(double value)
{
    return static_cast<qint16>(value > 0 ? (value + 0.5) : (value - 0.5));
}

qint32 Utils::round32(double value)
{
    return static_cast<qint32>(value > 0 ? (value + 0.5) : (value - 0.5));
}

qint32 Utils::floatToInt24(float f)
{
    // Limit the input
    if (f > 1.0f)
        f = 1.0f;
    else if (f < -1.0f)
        f = -1.0f;

    f = f * 8388607.5f /*(.5f + 0x7FFFFF)*/ - .5f;
    qint32 result = static_cast<qint32>(f + (f > 0 ? 0.5f : -0.5f));

    // Limit the output
    if (result > 8388607)
        return 8388607;
    else if (result < -8388608)
        return -8388608;

    return result;
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
