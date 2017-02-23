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

#include "completion_list_model.h"

#include <QFont>
#include <QIcon>


CompletionListModel::CompletionListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int CompletionListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return _snippets.count() + _words.count();
}

QVariant CompletionListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() > rowCount())
        return QVariant();

    if (index.row() < _snippets.count()) {
        const Snippet snippet = _snippets.at(index.row());

        switch (role) {
            case Qt::DecorationRole:
                return QIcon("fa-puzzle-piece.fontawesome");

            case Qt::DisplayRole:
                return QString("%1 %2").arg(snippet.trigger, -15).arg(snippet.description);

            case Qt::EditRole:
                return snippet.trigger;

            case Qt::ToolTipRole:
                return snippet.snippet.toHtmlEscaped();

            case Qt::FontRole:
                {
                    QFont font("Monospace", 8);
                    font.setStyleHint(QFont::TypeWriter);
                    return font;
                }
                break;
        }
    } else {
        switch (role) {
            case Qt::DisplayRole:
            case Qt::EditRole:
                return _words.at(index.row() - _snippets.count());
        }
    }

    return QVariant();
}

void CompletionListModel::setWords(const QStringList &words)
{
    beginInsertRows(QModelIndex(), _snippets.count(), _snippets.count() + words.count());
    _words = words;
    endInsertRows();
}

void CompletionListModel::snippetCollectionChanged(SnippetCollection::CollectionChangedType changedType, const Snippet &snippet)
{
    switch (changedType) {
    case SnippetCollection::ItemAdded:
        {
            QList<Snippet>::iterator it = qLowerBound(_snippets.begin(), _snippets.end(), snippet);
            int row = std::distance(_snippets.begin(), it);
            beginInsertRows(QModelIndex(), row, row);
            _snippets.insert(it, snippet);
            endInsertRows();
        }
        break;

    case SnippetCollection::ItemChanged:
        {
            int row = _snippets.indexOf(snippet);
            _snippets.replace(row, snippet);
        }
        break;

    case SnippetCollection::ItemDeleted:
        {
            int row = _snippets.indexOf(snippet);
            beginRemoveRows(QModelIndex(), row, row);
            _snippets.removeAt(row);
            endRemoveRows();
        }
        break;

    default:
        break;
    }
}
