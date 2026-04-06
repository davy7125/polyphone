#include "directorytablemodel.h"
#include "utils.h"

DirectoryTableModel::DirectoryTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    _dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
}

int DirectoryTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _files.count();
}

int DirectoryTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 4;
}

QVariant DirectoryTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= _files.count())
        return QVariant();

    const DirectoryFileData * fd = _files[_filePaths[index.row()]];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
        case 0:
            return QString("%1\n%2\n%3").arg(fd->getFileName(), Utils::getFormattedSize(fd->getFileSize()), fd->getLastModified().toString(_dateFormat));
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        }
    }

    // Data for delegates
    if (role == Qt::UserRole)
        return QVariant::fromValue(fd);

    return QVariant();
}

QVariant DirectoryTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0: return tr("File");
        case 1: return tr("Samples");
        case 2: return tr("Instruments");
        case 3: return tr("Presets");
        default: break;
        }
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void DirectoryTableModel::addFile(DirectoryFileData * fd)
{
    beginInsertRows(QModelIndex(), _files.count(), _files.count());
    _files[fd->getPath()] = fd;
    _filePaths.append(fd->getPath());
    endInsertRows();
}

void DirectoryTableModel::removeFile(QString filePath)
{
    if (_files.contains(filePath))
    {
        int row = _filePaths.indexOf(filePath);
        beginRemoveRows(QModelIndex(), row, row);
        delete _files.take(filePath);
        _filePaths.removeAll(filePath);
        endRemoveRows();
    }
}

void DirectoryTableModel::updateFile(DirectoryFileData * fd)
{
    // Find the corresponding soundfont
    if (_files.contains(fd->getPath()))
    {
        delete _files[fd->getPath()];
        _files[fd->getPath()] = fd;

        // Notify the change
        int row = _filePaths.indexOf(fd->getPath());
        QModelIndex topLeft = index(row, 0);
        QModelIndex bottomRight = index(row, columnCount() - 1);
        emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole, Qt::UserRole});
    }
    else
    {
        // Add it instead
        addFile(fd);
    }
}
