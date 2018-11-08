#ifndef SHOWSOUNDFONTS_H
#define SHOWSOUNDFONTS_H

#include <QWidget>
#include <QMutex>
class UrlReaderJson;
class QListWidgetItem;
class SoundfontFilter;

namespace Ui {
class ShowSoundfonts;
}

class ShowSoundfonts : public QWidget
{
    Q_OBJECT

public:
    explicit ShowSoundfonts(QWidget *parent = 0);
    ~ShowSoundfonts();

public slots:
    void initialize();
    void soundfontListAvailable(QString error);

signals:
    void itemClicked(SoundfontFilter * filter);

protected:
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

private slots:
    void dailyListAvailable(QString error);
    void on_listWidget_itemSelectionChanged();
    void on_pushRetry_clicked();

private:
    void populate();
    QString loadDailyIds();
    void updateCellHeight();

    Ui::ShowSoundfonts *ui;
    UrlReaderJson * _urlReaderJson;
    QList<int> _dailyIds;
    bool _soundfontListOk, _dailyListOk;
    QMutex _mutex; // Because two URL queries are concurrent
    QString _error1, _error2;
    int _lastWidth;
};

#endif // SHOWSOUNDFONTS_H
