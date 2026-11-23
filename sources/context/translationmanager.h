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

#ifndef TRANSLATIONSYSTEM_H
#define TRANSLATIONSYSTEM_H

#include <QString>
#include <QMap>
#include <QAtomicInt>
class QApplication;
class QTranslator;
class ConfManager;

class TranslationManager
{
public:
    enum TranslationType
    {
        TRANSLATION_STEREO_EDITING_HELP
    };

    /// While an object of this class exists, the texts translated with tr() are the source texts (English), in all
    /// threads: for the messages read by an AI whatever the language of the interface. To be used only around the code
    /// producing these messages, an interface created meanwhile would stay in English.
    class SourceLanguage
    {
    public:
        SourceLanguage() { s_count.ref(); }
        ~SourceLanguage() { s_count.deref(); }
        static bool isActive() { return s_count.loadRelaxed() > 0; }

    private:
        Q_DISABLE_COPY(SourceLanguage)
        static QAtomicInt s_count;
    };

    TranslationManager(ConfManager *configuration);
    ~TranslationManager();

    /// Translate the application according to the default locale or the overwritten local in QSettings
    void translate();

    /// Get all languages, key is two letters (en, fr, ...), value is the language native name
    QMap<QString, QString> getLanguages() { return _languages; }

    /// Get a translation common to several elements
    QString getTranslation(TranslationType translationType);

private:
    void addTranslations(QString path);
    static QString getNativeLanguageName(QString locale);
    static const QString DEFAULT_LANGUAGE;
    static const QString RESOURCE_PATH;
    static const QString TRANSLATION_DIRECTORY;

    ConfManager * _configuration;
    QTranslator * _translator;
    QMap<QString, QString> _languages;
};

#endif // TRANSLATIONSYSTEM_H
