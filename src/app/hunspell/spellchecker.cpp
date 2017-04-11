/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "spellchecker.h"
using hunspell::SpellChecker;

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QTextCodec>

#include <hunspell/hunspell.hxx>

#include <spellchecker/dictionary.h>
#include <data_location.h>

SpellChecker::SpellChecker()
    : _hunspell_checker(0), _text_codec(0)
{
}

SpellChecker::~SpellChecker()
{
    delete _hunspell_checker;
}

bool SpellChecker::isCorrect(const QString &word)
{
    if (!_text_codec || !_hunspell_checker) {
        return true;
    }

    QByteArray ba = _text_codec->fromUnicode(word);
    return _hunspell_checker->spell(ba.toStdString()) != 0;
}

QStringList SpellChecker::suggestions(const QString &word)
{
    QStringList suggestions;

    if (!_text_codec || !_hunspell_checker) {
        return suggestions;
    }

    QByteArray ba = _text_codec->fromUnicode(word);
    std::vector<std::string> suggestedWords = _hunspell_checker->suggest(ba.toStdString());

    for (std::size_t i = 0; i < suggestedWords.size(); ++i) {
        suggestions << _text_codec->toUnicode(suggestedWords[i].data());
    }

    return suggestions;
}

void SpellChecker::addToUserWordlist(const QString &word)
{
    _hunspell_checker->add(_text_codec->fromUnicode(word).constData());
    if(!_user_wordlist.isEmpty()) {
        QFile userWordlistFile(_user_wordlist);
        if(!userWordlistFile.open(QIODevice::Append))
            return;

        QTextStream stream(&userWordlistFile);
        stream << word << "\n";
        userWordlistFile.close();
    }
}

void SpellChecker::loadDictionary(const QString &dictFilePath)
{
    delete _hunspell_checker;

    qDebug() << "Load dictionary from path" << dictFilePath;

    QString affixFilePath(dictFilePath);
    affixFilePath.replace(".dic", ".aff");

    _hunspell_checker = new Hunspell(affixFilePath.toLocal8Bit(), dictFilePath.toLocal8Bit());

    _text_codec = QTextCodec::codecForName(_hunspell_checker->get_dic_encoding());
    if (!_text_codec) {
        _text_codec = QTextCodec::codecForName("UTF-8");
    }

    // also load user word list
    QString path = DataLocation::writableLocation();
    loadUserWordlist(path + "/user.dic");
}

void SpellChecker::loadUserWordlist(const QString &userWordlistPath)
{
    _user_wordlist = userWordlistPath;

    QFile userWordlistFile(userWordlistPath);
    if (!userWordlistFile.open(QIODevice::ReadOnly))
        return;

    QTextStream stream(&userWordlistFile);
    for (QString word = stream.readLine(); !word.isEmpty(); word = stream.readLine()) {
        _hunspell_checker->add(_text_codec->fromUnicode(word).constData());
    }
}
