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

#include <QPlainTextEdit>
#include <QScrollBar>

#if !WITH_QTWEBENGINE
#   include <QWebFrame>
#endif

#include "html_view_synchronizer.h"
#include "html_previewer.h"


HtmlViewSynchronizer::HtmlViewSynchronizer(HtmlPreviewer *webView, QPlainTextEdit *editor)
    : ViewSynchronizer(webView, editor)
{
    // synchronize scrollbars
    connect(editor->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(scrollValueChanged(int)));

    // restore scrollbar position after content size changed
#if WITH_QTWEBENGINE
    // TODO
#else
    connect(webView->page()->mainFrame(), SIGNAL(contentsSizeChanged(QSize)),
            this, SLOT(htmlContentSizeChanged()));
#endif
}

HtmlViewSynchronizer::~HtmlViewSynchronizer()
{}

void HtmlViewSynchronizer::webViewScrolled()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    double factor = (double)_editor->verticalScrollBar()->maximum() /
                    _web_view->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    int value = _web_view->page()->mainFrame()->scrollBarValue(Qt::Vertical);

    _editor->verticalScrollBar()->setValue(qRound(value * factor));
#endif

    // remember new vertical scrollbar position of markdown editor
    rememberScrollBarPos();
}

void HtmlViewSynchronizer::scrollValueChanged(int value)
{
#if WITH_QTWEBENGINE
    // TODO
#else
    int webMax = _web_view->page()->mainFrame()->scrollBarMaximum(Qt::Vertical);
    int textMax = _editor->verticalScrollBar()->maximum();
    double factor = (double)webMax / textMax;

    _web_view->page()->mainFrame()->setScrollBarValue(Qt::Vertical, qRound(value * factor));
#endif
}

void HtmlViewSynchronizer::htmlContentSizeChanged()
{
    if (_scrollbar_pos > 0)
    {
        // restore previous scrollbar position
        scrollValueChanged(_scrollbar_pos);
    }
}

void HtmlViewSynchronizer::rememberScrollBarPos()
{
    _scrollbar_pos = _editor->verticalScrollBar()->value();
}
