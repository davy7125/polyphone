#ifndef DIRECTORYTABLEMODEL_H
#define DIRECTORYTABLEMODEL_H

#include <QAbstractTableModel>
#include "directoryfiledata.h"

class DirectoryTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DirectoryTableModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Methods for updating the model
    void addFile(DirectoryFileData * fd);
    void removeFile(QString filePath);
    void updateFile(DirectoryFileData * fd);

private:
    QList<QString> _filePaths;
    QMap<QString, DirectoryFileData *> _files;
    QString _dateFormat;
};

#endif // DIRECTORYTABLEMODEL_H
