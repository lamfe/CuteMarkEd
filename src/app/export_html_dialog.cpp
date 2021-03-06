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

#include <QFileDialog>
#include <QFileInfo>

#include "export_html_dialog.h"
#include "ui_export_html_dialog.h"

ExportHtmlDialog::ExportHtmlDialog(const QString &fileName, QWidget *parent)
    : QDialog(parent), _ui(new Ui::ExportHtmlDialog)
{
    _ui->setupUi(this);

    // change button text of standard Ok button
    QPushButton *okButton = _ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setText("Export HTML");

    if (!fileName.isEmpty()) {
        QFileInfo info(fileName);
        QString exportFileName = info.absoluteFilePath().replace(info.suffix(), "html");
        _ui->exportToLineEdit->setText(exportFileName);
    }

    // initialize Ok button state
    exportToTextChanged(fileName);
}

ExportHtmlDialog::~ExportHtmlDialog()
{
    delete _ui;
}

QString ExportHtmlDialog::fileName() const
{
    return _ui->exportToLineEdit->text();
}

bool ExportHtmlDialog::includeCSS() const
{
    return _ui->styleCheckBox->isChecked();
}

bool ExportHtmlDialog::includeCodeHighlighting() const
{
    return _ui->highlightCheckBox->isChecked();
}

void ExportHtmlDialog::exportToTextChanged(const QString &text)
{
    // only enable ok button if a filename was provided
    QPushButton *okButton = _ui->buttonBox->button(QDialogButtonBox::Ok);
    okButton->setEnabled(!text.isEmpty());
}

void ExportHtmlDialog::chooseFileButtonClicked()
{
    QString fileName = _ui->exportToLineEdit->text();

    fileName = QFileDialog::getSaveFileName(this, tr("Export to HTML..."), fileName,
                                                  tr("HTML Files (*.html *.htm);;All Files (*)"));
    if (!fileName.isEmpty()) {
        _ui->exportToLineEdit->setText(fileName);
    }
}
