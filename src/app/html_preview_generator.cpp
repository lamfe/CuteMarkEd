/*
 * Copyright 2013-2015 Christian Loose <christian.loose@hamburg.de>
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

#include <converter/markdown_converter.h>
#include <converter/markdown_document.h>
#include <converter/discount_markdown_converter.h>
#include <converter/reveal_markdown_converter.h>

#ifdef ENABLE_HOEDOWN
#include <converter/hoedown_markdown_converter.h>
#endif

#include <template/template.h>

#include "html_preview_generator.h"
#include "options.h"
#include "yaml_header_checker.h"

HtmlPreviewGenerator::HtmlPreviewGenerator(Options *opt, QObject *parent)
    : QThread(parent), _options(opt), _document(0), _converter(0)
{
    connect(_options, SIGNAL(markdownConverterChanged()), SLOT(markdownConverterChanged()));
    markdownConverterChanged();
}

bool HtmlPreviewGenerator::isSupported(MarkdownConverter::ConverterOption option) const
{
    return _converter->supportedOptions().testFlag(option);
}

void HtmlPreviewGenerator::markdownTextChanged(const QString &text)
{
    // cut YAML header
    YamlHeaderChecker checker(text);
    QString actualText = checker.hasHeader() && _options->isYamlHeaderSupportEnabled() ?
                            checker.body() : text;
    // enqueue task to parse the markdown text and generate a new HTML document
    QMutexLocker locker(&_tasks_mutex);
    _tasks.enqueue(actualText);
    _buffer_not_empty.wakeOne();
}

QString HtmlPreviewGenerator::exportHtml(const QString &styleSheet, const QString &highlightingScript)
{
    if (!_document)
        return QString();

    QString header;
    if (!styleSheet.isEmpty()) {
        header += QString("\n<style>%1</style>").arg(styleSheet);
    }

    if (!highlightingScript.isEmpty()) {
        // FIXME: doesn't really belong here
        QString highlightStyle;
        QFile f(QString(":/resources/scripts/highlight.js/styles/%1.css").arg(
                    _converter->templateRenderer()->codeHighlightingStyle()));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            highlightStyle = f.readAll();
        }

        header += QString("\n<style>%1</style>").arg(highlightStyle);
        header += QString("\n<script>%1</script>").arg(highlightingScript);
        header += "\n<script>hljs.initHighlightingOnLoad();</script>";
    }

    return _converter->templateRenderer()->exportAsHtml(header,
        _converter->renderAsHtml(_document), renderOptions());
}

void HtmlPreviewGenerator::setMathSupportEnabled(bool enabled)
{
    _options->setMathSupportEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setDiagramSupportEnabled(bool enabled)
{
    _options->setDiagramSupportEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setCodeHighlightingEnabled(bool enabled)
{
    _options->setCodeHighlightingEnabled(enabled);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::setCodeHighlightingStyle(const QString &style)
{
    _converter->templateRenderer()->setCodeHighlightingStyle(style);

    // regenerate a HTML document
    generateHtmlFromMarkdown();
}

void HtmlPreviewGenerator::markdownConverterChanged()
{
    QString style;

    if (_converter) {
        style = _converter->templateRenderer()->codeHighlightingStyle();
        delete _converter;
    }

    switch (_options->markdownConverter()) {
#ifdef ENABLE_HOEDOWN
    case Options::HoedownMarkdownConverter:
        _converter = new HoedownMarkdownConverter();
        _converter->templateRenderer()->setCodeHighlightingStyle(style);
        break;
#endif

    case Options::RevealMarkdownConverter:
        _converter = new RevealMarkdownConverter();
        _converter->templateRenderer()->setCodeHighlightingStyle(style);
        break;

    case Options::DiscountMarkdownConverter:
    default:
        _converter = new DiscountMarkdownConverter();
        _converter->templateRenderer()->setCodeHighlightingStyle(style);
        break;
    }
}

void HtmlPreviewGenerator::run()
{
    forever {
        QString text;

        {
            // wait for new task
            QMutexLocker locker(&_tasks_mutex);
            while (_tasks.count() == 0) {
                _buffer_not_empty.wait(&_tasks_mutex);
            }

            // get last task from queue and skip all previous tasks
            while (!_tasks.isEmpty())
                text = _tasks.dequeue();
        }

        // end processing?
        if (text.isNull()) {
            return;
        }

        // delay processing to see if more tasks are coming
        // (e.g. because the user is typing fast)
        this->msleep(calculateDelay(text));

        // no more new tasks?
        if (_tasks.isEmpty()) {
            // delete previous markdown document
            delete _document;

            // generate HTML from markdown
            _document = _converter->createDocument(text, converterOptions());
            generateHtmlFromMarkdown();

            // generate table of contents
            generateTableOfContents();
        }
    }
}

void HtmlPreviewGenerator::generateHtmlFromMarkdown()
{
    if (!_document)
        return;

    QString html = _converter->templateRenderer()->render(
                _converter->renderAsHtml(_document), renderOptions());
    emit htmlResultReady(html);
}

void HtmlPreviewGenerator::generateTableOfContents()
{
    if (!_document)
        return;

    QString toc = _converter->renderAsTableOfContents(_document);
    QString styledToc = QString("<html><head>\n<style type=\"text/css\">ul { list-style-type: none; padding: 0; margin-left: 1em; } a { text-decoration: none; }</style>\n</head><body>%1</body></html>").arg(toc);
    emit tocResultReady(styledToc);
}

MarkdownConverter::ConverterOptions HtmlPreviewGenerator::converterOptions() const
{
    MarkdownConverter::ConverterOptions parserOptionFlags(MarkdownConverter::TableOfContentsOption | MarkdownConverter::NoStyleOption);

    // autolink
    if (_options->isAutolinkEnabled()) {
        parserOptionFlags |= MarkdownConverter::AutolinkOption;
    }

    // strikethrough
    if (!_options->isStrikethroughEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoStrikethroughOption;
    }

    // alphabetic lists
    if (!_options->isAlphabeticListsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoAlphaListOption;
    }

    // definition lists
    if (!_options->isDefinitionListsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoDefinitionListOption;
    }

    // SmartyPants
    if (!_options->isSmartyPantsEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoSmartypantsOption;
    }

    // Footnotes
    if (_options->isFootnotesEnabled()) {
        parserOptionFlags |= MarkdownConverter::ExtraFootnoteOption;
    }

    // Superscript
    if (!_options->isSuperscriptEnabled()) {
        parserOptionFlags |= MarkdownConverter::NoSuperscriptOption;
    }

    return parserOptionFlags;
}

Template::RenderOptions HtmlPreviewGenerator::renderOptions() const
{
    Template::RenderOptions renderOptionFlags;

    // math support
    if (_options->isMathSupportEnabled()) {
        renderOptionFlags |= Template::MathSupport;
    }

    // inline math support
    if (_options->isMathInlineSupportEnabled()) {
        renderOptionFlags |= Template::MathInlineSupport;
    }

    // diagram support
    if (_options->isDiagramSupportEnabled()) {
        renderOptionFlags |= Template::DiagramSupport;
    }

    // code highlighting
    if (_options->isCodeHighlightingEnabled()) {
        renderOptionFlags |= Template::CodeHighlighting;
    }

    return renderOptionFlags;
}

int HtmlPreviewGenerator::calculateDelay(const QString &text)
{
    const int MINIMUM_DELAY = 50;
    const int MAXIMUM_DELAY = 2000;

    // calculate the processing delay based on amount of characters in the
    // markdown text. The delay is restricted to the interval [50, 2000] milliseconds;
    int delay = qMin(qMax(text.size() / 100, MINIMUM_DELAY), MAXIMUM_DELAY);

    return delay;
}
