#ifndef SOUNDFONTVIEWER_H
#define SOUNDFONTVIEWER_H

#include <QWidget>

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

private:
    Ui::SoundfontViewer *ui;
    int _soundfontId;
};

#endif // SOUNDFONTVIEWER_H
