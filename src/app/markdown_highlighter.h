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
#ifndef MARKDOWNHIGHLIGHTER_H
#define MARKDOWNHIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "pmh_definitions.h"
#include "pmh-adapter/definitions.h"
#include "highlight_worker_thread.h"

namespace hunspell {
class SpellChecker;
}

class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    MarkdownHighlighter(QTextDocument *document, hunspell::SpellChecker *spellChecker);
    ~MarkdownHighlighter();
    
    void reset();
    void setStyles(const QVector<PegMarkdownHighlight::HighlightingStyle> &styles);
    void setSpellingCheckEnabled(bool enabled);
    void setYamlHeaderSupportEnabled(bool enabled);

protected:
    void highlightBlock(const QString &textBlock) Q_DECL_OVERRIDE;

private slots:
    void resultReady(pmh_element **elements, unsigned long base_offset);

private:
    void applyFormat(unsigned long pos, unsigned long end, QTextCharFormat format, bool merge);
    void checkSpelling(const QString &textBlock);

    HighlightWorkerThread *_worker_thread;
    QVector<PegMarkdownHighlight::HighlightingStyle> _highlighting_styles;
    QString _previous_text;
    QTextCharFormat _spell_format;
    hunspell::SpellChecker *_spell_checker;
    bool _spelling_check_enabled;
    bool _yaml_header_support_enabled;
};

#endif // MARKDOWNHIGHLIGHTER_H
