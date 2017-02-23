/*
 * Copyright 2015 Christian Loose <christian.loose@hamburg.de>
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

#include "theme_collection.h"

#include <jsonfile.h>
#include <themes/json_theme_translator_factory.h>

bool ThemeCollection::load(const QString &fileName)
{
    return JsonFile<Theme>::load(fileName, this);
}

int ThemeCollection::insert(const Theme &theme)
{
    _themes_index << theme.name();
    _themes << theme;
    return 0;
}

int ThemeCollection::count() const
{
    return _themes.count();
}

const Theme &ThemeCollection::at(int offset) const
{
    return _themes.at(offset);
}

bool ThemeCollection::contains(const QString &name) const
{
    return _themes_index.contains(name);
}

const Theme ThemeCollection::theme(const QString &name) const
{
    return at(_themes_index.indexOf(name));
}

QStringList ThemeCollection::themeNames() const
{
    return _themes_index;
}

const QString ThemeCollection::name() const
{
    return QStringLiteral("themes");
}
