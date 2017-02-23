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
#ifndef THEME_H
#define THEME_H

#include <QString>
#include <stdexcept>


class Theme
{
public:
    Theme(const QString &name, 
          const QString &markdownHighlighting,
          const QString &codeHighlighting,
          const QString &previewStylesheet,
          bool builtIn = false);

    QString name() const
    {
        return _name;
    }

    QString markdownHighlighting() const
    {
        return _markdown_highlighting;
    }

    QString codeHighlighting() const
    {
        return _code_highlighting;
    }

    QString previewStylesheet() const
    {
        return _preview_stylesheet;
    }

    bool isBuiltIn() const
    {
        return _builtin;
    }

    bool operator<(const Theme &rhs) const
    {
        return _name < rhs.name();
    }

    bool operator ==(const Theme &rhs) const
    {
        return _name == rhs.name();
    }

private:
    void checkInvariants() const;

private:
    QString _name;
    QString _markdown_highlighting;
    QString _code_highlighting;
    QString _preview_stylesheet;
    bool _builtin;
};

#endif // THEME_H
