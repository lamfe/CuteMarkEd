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

#include <QAction>
#include <QMouseEvent>

#include "active_label.h"

ActiveLabel::ActiveLabel(QWidget *parent)
    : QLabel(parent)
{}

ActiveLabel::ActiveLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{}

void ActiveLabel::mouseDoubleClickEvent(QMouseEvent *e)
{
    // double click with left mouse button?
    if (e->button() == Qt::LeftButton) {
        emit doubleClicked();
    }

    QLabel::mouseDoubleClickEvent(e);
}

void ActiveLabel::setAction(QAction *action)
{
    // if was previously defined, disconnect
    if (_action) {
        disconnect(_action, &QAction::changed, this, &ActiveLabel::updateFromAction);
        disconnect(this, &ActiveLabel::doubleClicked, _action, &QAction::trigger);
    }

    // set new action
    _action = action;

    // update label using action's data
    updateFromAction();

    // action action and label to have them synced
    // whenever one of them is triggered
    connect(_action, &QAction::changed, this, &ActiveLabel::updateFromAction);
    connect(this, &ActiveLabel::doubleClicked, _action, &QAction::trigger);
}

void ActiveLabel::updateFromAction()
{
    setStatusTip(_action->statusTip());
    setToolTip(_action->toolTip());
    setEnabled(_action->isEnabled());

    // update text based on QAction data
    QString actionText = _action->text();
    actionText.remove("&");

    if (_action->isCheckable()) {
        if (_action->isChecked())
            setText(QString("%1: %2").arg(actionText).arg(tr("on")));
        else
            setText(QString("%1: %2").arg(actionText).arg(tr("off")));
    } else {
        setText(actionText);
    }
}
