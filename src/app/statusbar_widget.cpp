
#include <QHBoxLayout>
#include <QTextBlock>
#include <QLocale>
#include <QLabel>
#include <QActionGroup>
#include <QMenu>

#include "statusbar_widget.h"
#include "controls/active_label.h"
#include "markdown_editor.h"

StatusBarWidget::StatusBarWidget(MarkdownEditor* editor)
    : _editor(editor), _line_col_label(0), _word_count_label(0),
    _style_label(0), _style_actions(0), _html_label(0)

{
    // main layout
    QHBoxLayout *topLayout = new QHBoxLayout(this);
    topLayout->setContentsMargins(4,0,4,0);
    topLayout->setSpacing(4);

    // line&column label
    _line_col_label = new QLabel;
    topLayout->addWidget(_line_col_label, 0);
    _line_col_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // word count label
    _word_count_label = new QLabel;
    _word_count_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    topLayout->addWidget(_word_count_label, 0);

    // add spacer
    topLayout->addItem(new QSpacerItem(0,10, QSizePolicy::Expanding, QSizePolicy::Expanding));

    // styles label
    _style_label = new QLabel;
    _style_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    topLayout->addWidget(_style_label, 0);

    _style_label->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(_style_label, &QLabel::customContextMenuRequested, this, &StatusBarWidget::styleContextMenu);

    // separator label
    topLayout->addWidget(new QLabel("|"), 0);

    // html preview label
    _html_label = new ActiveLabel;
    _html_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    topLayout->addWidget(_html_label, 0);

    connect(_editor, &MarkdownEditor::cursorPositionChanged, this, &StatusBarWidget::cursorPositionChanged);
    connect(_editor, &MarkdownEditor::textChanged, this, &StatusBarWidget::textChanged);
}

StatusBarWidget::~StatusBarWidget()
{
}

void StatusBarWidget::update()
{
    cursorPositionChanged();
    textChanged();
}

void StatusBarWidget::showLineColumn(bool enabled)
{
    if (enabled)
        _line_col_label->show();
    else
        _line_col_label->hide();
}

void StatusBarWidget::setHtmlAction(QAction *action)
{   
    _html_label->setAction(action);
}

void StatusBarWidget::setStyleActions(QActionGroup *actionGroup)
{
    // if was previously defined, disconnect
    if (_style_actions) {
        disconnect(_style_actions, &QActionGroup::triggered, this, &StatusBarWidget::updateStyleLabel);
    }

    // set new actions group
    _style_actions = actionGroup;
    updateStyleLabel();

    connect(_style_actions, &QActionGroup::triggered, this, &StatusBarWidget::updateStyleLabel);
}

void StatusBarWidget::cursorPositionChanged()
{
    QTextCursor cursor = _editor->textCursor();

    int line = cursor.blockNumber();
    int column = cursor.positionInBlock();

    _line_col_label->setText(tr("Line %1, Column %2 %3")
            .arg(QLocale().toString(line + 1))
            .arg(QLocale().toString(column + 1))
            .arg("|")
            );
}

void StatusBarWidget::textChanged()
{
    // update statistics
    if (_word_count_label) {
        int words = _editor->countWords();
        int lines = _editor->document()->lineCount();
        int chars = _editor->document()->characterCount();

        _word_count_label->setText(tr("%1 words").arg(words));
        _word_count_label->setToolTip(tr("Lines: %1  Words: %2  Characters: %3").arg(lines).arg(words).arg(chars));
    }
}

void StatusBarWidget::styleContextMenu(const QPoint &pos)
{
    if (_style_actions) {
        QMenu menu;
        menu.addActions(_style_actions->actions());
        menu.exec(_style_label->mapToGlobal(pos));
    }
}

void StatusBarWidget::updateStyleLabel()
{
    QAction* action = _style_actions->checkedAction();
    if (action)
        _style_label->setText(tr("Style: %1").arg(action->text()));
}
