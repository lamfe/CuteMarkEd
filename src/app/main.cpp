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

#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QLibraryInfo>
#include <QTranslator>
#include <QFileOpenEvent>

#ifdef Q_OS_WIN

#   include <QDir>
#   include <QFileInfo>
#   include <QSettings>

// Helper function to register supported file types
// This is needed to enable the application jump list to show the desired recent files
static void associate_file_types(const QStringList &fileTypes)
{
    QString displayName = QGuiApplication::applicationDisplayName();
    QString filePath = QCoreApplication::applicationFilePath();
    QString fileName = QFileInfo(filePath).fileName();

    QSettings settings("HKEY_CURRENT_USER\\Software\\Classes\\Applications\\" + fileName, QSettings::NativeFormat);
    settings.setValue("FriendlyAppName", displayName);

    settings.beginGroup("SupportedTypes");
    foreach (const QString& fileType, fileTypes)
        settings.setValue(fileType, QString());
    settings.endGroup();

    settings.beginGroup("shell");
    settings.beginGroup("open");
    settings.setValue("FriendlyAppName", displayName);
    settings.beginGroup("Command");
    settings.setValue(".", QChar('"') + QDir::toNativeSeparators(filePath) + QString("\" \"%1\""));
}

#endif


class MyApplication : public QApplication
{
    MainWindow *_main_window = nullptr;

public:
    MyApplication(int &argc, char **argv)
        : QApplication(argc, argv)
    {
        setOrganizationName("CuteMarkEd Project");
        setApplicationName("CuteMarkEd");
        setApplicationDisplayName("CuteMarkEd");
        setApplicationVersion("0.11.3");
    }

    ~MyApplication()
    {
        if (nullptr != _main_window)
            delete _main_window;
        _main_window = nullptr;
    }

    bool event(QEvent *event)
    {
        if (event->type() == QEvent::FileOpen && nullptr != _main_window)
        {
            QFileOpenEvent *openEvent = static_cast<QFileOpenEvent *>(event);
            return _main_window->loadFile(openEvent->file());
        }
        return QApplication::event(event);
    }

    MainWindow* get_main_window()
    {
        // NOTE Defer creating, or the language translations won't work.
        if (nullptr == _main_window)
            _main_window = new MainWindow();
        return _main_window;
    }
};


int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);

    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#ifdef Q_OS_WIN
    QStringList file_types;
    file_types << ".markdown" << ".md" << ".mdown";
    associate_file_types(file_types);
#endif

    // Load translation for Qt
    QTranslator qt_translator;
    if (!qt_translator.load("qt_" + QLocale::system().name(),
                            QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        qt_translator.load("qt_" + QLocale::system().name(), "translations");
    }
    app.installTranslator(&qt_translator);

    // Try to load translation for current locale from resource file
    QTranslator translator;
    translator.load("cutemarked_" + QLocale::system().name(), ":/translations");
    app.installTranslator(&translator);

    // Setup command line parser
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", QApplication::translate("main", "The file to open."));
    parser.process(app);

    // Get filename from command line arguments
    QString file_name;
    const QStringList cmdline_args = parser.positionalArguments();
    if (!cmdline_args.isEmpty()) {
        file_name = cmdline_args.at(0);
    }

    app.get_main_window()->loadFile(file_name);
    app.get_main_window()->show();

    return app.exec();
}
