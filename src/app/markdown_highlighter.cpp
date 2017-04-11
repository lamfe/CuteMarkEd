/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 * Copyright 2015 Aetf <7437103@gmail.com>
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

#include <QDebug>
#include <QFile>
#include <QTextDocument>
#include <QTextLayout>


#ifdef __cplusplus
extern "C" {
#endif
#   include <pmh_parser.h>
#ifdef __cplusplus
}
#endif

#include "markdown_highlighter.h"
#include "yaml_header_checker.h"

#include "pmh-adapter/definitions.h"
using PegMarkdownHighlight::HighlightingStyle;

#include "hunspell/spellchecker.h"
using hunspell::SpellChecker;

#include <QDebug>

MarkdownHighlighter::MarkdownHighlighter(QTextDocument *document, hunspell::SpellChecker *spellChecker)
    : QSyntaxHighlighter(document), _worker_thread(new HighlightWorkerThread(this)),
    _spelling_check_enabled(false), _yaml_header_support_enabled(false)
{
    _spell_checker = spellChecker;

    // QTextCharFormat::SpellCheckUnderline has issues with Qt 5.
    _spell_format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    _spell_format.setUnderlineColor(Qt::red);

    connect(_worker_thread, SIGNAL(resultReady(pmh_element**, unsigned long)),
            this, SLOT(resultReady(pmh_element**, unsigned long)));
    _worker_thread->start();
}

MarkdownHighlighter::~MarkdownHighlighter()
{
    // stop background worker thread
    _worker_thread->enqueue(QString());
    _worker_thread->wait();
    delete _worker_thread;
}

void MarkdownHighlighter::reset()
{
    _previous_text.clear();
}

void MarkdownHighlighter::setStyles(const QVector<PegMarkdownHighlight::HighlightingStyle> &styles)
{
    _highlighting_styles = styles;
    reset();
}

void MarkdownHighlighter::setSpellingCheckEnabled(bool enabled)
{
    _spelling_check_enabled = enabled;
}

void MarkdownHighlighter::setYamlHeaderSupportEnabled(bool enabled)
{
    _yaml_header_support_enabled = enabled;
}

void MarkdownHighlighter::highlightBlock(const QString &textBlock)
{
    if (document()->isEmpty()) {
        return;
    }

    // check spelling of passed text block
    if (_spelling_check_enabled) {
        checkSpelling(textBlock);
    }

    QString text = document()->toPlainText();

    // document changed since last call?
    if (text == _previous_text) {
        return;
    }

    // cut YAML headers
    QString actualText;
    unsigned long offset = 0;
    if (_yaml_header_support_enabled) {
        YamlHeaderChecker checker(text);
        actualText = checker.body();
        offset = checker.bodyOffset();
    } else {
        actualText = text;
    }

    _worker_thread->enqueue(actualText, offset);

    _previous_text = text;
}

void MarkdownHighlighter::applyFormat(unsigned long pos, unsigned long end,
                                      QTextCharFormat format, bool merge)
{
    // The QTextDocument contains an additional single paragraph separator (unicode 0x2029).
    // https://bugreports.qt-project.org/browse/QTBUG-4841
    unsigned long max_offset = document()->characterCount() - 1;

    if (end <= pos || max_offset < pos) {
        return;
    }

    if (max_offset < end) {
        end = max_offset;
    }

    // "The QTextLayout object can only be modified from the
    // documentChanged implementation of a QAbstractTextDocumentLayout
    // subclass. Any changes applied from the outside cause undefined
    // behavior." -- we are breaking this rule here. There might be
    // a better (more correct) way to do this.

    int startBlockNum = document()->findBlock(pos).blockNumber();
    int endBlockNum = document()->findBlock(end).blockNumber();
    for (int j = startBlockNum; j <= endBlockNum; j++)
    {
        QTextBlock block = document()->findBlockByNumber(j);

        QTextLayout *layout = block.layout();
        int blockpos = block.position();
        QTextLayout::FormatRange r;
        r.format = format;
        QList<QTextLayout::FormatRange> list;
        if (merge) {
            list = layout->additionalFormats();
        }

        if (j == startBlockNum) {
            r.start = pos - blockpos;
            r.length = (startBlockNum == endBlockNum)
                        ? end - pos
                        : block.length() - r.start;
        } else if (j == endBlockNum) {
            r.start = 0;
            r.length = end - blockpos;
        } else {
            r.start = 0;
            r.length = block.length();
        }

        list.append(r);
        layout->setAdditionalFormats(list);
    }
}

void MarkdownHighlighter::checkSpelling(const QString &textBlock)
{
    QStringList wordList = textBlock.split(QRegExp("\\W+"), QString::SkipEmptyParts);
    int index = 0;
    foreach (QString word, wordList) {
        index = textBlock.indexOf(word, index);

        if (!_spell_checker->isCorrect(word)) {
            setFormat(index, word.length(), _spell_format);
        }
        index += word.length();
    }
}

void MarkdownHighlighter::resultReady(pmh_element **elements, unsigned long base_offset)
{
    if (!elements) {
        qDebug() << "elements is null";
        return;
    }

    // clear any format before base_offset
    if (base_offset > 0) {
        applyFormat(0, base_offset - 1, QTextCharFormat(), false);
    }

    // apply highlight results
    for (int i = 0; i < _highlighting_styles.size(); i++) {
        HighlightingStyle style = _highlighting_styles.at(i);
        pmh_element *elem_cursor = elements[style.type];
        while (elem_cursor != NULL) {
            unsigned long pos = elem_cursor->pos + base_offset;
            unsigned long end = elem_cursor->end + base_offset;

            QTextCharFormat format = style.format;
            if (/*_makeLinksClickable
                &&*/ (elem_cursor->type == pmh_LINK
                    || elem_cursor->type == pmh_AUTO_LINK_URL
                    || elem_cursor->type == pmh_AUTO_LINK_EMAIL
                    || elem_cursor->type == pmh_REFERENCE)
                && elem_cursor->address != NULL)
            {
                QString address(elem_cursor->address);
                if (elem_cursor->type == pmh_AUTO_LINK_EMAIL && !address.startsWith("mailto:"))
                    address = "mailto:" + address;
                format.setAnchor(true);
                format.setAnchorHref(address);
                format.setToolTip(address);
            }
            applyFormat(pos, end, format, true);

            elem_cursor = elem_cursor->next;
        }
    }

    // mark complete document as dirty
    document()->markContentsDirty(0, document()->characterCount());

    // free highlighting elements
    pmh_free_elements(elements);
}
