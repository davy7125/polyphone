#ifndef SOUNDFONTVIEWER_H
#define SOUNDFONTVIEWER_H

#include <QWidget>
class SoundfontFilter;

namespace Ui {
class SoundfontViewer;
}

class SoundfontViewer : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontViewer(QWidget *parent = 0);
    ~SoundfontViewer();

    void initialize(int soundfontId);
    int getSoundfontId() { return _soundfontId; }

signals:
    void itemClicked(SoundfontFilter * filter);

private slots:
    void on_labelAuthor_linkActivated(const QString &link);
    void onDetailsReady(int soundfontId);
    void onDetailsFailed(int soundfontId, QString error);
    void on_pushRetry_clicked();

private:
    Ui::SoundfontViewer *ui;
    int _soundfontId;
};

#endif // SOUNDFONTVIEWER_H
