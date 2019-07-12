/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2019 Davy Triponney                                **
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
#include "openssl/rsa.h"
#include "openssl/pem.h"
#include "openssl/engine.h"
#include "soundfontmanager.h"

QString Utils::s_diacriticLetters;
QStringList Utils::s_noDiacriticLetters;
double Utils::s_concaveTable[128];
double Utils::s_convexTable[128];

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
    if (s_diacriticLetters.isEmpty())
    {
        s_diacriticLetters = QString::fromUtf8("ŠŒŽšœžŸ¥µÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝßàáâãäåæçèéêëìíîïðñòóôõöøùúûüýÿ");
        s_noDiacriticLetters << "S"<<"OE"<<"Z"<<"s"<<"oe"<<"z"<<"Y"<<"Y"<<"u"<<"A"<<"A"<<"A"<<"A"<<"A"<<"A"<<"AE"<<"C"<<"E"
                             <<"E"<<"E"<<"E"<<"I"<<"I"<<"I"<<"I"<<"D"<<"N"<<"O"<<"O"<<"O"<<"O"<<"O"<<"O"<<"U"<<"U"<<"U"<<"U"
                            <<"Y"<<"s"<<"a"<<"a"<<"a"<<"a"<<"a"<<"a"<<"ae"<<"c"<<"e"<<"e"<<"e"<<"e"<<"i"<<"i"<<"i"<<"i"<<"o"
                           <<"n"<<"o"<<"o"<<"o"<<"o"<<"o"<<"o"<<"u"<<"u"<<"u"<<"u"<<"y"<<"y";
    }

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
    QString result = QString::fromLatin1((char*) decryptedText, resultLen);
    delete [] decryptedText;
    return result;
}

int Utils::sortDivisions(EltID id1, EltID id2, int sortType)
{
    int result = 0;
    SoundfontManager * sm = SoundfontManager::getInstance();

    switch (sortType)
    {
    case 1:
        // Sort by velocity, by key, alphabetically
        if ((result = compareVelocity(sm, id1, id2)) == 0)
            if ((result = compareKey(sm, id1, id2)) == 0)
                result = compareName(sm, id1, id2);
        break;
    case 2:
        // Sort alphabetically, by key, by velocity
        if ((result = compareName(sm, id1, id2)) == 0)
            if ((result = compareKey(sm, id1, id2)) == 0)
                result = compareVelocity(sm, id1, id2);
        break;
    case 3:
        // Sort by id
        if (id1.indexElt2 < id2.indexElt2)
            result = -1;
        else if (id1.indexElt2 > id2.indexElt2)
            result = 1;
        break;
    default:
        // Sort by key, by velocity, alphabetically
        if ((result = compareKey(sm, id1, id2)) == 0)
            if ((result = compareVelocity(sm, id1, id2)) == 0)
                result = compareName(sm, id1, id2);
        break;
    }

    return result;
}

int Utils::compareKey(SoundfontManager * sm, EltID idDiv1, EltID idDiv2)
{
    int lKey1 = sm->get(idDiv1, champ_keyRange).rValue.byLo;
    int lKey2 = sm->get(idDiv2, champ_keyRange).rValue.byLo;
    if (lKey1 != lKey2)
        return (lKey1 < lKey2) ? -1 : 1;
    return 0;
}

int Utils::compareVelocity(SoundfontManager *sm, EltID idDiv1, EltID idDiv2)
{
    int lVel1 = sm->get(idDiv1, champ_velRange).rValue.byLo;
    int lVel2 = sm->get(idDiv2, champ_velRange).rValue.byLo;
    if (lVel1 != lVel2)
        return (lVel1 < lVel2) ? -1 : 1;
    return 0;
}

int Utils::compareName(SoundfontManager *sm, EltID idDiv1, EltID idDiv2)
{
    AttributeType champId = (idDiv1.typeElement == elementInstSmpl ? champ_sampleID : champ_instrument);
    ElementType divisionType = (idDiv1.typeElement == elementInstSmpl ? elementSmpl : elementInst);
    QString name1 = sm->getQstr(EltID(divisionType, idDiv1.indexSf2, sm->get(idDiv1, champId).wValue), champ_nameSort);
    QString name2 = sm->getQstr(EltID(divisionType, idDiv2.indexSf2, sm->get(idDiv2, champId).wValue), champ_nameSort);
    if (name1 != name2)
        return naturalOrder(name1, name2);
    return 0;
}

void Utils::prepareConversionTables()
{
    // Limits of the tables
    s_concaveTable[0] = 0.0;
    s_concaveTable[127] = 1.0;
    s_convexTable[0] = 0;
    s_convexTable[127] = 1.0;

    // Compute values of the tables
    double x;
    for (int i = 1; i < 127; i++)
    {
        x = -20.0 / 96.0 * log((i * i) / (127.0 * 127.0)) / log(10.0);
        s_convexTable[i] = 1.0 - x;
        s_concaveTable[127 - i] = x;
    }
}

double Utils::concave(double val)
{
  if (val < 0)
    return 0;
  if (val > 127)
    return 1;
  return s_concaveTable[static_cast<int>(val)];
}

double Utils::convex(double val)
{
  if (val < 0)
    return 0;
  if (val > 127)
    return 1;
  return s_convexTable[static_cast<int>(val)];
}
