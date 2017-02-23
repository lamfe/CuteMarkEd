/*
 * Copyright 2014-2015 Christian Loose <christian.loose@hamburg.de>
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
#include <QNetworkDiskCache>
#include <QStandardPaths>

#include "html_preview_controller.h"
#include "html_previewer.h"

static const qreal ZOOM_CHANGE_VALUE = 0.1;

HtmlPreviewController::HtmlPreviewController(HtmlPreviewer *view, QObject *parent)
    : QObject(parent), _view(view), _zoom_in_action(0), _zoom_out_action(0),
    _zoom_reset_action(0), _disk_cache(new QNetworkDiskCache(this))
{
    createActions();
    registerActionsWithView();

    // use registered actions as custom context menu
    _view->setContextMenuPolicy(Qt::ActionsContextMenu);

    setupNetworkDiskCache();
}

void HtmlPreviewController::createActions()
{
    _zoom_in_action = createAction(tr("Zoom &In"), QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(_zoom_in_action, SIGNAL(triggered()),
            this, SLOT(zoomInView()));

    _zoom_out_action = createAction(tr("Zoom &Out"), QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(_zoom_out_action, SIGNAL(triggered()),
            this, SLOT(zoomOutView()));

    _zoom_reset_action = createAction(tr("Reset &Zoom"), QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(_zoom_reset_action, SIGNAL(triggered()),
            this, SLOT(resetZoomOfView()));
}

QAction *HtmlPreviewController::createAction(const QString &text, const QKeySequence &shortcut)
{
    QAction *action = new QAction(text, this);
    action->setShortcut(shortcut);
    return action;
}

void HtmlPreviewController::registerActionsWithView()
{
#if WITH_QTWEBENGINE
    _view->addAction(_view->pageAction(QWebEnginePage::Copy));
#else
    _view->addAction(_view->pageAction(QWebPage::Copy));
#endif
    //view->addAction(view->pageAction(QWebPage::InspectElement));
    _view->addAction(_zoom_in_action);
    _view->addAction(_zoom_out_action);
    _view->addAction(_zoom_reset_action);
}

void HtmlPreviewController::setupNetworkDiskCache()
{
    // setup disk cache for network access
    QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    _disk_cache->setCacheDirectory(cacheDir);

#if WITH_QTWEBENGINE
    // TODO
#else
    _view->page()->networkAccessManager()->setCache(_disk_cache);
#endif
}

void HtmlPreviewController::zoomInView()
{
    _view->setZoomFactor(_view->zoomFactor() + ZOOM_CHANGE_VALUE);
}

void HtmlPreviewController::zoomOutView()
{
    _view->setZoomFactor(_view->zoomFactor() - ZOOM_CHANGE_VALUE);
}

void HtmlPreviewController::resetZoomOfView()
{
    _view->setZoomFactor(1.0);
}

