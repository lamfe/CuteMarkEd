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

#include "html_highlighter.h"

HtmlHighlighter::HtmlHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document),
    enabled(false)
{
    _keyword_format.setForeground(QColor("#6c71c4"));
    _keyword_format.setFontWeight(QFont::Bold);

    _image_format.setForeground(QColor("#cf009a"));

    _link_format.setForeground(QColor("#4e279a"));

    HighlightingRule rule;

    QString htmlTagRegExp = "<(/?)(%1)[^>]*(/?)>";
    QStringList keywords;
    keywords << htmlTagRegExp.arg("html")
             << "<head>" << "</head>"
             << htmlTagRegExp.arg("link")
             << htmlTagRegExp.arg("script")
             << "<body>" << "</body>"
             << "<title>" << "</title>"
             << "<b>" << "</b>"
             << htmlTagRegExp.arg("p")
             << "<i>" << "</i>"
             << "<u>" << "</u>"
             << "<sup>" << "</sup>"
             << "<sub>" << "</sub>"
             << htmlTagRegExp.arg("h1")
             << htmlTagRegExp.arg("h2")
             << htmlTagRegExp.arg("h3")
             << htmlTagRegExp.arg("h4")
             << htmlTagRegExp.arg("h5")
             << htmlTagRegExp.arg("h6")
             << htmlTagRegExp.arg("br")
             << htmlTagRegExp.arg("hr")
             << "<small>" << "</small>"
             << "<big>" << "</big>"
             << "<strong>" << "</strong>"
             << "<em>" << "</em>"
             << "<center>" << "</center>"
             << "<nobr>" << "</nobr>"
             << "<blockquote>" << "</blockquote>"
             << "<pre>" << "</pre>"
             << "<code>" << "</code>"
             << "<li>" << "</li>"
             << "<ul>" << "</ul>"
             << "<ol>" << "</ol>"
             << "<dl>" << "</dl>"
             << "<table>" << "</table>"
             << "<thead>" << "</thead>"
             << "<tbody>" << "</tbody>"
             << htmlTagRegExp.arg("th")
             << htmlTagRegExp.arg("td")
             << htmlTagRegExp.arg("tr")
             << "<strike>" << "</strike>"
             << "<del>" << "</del>";

    foreach(QString keyword, keywords) {
        rule.pattern = QRegExp(keyword);
        rule.format = &_keyword_format;
        _highlighting_rules.append(rule);
    }

    rule.pattern = QRegExp(htmlTagRegExp.arg("img"));
    rule.format = &_image_format;
    _highlighting_rules.append(rule);

    rule.pattern = QRegExp(htmlTagRegExp.arg("a"));
    rule.format = &_link_format;
    _highlighting_rules.append(rule);
}

bool HtmlHighlighter::isEnabled() const
{
    return enabled;
}

void HtmlHighlighter::setEnabled(bool enabled)
{
    this->enabled = enabled;
}

void HtmlHighlighter::highlightBlock(const QString &text)
{
    if (enabled) {
        foreach(HighlightingRule rule, _highlighting_rules) {
            QRegExp expression(rule.pattern);
            int index = text.indexOf(expression);
            while (index >= 0) {
                int length = expression.matchedLength();
                setFormat(index, length, *(rule.format));
                index = text.indexOf(expression, index + length);
            }
        }
        setCurrentBlockState(0);
    }
}
