/*
 * Copyright 2015 Aetf <7437103@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QRegularExpression>

#include "yaml_header_checker.h"

YamlHeaderChecker::YamlHeaderChecker(const QString &text)
{
    QRegularExpression rx(R"(^---\s*\n(.*?\n)?(---|\.\.\.)\s*(\n|$))",
                          QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = rx.match(text);
    if (match.hasMatch()) {
        _header = match.captured(0);
        _body = text.mid(_header.length());
    } else {
        _body = text;
    }
}

bool YamlHeaderChecker::hasHeader() const
{
    return !_header.isEmpty();
}

QString YamlHeaderChecker::header() const
{
    return _header;
}

QString YamlHeaderChecker::body() const
{
    return _body;
}

int YamlHeaderChecker::bodyOffset() const
{
    return _header.length();
}
