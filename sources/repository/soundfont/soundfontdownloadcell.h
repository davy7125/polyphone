#ifndef SOUNDFONTDOWNLOADCELL_H
#define SOUNDFONTDOWNLOADCELL_H

#include <QWidget>
class SoundfontDownloadData;

namespace Ui {
class SoundfontDownloadCell;
}

class SoundfontDownloadCell : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontDownloadCell(QWidget *parent = 0);
    ~SoundfontDownloadCell();

    void initialize(SoundfontDownloadData * data);

private slots:
    void on_pushDownload_clicked();

private:
    QString getSize(SoundfontDownloadData * data);
    QString getDate(SoundfontDownloadData * data);

    Ui::SoundfontDownloadCell *ui;
    int _id;
};

#endif // SOUNDFONTDOWNLOADCELL_H
