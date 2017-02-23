/*
 * Copyright 2014 Andreas Reischuck <https://github.com/arBmind>
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

#include <QPlainTextEdit>
#include <QTextBlock>

#if !WITH_QTWEBENGINE
#   include <QWebFrame>
#endif

#include "reveal_view_synchronizer.h"
#include "html_previewer.h"
#include "slideline_mapping.h"


RevealViewSynchronizer::RevealViewSynchronizer(HtmlPreviewer *webView, QPlainTextEdit *editor)
    : ViewSynchronizer(webView, editor), _current_slide(qMakePair(0, 0)),
      _slide_line_mapping(new SlideLineMapping())
{
#if WITH_QTWEBENGINE
    // TODO
#else
    connect(webView, SIGNAL(loadFinished(bool)),
            this, SLOT(registerEvents()));
    connect(webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(restoreSlidePosition()));
#endif

    connect(editor, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));
    connect(editor, SIGNAL(textChanged()),
            this, SLOT(textChanged()));

    textChanged();
}

RevealViewSynchronizer::~RevealViewSynchronizer()
{
    delete _slide_line_mapping;
}

int RevealViewSynchronizer::horizontalSlide() const
{
    return _current_slide.first;
}

int RevealViewSynchronizer::verticalSlide() const
{
    return _current_slide.second;
}

void RevealViewSynchronizer::slideChanged(int horizontal, int vertical)
{
    if (_current_slide.first == horizontal && _current_slide.second == vertical)
        return;

    _current_slide = qMakePair(horizontal, vertical);

    int lineNumber = _slide_line_mapping->lineForSlide(_current_slide);
    if (lineNumber > 0) {
        gotoLine(lineNumber);
    }
}

void RevealViewSynchronizer::registerEvents()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    _web_view->page()->mainFrame()->evaluateJavaScript(
                    "(function(){"
                    "  var mainWinUpdate = false;"
                    "  function feedbackPosition(event) {"
                    "    if (mainWinUpdate) return;"
                    "    synchronizer.slideChanged(event.indexh, event.indexv);"
                    "  }"
                    "  Reveal.addEventListener('ready', function() {"
                    "    Reveal.addEventListener('slidechanged', feedbackPosition);"
                    "  });"
                    "  function gotoSlide(horizontal, vertical) {"
                    "    mainWinUpdate = true;"
                    "    Reveal.slide(horizontal, vertical);"
                    "    mainWinUpdate = false;"
                    "  }"
                    "  synchronizer.gotoSlideRequested.connect(gotoSlide);"
                    "})();");
#endif
}

void RevealViewSynchronizer::restoreSlidePosition()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    static QString restorePosition =
        "window.location.hash = '/'+synchronizer.horizontalSlide+'/'+synchronizer.verticalSlide;";
    _web_view->page()->mainFrame()->evaluateJavaScript(restorePosition);
#endif
}

void RevealViewSynchronizer::cursorPositionChanged()
{
    int lineNumber = _editor->textCursor().blockNumber() + 1;

    QPair<int, int> slide = _slide_line_mapping->slideForLine(lineNumber);
    if (slide.first >= 0 && slide.second >= 0) {
        gotoSlide(slide);
    }
}

void RevealViewSynchronizer::textChanged()
{
    QString code = _editor->toPlainText();
    _slide_line_mapping->build(code);
}

void RevealViewSynchronizer::gotoLine(int lineNumber)
{
    QTextCursor cursor(_editor->document()->findBlockByNumber(lineNumber-1));
    _editor->setTextCursor(cursor);
}

void RevealViewSynchronizer::gotoSlide(QPair<int, int> slide)
{
    if (_current_slide.first == slide.first && _current_slide.second == slide.second)
        return;

    _current_slide = slide;
    emit gotoSlideRequested(slide.first, slide.second);
}
