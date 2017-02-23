
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#include "file_explorer_widget.h"
#include "ui_file_explorer_widget.h"

class FileSortFilterProxyModel : public QSortFilterProxyModel
{
public:
    FileSortFilterProxyModel(QObject *parent = 0) : QSortFilterProxyModel(parent) {}

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const
    {
        QFileSystemModel *model = static_cast<QFileSystemModel*>(this->sourceModel());

        QFileInfo leftInfo  = model->fileInfo(left);
        QFileInfo rightInfo = model->fileInfo(right);

        if (leftInfo.isDir() == rightInfo.isDir())
            return (leftInfo.filePath().compare(rightInfo.filePath(), Qt::CaseInsensitive) < 0);

        return leftInfo.isDir();
    }
};


FileExplorerWidget::FileExplorerWidget(QWidget *parent)
    : QWidget(parent), _initialized(false), _ui(new Ui::FileExplorerWidget),
    _model(new QFileSystemModel(this)), _sort_model(new FileSortFilterProxyModel(this))
{
    _ui->setupUi(this);

    _sort_model->setDynamicSortFilter(true);
    _sort_model->setSourceModel(_model);

    _ui->fileTreeView->setModel(_sort_model);
    _ui->fileTreeView->hideColumn(1);
    _ui->fileTreeView->sortByColumn(0, Qt::AscendingOrder);

    connect(_ui->fileTreeView, SIGNAL(doubleClicked(QModelIndex)),
            SLOT(fileOpen(QModelIndex)));
}

FileExplorerWidget::~FileExplorerWidget()
{
    delete _ui;
}

void FileExplorerWidget::showEvent(QShowEvent *event)
{
    if (!_initialized) {
        _model->setRootPath("");
        _initialized = true;
    }
    QWidget::showEvent(event);
}

void FileExplorerWidget::fileOpen(const QModelIndex &index)
{
    QFileInfo info = _model->fileInfo(_sort_model->mapToSource(index));
    if (info.isFile()) {
        const QString filePath = info.filePath();

        emit fileSelected(filePath);
    }
}
