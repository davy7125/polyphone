#ifndef SOUNDFONTDESCRIPTION_H
#define SOUNDFONTDESCRIPTION_H

#include <QWidget>
class SoundfontDescriptionData;
class UrlReaderImage;

namespace Ui {
class SoundfontDescription;
}

class SoundfontDescription : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontDescription(QWidget *parent = nullptr);
    ~SoundfontDescription();

    void display(SoundfontDescriptionData * data);

private slots:
    void documentSizeChanged(QSizeF size);
    void imageDownloaded(QString error);

private:
    static QString getCss();
    QString processFrame(QString frame);
    QString getBody(SoundfontDescriptionData * data);
    void downloadOneImage();

    Ui::SoundfontDescription *ui;
    UrlReaderImage * _imageReader;
    QStringList _imagesToDownload;
    static QString s_css;
};

#endif // SOUNDFONTDESCRIPTION_H
