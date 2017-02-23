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
#include "theme.h"

Theme::Theme(const QString &name, 
             const QString &markdownHighlighting,
             const QString &codeHighlighting,
             const QString &previewStylesheet,
             bool builtIn)
    : _name(name), _markdown_highlighting(markdownHighlighting),
    _code_highlighting(codeHighlighting), _preview_stylesheet(previewStylesheet),
    _builtin(builtIn)
{
    checkInvariants();
}

void Theme::checkInvariants() const
{
    if (_name.isEmpty())
        throw std::runtime_error("theme name must not be empty");
    if (_markdown_highlighting.isEmpty())
        throw std::runtime_error("markdown highlighting style must not be empty");
    if (_code_highlighting.isEmpty())
        throw std::runtime_error("code highlighting style must not be empty");
    if (_preview_stylesheet.isEmpty())
        throw std::runtime_error("preview stylesheet must not be empty");
}
