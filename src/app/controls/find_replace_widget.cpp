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

#include <QMenu>
#include <QPlainTextEdit>

#include "find_replace_widget.h"
#include "ui_find_replace_widget.h"

FindReplaceWidget::FindReplaceWidget(QWidget *parent)
    : QWidget(parent), _ui(new Ui::FindReplaceWidget), _text_editor(0),
    _find_case_sensitively(false), _find_whole_words_only(false),
    _find_use_regexp(false)
{
    _ui->setupUi(this);

    setupFindOptionsMenu();
    setFocusProxy(_ui->findLineEdit);
}

FindReplaceWidget::~FindReplaceWidget()
{
    delete _ui;
}

void FindReplaceWidget::setTextEdit(QPlainTextEdit *editor)
{
    _text_editor = editor;
}

void FindReplaceWidget::showEvent(QShowEvent *)
{
    _ui->findLineEdit->selectAll();
}

void FindReplaceWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        event->accept();
        close();
        emit dialogClosed();
    }
}

void FindReplaceWidget::findPreviousClicked()
{
    if (!_text_editor)
        return;

    find(_ui->findLineEdit->text(), QTextDocument::FindBackward);
}

void FindReplaceWidget::findNextClicked()
{
    if (!_text_editor)
        return;

    find(_ui->findLineEdit->text());
}

void FindReplaceWidget::replaceClicked()
{
    QString oldText = _ui->findLineEdit->text();
    QString newText = _ui->replaceLineEdit->text();

    QTextCursor cursor = _text_editor->textCursor();
    cursor.beginEditBlock();

    if (cursor.hasSelection()) {
        cursor.insertText(newText);
    }

    find(oldText);

    cursor.endEditBlock();
}

void FindReplaceWidget::replaceAllClicked()
{
    QString oldText = _ui->findLineEdit->text();
    QString newText = _ui->replaceLineEdit->text();

    _text_editor->moveCursor(QTextCursor::Start);
    QTextCursor cursor = _text_editor->textCursor();
    cursor.beginEditBlock();

    bool found = find(oldText);
    while (found) {
        QTextCursor tc = _text_editor->textCursor();
        if (tc.hasSelection()) {
            tc.insertText(newText);
        }
        found = find(oldText);
    }

    cursor.endEditBlock();
}

void FindReplaceWidget::caseSensitiveToggled(bool enabled)
{
    _find_case_sensitively = enabled;
}

void FindReplaceWidget::wholeWordsOnlyToggled(bool enabled)
{
    _find_whole_words_only = enabled;
}

void FindReplaceWidget::useRegularExpressionsToggled(bool enabled)
{
    _find_use_regexp = enabled;
}

void FindReplaceWidget::showOptionsMenu()
{
    QMenu *findOptionsMenu = qobject_cast<QAction*>(sender())->menu();

    // show menu above the line edit
    QPoint pos = _ui->findLineEdit->mapToGlobal(QPoint(0, 0));
    pos.ry() -= findOptionsMenu->sizeHint().height();
    findOptionsMenu->exec(pos);
}

void FindReplaceWidget::setupFindOptionsMenu()
{
    QMenu *findOptionsMenu = new QMenu(this);

    QAction *action = findOptionsMenu->addAction(tr("Case Sensitive"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), SLOT(caseSensitiveToggled(bool)));

    action = findOptionsMenu->addAction(tr("Whole Words Only"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), SLOT(wholeWordsOnlyToggled(bool)));

    action = findOptionsMenu->addAction(tr("Use Regular Expressions"));
    action->setCheckable(true);
    connect(action, SIGNAL(toggled(bool)), SLOT(useRegularExpressionsToggled(bool)));

    // add action to line edit to show the options menu
    action = new QAction(tr("Find Options"), this);
    action->setIcon(QIcon("fa-search.fontawesome"));
    action->setMenu(findOptionsMenu);
    connect(action, SIGNAL(triggered(bool)), SLOT(showOptionsMenu()));
    _ui->findLineEdit->addAction(action, QLineEdit::LeadingPosition);
}

bool FindReplaceWidget::find(const QString &searchString, QTextDocument::FindFlags findOptions) const
{
    if (_find_case_sensitively)
        findOptions |= QTextDocument::FindCaseSensitively;

    if (_find_whole_words_only)
        findOptions |= QTextDocument::FindWholeWords;

    if (_find_use_regexp) {
        return findUsingRegExp(searchString, findOptions);
    } else {
        return _text_editor->find(searchString, findOptions);
    }
}

bool FindReplaceWidget::findUsingRegExp(const QString &pattern, QTextDocument::FindFlags findOptions) const
{
    QRegExp rx(pattern, _find_case_sensitively ? Qt::CaseSensitive : Qt::CaseInsensitive);

    QTextCursor search = _text_editor->document()->find(rx, _text_editor->textCursor(), findOptions);
    if (search.isNull())
        return false;

    _text_editor->setTextCursor(search);
    return true;
}
