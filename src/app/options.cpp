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
#include "options.h"

#if WITH_QTWEBENGINE
// TODO
#else
#   include <QWebSettings>
#endif


static const char* MARKDOWN_CONVERTER = "General/converter";
static const char* LAST_USED_THEME = "General/lastusedtheme";
static const char* THEME_DEFAULT = "Default";
static const char* FONT_FAMILY_DEFAULT = "Monospace";
static const char* FONT_FAMILY = "editor/font/family";
static const char* FONT_SIZE = "editor/font/size";
static const char* TAB_WIDTH = "editor/tabwidth";
static const char* LINECOLUMN_ENABLED = "editor/linecolumn/enabled";
static const char* RULER_ENABLED = "editor/ruler/enabled";
static const char* RULER_POS = "editor/ruler/pos";
static const char* PREVIEW_STANDARD_FONT = "preview/standardfont";
static const char* PREVIEW_FIXED_FONT = "preview/fixedfont";
static const char* PREVIEW_SERIF_FONT = "preview/seriffont";
static const char* PREVIEW_SANSSERIF_FONT = "preview/sansseriffont";
static const char* PREVIEW_DEFAULT_FONT_SIZE = "preview/defaultfontsize";
static const char* PREVIEW_DEFAULT_FIXED_FONT_SIZE = "preview/defaultfixedfontsize";
static const char* PROXY_MODE = "internet/proxy/mode";
static const char* PROXY_HOST = "internet/proxy/host";
static const char* PROXY_PORT = "internet/proxy/port";
static const char* PROXY_USER = "internet/proxy/user";
static const char* PROXY_PASSWORD = "internet/proxy/password";
static const char* AUTOLINK_ENABLED = "extensions/autolink";
static const char* STRIKETHROUGH_ENABLED = "extensions/strikethrough";
static const char* ALPHABETICLISTS_ENABLED = "extensions/alphabeticLists";
static const char* DEFINITIONSLISTS_ENABLED = "extensions/definitionLists";
static const char* SMARTYPANTS_ENABLED = "extensions/smartyPants";
static const char* FOOTNOTES_ENABLED = "extensions/footnotes";
static const char* SUPERSCRIPT_ENABLED = "extensions/superscript";
static const char* MATHSUPPORT_ENABLED = "mathsupport/enabled";
static const char* MATHINLINESUPPORT_ENABLED = "mathinlinesupport/enabled";
static const char* CODEHIGHLIGHT_ENABLED = "codehighlighting/enabled";
static const char* SHOWSPECIALCHARACTERS_ENABLED = "specialchars/enabled";
static const char* WORDWRAP_ENABLED = "wordwrap/enabled";
static const char* SOURCEATSINGLESIZE_ENABLED = "sourceatsinglesize/enabled";
static const char* SPELLINGCHECK_ENABLED = "spelling/enabled";
static const char* DICTIONARY_LANGUAGE = "spelling/language";
static const char* YAMLHEADERSUPPORT_ENABLED = "yamlheadersupport/enabled";
static const char* DIAGRAMSUPPORT_ENABLED = "diagramsupport/enabled";

static const char* DEPRECATED__LAST_USED_STYLE = "general/lastusedstyle";

Options::Options(QObject *parent)
    : QObject(parent), _tab_width(8), _proxy_mode(NoProxy), _proxy_port(0),
    _autolink_enabled(true), _strikethrough_enabled(true),
    _alphabetic_lists_enabled(true), _definition_lists_enabled(true),
    _smarty_pants_enabled(true), _footnotes_enabled(true),
    _superscript_enabled(true), _math_support_enabled(false),
    _math_inline_support_enabled(false), _code_highlighting_enabled(false),
    _show_special_characters_enabled(false), _word_wrap_enabled(true),
    _source_at_single_size_enabled(true), _spelling_check_enabled(true),
    _diagram_support_enabled(false), _line_column_enabled(true),
    _ruler_enabled(false), _ruler_pos(80), _markdown_converter(DiscountMarkdownConverter),
    _last_used_theme(THEME_DEFAULT)
{}

void Options::apply()
{
#if WITH_QTWEBENGINE
    // TODO
#else
    QWebSettings *globalWebSettings = QWebSettings::globalSettings();
    globalWebSettings->setFontFamily(QWebSettings::StandardFont, _standard_font_family);
    globalWebSettings->setFontFamily(QWebSettings::FixedFont, _fixed_font_family);
    globalWebSettings->setFontFamily(QWebSettings::SerifFont, _serif_font_family);
    globalWebSettings->setFontFamily(QWebSettings::SansSerifFont, _sans_serif_font_family);
    globalWebSettings->setFontSize(QWebSettings::DefaultFontSize, _default_font_size);
    globalWebSettings->setFontSize(QWebSettings::DefaultFixedFontSize, _default_fixed_font_size);
#endif

    emit proxyConfigurationChanged();
    emit markdownConverterChanged();
    emit lineColumnEnabledChanged(_line_column_enabled);
    emit rulerEnabledChanged(_ruler_enabled);
    emit rulerPosChanged(_ruler_pos);
}

QFont Options::editorFont() const
{
    return _font;
}

void Options::setEditorFont(const QFont &font)
{
    _font = font;
    emit editorFontChanged(font);
}

int Options::tabWidth() const
{
    return _tab_width;
}

void Options::setTabWidth(int width)
{
    _tab_width = width;
    emit tabWidthChanged(width);
}

bool Options::isLineColumnEnabled() const
{
    return _line_column_enabled;
}

void Options::setLineColumnEnabled(bool enabled)
{
    _line_column_enabled = enabled;
    emit lineColumnEnabledChanged(enabled);
}

bool Options::isRulerEnabled() const
{
    return _ruler_enabled;
}

void Options::setRulerEnabled(bool enabled)
{
    _ruler_enabled = enabled;
    emit rulerEnabledChanged(enabled);
}

int Options::rulerPos() const
{
    return _ruler_pos;
}

void Options::setRulerPos(int pos)
{
    _ruler_pos = pos;
    emit rulerPosChanged(pos);
}

QFont Options::standardFont() const
{
    return QFont(_standard_font_family);
}

void Options::setStandardFont(const QFont &font)
{
    _standard_font_family = font.family();
}

QFont Options::serifFont() const
{
    return QFont(_serif_font_family);
}

void Options::setSerifFont(const QFont &font)
{
    _serif_font_family = font.family();
}

QFont Options::sansSerifFont() const
{
    return QFont(_sans_serif_font_family);
}

void Options::setSansSerifFont(const QFont &font)
{
    _sans_serif_font_family = font.family();
}

QFont Options::fixedFont() const
{
    return QFont(_fixed_font_family);
}

void Options::setFixedFont(const QFont &font)
{
    _fixed_font_family = font.family();
}

int Options::defaultFontSize() const
{
    return _default_font_size;
}

void Options::setDefaultFontSize(int size)
{
    _default_font_size = size;
}

int Options::defaultFixedFontSize() const
{
    return _default_fixed_font_size;
}

void Options::setDefaultFixedFontSize(int size)
{
    _default_fixed_font_size = size;
}

Options::ProxyMode Options::proxyMode() const
{
    return _proxy_mode;
}

void Options::setProxyMode(Options::ProxyMode mode)
{
    _proxy_mode = mode;
}

QString Options::proxyHost() const
{
    return _proxy_host;
}

void Options::setProxyHost(const QString &host)
{
    _proxy_host = host;
}

quint16 Options::proxyPort() const
{
    return _proxy_port;
}

void Options::setProxyPort(quint16 port)
{
    _proxy_port = port;
}

QString Options::proxyUser() const
{
    return _proxy_user;
}

void Options::setProxyUser(const QString &user)
{
    _proxy_user = user;
}

QString Options::proxyPassword() const
{
    return _proxy_password;
}

void Options::setProxyPassword(const QString &password)
{
    _proxy_password = password;
}

void Options::addCustomShortcut(const QString &actionName, const QKeySequence &keySequence)
{
    if (actionName.isEmpty()) return;
    _custom_shortcuts.insert(actionName, keySequence);
}

bool Options::hasCustomShortcut(const QString &actionName) const
{
    return _custom_shortcuts.contains(actionName);
}

QKeySequence Options::customShortcut(const QString &actionName) const
{
    return _custom_shortcuts.value(actionName);
}

bool Options::isAutolinkEnabled() const
{
    return _autolink_enabled;
}

void Options::setAutolinkEnabled(bool enabled)
{
    _autolink_enabled = enabled;
}

bool Options::isStrikethroughEnabled() const
{
    return _strikethrough_enabled;
}

void Options::setStrikethroughEnabled(bool enabled)
{
    _strikethrough_enabled = enabled;
}

bool Options::isAlphabeticListsEnabled() const
{
    return _alphabetic_lists_enabled;
}

void Options::setAlphabeticListsEnabled(bool enabled)
{
    _alphabetic_lists_enabled = enabled;
}

bool Options::isDefinitionListsEnabled() const
{
    return _definition_lists_enabled;
}

void Options::setDefinitionListsEnabled(bool enabled)
{
    _definition_lists_enabled = enabled;
}

bool Options::isSmartyPantsEnabled() const
{
    return _smarty_pants_enabled;
}

void Options::setSmartyPantsEnabled(bool enabled)
{
    _smarty_pants_enabled = enabled;
}

bool Options::isFootnotesEnabled() const
{
    return _footnotes_enabled;
}

void Options::setFootnotesEnabled(bool enabled)
{
    _footnotes_enabled = enabled;
}

bool Options::isSuperscriptEnabled() const
{
    return _superscript_enabled;
}

void Options::setSuperscriptEnabled(bool enabled)
{
    _superscript_enabled = enabled;
}

bool Options::isMathSupportEnabled() const
{
    return _math_support_enabled;
}

void Options::setMathSupportEnabled(bool enabled)
{
    _math_support_enabled = enabled;
}

bool Options::isMathInlineSupportEnabled() const
{
    return _math_inline_support_enabled;
}

void Options::setMathInlineSupportEnabled(bool enabled)
{
    _math_inline_support_enabled = enabled;
}

bool Options::isCodeHighlightingEnabled() const
{
    return _code_highlighting_enabled;
}

void Options::setCodeHighlightingEnabled(bool enabled)
{
    _code_highlighting_enabled = enabled;
}

bool Options::isShowSpecialCharactersEnabled() const
{
    return _show_special_characters_enabled;
}

void Options::setShowSpecialCharactersEnabled(bool enabled)
{
    _show_special_characters_enabled = enabled;
}

bool Options::isWordWrapEnabled() const
{
    return _word_wrap_enabled;
}

void Options::setWordWrapEnabled(bool enabled)
{
    _word_wrap_enabled = enabled;
}

bool Options::isSourceAtSingleSizeEnabled() const
{
    return _source_at_single_size_enabled;
}

void Options::setSourceAtSingleSizeEnabled(bool enabled)
{
    _source_at_single_size_enabled = enabled;
    emit editorStyleChanged();
}

bool Options::isSpellingCheckEnabled() const
{
    return _spelling_check_enabled;
}

void Options::setSpellingCheckEnabled(bool enabled)
{
    _spelling_check_enabled = enabled;
}

bool Options::isYamlHeaderSupportEnabled() const
{
    return _yaml_header_support_enabled;
}

void Options::setYamlHeaderSupportEnabled(bool enabled)
{
    _yaml_header_support_enabled = enabled;
}

bool Options::isDiagramSupportEnabled() const
{
    return _diagram_support_enabled;
}

void Options::setDiagramSupportEnabled(bool enabled)
{
    _diagram_support_enabled = enabled;
}

QString Options::dictionaryLanguage() const
{
    return _dictionary_language;
}

void Options::setDictionaryLanguage(const QString &language)
{
    _dictionary_language = language;
}

Options::MarkdownConverter Options::markdownConverter() const
{
    return _markdown_converter;
}

void Options::setMarkdownConverter(Options::MarkdownConverter converter)
{
    if (_markdown_converter != converter) {
        _markdown_converter = converter;
        emit markdownConverterChanged();
    }
}

QString Options::lastUsedTheme() const
{
    return _last_used_theme;
}

void Options::setLastUsedTheme(const QString &theme)
{
    _last_used_theme = theme;
}

void Options::readSettings()
{
    QSettings settings;

    // general settings
    _markdown_converter = (Options::MarkdownConverter)settings.value(MARKDOWN_CONVERTER, 0).toInt();
    _last_used_theme = settings.value(LAST_USED_THEME, THEME_DEFAULT).toString();

    // editor settings
    QString fontFamily = settings.value(FONT_FAMILY, FONT_FAMILY_DEFAULT).toString();
    int fontSize = settings.value(FONT_SIZE, 10).toInt();

    _tab_width = settings.value(TAB_WIDTH, 8).toInt();
    _line_column_enabled = settings.value(LINECOLUMN_ENABLED, false).toBool();
    _ruler_enabled = settings.value(RULER_ENABLED, false).toBool();
    _ruler_pos = settings.value(RULER_POS, 80).toInt();

    QFont f(fontFamily, fontSize);
    f.setStyleHint(QFont::TypeWriter);
    setEditorFont(f);

    // html preview settings
#if WITH_QTWEBENGINE
    // TODO
#else
    QWebSettings *globalWebSettings = QWebSettings::globalSettings();
    _standard_font_family = settings.value(PREVIEW_STANDARD_FONT, globalWebSettings->fontFamily(QWebSettings::StandardFont)).toString();
    _fixed_font_family = settings.value(PREVIEW_FIXED_FONT, globalWebSettings->fontFamily(QWebSettings::FixedFont)).toString();
    _serif_font_family = settings.value(PREVIEW_SERIF_FONT, globalWebSettings->fontFamily(QWebSettings::SerifFont)).toString();
    _sans_serif_font_family = settings.value(PREVIEW_SANSSERIF_FONT, globalWebSettings->fontFamily(QWebSettings::SansSerifFont)).toString();
    _default_font_size = settings.value(PREVIEW_DEFAULT_FONT_SIZE, globalWebSettings->fontSize(QWebSettings::DefaultFontSize)).toInt();
    _default_fixed_font_size = settings.value(PREVIEW_DEFAULT_FIXED_FONT_SIZE, globalWebSettings->fontSize(QWebSettings::DefaultFixedFontSize)).toInt();
#endif
    _math_inline_support_enabled = settings.value(MATHINLINESUPPORT_ENABLED, false).toBool();

    // proxy settings
    _proxy_mode = (Options::ProxyMode)settings.value(PROXY_MODE, 0).toInt();
    _proxy_host = settings.value(PROXY_HOST, "").toString();
    _proxy_port = settings.value(PROXY_PORT, 0).toInt();
    _proxy_user = settings.value(PROXY_USER, "").toString();
    _proxy_password = settings.value(PROXY_PASSWORD, "").toString();

    // shortcut settings
    settings.beginGroup("shortcuts");
    foreach (QString actionName, settings.childKeys()) {
        QKeySequence keySequence = settings.value(actionName, "").value<QKeySequence>();
        addCustomShortcut(actionName, keySequence);
    }
    settings.endGroup();

    // extension settings
    _autolink_enabled = settings.value(AUTOLINK_ENABLED, true).toBool();
    _strikethrough_enabled = settings.value(STRIKETHROUGH_ENABLED, true).toBool();
    _alphabetic_lists_enabled = settings.value(ALPHABETICLISTS_ENABLED, true).toBool();
    _definition_lists_enabled = settings.value(DEFINITIONSLISTS_ENABLED, true).toBool();
    _smarty_pants_enabled = settings.value(SMARTYPANTS_ENABLED, true).toBool();
    _footnotes_enabled = settings.value(FOOTNOTES_ENABLED, true).toBool();
    _superscript_enabled = settings.value(SUPERSCRIPT_ENABLED, true).toBool();

    _math_support_enabled = settings.value(MATHSUPPORT_ENABLED, false).toBool();
    _code_highlighting_enabled = settings.value(CODEHIGHLIGHT_ENABLED, false).toBool();
    _show_special_characters_enabled = settings.value(SHOWSPECIALCHARACTERS_ENABLED, false).toBool();
    _word_wrap_enabled = settings.value(WORDWRAP_ENABLED, true).toBool();
    _source_at_single_size_enabled = settings.value(SOURCEATSINGLESIZE_ENABLED, true).toBool();
    _yaml_header_support_enabled = settings.value(YAMLHEADERSUPPORT_ENABLED, false).toBool();
    _diagram_support_enabled = settings.value(DIAGRAMSUPPORT_ENABLED, false).toBool();

    // spelling check settings
    _spelling_check_enabled = settings.value(SPELLINGCHECK_ENABLED, true).toBool();
    _dictionary_language = settings.value(DICTIONARY_LANGUAGE, "en_US").toString();

    // migrate deprecated lastUsedStyle option
    if (settings.contains(DEPRECATED__LAST_USED_STYLE)) {
        migrateLastUsedStyleOption(settings);
    }

    apply();
}

void Options::writeSettings()
{
    QSettings settings;

    // general settings
    settings.setValue(MARKDOWN_CONVERTER, _markdown_converter);
    settings.setValue(LAST_USED_THEME, _last_used_theme);

    // editor settings
    settings.setValue(FONT_FAMILY, _font.family());
    settings.setValue(FONT_SIZE, _font.pointSize());
    settings.setValue(TAB_WIDTH, _tab_width);
    settings.setValue(LINECOLUMN_ENABLED, _line_column_enabled);
    settings.setValue(RULER_ENABLED, _ruler_enabled);
    settings.setValue(RULER_POS, _ruler_pos);

    // html preview settings
    settings.setValue(PREVIEW_STANDARD_FONT, _standard_font_family);
    settings.setValue(PREVIEW_FIXED_FONT, _fixed_font_family);
    settings.setValue(PREVIEW_SERIF_FONT, _serif_font_family);
    settings.setValue(PREVIEW_SANSSERIF_FONT, _sans_serif_font_family);
    settings.setValue(PREVIEW_DEFAULT_FONT_SIZE, _default_font_size);
    settings.setValue(PREVIEW_DEFAULT_FIXED_FONT_SIZE, _default_fixed_font_size);
    settings.setValue(MATHINLINESUPPORT_ENABLED, _math_inline_support_enabled);

    // proxy settings
    settings.setValue(PROXY_MODE, _proxy_mode);
    settings.setValue(PROXY_HOST, _proxy_host);
    settings.setValue(PROXY_PORT, _proxy_port);
    settings.setValue(PROXY_USER, _proxy_user);
    settings.setValue(PROXY_PASSWORD, _proxy_password);

    // shortcut settings
    settings.beginGroup("shortcuts");
    QMap<QString, QKeySequence>::const_iterator it = _custom_shortcuts.constBegin();
    while (it != _custom_shortcuts.constEnd()) {
        settings.setValue(it.key(), it.value());
        ++it;
    }
    settings.endGroup();

    // extensions settings
    settings.setValue(AUTOLINK_ENABLED, _autolink_enabled);
    settings.setValue(STRIKETHROUGH_ENABLED, _strikethrough_enabled);
    settings.setValue(ALPHABETICLISTS_ENABLED, _alphabetic_lists_enabled);
    settings.setValue(DEFINITIONSLISTS_ENABLED, _definition_lists_enabled);
    settings.setValue(SMARTYPANTS_ENABLED, _smarty_pants_enabled);
    settings.setValue(FOOTNOTES_ENABLED, _footnotes_enabled);
    settings.setValue(SUPERSCRIPT_ENABLED, _superscript_enabled);

    settings.setValue(MATHSUPPORT_ENABLED, _math_support_enabled);
    settings.setValue(CODEHIGHLIGHT_ENABLED, _code_highlighting_enabled);
    settings.setValue(SHOWSPECIALCHARACTERS_ENABLED, _show_special_characters_enabled);
    settings.setValue(WORDWRAP_ENABLED, _word_wrap_enabled);
    settings.setValue(SOURCEATSINGLESIZE_ENABLED, _source_at_single_size_enabled);
    settings.setValue(YAMLHEADERSUPPORT_ENABLED, _yaml_header_support_enabled);
    settings.setValue(DIAGRAMSUPPORT_ENABLED, _diagram_support_enabled);

    // spelling check settings
    settings.setValue(SPELLINGCHECK_ENABLED, _spelling_check_enabled);
    settings.setValue(DICTIONARY_LANGUAGE, _dictionary_language);
}

void Options::migrateLastUsedStyleOption(QSettings &settings)
{
    static const QMap<QString, QString> migrations {
        { "actionDefault", "Default" },
        { "actionGithub", "Github" },
        { "actionSolarizedLight", "Solarized Light" },
        { "actionSolarizedDark", "Solarized Dark" },
        { "actionClearness", "Clearness" },
        { "actionClearnessDark", "Clearness Dark" },
        { "actionBywordDark", "Byword Dark" }
    };

    QString lastUsedStyle = settings.value(DEPRECATED__LAST_USED_STYLE).toString();
    _last_used_theme = migrations[lastUsedStyle];

    settings.remove(DEPRECATED__LAST_USED_STYLE);
}
