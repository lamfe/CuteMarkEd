/*
 * Copyright 2013-2015 Christian Loose <christian.loose@hamburg.de>
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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDirIterator>
#include <QFileDialog>
#ifndef Q_OS_OSX
#include <QIcon>
#endif
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QNetworkProxy>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcess>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QTextDocumentWriter>
#include <QTimer>

#if WITH_QTWEBENGINE
#   include <QWebEngineSettings>
// TODO
#else
#   include <QWebFrame>
#   include <QWebPage>
#   include <QWebInspector>
#endif

#ifdef Q_OS_WIN
#include <QWinJumpList>
#include <QWinJumpListCategory>
#endif

#include <jsonfile.h>
#include <snippets/json_snippet_translator_factory.h>
#include <snippets/snippet_collection.h>
#include <spellchecker/dictionary.h>
#include <themes/style_manager.h>
#include <themes/theme_collection.h>
#include <data_location.h>
#include "controls/active_label.h"
#include "controls/find_replace_widget.h"
#include "controls/language_menu.h"
#include "controls/recent_files_menu.h"
#include "about_dialog.h"
#include "html_previewer.h"
#include "html_preview_controller.h"
#include "html_preview_generator.h"
#include "html_view_synchronizer.h"
#include "html_highlighter.h"
#include "image_tool_dialog.h"
#include "markdown_manipulator.h"
#include "export_html_dialog.h"
#include "export_pdf_dialog.h"
#include "options.h"
#include "options_dialog.h"
#include "reveal_view_synchronizer.h"
#include "save_file_adapter.h"
#include "snippet_completer.h"
#include "table_tool_dialog.h"
#include "statusbar_widget.h"

MainWindow::MainWindow(const QString &fileName, QWidget *parent)
    : QMainWindow(parent), _ui(new Ui::MainWindow), _options(new Options(this)),
    _styles_group(new QActionGroup(this)), _status_bar_widget(0),
    _generator(new HtmlPreviewGenerator(_options, this)),
    _snippet_collection(new SnippetCollection(this)),
    _view_synchronizer(0), _html_preview_controller(0),
    _theme_collection(new ThemeCollection()),
    _split_factor(0.5), _right_view_collapsed(false)
{
    _ui->setupUi(this);
    setupUi();

    setFileName(fileName);

    QTimer::singleShot(0, this, SLOT(initializeApp()));
}

MainWindow::~MainWindow()
{
    delete _view_synchronizer;

    // stop background HTML preview generator
    _generator->markdownTextChanged(QString());
    _generator->wait();
    delete _generator;

    delete _ui;
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    // check if file needs saving
    if (maybeSave()) {
        writeSettings();
        e->accept();
    } else {
        e->ignore();
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    updateSplitter();
}

void MainWindow::initializeApp()
{
    // inform us when a link in the table of contents or preview view is clicked
#if WITH_QTWEBENGINE
    // TODO
#else
    _ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    _ui->tocWebView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
#endif

    _theme_collection->load(":/builtin-htmlpreview-themes.json");
    loadCustomStyles();
    setupHtmlPreviewThemes();

    // apply last used theme
    lastUsedTheme();

    _ui->plainTextEdit->tabWidthChanged(_options->tabWidth());
    _ui->plainTextEdit->rulerEnabledChanged(_options->isRulerEnabled());
    _ui->plainTextEdit->rulerPosChanged(_options->rulerPos());

    // init extension flags
    _ui->actionAutolink->setChecked(_options->isAutolinkEnabled());
    _ui->actionStrikethroughOption->setChecked(_options->isStrikethroughEnabled());
    _ui->actionAlphabeticLists->setChecked(_options->isAlphabeticListsEnabled());
    _ui->actionDefinitionLists->setChecked(_options->isDefinitionListsEnabled());
    _ui->actionSmartyPants->setChecked(_options->isSmartyPantsEnabled());
    _ui->actionFootnotes->setChecked(_options->isFootnotesEnabled());
    _ui->actionSuperscript->setChecked(_options->isSuperscriptEnabled());

    // init option flags
    _ui->actionMathSupport->setChecked(_options->isMathSupportEnabled());
    _ui->actionDiagramSupport->setChecked(_options->isDiagramSupportEnabled());
    _ui->actionCodeHighlighting->setChecked(_options->isCodeHighlightingEnabled());
    _ui->actionShowSpecialCharacters->setChecked(_options->isShowSpecialCharactersEnabled());
    _ui->actionWordWrap->setChecked(_options->isWordWrapEnabled());
    _ui->actionCheckSpelling->setChecked(_options->isSpellingCheckEnabled());
    _ui->plainTextEdit->setSpellingCheckEnabled(_options->isSpellingCheckEnabled());
    _ui->actionYamlHeaderSupport->setChecked(_options->isYamlHeaderSupportEnabled());

    // set url to markdown syntax help
    _ui->webView_2->setUrl(tr("qrc:/syntax.html"));

#if WITH_QTWEBENGINE
    // TODO
#else
    // allow loading of remote javascript
    QWebSettings::globalSettings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);

    _ui->webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    QWebInspector *inspector = new QWebInspector();
    inspector->setPage(_ui->webView->page());
#endif

    _ui->menuLanguages->loadDictionaries(_options->dictionaryLanguage());

    //: path to built-in snippets resource.
    JsonFile<Snippet>::load(":/markdown-snippets.json", _snippet_collection);
    QString path = DataLocation::writableLocation();
    JsonFile<Snippet>::load(path + "/user-snippets.json", _snippet_collection);

    // setup file explorer
    connect(_ui->fileExplorerDockContents, SIGNAL(fileSelected(QString)),
            this, SLOT(openRecentFile(QString)));

    // setup jump list on windows
#ifdef Q_OS_WIN
    QWinJumpList jumplist;
    jumplist.recent()->setVisible(true);
#endif

    // load file passed to application on start
    if (!_file_name.isEmpty()) {
        load(_file_name);
    }
}

void MainWindow::openRecentFile(const QString &fileName)
{
    if (maybeSave()) {
        load(fileName);
    }
}

void MainWindow::languageChanged(const Dictionary &dictionary)
{
    _options->setDictionaryLanguage(dictionary.language());
    _ui->plainTextEdit->setSpellingDictionary(dictionary);
}

void MainWindow::fileNew()
{
    if (maybeSave()) {
        _ui->plainTextEdit->clear();
        _ui->plainTextEdit->resetHighlighting();
        _ui->webView->setHtml(QString());
        _ui->htmlSourceTextEdit->clear();
        setFileName(QString());
    }
}

void MainWindow::fileOpen()
{
    if (maybeSave()) {
        QString name = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                                    QString(), tr("Markdown Files (*.markdown *.md *.mdown);;All Files (*)"));
        if (!name.isEmpty()) {
            load(name);
        }
    }
}

bool MainWindow::fileSave()
{
    // file has no name yet?
    if (_file_name.isEmpty()) {
        return fileSaveAs();
    }

    SaveFileAdapter file(_file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextDocumentWriter writer(&file, "plaintext");
    bool success = writer.write(_ui->plainTextEdit->document());
    if (success) {
        file.commit();

        // set status to unmodified
        _ui->plainTextEdit->document()->setModified(false);
        setWindowModified(false);

        // add to recent file list
        _recent_files_menu->addFile(_file_name);
    }

    return success;
}

bool MainWindow::fileSaveAs()
{
    QString name = QFileDialog::getSaveFileName(this, tr("Save as..."), QString(),
                                              tr("Markdown Files (*.markdown *.md *.mdown);;All Files (*)"));
    if (name.isEmpty()) {
        return false;
    }

    // Add default extension ".md" if the file name as no extension yet.
    if (QFileInfo(name).suffix().isEmpty()) {
        name.append(".md");
    }

    setFileName(name);
    return fileSave();
}

void MainWindow::fileExportToHtml()
{
    ExportHtmlDialog dialog(_file_name);
    if (dialog.exec() == QDialog::Accepted) {

        QString cssStyle;
        if (dialog.includeCSS()) {
            // get url of current css stylesheet
#if WITH_QTWEBENGINE
            // TODO
            QUrl cssUrl;
#else
            QUrl cssUrl = _ui->webView->page()->settings()->userStyleSheetUrl();
#endif

            // get resource or file name from url
            QString cssFileName;
            if (cssUrl.scheme() == "qrc") {
                cssFileName = cssUrl.toString().remove(0, 3);
            } else {
                cssFileName = cssUrl.toLocalFile();
            }

            // read currently used css stylesheet file
            QFile f(cssFileName);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                cssStyle = f.readAll();
            }
        }

        QString highlightJs;
        if (dialog.includeCodeHighlighting()) {
            QFile f(":/resources/scripts/highlight.js/highlight.pack.js");
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                highlightJs = f.readAll();
            }
        }

        QString html = _generator->exportHtml(cssStyle, highlightJs);

        // write HTML source to disk
        QFile f(dialog.fileName());
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&f);
            out.setCodec("UTF-8");
            out << html;
        }
    }
}

void MainWindow::fileExportToPdf()
{
    // using temporary QTextDocument instance to get links exported\printed correctly,
    // as links will dissappear when printing directly from QWebView in current Qt implementation
    // of QWebView::print() method (possible bug in Qt?)
    // more info here: http://stackoverflow.com/questions/11629093/add-working-url-into-pdf-using-qt-qprinter

    ExportPdfDialog dialog(_file_name);
    if (dialog.exec() == QDialog::Accepted) {
         QTextDocument doc;
#if WITH_QTWEBENGINE
         // TODO
#else
         doc.setHtml(_ui->webView->page()->currentFrame()->toHtml());
#endif
         doc.print(dialog.printer());
    }
}

void MainWindow::filePrint()
{
    QPrinter printer;
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    dlg->setWindowTitle(tr("Print Document"));

    if (_ui->webView->hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);

#if WITH_QTWEBENGINE
    // TODO
#else
    if (dlg->exec() == QDialog::Accepted)
        _ui->webView->print(&printer);
#endif

    delete dlg;
}

void MainWindow::editUndo()
{
    if (_ui->plainTextEdit->document()->isUndoAvailable()) {
        _ui->plainTextEdit->document()->undo();
    }
}

void MainWindow::editRedo()
{
    if (_ui->plainTextEdit->document()->isRedoAvailable()) {
        _ui->plainTextEdit->document()->redo();
    }
}

void MainWindow::editCopyHtml()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(_ui->htmlSourceTextEdit->toPlainText());
}

void MainWindow::editGotoLine()
{
    const int STEP = 1;
    const int MIN_VALUE = 1;

    QTextCursor cursor = _ui->plainTextEdit->textCursor();
    int currentLine = cursor.blockNumber()+1;
    int maxValue = _ui->plainTextEdit->document()->blockCount();

    bool ok;
    int line = QInputDialog::getInt(this, tr("Go to..."),
                                          tr("Line: ", "Line number in the Markdown editor"), currentLine, MIN_VALUE, maxValue, STEP, &ok);
    if (!ok) return;
    _ui->plainTextEdit->gotoLine(line);
}

void MainWindow::editFindReplace()
{
    _ui->findReplaceWidget->setTextEdit(_ui->plainTextEdit);
    _ui->findReplaceWidget->show();
    _ui->findReplaceWidget->setFocus();
}

void MainWindow::editStrong()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.wrapSelectedText("**");
}

void MainWindow::editEmphasize()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.wrapSelectedText("*");
}

void MainWindow::editStrikethrough()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.wrapSelectedText("~~");
}

void MainWindow::editInlineCode()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.wrapSelectedText("`");
}

void MainWindow::editCenterParagraph()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.wrapCurrentParagraph("->", "<-");
}

void MainWindow::editHardLinebreak()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.appendToLine("  \n");
}

void MainWindow::editBlockquote()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.formatTextAsQuote();
}

void MainWindow::editIncreaseHeaderLevel()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.increaseHeadingLevel();
}

void MainWindow::editDecreaseHeaderLevel()
{
    MarkdownManipulator manipulator(_ui->plainTextEdit);
    manipulator.decreaseHeadingLevel();
}

void MainWindow::editInsertTable()
{
    TableToolDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        MarkdownManipulator manipulator(_ui->plainTextEdit);
        manipulator.insertTable(dialog.rows(), dialog.columns(),
                                dialog.alignments(), dialog.tableCells());
    }
}

void MainWindow::editInsertImage()
{
    ImageToolDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        MarkdownManipulator manipulator(_ui->plainTextEdit);
        manipulator.insertImageLink(dialog.alternateText(), dialog.imageSourceLink(), dialog.optionalTitle());
    }
}

void MainWindow::viewChangeSplit()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action->objectName() == _ui->actionSplit_1_1->objectName()) {
        _split_factor = 0.5;
    } else if (action->objectName() == _ui->actionSplit_2_1->objectName()) {
        _split_factor = 0.666;
    } else if (action->objectName() == _ui->actionSplit_1_2->objectName()) {
        _split_factor = 0.333;
    } else if (action->objectName() == _ui->actionSplit_3_1->objectName()) {
        _split_factor = 0.75;
    } else if (action->objectName() == _ui->actionSplit_1_3->objectName()) {
        _split_factor = 0.25;
    }

    updateSplitter();

    // web view was collapsed and is now visible again, so update it
    if (_right_view_collapsed) {
        syncWebViewToHtmlSource();
    }
}

void MainWindow::lastUsedTheme()
{
    QString themeName = _options->lastUsedTheme();

    _current_theme = _theme_collection->theme(themeName);
    applyCurrentTheme();

    for (auto action : _styles_group->actions()) {
        if (action->text() == themeName) {
            action->setChecked(true);
            _styles_group->triggered(action);
            break;
        }
    }
}

void MainWindow::themeChanged()
{
    QAction *action = qobject_cast<QAction*>(sender());
    QString themeName = action->text();

    _current_theme = _theme_collection->theme(themeName);
    applyCurrentTheme();

    _options->setLastUsedTheme(themeName);
}

void MainWindow::editorStyleChanged()
{
    QString markdownHighlighting = StyleManager::markdownHighlightingPath(_current_theme);
    _ui->plainTextEdit->loadStyleFromStylesheet(stylePath(markdownHighlighting));
}

void MainWindow::applyCurrentTheme()
{
    QString markdownHighlighting = StyleManager::markdownHighlightingPath(_current_theme);
    QString codeHighlighting = StyleManager::codeHighlightingPath(_current_theme);
    QString previewStylesheet = StyleManager::previewStylesheetPath(_current_theme);

    _generator->setCodeHighlightingStyle(codeHighlighting);
    _ui->plainTextEdit->loadStyleFromStylesheet(stylePath(markdownHighlighting));
#if WITH_QTWEBENGINE
    // TODO
#else
    _ui->webView->page()->settings()->setUserStyleSheetUrl(QUrl(previewStylesheet));
#endif
}

void MainWindow::viewFullScreenMode()
{
    if (_ui->actionFullScreenMode->isChecked()) {
        showFullScreen();
    } else {
        showNormal();
    }
}

void MainWindow::viewHorizontalLayout(bool checked)
{
    if (checked) {
        _ui->splitter->setOrientation(Qt::Vertical);
    } else {
        _ui->splitter->setOrientation(Qt::Horizontal);
    }
}

void MainWindow::extrasShowSpecialCharacters(bool checked)
{
    _options->setShowSpecialCharactersEnabled(checked);
    _ui->plainTextEdit->setShowSpecialCharacters(checked);
}

void MainWindow::extrasYamlHeaderSupport(bool checked)
{
    _options->setYamlHeaderSupportEnabled(checked);
    _ui->plainTextEdit->setYamlHeaderSupportEnabled(checked);
    plainTextChanged();
}

void MainWindow::extrasWordWrap(bool checked)
{
    _options->setWordWrapEnabled(checked);
    _ui->plainTextEdit->setLineWrapMode(checked ? MarkdownEditor::WidgetWidth : MarkdownEditor::NoWrap);
}

void MainWindow::extensionsAutolink(bool checked)
{
    _options->setAutolinkEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsStrikethrough(bool checked)
{
    _options->setStrikethroughEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsAlphabeticLists(bool checked)
{
    _options->setAlphabeticListsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsDefinitionLists(bool checked)
{
    _options->setDefinitionListsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsSmartyPants(bool checked)
{
    _options->setSmartyPantsEnabled(checked);
    plainTextChanged();
}

void MainWindow::extensionsFootnotes(bool enabled)
{
    _options->setFootnotesEnabled(enabled);
    plainTextChanged();
}

void MainWindow::extensionsSuperscript(bool enabled)
{
    _options->setSuperscriptEnabled(enabled);
    plainTextChanged();
}

void MainWindow::extrasCheckSpelling(bool checked)
{
    _ui->plainTextEdit->setSpellingCheckEnabled(checked);
    _options->setSpellingCheckEnabled(checked);
}

void MainWindow::extrasOptions()
{
    QList<QAction*> actions;
    // file menu
    actions << _ui->actionNew
            << _ui->actionOpen
            << _ui->actionSave
            << _ui->actionSaveAs
            << _ui->actionExportToHTML
            << _ui->actionExportToPDF
            << _ui->action_Print
            << _ui->actionExit;
    // edit menu
    actions << _ui->actionUndo
            << _ui->actionRedo
            << _ui->actionCut
            << _ui->actionCopy
            << _ui->actionPaste
            << _ui->actionStrong
            << _ui->actionCopyHtmlToClipboard
            << _ui->actionEmphasize
            << _ui->actionStrikethrough
            << _ui->actionInline_Code
            << _ui->actionCenterParagraph
            << _ui->actionBlockquote
            << _ui->actionIncreaseHeaderLevel
            << _ui->actionDecreaseHeaderLevel
            << _ui->actionInsertTable
            << _ui->actionInsertImage
            << _ui->actionFindReplace
            << _ui->actionFindNext
            << _ui->actionFindPrevious
            << _ui->actionGotoLine;
    // view menu
    actions << _ui->dockWidget->toggleViewAction()
            << _ui->fileExplorerDockWidget->toggleViewAction()
            << _ui->actionHtmlSource
            << _ui->actionSplit_1_1
            << _ui->actionSplit_2_1
            << _ui->actionSplit_1_2
            << _ui->actionSplit_3_1
            << _ui->actionSplit_1_3
            << _ui->actionFullScreenMode
            << _ui->actionHorizontalLayout;

    // snippet complete
    actions << _ui->plainTextEdit->actions();

    OptionsDialog dialog(_options, _snippet_collection, actions, this);
    if (dialog.exec() == QDialog::Accepted) {
        _options->writeSettings();

        QString path = DataLocation::writableLocation();
        QSharedPointer<SnippetCollection> userDefinedSnippets = _snippet_collection->userDefinedSnippets();
        JsonFile<Snippet>::save(path + "/user-snippets.json", userDefinedSnippets.data());

        // update shortcuts
        setupCustomShortcuts();
    }
}

void MainWindow::helpMarkdownSyntax()
{
    _ui->dockWidget_2->show();
}

void MainWindow::helpAbout()
{
    AboutDialog dialog;
    dialog.exec();
}

void MainWindow::setHtmlSource(bool enabled)
{
    if (enabled) {
        _ui->stackedWidget->setCurrentWidget(_ui->htmlSourcePage);

        // activate HTML highlighter
        _html_highlighter->setEnabled(true);
        _html_highlighter->rehighlight();
    } else {
        _ui->stackedWidget->setCurrentWidget(_ui->webViewPage);

        // deactivate HTML highlighter
        _html_highlighter->setEnabled(false);

        // update webView now since it was not updated while hidden
        syncWebViewToHtmlSource();
    }

    // sync view menu action
    if (_ui->actionHtmlSource->isChecked() != enabled)
        _ui->actionHtmlSource->setChecked(enabled);

    updateSplitter();
}

void MainWindow::plainTextChanged()
{
    QString code = _ui->plainTextEdit->toPlainText();

    // generate HTML from markdown
    _generator->markdownTextChanged(code);

    // show modification indicator in window title
    setWindowModified(_ui->plainTextEdit->document()->isModified());
}

void MainWindow::htmlResultReady(const QString &html)
{
    // show html preview
    QUrl baseUrl;
    if (_file_name.isEmpty()) {
        baseUrl = QUrl::fromLocalFile(qApp->applicationDirPath());
    } else {
        baseUrl = QUrl::fromLocalFile(QFileInfo(_file_name).absolutePath() + "/");
    }

    QList<int> childSizes = _ui->splitter->sizes();
    if (_ui->webView->isVisible() && childSizes[1] != 0) {
        _ui->webView->setHtml(html, baseUrl);
    }

    // show html source
    _ui->htmlSourceTextEdit->setPlainText(html);
}

void MainWindow::tocResultReady(const QString &toc)
{
    _ui->tocWebView->setHtml(toc);
}

void MainWindow::previewLinkClicked(const QUrl &url)
{
    if (url.isLocalFile()) {
        // directories are not supported
        if (QFileInfo(url.toLocalFile()).isDir()) return;

        QString filePath = url.toLocalFile();
        // Links to markdown files open new instance
        if (filePath.endsWith(".md") || filePath.endsWith(".markdown") || filePath.endsWith(".mdown")) {
            QProcess::startDetached(qApp->applicationFilePath(), QStringList() << filePath);
            return;
        }
    }

    QDesktopServices::openUrl(url);
}

void MainWindow::tocLinkClicked(const QUrl &url)
{
#if WITH_QTWEBENGINE
    // TODO
#else
    QString anchor = url.toString().remove("#");
    _ui->webView->page()->mainFrame()->scrollToAnchor(anchor);
#endif
}

void MainWindow::splitterMoved(int pos, int index)
{
    Q_UNUSED(index)

    int maxViewWidth = _ui->splitter->size().width() - _ui->splitter->handleWidth();
    _split_factor = (float)pos / maxViewWidth;

    // web view was collapsed and is now visible again, so update it
    if (_right_view_collapsed && _ui->splitter->sizes().at(1) > 0) {
        syncWebViewToHtmlSource();
    }
    _right_view_collapsed = (_ui->splitter->sizes().at(1) == 0);
}

void MainWindow::addJavaScriptObject()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    // add view synchronizer object to javascript engine
    _ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("synchronizer", _view_synchronizer);
#endif
}

bool MainWindow::load(const QString &fileName)
{
    if (!QFile::exists(fileName)) {
        return false;
    }

    // open file
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // read content from file
    QByteArray content = file.readAll();
    QString text = QString::fromUtf8(content);

    _ui->plainTextEdit->resetHighlighting();
    _ui->plainTextEdit->setPlainText(text);

    // remember name of new file
    setFileName(fileName);

    // add to recent files
    _recent_files_menu->addFile(fileName);

    return true;
}

void MainWindow::proxyConfigurationChanged()
{
    if (_options->proxyMode() == Options::SystemProxy) {
        qDebug() << "Use system proxy configuration";
        QNetworkProxyFactory::setUseSystemConfiguration(true);
    } else if (_options->proxyMode() == Options::ManualProxy) {
        qDebug() << "Use proxy" << _options->proxyHost();
        QNetworkProxyFactory::setUseSystemConfiguration(false);

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(_options->proxyHost());
        proxy.setPort(_options->proxyPort());
        proxy.setUser(_options->proxyUser());
        proxy.setPassword(_options->proxyPassword());
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        qDebug() << "Don't use a proxy";
        QNetworkProxyFactory::setUseSystemConfiguration(false);

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::NoProxy);
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void MainWindow::markdownConverterChanged()
{
    // regenerate HTML
    plainTextChanged();

    // disable unsupported extensions
    updateExtensionStatus();

    delete _view_synchronizer;
    switch (_options->markdownConverter()) {
#ifdef ENABLE_HOEDOWN
    case Options::HoedownMarkdownConverter:
#endif
    case Options::DiscountMarkdownConverter:
        _view_synchronizer = new HtmlViewSynchronizer(_ui->webView, _ui->plainTextEdit);
        connect(_generator, SIGNAL(htmlResultReady(QString)),
                _view_synchronizer, SLOT(rememberScrollBarPos()));
        break;

    case Options::RevealMarkdownConverter:
        _view_synchronizer = new RevealViewSynchronizer(_ui->webView, _ui->plainTextEdit);
        break;

    default:
        _view_synchronizer = nullptr;
        break;
    }
}

void MainWindow::setupUi()
{
    _html_preview_controller = new HtmlPreviewController(_ui->webView, this);

    setupActions();
    setupMarkdownEditor();
    setupHtmlPreview();
    setupHtmlSourceView();
    setupStatusBar();

    // hide find/replace widget on startup
    _ui->findReplaceWidget->hide();
    connect(_ui->findReplaceWidget, SIGNAL(dialogClosed()),
            _ui->plainTextEdit, SLOT(setFocus()));

    // close table of contents dockwidget
    _ui->dockWidget->close();

    // hide markdown syntax help dockwidget
    _ui->dockWidget_2->hide();
    _ui->dockWidget_2->setFloating(true);
    _ui->dockWidget_2->resize(550, 400);

    // show HTML preview on right panel
    setHtmlSource(_ui->actionHtmlSource->isChecked());

    connect(_options, SIGNAL(proxyConfigurationChanged()),
            this, SLOT(proxyConfigurationChanged()));
    connect(_options, SIGNAL(markdownConverterChanged()),
            this, SLOT(markdownConverterChanged()));
    connect(_options, &Options::editorStyleChanged,
            this, &MainWindow::editorStyleChanged);

    readSettings();
    setupCustomShortcuts();

    _ui->actionFullScreenMode->setChecked(this->isFullScreen());
}

void SetActionShortcut(QAction *action, const QKeySequence &shortcut)
{
    action->setShortcut(shortcut);
    action->setProperty("defaultshortcut", shortcut);
}

void MainWindow::setupActions()
{
    // file menu
    SetActionShortcut(_ui->actionNew, QKeySequence::New);
    SetActionShortcut(_ui->actionOpen, QKeySequence::Open);
    SetActionShortcut(_ui->actionSave, QKeySequence::Save);
    SetActionShortcut(_ui->actionSaveAs, QKeySequence::SaveAs);
    SetActionShortcut(_ui->action_Print, QKeySequence::Print);
    SetActionShortcut(_ui->actionExit, QKeySequence::Quit);

    _recent_files_menu = new RecentFilesMenu(_ui->menuFile);
    _ui->menuFile->insertMenu(_ui->actionSave, _recent_files_menu);

    connect(_recent_files_menu, SIGNAL(recentFileTriggered(QString)),
            this, SLOT(openRecentFile(QString)));

    // edit menu
    SetActionShortcut(_ui->actionUndo, QKeySequence::Undo);
    SetActionShortcut(_ui->actionRedo, QKeySequence::Redo);

    SetActionShortcut(_ui->actionCut, QKeySequence::Cut);
    SetActionShortcut(_ui->actionCopy, QKeySequence::Copy);
    SetActionShortcut(_ui->actionPaste, QKeySequence::Paste);

    SetActionShortcut(_ui->actionStrong, QKeySequence::Bold);
    SetActionShortcut(_ui->actionEmphasize, QKeySequence::Italic);

    SetActionShortcut(_ui->actionFindReplace, QKeySequence::Find);
    SetActionShortcut(_ui->actionFindNext, QKeySequence::FindNext);
    SetActionShortcut(_ui->actionFindPrevious, QKeySequence::FindPrevious);

    connect(_ui->actionFindNext, SIGNAL(triggered()),
            _ui->findReplaceWidget, SLOT(findNextClicked()));

    connect(_ui->actionFindPrevious, SIGNAL(triggered()),
            _ui->findReplaceWidget, SLOT(findPreviousClicked()));

    // view menu
    _ui->menuView->insertAction(_ui->menuView->actions()[0], _ui->dockWidget->toggleViewAction());
    _ui->menuView->insertAction(_ui->menuView->actions()[1], _ui->fileExplorerDockWidget->toggleViewAction());
    SetActionShortcut(_ui->fileExplorerDockWidget->toggleViewAction(), QKeySequence(Qt::ALT + Qt::Key_E));
    SetActionShortcut(_ui->actionFullScreenMode, QKeySequence::FullScreen);

    // extras menu
    connect(_ui->actionMathSupport, SIGNAL(triggered(bool)),
            _generator, SLOT(setMathSupportEnabled(bool)));
    connect(_ui->actionDiagramSupport, SIGNAL(triggered(bool)),
            _generator, SLOT(setDiagramSupportEnabled(bool)));
     connect(_ui->actionCodeHighlighting, SIGNAL(triggered(bool)),
            _generator, SLOT(setCodeHighlightingEnabled(bool)));
    connect(_ui->menuLanguages, SIGNAL(languageTriggered(Dictionary)),
            this, SLOT(languageChanged(Dictionary)));

    // help menu
    _ui->actionMarkdownSyntax->setShortcut(QKeySequence::HelpContents);

    // set actions icons
    setActionsIcons();

    // set names for dock widget actions
    _ui->dockWidget->toggleViewAction()->setObjectName("actionTableOfContents");
    _ui->fileExplorerDockWidget->toggleViewAction()->setObjectName("actionFileExplorer");

    // setup default shortcuts
    _ui->actionGotoLine->setProperty("defaultshortcut", _ui->actionGotoLine->shortcut());
    _ui->actionBlockquote->setProperty("defaultshortcut", _ui->actionBlockquote->shortcut());
    _ui->actionIncreaseHeaderLevel->setProperty("defaultshortcut", _ui->actionIncreaseHeaderLevel->shortcut());
    _ui->actionDecreaseHeaderLevel->setProperty("defaultshortcut", _ui->actionDecreaseHeaderLevel->shortcut());
    _ui->actionInsertTable->setProperty("defaultshortcut", _ui->actionInsertTable->shortcut());
    _ui->actionInsertImage->setProperty("defaultshortcut", _ui->actionInsertImage->shortcut());
    _ui->dockWidget->toggleViewAction()->setProperty("defaultshortcut", _ui->dockWidget->toggleViewAction()->shortcut());
    _ui->fileExplorerDockWidget->toggleViewAction()->setProperty("defaultshortcut", _ui->fileExplorerDockWidget->toggleViewAction()->shortcut());
    _ui->actionHtmlSource->setProperty("defaultshortcut", _ui->actionHtmlSource->shortcut());
}

void MainWindow::setActionsIcons()
{
#ifndef Q_OS_OSX
  // file menu
  _ui->actionSave->setIcon(QIcon("fa-floppy-o.fontawesome"));
  _ui->actionExportToPDF->setIcon(QIcon("fa-file-pdf-o.fontawesome"));
  _ui->action_Print->setIcon(QIcon("fa-print.fontawesome"));

  // edit menu
  _ui->actionUndo->setIcon(QIcon("fa-undo.fontawesome"));
  _ui->actionRedo->setIcon(QIcon("fa-repeat.fontawesome"));

  _ui->actionCut->setIcon(QIcon("fa-scissors.fontawesome"));
  _ui->actionCopy->setIcon(QIcon("fa-files-o.fontawesome"));
  _ui->actionPaste->setIcon(QIcon("fa-clipboard.fontawesome"));

  _ui->actionStrong->setIcon(QIcon("fa-bold.fontawesome"));
  _ui->actionEmphasize->setIcon(QIcon("fa-italic.fontawesome"));
  _ui->actionStrikethrough->setIcon(QIcon("fa-strikethrough.fontawesome"));
  _ui->actionCenterParagraph->setIcon(QIcon("fa-align-center.fontawesome"));
  _ui->actionIncreaseHeaderLevel->setIcon(QIcon("fa-level-up.fontawesome"));
  _ui->actionBlockquote->setIcon(QIcon("fa-quote-left.fontawesome"));
  _ui->actionDecreaseHeaderLevel->setIcon(QIcon("fa-level-down.fontawesome"));

  _ui->actionInsertTable->setIcon(QIcon("fa-table.fontawesome"));
  _ui->actionInsertImage->setIcon(QIcon("fa-picture-o.fontawesome"));

  _ui->actionFindReplace->setIcon(QIcon("fa-search.fontawesome"));

  // view menu
  _ui->actionFullScreenMode->setIcon(QIcon("fa-arrows-alt.fontawesome"));

  _ui->webView->pageAction(QWebPage::Copy)->setIcon(QIcon("fa-copy.fontawesome"));
#endif
}

void MainWindow::setupStatusBar()
{
    _status_bar_widget = new StatusBarWidget(_ui->plainTextEdit);
    _status_bar_widget->setHtmlAction(_ui->actionHtmlSource);

    connect(_options, &Options::lineColumnEnabledChanged,
            _status_bar_widget, &StatusBarWidget::showLineColumn);

    _status_bar_widget->update();

    // remove border around statusbar widgets
    statusBar()->setStyleSheet("QStatusBar::item { border: 0px solid black }; ");
    statusBar()->addPermanentWidget(_status_bar_widget, 1);
}

void MainWindow::setupMarkdownEditor()
{
    _ui->plainTextEdit->setSnippetCompleter(new SnippetCompleter(_snippet_collection, _ui->plainTextEdit));

    // load file that are dropped on the editor
    connect(_ui->plainTextEdit, SIGNAL(loadDroppedFile(QString)),
            this, SLOT(load(QString)));

    connect(_options, &Options::editorFontChanged,
            _ui->plainTextEdit, &MarkdownEditor::editorFontChanged);
    connect(_options, &Options::tabWidthChanged,
            _ui->plainTextEdit, &MarkdownEditor::tabWidthChanged);
    connect(_options, &Options::rulerEnabledChanged,
            _ui->plainTextEdit, &MarkdownEditor::rulerEnabledChanged);
    connect(_options, &Options::rulerPosChanged,
            _ui->plainTextEdit, &MarkdownEditor::rulerPosChanged);
}

void MainWindow::setupHtmlPreview()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    // add our objects everytime JavaScript environment is cleared
    connect(_ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(addJavaScriptObject()));
#endif

    // start background HTML preview generator
    connect(_generator, SIGNAL(htmlResultReady(QString)),
            this, SLOT(htmlResultReady(QString)));
    connect(_generator, SIGNAL(tocResultReady(QString)),
            this, SLOT(tocResultReady(QString)));
    _generator->start();
}

void MainWindow::setupHtmlSourceView()
{
    QFont font("Monospace", 10);
    font.setStyleHint(QFont::TypeWriter);
    _ui->htmlSourceTextEdit->setFont(font);
    _html_highlighter = new HtmlHighlighter(_ui->htmlSourceTextEdit->document());
}

void MainWindow::setupCustomShortcuts()
{
    setCustomShortcut(_ui->menuFile);
    setCustomShortcut(_ui->menuEdit);
    setCustomShortcut(_ui->menuView);

    foreach (QAction *action, _ui->plainTextEdit->actions()) {
        setCustomShortcut(action);
    }
}

void MainWindow::setCustomShortcut(QMenu *menu)
{
    foreach (QAction *action, menu->actions()) {
        if (action->menu()) {
            // recurse into submenu
            setCustomShortcut(action->menu());
        } else {
            setCustomShortcut(action);
        }
    }
}

void MainWindow::setCustomShortcut(QAction *action)
{
    if (_options->hasCustomShortcut(action->objectName())) {
        action->setShortcut(_options->customShortcut(action->objectName()));
    }
}

void MainWindow::updateExtensionStatus()
{
    _ui->actionAutolink->setEnabled(_generator->isSupported(MarkdownConverter::AutolinkOption));
    _ui->actionAlphabeticLists->setEnabled(_generator->isSupported(MarkdownConverter::NoAlphaListOption));
    _ui->actionDefinitionLists->setEnabled(_generator->isSupported(MarkdownConverter::NoDefinitionListOption));
    _ui->actionFootnotes->setEnabled(_generator->isSupported(MarkdownConverter::ExtraFootnoteOption));
    _ui->actionSmartyPants->setEnabled(_generator->isSupported(MarkdownConverter::NoSmartypantsOption));
    _ui->actionStrikethroughOption->setEnabled(_generator->isSupported(MarkdownConverter::NoStrikethroughOption));
    _ui->actionSuperscript->setEnabled(_generator->isSupported(MarkdownConverter::NoSuperscriptOption));
}

void MainWindow::syncWebViewToHtmlSource()
{
    htmlResultReady(_ui->htmlSourceTextEdit->toPlainText());
}

bool MainWindow::maybeSave()
{
    if (!_ui->plainTextEdit->document()->isModified())
        return true;

    if (_file_name.startsWith(QLatin1String(":/")))
        return true;

    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Save Changes"),
                               tr("The document has been modified.<br>"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return fileSave();
    else if (ret == QMessageBox::Cancel)
        return false;

    return true;
}

void MainWindow::setFileName(const QString &fileName)
{
    _file_name = fileName;

    // set to unmodified
    _ui->plainTextEdit->document()->setModified(false);
    setWindowModified(false);

    // update window title
    QString shownName = fileName;
    if (shownName.isEmpty()) {
        //: default file name for new markdown documents
        shownName = tr("untitled.md");
    }
    setWindowFilePath(shownName);
}

void MainWindow::updateSplitter()
{
    // not fully initialized yet?
    if (centralWidget()->size() != _ui->splitter->size()) {
        return;
    }

    // calculate new width of left and right pane
    QList<int> childSizes = _ui->splitter->sizes();
    childSizes[0] = _ui->splitter->width() * _split_factor;
    childSizes[1] = _ui->splitter->width() * (1 - _split_factor);

    _ui->splitter->setSizes(childSizes);
}

void MainWindow::setupHtmlPreviewThemes()
{
    _ui->menuStyles->clear();

    delete _styles_group;
    _styles_group = new QActionGroup(this);

    int key = 1;
    bool separatorAdded = false;
    foreach(const QString &themeName, _theme_collection->themeNames()) {
        if (!separatorAdded && !_theme_collection->theme(themeName).isBuiltIn()) {
            addSeparatorAfterBuiltInThemes();
            separatorAdded = true;
        }

        QAction *action = _ui->menuStyles->addAction(themeName);
        action->setShortcut(QKeySequence(tr("Ctrl+%1").arg(key++)));
        action->setCheckable(true);
        action->setActionGroup(_styles_group);
        connect(action, &QAction::triggered,
                this, &MainWindow::themeChanged);
    }

    if (_status_bar_widget)
        _status_bar_widget->setStyleActions(_styles_group);
}

void MainWindow::addSeparatorAfterBuiltInThemes()
{
    _ui->menuStyles->addSeparator();

    QAction *separator = new QAction(_styles_group);
    separator->setSeparator(true);
    _styles_group->addAction(separator);
}

void MainWindow::loadCustomStyles()
{
    QStringList paths = DataLocation::standardLocations();
    qDebug() << paths;
    QDir dataPath(paths.first() + QDir::separator() + "styles");
    dataPath.setFilter(QDir::Files);
    if (dataPath.exists()) {
        // iterate over all files in the styles subdirectory
        QDirIterator it(dataPath);
        while (it.hasNext()) {
            it.next();

            QString fileName = it.fileName();
            QString styleName = QFileInfo(fileName).baseName();
            QString stylePath = QUrl::fromLocalFile(it.filePath()).toString();

            Theme customTheme { styleName, "Default", "Default", styleName };
            _theme_collection->insert(customTheme);

            StyleManager styleManager;
            styleManager.insertCustomPreviewStylesheet(styleName, stylePath);
        }
    }
}

void MainWindow::readSettings()
{
    // restore window size, position and state
    QSettings settings;
    restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
    restoreState(settings.value("mainWindow/windowState").toByteArray());

    // restore recent files menu
    _recent_files_menu->readState();

    _options->readSettings();
}

void MainWindow::writeSettings()
{
    _options->writeSettings();

    // save recent files menu
    _recent_files_menu->saveState();

    // save window size, position and state
    QSettings settings;
    settings.setValue("mainWindow/geometry", saveGeometry());
    settings.setValue("mainWindow/windowState", saveState());
}

QString MainWindow::stylePath(const QString &styleName)
{
    QString suffix = _options->isSourceAtSingleSizeEnabled() ? "" : "+";
    return QString(":/theme/%1%2.txt").arg(styleName).arg(suffix);
}
