#ifndef SOUNDFONTBROWSER_H
#define SOUNDFONTBROWSER_H

#include <QWidget>
class SoundfontFilter;
class SoundfontInformation;

namespace Ui {
class SoundfontBrowser;
}

class SoundfontBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit SoundfontBrowser(QWidget *parent = 0);
    ~SoundfontBrowser();

public slots:
    void applyFilter(SoundfontFilter * filter);
    void initialize();
    void soundfontListAvailable(QString error);

protected:
    void resizeEvent(QResizeEvent * event);
    void keyPressEvent(QKeyEvent * event);

private slots:
    void on_pushRetry_clicked();
    void on_pushResetFilters_clicked();
    void updateList();
    void on_listWidget_itemSelectionChanged();
    void on_comboSort_currentIndexChanged(int index);
    void on_pushClearSearch_clicked();

private:
    void fillFilter();
    SoundfontFilter * getFilter();
    void updateCellHeight();
    void updateList2();

    Ui::SoundfontBrowser *ui;
    bool _loadingFilter;
    QList<SoundfontInformation *> _currentSoundfontInfos;
    int _lastWidth;
};

#endif // SOUNDFONTBROWSER_H
