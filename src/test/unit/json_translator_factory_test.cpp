/*
 * Copyright 2014 Christian Loose <christian.loose@hamburg.de>
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

#include <QTest>

#include <json_translator.h>
#include <json_translator_factory.h>
#include <snippets/json_snippet_translator_factory.h>
#include <snippets/snippet.h>

#include "json_translator_factory_test.h"


void JsonTranslatorFactoryTest::returnsNullIfNoJsonTranslatorExists()
{
    JsonTranslator<int> *translator = JsonTranslatorFactory<int>::create();

    QVERIFY(translator == 0);
}

void JsonTranslatorFactoryTest::returnsValidJsonTranslatorForSnippets()
{
    JsonTranslator<Snippet> *translator = JsonTranslatorFactory<Snippet>::create();

    QVERIFY(translator != 0);
}
