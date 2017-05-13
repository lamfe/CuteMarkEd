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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QHash>
#include <themes/theme.h>

namespace Ui {
class MainWindow;
}

class QAction;
class QActionGroup;
class QLabel;
class ActiveLabel;
class Dictionary;
class HtmlPreviewController;
class HtmlPreviewGenerator;
class HtmlHighlighter;
class RecentFilesMenu;
class Options;
class SlideLineMapping;
class SnippetCollection;
class ThemeCollection;
class ViewSynchronizer;
class StatusBarWidget;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &fileName = QString(), QWidget *parent = 0);
    ~MainWindow();

    bool loadFile(const QString& fileName);

protected:
    void closeEvent(QCloseEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;

private slots:
    void initializeApp();
    void openRecentFile(const QString &fileName);
    void languageChanged(const Dictionary &dictionary);

    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void fileExportToHtml();
    void fileExportToPdf();
    void filePrint();

    void editUndo();
    void editRedo();
    void editCopyHtml();
    void editGotoLine();
    void editFindReplace();
    void editStrong();
    void editEmphasize();
    void editStrikethrough();
    void editInlineCode();
    void editCenterParagraph();
    void editHardLinebreak();
    void editBlockquote();
    void editIncreaseHeaderLevel();
    void editDecreaseHeaderLevel();
    void editInsertTable();
    void editInsertImage();

    void viewChangeSplit();
    void lastUsedTheme();
    void themeChanged();
    void editorStyleChanged();
    void viewFullScreenMode();
    void viewHorizontalLayout(bool checked);

    void extrasShowSpecialCharacters(bool checked);
    void extrasYamlHeaderSupport(bool checked);
    void extrasWordWrap(bool checked);
    void extensionsAutolink(bool checked);
    void extensionsStrikethrough(bool checked);
    void extensionsAlphabeticLists(bool checked);
    void extensionsDefinitionLists(bool checked);
    void extensionsSmartyPants(bool checked);
    void extensionsFootnotes(bool enabled);
    void extensionsSuperscript(bool enabled);
    void extrasCheckSpelling(bool checked);
    void extrasOptions();

    void helpMarkdownSyntax();
    void helpAbout();

    void setHtmlSource(bool enabled);

    void plainTextChanged();
    void htmlResultReady(const QString &html);
    void tocResultReady(const QString &toc);

    void previewLinkClicked(const QUrl &url);
    void tocLinkClicked(const QUrl &url);

    void splitterMoved(int pos, int index);

    void addJavaScriptObject();
    bool load(const QString &fileName);
    void proxyConfigurationChanged();
    void markdownConverterChanged();

private:
    void setupUi();
    void setupActions();
    void setActionsIcons();
    void setupStatusBar();
    void setupMarkdownEditor();
    void setupHtmlPreview();
    void setupHtmlSourceView();
    void setupCustomShortcuts();
    void setCustomShortcut(QMenu *menu);
    void setCustomShortcut(QAction *action);
    void updateExtensionStatus();
    void syncWebViewToHtmlSource();
    bool maybeSave();
    void setFileName(const QString &fileName);
    void updateSplitter();
    void setupHtmlPreviewThemes();
    void addSeparatorAfterBuiltInThemes();
    void loadCustomStyles();
    void readSettings();
    void writeSettings();
    void applyCurrentTheme();
    QString stylePath(const QString &styleName);

private:
    Ui::MainWindow *_ui;
    RecentFilesMenu *_recent_files_menu;
    Options *_options;
    QActionGroup *_styles_group;
    StatusBarWidget* _status_bar_widget;
    HtmlPreviewGenerator* _generator;
    HtmlHighlighter *_html_highlighter;
    SnippetCollection *_snippet_collection;
    ViewSynchronizer *_view_synchronizer;
    HtmlPreviewController *_html_preview_controller;
    ThemeCollection *_theme_collection;
    Theme _current_theme { "Default", "Default", "Default", "Default" };
    QString _file_name;
    qreal _split_factor;
    bool _right_view_collapsed;
};

#endif // MAINWINDOW_H
