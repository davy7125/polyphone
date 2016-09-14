/***************************************************************************
**                                                                        **
**  Polyphone, a soundfont editor                                         **
**  Copyright (C) 2013-2015 Davy Triponney                                **
**                2014      Andrea Celani                                 **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
**           Author: Davy Triponney                                       **
**  Website/Contact: http://www.polyphone.fr/                             **
**             Date: 01.01.2013                                           **
***************************************************************************/

#include "translationmanager.h"
#include <QLocale>
#include <QApplication>
#include <QSettings>
#include <QTranslator>
#include <QDirIterator>

TranslationManager * TranslationManager::_instance = NULL;
const QString TranslationManager::DEFAULT_LANGUAGE = "en"; // English is the default language;
const QString TranslationManager::RESOURCE_PATH = ":/traductions/ressources/traductions";

TranslationManager::TranslationManager() :
    _translator(new QTranslator())
{
    // List of available languages (in code + in translation files)
    this->addTranslation(QLocale("fr").nativeLanguageName(), "fr");
    QDirIterator it(RESOURCE_PATH, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        // Language
        QString language = it.next().section('_', 1, 1).section(".", 0, 0);
        QString languageName = QLocale(language).nativeLanguageName();
        if (languageName != "")
            this->addTranslation(languageName, language);
    }
}

TranslationManager::~TranslationManager()
{
    delete _translator;
    _instance = NULL;
}

TranslationManager * TranslationManager::getInstance()
{
    if (!_instance)
        _instance = new TranslationManager();
    return _instance;
}

void TranslationManager::addTranslation(QString languageName, QString locale)
{
    _languages[locale] = languageName;
}

void TranslationManager::translate(QApplication * a)
{
    // Language used
    QSettings settings;
    QString language = settings.value("language", QLocale::system().name().section('_', 0, 0)).toString();

    // If not french, the application is translated
    if (language != "fr")
    {
        if (!getInstance()->_languages.keys().contains(language))
            language = DEFAULT_LANGUAGE;
        getInstance()->_translator->load(RESOURCE_PATH + "/polyphone_" + language + ".qm");
        a->installTranslator(getInstance()->_translator);
    }
}

QMap<QString, QString> TranslationManager::getLanguages()
{
    return getInstance()->_languages;
}
