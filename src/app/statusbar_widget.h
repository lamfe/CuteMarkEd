#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QWidget>

class QLabel;
class QActionGroup;

class MarkdownEditor;
class ActiveLabel;

class StatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBarWidget(MarkdownEditor* editor);
    ~StatusBarWidget();

public slots:
    void update();
    void showLineColumn(bool enabled);

    void setHtmlAction(QAction *action);
    void setStyleActions(QActionGroup *actionGroup);

private slots:
    void cursorPositionChanged();
    void textChanged();

    void styleContextMenu(const QPoint &pos);
    void updateStyleLabel();

private:
    MarkdownEditor* _editor;

    QLabel *_line_col_label;
    QLabel *_word_count_label;

    QLabel *_style_label;

    QActionGroup *_style_actions;

    ActiveLabel *_html_label;
};

#endif // STATUSBARWIDGET_H
