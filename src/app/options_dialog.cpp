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

#include <QFontComboBox>
#include <QItemEditorFactory>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QMessageBox>
#include <QStyledItemDelegate>
#include <QTableWidgetItem>

#include "options_dialog.h"
#include "ui_options_dialog.h"
#include <snippets/snippet_collection.h>
#include "options.h"
#include "snippets_table_model.h"

class KeySequenceTableItem : public QTableWidgetItem
{
public:
    KeySequenceTableItem (const QKeySequence &keySequence) : 
        QTableWidgetItem(QTableWidgetItem::UserType+1),
        m_keySequence(keySequence)
    {
    }

    QVariant data(int role) const
    {
        switch (role) {
            case Qt::DisplayRole:
                return m_keySequence;
            case Qt::EditRole:
                return m_keySequence;
            default:
                return QVariant();
        }
    }

    void setData(int role, const QVariant &data)
    {
        if (role == Qt::EditRole)
            m_keySequence = data.value<QKeySequence>();

        QTableWidgetItem::setData(role, data);
    }

private:
    QKeySequence m_keySequence;
};

class KeySequenceEditFactory : public QItemEditorCreatorBase
{
public:
    QWidget *createWidget(QWidget *parent) const
    {
        return new QKeySequenceEdit(parent);
    }

    QByteArray valuePropertyName() const
    {
        return "keySequence";
    }
};


OptionsDialog::OptionsDialog(Options *opt, SnippetCollection *collection, const QList<QAction*> &acts, QWidget *parent)
    : QDialog(parent), _ui(new Ui::OptionsDialog), _options(opt),
      _snippet_collection(collection), _actions(acts)
{
    _ui->setupUi(this);

    _ui->tabWidget->setIconSize(QSize(24, 24));
    _ui->tabWidget->setTabIcon(0, QIcon("fa-cog.fontawesome"));
    _ui->tabWidget->setTabIcon(1, QIcon("fa-file-text-o.fontawesome"));
    _ui->tabWidget->setTabIcon(2, QIcon("fa-html5.fontawesome"));
    _ui->tabWidget->setTabIcon(3, QIcon("fa-globe.fontawesome"));
    _ui->tabWidget->setTabIcon(4, QIcon("fa-puzzle-piece.fontawesome"));
    _ui->tabWidget->setTabIcon(5, QIcon("fa-keyboard-o.fontawesome"));

    foreach (int size, QFontDatabase::standardSizes())
    {
        _ui->sizeComboBox->addItem(QString().setNum(size));
        _ui->defaultSizeComboBox->addItem(QString().setNum(size));
        _ui->defaultFixedSizeComboBox->addItem(QString().setNum(size));
    }

    _ui->portLineEdit->setValidator(new QIntValidator(0, 65535));
    _ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

    _ui->snippetTableView->setModel(new SnippetsTableModel(_snippet_collection, _ui->snippetTableView));
    connect(_ui->snippetTableView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(currentSnippetChanged(QModelIndex,QModelIndex)));

    connect(_ui->snippetTextEdit, SIGNAL(textChanged()),
            this, SLOT(snippetTextChanged()));

    // remove hoedown item from converter combo box, if hoedown is disabled
#ifndef ENABLE_HOEDOWN
    _ui->converterComboBox->removeItem(1);
#endif

    setupShortcutsTable();

    // read configuration state
    readState();
}

OptionsDialog::~OptionsDialog()
{
    delete _ui;
}

void OptionsDialog::done(int result)
{
    if (result == QDialog::Accepted) {
        // save configuration state
        saveState();
    }

    QDialog::done(result);
}

void OptionsDialog::manualProxyRadioButtonToggled(bool checked)
{
    _ui->hostLineEdit->setEnabled(checked);
    _ui->portLineEdit->setEnabled(checked);
    _ui->userNameLineEdit->setEnabled(checked);
    _ui->passwordLineEdit->setEnabled(checked);
}

void OptionsDialog::currentSnippetChanged(const QModelIndex &current, const QModelIndex &)
{
    const Snippet snippet = _snippet_collection->at(current.row());

    // update text edit for snippet content
    QString formattedSnippet(snippet.snippet);
    formattedSnippet.insert(snippet.cursor_position, "$|");
    _ui->snippetTextEdit->setPlainText(formattedSnippet);
    _ui->snippetTextEdit->setReadOnly(snippet.builtin);

    // disable remove button when built-in snippet is selected
    _ui->removeSnippetButton->setEnabled(!snippet.builtin);
}

void OptionsDialog::snippetTextChanged()
{
    const QModelIndex &modelIndex = _ui->snippetTableView->selectionModel()->currentIndex();
    if (modelIndex.isValid()) {
        Snippet snippet = _snippet_collection->at(modelIndex.row());
        if (!snippet.builtin) {
            snippet.snippet = _ui->snippetTextEdit->toPlainText();

            // find cursor marker
            int pos = snippet.snippet.indexOf("$|");
            if (pos >= 0) {
                snippet.cursor_position = pos;
                snippet.snippet.remove(pos, 2);
            }

            _snippet_collection->update(snippet);
        }
    }
}

void OptionsDialog::addSnippetButtonClicked()
{
    SnippetsTableModel *snippetModel = qobject_cast<SnippetsTableModel*>(_ui->snippetTableView->model());

    const QModelIndex &index = snippetModel->createSnippet();

    const int row = index.row();
    QModelIndex topLeft = snippetModel->index(row, 0, QModelIndex());
    QModelIndex bottomRight = snippetModel->index(row, 1, QModelIndex());
    QItemSelection selection(topLeft, bottomRight);
    _ui->snippetTableView->selectionModel()->select(selection, QItemSelectionModel::SelectCurrent);
    _ui->snippetTableView->setCurrentIndex(topLeft);
    _ui->snippetTableView->scrollTo(topLeft);

    _ui->snippetTableView->edit(index);
}

void OptionsDialog::removeSnippetButtonClicked()
{
    const QModelIndex &modelIndex = _ui->snippetTableView->selectionModel()->currentIndex();
    if (!modelIndex.isValid()) {
        QMessageBox::critical(0, tr("Error", "Title of error message box"), tr("No snippet selected."));
        return;
    }

    SnippetsTableModel *snippetModel = qobject_cast<SnippetsTableModel*>(_ui->snippetTableView->model());
    snippetModel->removeSnippet(modelIndex);
}

void OptionsDialog::validateShortcut(int row, int column)
{
    // Check changes to shortcut column only
    if (column != 1)
        return;

    QString newShortcut = _ui->shortcutsTable->item(row, column)->text();
    QKeySequence ks(newShortcut);
    if (ks.isEmpty() && !newShortcut.isEmpty()) {
        // If new shortcut was invalid, restore the original
        _ui->shortcutsTable->setItem(row, column,
            new KeySequenceTableItem(_actions[row]->shortcut()));
    } else {
        // Check for conflicts.
        if (!ks.isEmpty()) {
            for (int c = 0; c < _actions.size(); ++c) {
                if (c != row && ks == QKeySequence(_ui->shortcutsTable->item(c, 1)->text())) {
                    _ui->shortcutsTable->setItem(row, column,
                        new KeySequenceTableItem(_actions[row]->shortcut()));
                    QMessageBox::information(this, tr("Conflict"), tr("This shortcut is already used for \"%1\"").arg(_actions[c]->text().remove('&')));
                    return;
                }
            }
        }
        // If the new shortcut is not the same as the default, make the
        // action label bold.
        QFont font = _ui->shortcutsTable->item(row, 0)->font();
        font.setBold(ks != _actions[row]->property("defaultshortcut").value<QKeySequence>());
        _ui->shortcutsTable->item(row, 0)->setFont(font);
    }
}

void OptionsDialog::setupShortcutsTable()
{
    QStyledItemDelegate *delegate = new QStyledItemDelegate(_ui->shortcutsTable);
    QItemEditorFactory *factory = new QItemEditorFactory();
    factory->registerEditor(QVariant::nameToType("QKeySequence"), new KeySequenceEditFactory());
    delegate->setItemEditorFactory(factory);
    _ui->shortcutsTable->setItemDelegateForColumn(1, delegate);

    _ui->shortcutsTable->setRowCount(_actions.size());

    int i = 0;
    foreach (QAction *action, _actions) {
        QTableWidgetItem *label = new QTableWidgetItem(action->text().remove('&'));
        label->setFlags(Qt::ItemIsSelectable);
        const QKeySequence &defaultKeySeq = action->property("defaultshortcut").value<QKeySequence>();
        if (action->shortcut() != defaultKeySeq) {
            QFont font = label->font();
            font.setBold(true);
            label->setFont(font);
        }
        QTableWidgetItem *accel = new KeySequenceTableItem(action->shortcut());
        QTableWidgetItem *def = new KeySequenceTableItem(defaultKeySeq);
        def->setFlags(Qt::ItemIsSelectable);
        _ui->shortcutsTable->setItem(i, 0, label);
        _ui->shortcutsTable->setItem(i, 1, accel);
        _ui->shortcutsTable->setItem(i, 2, def);
        ++i;
    }

    _ui->shortcutsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _ui->shortcutsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _ui->shortcutsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    connect(_ui->shortcutsTable, SIGNAL(cellChanged(int,int)),
            this, SLOT(validateShortcut(int,int)));
}

void OptionsDialog::readState()
{
    // general settings
    _ui->converterComboBox->setCurrentIndex(_options->markdownConverter());

    // editor settings
    QFont font = _options->editorFont();
    _ui->fontComboBox->setCurrentFont(font);
    _ui->sizeComboBox->setCurrentText(QString().setNum(font.pointSize()));
    _ui->sourceSingleSizedCheckBox->setChecked(_options->isSourceAtSingleSizeEnabled());
    _ui->tabWidthSpinBox->setValue(_options->tabWidth());
    _ui->lineColumnCheckBox->setChecked(_options->isLineColumnEnabled());
    _ui->rulerEnableCheckBox->setChecked(_options->isRulerEnabled());
    _ui->rulerPosSpinBox->setValue(_options->rulerPos());

    // html preview settings
    _ui->standardFontComboBox->setCurrentFont(_options->standardFont());
    _ui->defaultSizeComboBox->setCurrentText(QString().setNum(_options->defaultFontSize()));
    _ui->serifFontComboBox->setCurrentFont(_options->serifFont());
    _ui->sansSerifFontComboBox->setCurrentFont(_options->sansSerifFont());
    _ui->fixedFontComboBox->setCurrentFont(_options->fixedFont());
    _ui->defaultFixedSizeComboBox->setCurrentText(QString().setNum(_options->defaultFixedFontSize()));
    _ui->mathInlineCheckBox->setChecked(_options->isMathInlineSupportEnabled());
    _ui->mathSupportCheckBox->setChecked(_options->isMathSupportEnabled());

    // proxy settings
    switch (_options->proxyMode()) {
    case Options::NoProxy:
        _ui->noProxyRadioButton->setChecked(true);
        break;

    case Options::SystemProxy:
        _ui->systemProxyRadioButton->setChecked(true);
        break;

    case Options::ManualProxy:
        _ui->manualProxyRadioButton->setChecked(true);
        break;
    }
    _ui->hostLineEdit->setText(_options->proxyHost());
    _ui->portLineEdit->setText(QString::number(_options->proxyPort()));
    _ui->userNameLineEdit->setText(_options->proxyUser());
    _ui->passwordLineEdit->setText(_options->proxyPassword());

    // shortcut settings
    for (int i = 0; i < _ui->shortcutsTable->rowCount(); ++i) {
        if (_options->hasCustomShortcut(_actions[i]->objectName())) {
            _ui->shortcutsTable->item(i, 1)->setData(Qt::EditRole, _options->customShortcut(_actions[i]->objectName()));
        }
    }
}

void OptionsDialog::saveState()
{
    // general settings
    _options->setMarkdownConverter((Options::MarkdownConverter)_ui->converterComboBox->currentIndex());

    // editor settings
    QFont font = _ui->fontComboBox->currentFont();
    font.setPointSize(_ui->sizeComboBox->currentText().toInt());
    _options->setEditorFont(font);
    _options->setSourceAtSingleSizeEnabled(_ui->sourceSingleSizedCheckBox->isChecked());
    _options->setTabWidth(_ui->tabWidthSpinBox->value());
    _options->setLineColumnEnabled(_ui->lineColumnCheckBox->isChecked());
    _options->setRulerEnabled(_ui->rulerEnableCheckBox->isChecked());
    _options->setRulerPos(_ui->rulerPosSpinBox->value());
    _options->setMathInlineSupportEnabled(_ui->mathInlineCheckBox->isChecked());
    _options->setMathSupportEnabled(_ui->mathSupportCheckBox->isChecked());

    // html preview settings
    _options->setStandardFont(_ui->standardFontComboBox->currentFont());
    _options->setDefaultFontSize(_ui->defaultSizeComboBox->currentText().toInt());
    _options->setSerifFont(_ui->serifFontComboBox->currentFont());
    _options->setSansSerifFont(_ui->sansSerifFontComboBox->currentFont());
    _options->setFixedFont(_ui->fixedFontComboBox->currentFont());
    _options->setDefaultFixedFontSize(_ui->defaultFixedSizeComboBox->currentText().toInt());

    // proxy settings
    if (_ui->noProxyRadioButton->isChecked()) {
        _options->setProxyMode(Options::NoProxy);
    } else if (_ui->systemProxyRadioButton->isChecked()) {
        _options->setProxyMode(Options::SystemProxy);
    } else if (_ui->manualProxyRadioButton->isChecked()) {
        _options->setProxyMode(Options::ManualProxy);
    }
    _options->setProxyHost(_ui->hostLineEdit->text());
    _options->setProxyPort(_ui->portLineEdit->text().toInt());
    _options->setProxyUser(_ui->userNameLineEdit->text());
    _options->setProxyPassword(_ui->passwordLineEdit->text());

    // shortcut settings
    for (int i = 0; i < _ui->shortcutsTable->rowCount(); ++i) {
        QKeySequence customKeySeq(_ui->shortcutsTable->item(i, 1)->text());
        _options->addCustomShortcut(_actions[i]->objectName(), customKeySeq);
    }
    
    _options->apply();
}

