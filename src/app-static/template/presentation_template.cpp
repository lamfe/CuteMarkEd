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

#include <QFile>

#include "presentation_template.h"

PresentationTemplate::PresentationTemplate()
{
    QFile f(":/template_presentation.html");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        _presentation_template = f.readAll();
    }
}

QString PresentationTemplate::render(const QString &body, RenderOptions options) const
{
    if (_presentation_template.isEmpty()) {
        return body;
    }

    return QString(_presentation_template)
            .replace(QLatin1String("<!--__HTML_HEADER__-->"), QString())
            .replace(QLatin1String("<!--__HTML_CONTENT__-->"), body)
            .replace(QLatin1String("<!--__REVEAL_PLUGINS__-->"), buildRevealPlugins(options));
}

QString PresentationTemplate::exportAsHtml(const QString &, const QString &body, RenderOptions options) const
{
    return render(body, options);
}

QString PresentationTemplate::buildRevealPlugins(RenderOptions options) const
{
    QString plugins;

    // add MathJax.js script as reveal plugin
    if (options.testFlag(Template::MathSupport)) {
        plugins += "{ src: 'https://cdn.jsdelivr.net/reveal.js/2.6.2/plugin/math/math.js', async: true },\n";
    }

    // add Highlight.js script as reveal plugin
    if (options.testFlag(Template::CodeHighlighting)) {
        plugins += "{ src: 'https://cdn.jsdelivr.net/reveal.js/2.6.2/plugin/highlight/highlight.js', async: true, callback: function() { hljs.initHighlightingOnLoad(); } },\n";
    }

    return plugins;
}
