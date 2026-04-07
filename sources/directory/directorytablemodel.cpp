#include "directorytablemodel.h"
#include "utils.h"
#include "contextmanager.h"

DirectoryTableModel::DirectoryTableModel(QObject *parent) : QAbstractTableModel(parent)
{
    _dateFormat = QLocale().dateFormat(QLocale::ShortFormat);
    _colorEnabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::NORMAL);
    _colorDisabled = ContextManager::theme()->getColor(ThemeManager::LIST_TEXT, ThemeManager::DISABLED);
}

DirectoryTableModel::~DirectoryTableModel()
{
    qDeleteAll(_files);
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

    switch (role)
    {
    case Qt::DisplayRole: {
        const DirectoryFileData * fd = _files[_filePaths[index.row()]];
        switch (index.column())
        {
        case 0:
            if (!fd->isOpenable())
                return fd->getFileName();
            if (!fd->isReadable())
                return fd->getFileName() + "\n" + tr("not readable");
            return QString("%1\n%2\n%3").arg(fd->getFileName(), Utils::getFormattedSize(fd->getFileSize()), fd->getLastModified().toString(_dateFormat));
        case 1:
            if (fd->isOpenable() && fd->isReadable())
                return fd->isScanned() ? QString("%1 sample(s)").arg(fd->getSampleCount()) : "?";
            return "";
            break;
        case 2:
            if (fd->isOpenable() && fd->isReadable())
                return fd->isScanned() ? QString("%1 instrument(s)").arg(fd->getInstrumentCount()) : "?";
            return "";
            break;
        case 3:
            if (fd->isOpenable() && fd->isReadable())
                return fd->isScanned() ? QString("%1 preset(s)").arg(fd->getPresetCount()) : "?";
            return "";
            break;
        }
    } break;
    case Qt::ForegroundRole:
        return QVariant::fromValue(_files[_filePaths[index.row()]]->isOpenable() ? _colorEnabled : _colorDisabled);
    case Qt::TextAlignmentRole:
        return QVariant::fromValue(Qt::AlignLeft | Qt::AlignTop);
    case Qt::UserRole:
        return QVariant::fromValue(_files[_filePaths[index.row()]]);
    default:
        break;
    }

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
    QString path = fd->getPath();
    if (_files.contains(path))
    {
        // Update it instead
        updateFile(fd);
    }
    else
    {
        beginInsertRows(QModelIndex(), _files.count(), _files.count());
        _files[path] = fd;
        _filePaths.append(path);
        endInsertRows();
    }
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
    QString path = fd->getPath();
    if (_files.contains(path))
    {
        delete _files[path];
        _files[path] = fd;

        // Notify the change
        int row = _filePaths.indexOf(path);
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
