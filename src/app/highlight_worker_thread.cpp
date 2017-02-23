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

#include "highlight_worker_thread.h"
#include "pmh_parser.h"

HighlightWorkerThread::HighlightWorkerThread(QObject *parent) :
    QThread(parent)
{
}


void HighlightWorkerThread::enqueue(const QString &text, unsigned long offset)
{
    QMutexLocker locker(&_tasks_mutex);
    _tasks.enqueue(Task {text, offset});
    _buffer_not_empty.wakeOne();
}


void HighlightWorkerThread::run()
{
    forever {
        Task task;

        {
            // wait for new task
            QMutexLocker locker(&_tasks_mutex);
            while (_tasks.count() == 0) {
                _buffer_not_empty.wait(&_tasks_mutex);
            }

            // get last task from queue and skip all previous tasks
            while (!_tasks.isEmpty())
                task = _tasks.dequeue();
        }

        // end processing?
        if (task.text.isNull()) {
            return;
        }

        // delay processing by 500 ms to see if more tasks are coming
        // (e.g. because the user is typing fast)
        this->msleep(500);

        // no more new tasks?
        if (_tasks.isEmpty()) {
            // parse markdown and generate syntax elements
            pmh_element **elements;
            pmh_markdown_to_elements(task.text.toUtf8().data(), pmh_EXT_NONE, &elements);

            emit resultReady(elements, task.offset);
        }
    }
}
