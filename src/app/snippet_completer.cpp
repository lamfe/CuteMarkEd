/*
 * Copyright 2013-2014 Christian Loose <christian.loose@hamburg.de>
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

#include <QApplication>
#include <QAbstractItemView>
#include <QCompleter>
#include <QClipboard>
#include <QScrollBar>

#include <snippets/snippet.h>
#include <snippets/snippet_collection.h>
#include <completion_list_model.h>

#include "snippet_completer.h"

SnippetCompleter::SnippetCompleter(SnippetCollection *collection, QWidget *parentWidget)
    : QObject(parentWidget), _snippet_collection(collection), _completer(new QCompleter(this))
{
    _completer->setWidget(parentWidget);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseSensitive);

    connect(_completer, SIGNAL(activated(QString)),
            this, SLOT(insertSnippet(QString)));

    CompletionListModel *model = new CompletionListModel(_completer);
    connect(collection, SIGNAL(collectionChanged(SnippetCollection::CollectionChangedType,Snippet)),
            model, SLOT(snippetCollectionChanged(SnippetCollection::CollectionChangedType,Snippet)));
    _completer->setModel(model);
}

void SnippetCompleter::performCompletion(const QString &textUnderCursor, const QStringList &words, const QRect &popupRect)
{
    const QString completionPrefix = textUnderCursor;

    // TODO: find more elegant solution
    qobject_cast<CompletionListModel*>(_completer->model())->setWords(words);

    if (completionPrefix != _completer->completionPrefix()) {
        _completer->setCompletionPrefix(completionPrefix);
        _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
    }

    if (_completer->completionCount() == 1) {
        insertSnippet(_completer->currentCompletion());
    } else {
        QRect rect = popupRect;
        rect.setWidth(_completer->popup()->sizeHintForColumn(0) +
                _completer->popup()->verticalScrollBar()->sizeHint().width());
        _completer->complete(rect);
    }
}

bool SnippetCompleter::isPopupVisible() const
{
    return _completer->popup()->isVisible();
}

void SnippetCompleter::hidePopup()
{
    _completer->popup()->hide();
}

void SnippetCompleter::insertSnippet(const QString &trigger)
{
    if (!_snippet_collection || !_snippet_collection->contains(trigger)) {
        // insert word directly
        emit snippetSelected(_completer->completionPrefix(), trigger, trigger.length());
        return;
    }

    const Snippet snippet = _snippet_collection->snippet(trigger);

    QString snippetContent(snippet.snippet);
    replaceClipboardVariable(snippetContent);

    emit snippetSelected(_completer->completionPrefix(), snippetContent, snippet.cursor_position);
}

void SnippetCompleter::replaceClipboardVariable(QString &snippetContent)
{
    if (snippetContent.contains("%clipboard")) {
        QClipboard *clipboard = QApplication::clipboard();
        snippetContent.replace("%clipboard", clipboard->text());
    }
}
