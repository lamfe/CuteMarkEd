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

#include "table_tool_dialog.h"
#include "ui_table_tool_dialog.h"

#include <QComboBox>
#include <QLineEdit>

// Make QPoint in QMap work
bool operator<(const QPoint& lhs, const QPoint& rhs)
{
    if (lhs.x() < rhs.x())
        return true;
    else if (lhs.x() == rhs.x())
        return lhs.y() < rhs.y();
    else
        return false;
}

TableToolDialog::TableToolDialog(QWidget *parent)
    : QDialog(parent), _ui(new Ui::TableToolDialog),
    _previous_row_count(0), _previous_column_count(0)
{
    _ui->setupUi(this);
    tableSizeChanged();
}

TableToolDialog::~TableToolDialog()
{
    delete _ui;
}

int TableToolDialog::rows() const
{
    return _ui->rowsSpinBox->value();
}

int TableToolDialog::columns() const
{
    return _ui->columnsSpinBox->value();
}

QList<Qt::Alignment> TableToolDialog::alignments() const
{
    QList<Qt::Alignment> alignments;

    foreach (QComboBox *cb, _alignment_combo_box_list) {
        Qt::Alignment alignment = (Qt::Alignment)cb->itemData(cb->currentIndex()).toInt();
        alignments.append(alignment);
    }

    return alignments;
}

QList<QStringList> TableToolDialog::tableCells() const
{
    QList<QStringList> table;

    for (int row = 0; row < rows(); ++row) {
        QStringList rowData;

        for (int col = 0; col < columns(); ++col) {
            rowData << _cell_editor_map[QPoint(col, row)]->text();
        }

        table.append(rowData);
    }

    return table;
}

void TableToolDialog::tableSizeChanged()
{
    int rowDiff = rows() - _previous_row_count;
    int columnDiff = columns() - _previous_column_count;

    if (columnDiff > 0) {
        addColumns(columnDiff);
    } else if (columnDiff < 0) {
        removeColumns(columnDiff);
    } else if (rowDiff > 0) {
        addRows(rowDiff);
    } else if (rowDiff < 0) {
        removeRows(rowDiff);
    }

    updateTabOrder();

    _previous_column_count = columns();
    _previous_row_count = rows();
}

void TableToolDialog::addColumns(int newColumns)
{
    for (int col = 0; col < newColumns; ++col) {
        // add combo box to choose alignment of column
        QComboBox *cb = new QComboBox(_ui->tableGroupBox);
        cb->addItem(tr("Left"), Qt::AlignLeft);
        cb->addItem(tr("Center"), Qt::AlignCenter);
        cb->addItem(tr("Right"), Qt::AlignRight);
        _ui->tableGridLayout->addWidget(cb, 0, col+_previous_column_count+1);

        _alignment_combo_box_list << cb;

        // add a column of new line edits
        for (int row = 0; row < rows(); ++row) {
            QLineEdit *edit = new QLineEdit(_ui->tableGroupBox);
            _ui->tableGridLayout->addWidget(edit, row + 1, col + _previous_column_count + 1);

            _cell_editor_map.insert(QPoint(col+_previous_column_count, row), edit);
        }
    }
}

void TableToolDialog::removeColumns(int removedColumns)
{
    for (int col = 0; col < qAbs(removedColumns); ++col) {
        // remove alignment combo box in last column
        QComboBox *cb = _alignment_combo_box_list.last();

        _alignment_combo_box_list.removeLast();
        _ui->tableGridLayout->removeWidget(cb);

        cb->deleteLater();

        // remove all line edits in last column
        for (int row = 0; row < rows(); ++row) {
            QLineEdit *edit = _cell_editor_map[QPoint(_previous_column_count-col-1, row)];

            _cell_editor_map.remove(QPoint(_previous_column_count-col-1, row));
            _ui->tableGridLayout->removeWidget(edit);

            edit->deleteLater();
        }
    }
}

void TableToolDialog::addRows(int newRows)
{
    for (int row = 0; row < newRows; ++row) {
        // add a new row of line edits
        for (int col = 0; col < columns(); ++col) {
            QLineEdit *edit = new QLineEdit(_ui->tableGroupBox);
            _ui->tableGridLayout->addWidget(edit, row+_previous_row_count+1, col+1);

            _cell_editor_map.insert(QPoint(col, row+_previous_row_count), edit);
        }
    }
}

void TableToolDialog::removeRows(int removedRows)
{
    for (int row = 0; row < qAbs(removedRows); ++row) {
        // remove all line edits in current row
        for (int col = 0; col < columns(); ++col) {
            QLineEdit *edit = _cell_editor_map[QPoint(col, _previous_row_count-row-1)];

            _cell_editor_map.remove(QPoint(col, _previous_row_count-row-1));
            _ui->tableGridLayout->removeWidget(edit);

            edit->deleteLater();
        }
    }
}

void TableToolDialog::updateTabOrder()
{
    // tab between spin boxes
    this->setTabOrder(_ui->rowsSpinBox, _ui->columnsSpinBox);

    QWidget *first = _ui->columnsSpinBox;
    foreach (QComboBox *cb, _alignment_combo_box_list) {
        this->setTabOrder(first, cb);
        first = cb;
    }

    // tab between line edits from left-to-right
    for (int row = 0; row < rows(); ++row) {
        for (int col = 0; col < columns(); ++col) {
            this->setTabOrder(first, _cell_editor_map[QPoint(col, row)]);
            first = _cell_editor_map[QPoint(col, row)];
        }
    }

    // tab between last line edit and okay button
    this->setTabOrder(first, _ui->buttonBox);
}
