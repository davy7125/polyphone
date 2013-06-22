#ifndef DIALOG_MAGNETOPHONE_H
#define DIALOG_MAGNETOPHONE_H

#include <QWidget>

namespace Ui {
class DialogMagnetophone;
}

class DialogMagnetophone : public QWidget
{
    Q_OBJECT
    
public:
    explicit DialogMagnetophone(QWidget *parent = 0);
    ~DialogMagnetophone();

public slots:
    void avanceSamples(int number);
    void setSampleRate(qint32 sampleRate);

signals:
    void startRecord(QString fileName);
    void endRecord();
    void pause(bool isOn);
    
private slots:
    void on_pushRecord_toggled(bool checked);
    void on_pushPlayPause_clicked();

private:
    Ui::DialogMagnetophone *ui;
    int _sampleRate;
    int _currentSample;
    bool _isPause;

    void closeEvent(QCloseEvent *);
    void initGui();
    QString getDefaultPath();
};

#endif // DIALOG_MAGNETOPHONE_H
