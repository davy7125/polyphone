#ifndef SOUNDFONTDOWNLOAD_H
#define SOUNDFONTDOWNLOAD_H

#include <QWidget>
class SoundfontDownloadData;

namespace Ui {
class SoundfontDownload;
}

class SoundfontDownload : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontDownload(QWidget *parent = 0);
    ~SoundfontDownload();

    void display(QList<SoundfontDownloadData *> data);

private:
    Ui::SoundfontDownload *ui;
};

#endif // SOUNDFONTDOWNLOAD_H
